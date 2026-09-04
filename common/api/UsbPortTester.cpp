#include "UsbPortTester.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QProcess>
#include <QRegularExpression>
#include <QThread>
#include <QtGlobal>

namespace {

QString readSysfsTrimmed(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    return QString::fromLatin1(f.readAll()).trimmed();
}

bool isUsbRootHubDirName(const QString &name)
{
    if (!name.startsWith(QStringLiteral("usb")))
        return false;
    bool ok = false;
    name.mid(3).toInt(&ok);
    return ok;
}

bool isUsbDeviceDirName(const QString &name)
{
    // 外设节点：1-1、1-1.2（不含接口 1-1:1.0）
    return name.contains(QLatin1Char('-')) && !name.contains(QLatin1Char(':'));
}

bool isUsbInterfaceDirName(const QString &name)
{
    // 接口节点：1-1.1:1.0
    return name.contains(QLatin1Char('-')) && name.contains(QLatin1Char(':'));
}

// 将 USB 口归一到「板载物理口」：
// - usb1/1-1、usb1/1-2           → 直连控制器的两个口
// - usb1/1-1.1、usb1/1-1.2       → 板载 Hub 下的两个口（常见）
// - usb1/1-1.1.2                 → 外部分线/Hub 下游，归并回板载口 usb1/1-1.1
// 这样：两只 U 盘经两根子线分别插两个板载口 → 可识别为 2 口；
//       两只 U 盘经一分二插在同一个板载口 → 只算 1 口（失败）。
QString normalizeToBoardPortKey(const QString &rawKey)
{
    if (rawKey.isEmpty())
        return QString();
    const int slash = rawKey.indexOf(QLatin1Char('/'));
    if (slash <= 0)
        return rawKey;
    const QString hub = rawKey.left(slash);          // usb1
    const QString port = rawKey.mid(slash + 1);      // 1-1.1.2
    const QStringList segs = port.split(QLatin1Char('.'));
    if (segs.isEmpty())
        return rawKey;
    if (segs.size() == 1)
        return hub + QLatin1Char('/') + segs.at(0);  // usb1/1-2
    // 最多保留一段点号：N-M.P
    return hub + QLatin1Char('/') + segs.at(0) + QLatin1Char('.') + segs.at(1);
}

// 设备名 1-1.1 -> usb1/1-1.1，再归一到板载口
QString portKeyFromUsbDevName(const QString &devName)
{
    const int dash = devName.indexOf(QLatin1Char('-'));
    if (dash <= 0)
        return QString();
    bool ok = false;
    const int bus = devName.left(dash).toInt(&ok);
    if (!ok || bus <= 0)
        return QString();
    return normalizeToBoardPortKey(QStringLiteral("usb%1/%2").arg(bus).arg(devName));
}

// 从 canonical 路径取叶子 USB 口，再归一到板载口
QString usbPhysicalPortKeyFromPath(const QString &resolvedPath)
{
    const QStringList parts = resolvedPath.split(QLatin1Char('/'),
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
                                                 Qt::SkipEmptyParts
#else
                                                 QString::SkipEmptyParts
#endif
    );
    static const QRegularExpression reHub(QStringLiteral("^usb(\\d+)$"));
    static const QRegularExpression rePort(QStringLiteral("^(\\d+-\\d+(?:\\.\\d+)*)$"));
    QString hubNum;
    QString lastPort;
    for (const QString &part : parts) {
        const QRegularExpressionMatch mHub = reHub.match(part);
        if (mHub.hasMatch()) {
            hubNum = mHub.captured(1);
            continue;
        }
        if (part.contains(QLatin1Char(':')))
            break;
        const QRegularExpressionMatch mPort = rePort.match(part);
        if (mPort.hasMatch())
            lastPort = mPort.captured(1);
    }
    if (hubNum.isEmpty() || lastPort.isEmpty())
        return QString();
    return normalizeToBoardPortKey(QStringLiteral("usb%1/%2").arg(hubNum, lastPort));
}

bool deviceHasMassStorageInterface(const QString &devSysPath, QString *ifaceLog)
{
    // 设备级 class=08
    const QString devClass = readSysfsTrimmed(devSysPath + QStringLiteral("/bDeviceClass"));
    if (devClass == QStringLiteral("08")) {
        if (ifaceLog)
            *ifaceLog += QStringLiteral(" deviceClass=08");
        return true;
    }

    QDir devDir(devSysPath);
    const QStringList children = devDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &child : children) {
        if (!isUsbInterfaceDirName(child))
            continue;
        const QString ifaceClass = readSysfsTrimmed(
            devSysPath + QLatin1Char('/') + child + QStringLiteral("/bInterfaceClass"));
        if (ifaceLog)
            *ifaceLog += QStringLiteral(" %1=class%2").arg(child, ifaceClass);
        // 08 = Mass Storage
        if (ifaceClass == QStringLiteral("08"))
            return true;
    }
    return false;
}

