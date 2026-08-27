#ifndef RESPONSEPARSER_H
#define RESPONSEPARSER_H

#include "protocolframe.h"

#include <QString>
#include <QVector>

namespace Protocol {

struct VoltageReading
{
    quint8 faultFlag = 0;   // 0x00 正常, 0x01 异常
    double voltageV = 0.0;    // (hi<<8|lo)/100

    bool isNormal() const { return faultFlag == 0x00; }
};

struct VccCn52Reading
{
    VoltageReading vcc12v;
    VoltageReading vcc5v;
    VoltageReading vcc3v3;
};

class ResponseParser
{
public:
    static QString parse(const Frame &frame);

    static QString versionText(const Frame &frame);

    static bool parseVoltageReading(const QByteArray &info, int offset, VoltageReading &out);
    static bool parseVccCn52(const Frame &frame, VccCn52Reading &out);
    static bool parseSingleVoltage(const Frame &frame, VoltageReading &out);
    static bool parseStInputIo(const Frame &frame, quint8 &faultFlag);

    // 单行摘要，供主界面结果栏展示
    static QString summaryText(const Frame &frame);
    static QString formatVoltageSummary(const QString &name, const VoltageReading &r);

private:
    static QString parseVersion(const Frame &frame);
    static QString parseVccCn52Text(const Frame &frame);
    static QString parseSingleVoltageText(const Frame &frame, const QString &label);
    static QString parseStInputIoText(const Frame &frame);
    static QString faultText(quint8 faultFlag);
    static QString formatVoltageLine(const QString &name, const VoltageReading &r);
};

} // namespace Protocol

#endif // RESPONSEPARSER_H
