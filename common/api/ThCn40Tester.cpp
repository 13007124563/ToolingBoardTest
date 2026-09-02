#include "ThCn40Tester.h"

#include <QProcess>
#include <QRegularExpression>
#include <QtGlobal>

namespace {

bool parseDouble(const QString &text, double *out)
{
    if (!out)
        return false;
    bool ok = false;
    const double v = text.trimmed().toDouble(&ok);
    if (!ok)
        return false;
    *out = v;
    return true;
}

bool extractTempHumidity(const QString &text, double *tempC, double *humiPct, QString *log)
{
    if (!tempC || !humiPct)
        return false;

    // stm32_i2c_test 0x4 实际输出：humi:25.3%RH temp:26.1℃
    static const QRegularExpression reStm32(
        QStringLiteral("humi\\s*:\\s*(-?\\d+(?:\\.\\d+)?)\\s*%RH\\s+temp\\s*:\\s*(-?\\d+(?:\\.\\d+)?)\\s*[℃°C]"),
        QRegularExpression::CaseInsensitiveOption);
    const QRegularExpressionMatch mStm32 = reStm32.match(text);
    if (mStm32.hasMatch()
        && parseDouble(mStm32.captured(1), humiPct)
        && parseDouble(mStm32.captured(2), tempC)) {
        if (log)
            *log += QStringLiteral("Parsed stm32_i2c_test: H=%1% T=%2°C\n")
                        .arg(*humiPct, 0, 'f', 1)
                        .arg(*tempC, 0, 'f', 1);
        return true;
    }

    // 兜底：从 Data 行解析 7 字节响应 07 04 HH HL TT TL CRC
    static const QRegularExpression reHexData(
        QStringLiteral("Data(?:\\s*\\(Hex\\))?:\\s*"
                       "([0-9A-Fa-f]{2})\\s+([0-9A-Fa-f]{2})\\s+"
                       "([0-9A-Fa-f]{2})\\s+([0-9A-Fa-f]{2})\\s+"
                       "([0-9A-Fa-f]{2})\\s+([0-9A-Fa-f]{2})\\s+([0-9A-Fa-f]{2})"));
    const QRegularExpressionMatch mHex = reHexData.match(text);
    if (mHex.hasMatch()) {
        bool okH = false;
        bool okT = false;
        const int rawH = (mHex.captured(3).toInt(&okH, 16) << 8) | mHex.captured(4).toInt(&okH, 16);
        const int rawT = (mHex.captured(5).toInt(&okT, 16) << 8) | mHex.captured(6).toInt(&okT, 16);
        if (!okH || !okT)
            return false;
        *humiPct = rawH / 10.0;
        *tempC = rawT / 10.0;
        if (log) {
            *log += QStringLiteral("Parsed hex Data: rawH=0x%1 rawT=0x%2 -> H=%3% T=%4°C\n")
                        .arg(rawH, 4, 16, QChar('0'))
                        .arg(rawT, 4, 16, QChar('0'))
                        .arg(*humiPct, 0, 'f', 1)
                        .arg(*tempC, 0, 'f', 1);
        }
        return true;
    }

    return false;
}

bool isValueInRange(double tempC, double humiPct)
{
    return tempC >= -40.0 && tempC <= 125.0
        && humiPct >= 0.0 && humiPct <= 100.0;
}

} // namespace

ThCn40TestResult ThCn40Tester::readSensor()
{
    ThCn40TestResult result;

#if !defined(Q_OS_LINUX)
    result.ok = true;
    result.temperatureC = 25.0;
    result.humidityPct = 50.0;
    result.summary = QStringLiteral("T:25.0°C H:50.0%");
    result.detail = QStringLiteral("Simulated CN40 temp/humidity (Windows)\n");
    return result;
#else
    const QString cmdLine = QStringLiteral("./stm32_i2c_test 0x%1")
                                .arg(kI2cCmd, 2, 16, QChar('0'));
    result.detail += QStringLiteral("$ /etc/zl_test/%1\n").arg(cmdLine);

    QProcess process;
    process.setWorkingDirectory(QStringLiteral("/etc/zl_test"));
    process.start(QStringLiteral("/etc/zl_test/stm32_i2c_test"),
                  QStringList() << QStringLiteral("0x%1").arg(kI2cCmd, 0, 16));
    if (!process.waitForFinished(10000)) {
        process.kill();
        result.summary = QStringLiteral("CN40 FAIL (timeout)");
        result.detail += QStringLiteral("Command timeout\n");
        return result;
    }

    const QString out = QString::fromUtf8(process.readAllStandardOutput());
    const QString err = QString::fromUtf8(process.readAllStandardError());
    const QString combined = out + err;
    if (!combined.trimmed().isEmpty())
        result.detail += combined.trimmed() + QLatin1Char('\n');

    if (process.exitCode() != 0) {
        result.summary = QStringLiteral("CN40 FAIL (cmd error)");
        result.detail += QStringLiteral("Exit code: %1\n").arg(process.exitCode());
        return result;
    }

    if (!combined.contains(QStringLiteral("Test completed successfully"))) {
        result.summary = QStringLiteral("CN40 FAIL (incomplete)");
        result.detail += QStringLiteral("Missing 'Test completed successfully!'\n");
        return result;
    }

    double tempC = 0.0;
    double humiPct = 0.0;
    if (!extractTempHumidity(combined, &tempC, &humiPct, &result.detail)) {
        result.summary = QStringLiteral("CN40 FAIL (parse error)");
        result.detail += QStringLiteral(
            "Cannot parse temperature/humidity from output. "
            "Expected line like: humi:25.3%RH temp:26.1℃\n");
        return result;
    }

    result.temperatureC = tempC;
    result.humidityPct = humiPct;

    if (!isValueInRange(tempC, humiPct)) {
        result.summary = QStringLiteral("CN40 FAIL (out of range)");
        result.detail += QStringLiteral("Value out of range: T=%1°C H=%2%%\n")
                             .arg(tempC, 0, 'f', 1)
                             .arg(humiPct, 0, 'f', 1);
        return result;
    }

    result.ok = true;
    result.summary = QStringLiteral("T:%1°C H:%2%")
                         .arg(tempC, 0, 'f', 1)
                         .arg(humiPct, 0, 'f', 1);
    result.detail += QStringLiteral("Result: PASS\n");
    return result;
#endif
}