void addStick(QMap<QString, QStringList> *sticksByPort, const QString &portKey, const QString &tag)
{
    if (!sticksByPort || portKey.isEmpty() || tag.isEmpty())
        return;
    QStringList &list = (*sticksByPort)[portKey];
    if (!list.contains(tag))
        list << tag;
}

} // namespace

UsbHostTestResult UsbPortTester::testHost()
{
    UsbHostTestResult result;

#if !defined(Q_OS_LINUX)
    result.detail = QStringLiteral("USB host test is only supported on Linux");
    return result;
#else
    const QString usbBusPath = QStringLiteral("/sys/bus/usb/devices");
    QDir busDir(usbBusPath);
    if (!busDir.exists()) {
        result.detail = QStringLiteral("Missing %1 (USB subsystem not available)\n").arg(usbBusPath);
        result.summary = QStringLiteral("USB FAIL (no subsystem)");
        return result;
    }

    const QStringList entries = busDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList rootHubs;
    QStringList peripherals;
    QMap<QString, QStringList> sticksByPort;

    for (const QString &name : entries) {
        if (isUsbRootHubDirName(name)) {
            rootHubs << name;
            continue;
        }
        if (!isUsbDeviceDirName(name))
            continue;

        const QString base = usbBusPath + QLatin1Char('/') + name;
        const QString vendor = readSysfsTrimmed(base + QStringLiteral("/idVendor"));
        const QString product = readSysfsTrimmed(base + QStringLiteral("/idProduct"));
        const QString manuf = readSysfsTrimmed(base + QStringLiteral("/manufacturer"));
        const QString prodName = readSysfsTrimmed(base + QStringLiteral("/product"));
        const QString devClass = readSysfsTrimmed(base + QStringLiteral("/bDeviceClass"));

        if (vendor.isEmpty() || product.isEmpty())
            continue;

        // Hub 本身不算 U 盘
        if (devClass == QStringLiteral("09")) {
            QString line = QStringLiteral("%1 %2:%3").arg(name, vendor, product);
            if (!prodName.isEmpty())
                line += QStringLiteral(" %1").arg(prodName);
            line += QStringLiteral(" class=09 (Hub)");
            peripherals << line;
            continue;
        }

        QString ifaceLog;
        const bool massStorage = deviceHasMassStorageInterface(base, &ifaceLog);

        QString line = QStringLiteral("%1 %2:%3").arg(name, vendor, product);
        if (!prodName.isEmpty())
            line += QStringLiteral(" %1").arg(prodName);
        else if (!manuf.isEmpty())
            line += QStringLiteral(" %1").arg(manuf);
        if (!devClass.isEmpty())
            line += QStringLiteral(" class=%1").arg(devClass);
        if (massStorage)
            line += QStringLiteral(" [MassStorage%1]").arg(ifaceLog);
        peripherals << line;

        if (massStorage) {
            const QString portKey = portKeyFromUsbDevName(name);
            addStick(&sticksByPort, portKey, name);
            result.detail += QStringLiteral(
                                 "Mass storage device: %1 port=%2%3\n")
                                 .arg(name,
                                      portKey.isEmpty() ? QStringLiteral("?") : portKey,
                                      ifaceLog);
        }
    }

    result.detail += QStringLiteral("sysfs %1\n").arg(usbBusPath);
    result.detail += QStringLiteral("Root hubs: %1\n")
                         .arg(rootHubs.isEmpty() ? QStringLiteral("(none)")
                                                 : rootHubs.join(QStringLiteral(", ")));
    result.detail += QStringLiteral("Required USB ports with U盘: %1\n")
                         .arg(kRequiredUsbPorts);
    result.detail += QStringLiteral("Peripherals: %1\n").arg(peripherals.size());
    for (const QString &p : peripherals)
        result.detail += QStringLiteral("  %1\n").arg(p);

    if (rootHubs.isEmpty()) {
        result.summary = QStringLiteral("USB FAIL (no host controller)");
        result.detail += QStringLiteral("No USB root hub found\n");
        return result;
    }

    // 块设备补充识别（若驱动已创建 /dev/sd*）
    // 部分板子枚举后稍晚才出块设备，短重试一次
    QStringList nonRemovableUsbStorage;
    for (int attempt = 0; attempt < 2; ++attempt) {
        if (attempt > 0)
            QThread::msleep(800);

        QDir blockDir(QStringLiteral("/sys/block"));
        const QStringList blocks = blockDir.entryList(QStringList() << QStringLiteral("sd*"),
                                                      QDir::Dirs | QDir::NoDotAndDotDot);
        if (blocks.isEmpty() && attempt == 0) {
            result.detail += QStringLiteral(
                "USB storage block: (none yet), wait and retry...\n");
            continue;
        }

        for (const QString &blk : blocks) {
            const QString deviceLink = QStringLiteral("/sys/block/%1/device").arg(blk);
            QFileInfo fi(deviceLink);
            if (!fi.exists())
                continue;
            const QString resolved = fi.canonicalFilePath();
            if (!resolved.contains(QStringLiteral("/usb")))
                continue;

            const QString removable = readSysfsTrimmed(
                QStringLiteral("/sys/block/%1/removable").arg(blk));
            const QString portKey = usbPhysicalPortKeyFromPath(resolved);
            result.detail += QStringLiteral(
                                 "USB storage block: /dev/%1 removable=%2 port=%3 (%4)\n")
                                 .arg(blk,
                                      removable.isEmpty() ? QStringLiteral("?") : removable,
                                      portKey.isEmpty() ? QStringLiteral("?") : portKey,
                                      resolved);

            if (removable != QStringLiteral("1")) {
                if (!nonRemovableUsbStorage.contains(blk))
                    nonRemovableUsbStorage << blk;
                continue;
            }
            addStick(&sticksByPort, portKey, blk);
        }
        break;
    }
    if (sticksByPort.isEmpty() && nonRemovableUsbStorage.isEmpty())
        result.detail += QStringLiteral("USB storage block: (none)\n");

    {
        QProcess proc;
        proc.start(QStringLiteral("lsusb"), QStringList());
        if (proc.waitForFinished(2000) && proc.exitCode() == 0) {
            const QString out = QString::fromLocal8Bit(proc.readAllStandardOutput()).trimmed();
            if (!out.isEmpty())
                result.detail += QStringLiteral("lsusb:\n%1\n").arg(out);
        } else {
            result.detail += QStringLiteral("lsusb: unavailable\n");
        }
    }

    const int portCount = sticksByPort.size();
    result.detail += QStringLiteral("USB ports with U盘: %1\n").arg(portCount);
    for (auto it = sticksByPort.constBegin(); it != sticksByPort.constEnd(); ++it) {
        result.detail += QStringLiteral("  %1 -> %2\n")
                             .arg(it.key(), it.value().join(QLatin1Char(',')));
    }

    if (portCount >= kRequiredUsbPorts) {
        QStringList parts;
        for (auto it = sticksByPort.constBegin(); it != sticksByPort.constEnd(); ++it)
            parts << QStringLiteral("%1@%2").arg(it.value().join(QLatin1Char('+')), it.key());
        result.ok = true;
        result.summary = QStringLiteral("USB OK (%1 ports: %2)")
                             .arg(portCount)
                             .arg(parts.join(QStringLiteral(", ")));
        result.detail += QStringLiteral("Result: PASS (both USB ports have U盘)\n");
        return result;
    }

    result.ok = false;
    if (portCount == 1) {
        const QString onlyPort = sticksByPort.firstKey();
        result.summary = QStringLiteral("USB FAIL (only 1/%1 port)").arg(kRequiredUsbPorts);
        result.detail += QStringLiteral(
            "Only 1 USB port has a U盘 (%1 -> %2). "
            "Please plug U盘 into BOTH USB Host ports and retry.\n")
                             .arg(onlyPort, sticksByPort.value(onlyPort).join(QLatin1Char(',')));
    } else if (portCount == 0 && !nonRemovableUsbStorage.isEmpty()) {
        result.summary = QStringLiteral("USB FAIL (not a USB stick)");
        result.detail += QStringLiteral(
            "Found USB storage (%1) but removable!=1. Only USB flash drives (U盘) count.\n")
                             .arg(nonRemovableUsbStorage.join(QLatin1Char(',')));
    } else if (portCount == 0 && !peripherals.isEmpty()) {
        result.summary = QStringLiteral("USB FAIL (no USB stick)");
        result.detail += QStringLiteral(
            "USB peripherals found, but no Mass Storage U盘 / removable block on Host ports. "
            "Please plug U盘 into BOTH USB ports and retry.\n");
    } else {
        result.summary = QStringLiteral("USB FAIL (0/%1 port)").arg(kRequiredUsbPorts);
        result.detail += QStringLiteral(
            "No USB flash drive detected. Please plug U盘 into BOTH USB Host ports and retry.\n");
    }
    result.detail += QStringLiteral("Result: FAIL\n");
    return result;
#endif
}
