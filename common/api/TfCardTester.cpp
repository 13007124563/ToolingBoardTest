#include "TfCardTester.h"

#include <QDir>
#include <QFile>
#include <QtGlobal>

namespace {

QString readSysfsTrimmed(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    return QString::fromLatin1(f.readAll()).trimmed();
}

} // namespace

TfCardTestResult TfCardTester::testCard()
{
    TfCardTestResult result;

#if !defined(Q_OS_LINUX)
    result.detail = QStringLiteral("TF card test is only supported on Linux");
    return result;
#else
    const QString blockRoot = QStringLiteral("/sys/block");
    QDir blockDir(blockRoot);
    if (!blockDir.exists()) {
        result.summary = QStringLiteral("TF FAIL (no /sys/block)");
        result.detail = QStringLiteral("Missing %1\n").arg(blockRoot);
        return result;
    }

    const QStringList blocks = blockDir.entryList(QStringList() << QStringLiteral("mmcblk*"),
                                                  QDir::Dirs | QDir::NoDotAndDotDot);
    result.detail += QStringLiteral("Scan %1/mmcblk*\n").arg(blockRoot);

    QStringList sdDevs;
    QStringList mmcDevs; // eMMC 等，仅记录不计入通过

    for (const QString &blk : blocks) {
        // 跳过分区目录名（正常 entryList 的 mmcblk* 不会列出 mmcblk0p1，但防御一下）
        if (blk.contains(QLatin1Char('p')) && blk.indexOf(QLatin1Char('p')) > 6)
            continue;

        const QString base = blockRoot + QLatin1Char('/') + blk;
        const QString typePath = base + QStringLiteral("/device/type");
        QString type = readSysfsTrimmed(typePath);

        // 部分内核 type 在 device 上一层或通过 uevent
        if (type.isEmpty()) {
            const QString uevent = readSysfsTrimmed(base + QStringLiteral("/device/uevent"));
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
            const QStringList lines = uevent.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
#else
            const QStringList lines = uevent.split(QLatin1Char('\n'), QString::SkipEmptyParts);
#endif
            for (const QString &line : lines) {
                if (line.startsWith(QStringLiteral("MMC_TYPE="))) {
                    type = line.mid(9).trimmed();
                    break;
                }
            }
        }

        const QString removable = readSysfsTrimmed(base + QStringLiteral("/removable"));
        const QString sizeSectors = readSysfsTrimmed(base + QStringLiteral("/size"));
        qint64 sizeBytes = 0;
        bool sizeOk = false;
        const qint64 sectors = sizeSectors.toLongLong(&sizeOk);
        if (sizeOk && sectors > 0)
            sizeBytes = sectors * 512;

        result.detail += QStringLiteral("  %1 type=%2 removable=%3 size=%4 bytes\n")
                             .arg(blk,
                                  type.isEmpty() ? QStringLiteral("?") : type,
                                  removable.isEmpty() ? QStringLiteral("?") : removable)
                             .arg(sizeBytes);

        const QString typeUpper = type.toUpper();
        if (typeUpper == QStringLiteral("SD") || typeUpper == QStringLiteral("SDIO")
            || typeUpper == QStringLiteral("SD_COMBO")) {
            // 无容量则可能未真正插入
            if (sizeOk && sectors > 0) {
                sdDevs << blk;
            } else {
                result.detail += QStringLiteral("    skip %1: SD type but size=0 (empty slot?)\n")
                                     .arg(blk);
            }
        } else if (typeUpper == QStringLiteral("MMC")) {
            mmcDevs << blk;
        } else if (type.isEmpty() && removable == QStringLiteral("1") && sizeOk && sectors > 0) {
            // 兜底：无 type 但标记可移除且有容量，按 TF 候选
            sdDevs << blk;
            result.detail += QStringLiteral("    treat %1 as SD candidate (removable=1)\n").arg(blk);
        }
    }

    if (sdDevs.isEmpty() && mmcDevs.isEmpty() && blocks.isEmpty()) {
        result.summary = QStringLiteral("TF FAIL (no mmcblk)");
        result.detail += QStringLiteral("No mmcblk devices. Insert TF card and retry.\n");
        result.detail += QStringLiteral("Result: FAIL\n");
        return result;
    }

    if (sdDevs.isEmpty()) {
        result.summary = QStringLiteral("TF FAIL (no SD card)");
        if (!mmcDevs.isEmpty()) {
            result.detail += QStringLiteral("Found eMMC/MMC only: %1 (not TF)\n")
                                 .arg(mmcDevs.join(QLatin1Char(',')));
        }
        result.detail += QStringLiteral("No SD-type block device. Insert TF card and retry.\n");
        result.detail += QStringLiteral("Result: FAIL\n");
        return result;
    }

    result.ok = true;
    result.summary = QStringLiteral("TF OK (%1)").arg(sdDevs.join(QLatin1Char(',')));
    result.detail += QStringLiteral("Result: PASS\n");
    return result;
#endif
}
