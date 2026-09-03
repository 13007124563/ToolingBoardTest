#include "UsbPortTester.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QProcess>
#include <QRegularExpression>
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
    // 根 Hub：usb1 / usb2 ...
    if (!name.startsWith(QStringLiteral("usb")))
        return false;
    bool ok = false;
    name.mid(3).toInt(&ok);
    return ok;
}

bool isUsbDeviceDirName(const QString &name)
{
    // 外设节点：1-1、1-1.2、2-3 等（含 '-'，排除 usbN）
    return name.contains(QLatin1Char('-')) && !name.contains(QLatin1Char(':'));
}

// 从块设备 canonical 路径提取板载物理口键：usbN/N-M（忽略下游 hub 的 .x）
QString usbPhysicalPortKey(const QString &resolvedPath)
{
    const QStringList parts = resolvedPath.split(QLatin1Char('/'),
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
                                                 Qt::SkipEmptyParts
#else
                                                 QString::SkipEmptyParts
#endif
    );
    static const QRegularExpression reHub(QStringLiteral("^usb\\d+$"));
    static const QRegularExpression rePort(QStringLiteral("^\\d+-\\d+"));
    for (int i = 0; i + 1 < parts.size(); ++i) {
        if (!reHub.match(parts.at(i)).hasMatch())
            continue;
        const QString &next = parts.at(i + 1);
        if (!rePort.match(next).hasMatch())
            continue;
        // 1-1.3 -> 1-1（同一物理口下的 hub 下游仍算该口）
        const QString rootPort = next.section(QLatin1Char('.'), 0, 0);
        return parts.at(i) + QLatin1Char('/') + rootPort;
    }
    return QString();
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

        QString line = QStringLiteral("%1 %2:%3").arg(name, vendor, product);
        if (!prodName.isEmpty())
            line += QStringLiteral(" %1").arg(prodName);
        else if (!manuf.isEmpty())
            line += QStringLiteral(" %1").arg(manuf);
        if (!devClass.isEmpty())
            line += QStringLiteral(" class=%1").arg(devClass);
        peripherals << line;
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

    // portKey -> block names（同一物理口上的 U 盘）
    QMap<QString, QStringList> sticksByPort;
    QStringList nonRemovableUsbStorage;
    QDir blockDir(QStringLiteral("/sys/block"));
    const QStringList blocks = blockDir.entryList(QStringList() << QStringLiteral("sd*"),
                                                  QDir::Dirs | QDir::NoDotAndDotDot);
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
        const QString portKey = usbPhysicalPortKey(resolved);
        result.detail += QStringLiteral(
                             "USB storage block: /dev/%1 removable=%2 port=%3 (%4)\n")
                             .arg(blk,
                                  removable.isEmpty() ? QStringLiteral("?") : removable,
                                  portKey.isEmpty() ? QStringLiteral("?") : portKey,
                                  resolved);

        if (removable != QStringLiteral("1")) {
            nonRemovableUsbStorage << blk;
            continue;
        }
        if (portKey.isEmpty()) {
            result.detail += QStringLiteral(
                "  skip /dev/%1: cannot map to a physical USB port\n").arg(blk);
            continue;
        }
        sticksByPort[portKey] << blk;
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
            "USB peripherals found, but no removable U盘 on the Host ports. "
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
