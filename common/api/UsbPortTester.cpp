#include "UsbPortTester.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
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

        // 跳过没有 vendor 的异常节点
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
    result.detail += QStringLiteral("Peripherals: %1\n").arg(peripherals.size());
    for (const QString &p : peripherals)
        result.detail += QStringLiteral("  %1\n").arg(p);

    if (rootHubs.isEmpty()) {
        result.summary = QStringLiteral("USB FAIL (no host controller)");
        result.detail += QStringLiteral("No USB root hub found\n");
        return result;
    }

    // 仅识别 U 盘：USB 总线下的块设备，且 removable=1
    QStringList usbSticks;
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
        result.detail += QStringLiteral("USB storage block: /dev/%1 removable=%2 (%3)\n")
                             .arg(blk, removable.isEmpty() ? QStringLiteral("?") : removable, resolved);

        if (removable == QStringLiteral("1"))
            usbSticks << blk;
        else
            nonRemovableUsbStorage << blk;
    }
    if (usbSticks.isEmpty() && nonRemovableUsbStorage.isEmpty())
        result.detail += QStringLiteral("USB storage block: (none)\n");

    // 可选：追加 lsusb 输出便于现场对照
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

    if (usbSticks.isEmpty()) {
        result.ok = false;
        if (!nonRemovableUsbStorage.isEmpty()) {
            result.summary = QStringLiteral("USB FAIL (not a USB stick)");
            result.detail += QStringLiteral(
                "Found USB storage (%1) but removable!=1 (likely USB HDD/other). "
                "Only USB flash drive (U盘) counts as PASS.\n")
                                 .arg(nonRemovableUsbStorage.join(QLatin1Char(',')));
        } else if (!peripherals.isEmpty()) {
            result.summary = QStringLiteral("USB FAIL (no USB stick)");
            result.detail += QStringLiteral(
                "USB peripheral enumerated, but no removable USB mass-storage (/dev/sd* removable=1). "
                "Please plug a USB flash drive (U盘) and retry.\n");
        } else {
            result.summary = QStringLiteral("USB FAIL (no USB stick)");
            result.detail += QStringLiteral(
                "No USB flash drive detected. Please plug a U盘 into the USB Host port and retry.\n");
        }
        result.detail += QStringLiteral("Result: FAIL\n");
        return result;
    }

    result.ok = true;
    result.summary = QStringLiteral("USB OK (U盘 %1)").arg(usbSticks.join(QLatin1Char(',')));
    result.detail += QStringLiteral("Result: PASS (USB stick detected)\n");
    return result;
#endif
}
