#include "BacklightTester.h"

#include <QApplication>
#include <QFile>
#include <QThread>
#include <QtGlobal>

const char *BacklightTester::kBrightnessPath =
    "/sys/class/backlight/lvds_backlight/brightness";

namespace {

bool readBrightness(int *value, QString *log)
{
    if (!value)
        return false;
    QFile f(QString::fromLatin1(BacklightTester::kBrightnessPath));
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (log) {
            *log += QStringLiteral("Cannot open %1 for read\n")
                        .arg(QString::fromLatin1(BacklightTester::kBrightnessPath));
        }
        return false;
    }
    bool ok = false;
    *value = QString::fromLatin1(f.readAll()).trimmed().toInt(&ok);
    return ok;
}

bool writeBrightness(int value, QString *log)
{
    QFile f(QString::fromLatin1(BacklightTester::kBrightnessPath));
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        if (log) {
            *log += QStringLiteral("Cannot open %1 for write\n")
                        .arg(QString::fromLatin1(BacklightTester::kBrightnessPath));
        }
        return false;
    }
    const QByteArray data = QByteArray::number(value);
    if (f.write(data) != data.size()) {
        if (log) {
            *log += QStringLiteral("Write brightness %1 failed\n").arg(value);
        }
        return false;
    }
    if (log) {
        *log += QStringLiteral("echo %1 > %2\n")
                    .arg(value)
                    .arg(QString::fromLatin1(BacklightTester::kBrightnessPath));
    }
    return true;
}

bool writeAndVerify(int value, QString *log)
{
    if (!writeBrightness(value, log))
        return false;

    int readBack = -1;
    if (!readBrightness(&readBack, log)) {
        if (log)
            *log += QStringLiteral("Readback failed after write %1\n").arg(value);
        return false;
    }
    if (readBack != value) {
        if (log) {
            *log += QStringLiteral("Readback mismatch: wrote %1 got %2\n")
                        .arg(value)
                        .arg(readBack);
        }
        return false;
    }
    if (log)
        *log += QStringLiteral("Readback OK: %1\n").arg(value);
    return true;
}

} // namespace

BacklightTestResult BacklightTester::runGradient()
{
    BacklightTestResult result;

#if !defined(Q_OS_LINUX)
    result.detail = QStringLiteral("Simulated backlight gradient 10→100 (Windows)\n");
    for (int v = kMinBrightness; v <= kMaxBrightness; ++v) {
        result.detail += QStringLiteral("echo %1 > %2\n").arg(v).arg(kBrightnessPath);
        QThread::msleep(kGradientDurationMs / (kMaxBrightness - kMinBrightness + 1));
        QApplication::processEvents();
    }
    result.ok = true;
    result.summary = QStringLiteral("Gradient 10→100 OK (simulated)");
    result.detail += QStringLiteral("Readback OK: 10\nReadback OK: 100\n");
    result.detail += QStringLiteral("Result: PASS\n");
    return result;
#else
    int originalBrightness = -1;
    readBrightness(&originalBrightness, &result.detail);

    result.detail += QStringLiteral("Gradient %1→%2 in %3ms\n")
                         .arg(kMinBrightness)
                         .arg(kMaxBrightness)
                         .arg(kGradientDurationMs);

    const int stepCount = kMaxBrightness - kMinBrightness;
    const int intervalMs = stepCount > 0 ? kGradientDurationMs / stepCount : kGradientDurationMs;

    for (int v = kMinBrightness; v <= kMaxBrightness; ++v) {
        const bool verifyEndpoints = (v == kMinBrightness || v == kMaxBrightness);
        const bool stepOk = verifyEndpoints
            ? writeAndVerify(v, &result.detail)
            : writeBrightness(v, &result.detail);
        if (!stepOk) {
            result.summary = QStringLiteral("Backlight FAIL");
            result.detail += QStringLiteral("Result: FAIL\n");
            if (originalBrightness >= 0)
                writeBrightness(originalBrightness, nullptr);
            return result;
        }
        if (v < kMaxBrightness && intervalMs > 0) {
            QThread::msleep(intervalMs);
            QApplication::processEvents();
        }
    }

    if (originalBrightness >= 0 && originalBrightness != kMaxBrightness) {
        writeBrightness(originalBrightness, &result.detail);
        result.detail += QStringLiteral("Restored brightness to %1\n").arg(originalBrightness);
    }

    result.ok = true;
    result.summary = QStringLiteral("Gradient 10→100 OK");
    result.detail += QStringLiteral("Result: PASS\n");
    return result;
#endif
}
