#include "responseparser.h"

#include <QObject>

namespace Protocol {

QString ResponseParser::faultText(quint8 faultFlag)
{
    return faultFlag == 0x00 ? QObject::tr("OK") : QObject::tr("Fault");
}

QString ResponseParser::formatVoltageLine(const QString &name, const VoltageReading &r)
{
    return QObject::tr("%1: %2 V (%3)")
        .arg(name)
        .arg(r.voltageV, 0, 'f', 2)
        .arg(faultText(r.faultFlag));
}

QString ResponseParser::formatVoltageSummary(const QString &name, const VoltageReading &r)
{
    return QObject::tr("%1: %2V (%3)")
        .arg(name)
        .arg(r.voltageV, 0, 'f', 2)
        .arg(faultText(r.faultFlag));
}

bool ResponseParser::parseVoltageReading(const QByteArray &info, int offset, VoltageReading &out)
{
    if (offset + 3 > info.size())
        return false;

    out.faultFlag = static_cast<quint8>(info.at(offset));
    const quint8 hi = static_cast<quint8>(info.at(offset + 1));
    const quint8 lo = static_cast<quint8>(info.at(offset + 2));
    out.voltageV = ((hi << 8) | lo) / 100.0;
    return true;
}

bool ResponseParser::parseVccCn52(const Frame &frame, VccCn52Reading &out)
{
    if (frame.info.size() < 9)
        return false;

    return parseVoltageReading(frame.info, 0, out.vcc12v)
        && parseVoltageReading(frame.info, 3, out.vcc5v)
        && parseVoltageReading(frame.info, 6, out.vcc3v3);
}

bool ResponseParser::parseSingleVoltage(const Frame &frame, VoltageReading &out)
{
    return parseVoltageReading(frame.info, 0, out);
}

bool ResponseParser::parseStInputIo(const Frame &frame, quint8 &faultFlag)
{
    if (frame.info.isEmpty())
        return false;

    faultFlag = static_cast<quint8>(frame.info.at(0));
    return true;
}

QString ResponseParser::parse(const Frame &frame)
{
    if (!frame.crcValid)
        return QObject::tr("CRC verification failed");

    QString header = QObject::tr("CMD 0x%1 | RESP: %2 (%3) | SEQ: %4")
                         .arg(frame.cmd, 2, 16, QChar('0'))
                         .arg(frame.resp, 2, 16, QChar('0'))
                         .arg(ProtocolCodec::respCodeText(frame.resp))
                         .arg(frame.seq);

    if (frame.resp != kRespUpOk)
        return header;

    switch (frame.cmd) {
    case CmdQueryVersion:
        return header + "\n" + parseVersion(frame);
    case CmdVccCn52Test:
        return header + "\n" + parseVccCn52Text(frame);
    case CmdPrinterCn43Test:
        return header + "\n" + parseSingleVoltageText(frame, QObject::tr("Printer power (CN43)"));
    case CmdVout5vCn39Test:
        return header + "\n" + parseSingleVoltageText(frame, QObject::tr("5V control output (CN39)"));
    case CmdVout12vCn47Test:
        return header + "\n" + parseSingleVoltageText(frame, QObject::tr("12V controlled output (CN47)"));
    case CmdProximityCn13Test:
        return header + "\n" + parseSingleVoltageText(frame, QObject::tr("5V proximity switch (CN13)"));
    case CmdStInputIoTest:
        return header + "\n" + parseStInputIoText(frame);
    default:
        return header + "\n" + QObject::tr("INFO: %1")
                            .arg(ProtocolCodec::frameToHex(frame.info));
    }
}

QString ResponseParser::versionText(const Frame &frame)
{
    if (frame.info.isEmpty())
        return QString();

    for (char ch : frame.info) {
        const auto u = static_cast<unsigned char>(ch);
        if (u < 0x20 || u > 0x7E)
            return QString();
    }

    return QString::fromLatin1(frame.info.constData(), frame.info.size()).trimmed();
}

QString ResponseParser::parseVersion(const Frame &frame)
{
    const QString text = versionText(frame);
    if (text.isEmpty())
        return QObject::tr("Invalid version payload");

    return QObject::tr("Test board version: %1").arg(text);
}

QString ResponseParser::parseVccCn52Text(const Frame &frame)
{
    VccCn52Reading reading;
    if (!parseVccCn52(frame, reading))
        return QObject::tr("Invalid VCC CN52 payload (expected 9 bytes)");

    QStringList lines;
    lines << formatVoltageLine(QObject::tr("VCC_12V (CN52-19)"), reading.vcc12v);
    lines << formatVoltageLine(QObject::tr("VCC_5V (CN52-18)"), reading.vcc5v);
    lines << formatVoltageLine(QObject::tr("VCC_3.3V (CN52-1)"), reading.vcc3v3);
    return lines.join("\n");
}

QString ResponseParser::parseSingleVoltageText(const Frame &frame, const QString &label)
{
    VoltageReading reading;
    if (!parseSingleVoltage(frame, reading))
        return QObject::tr("Invalid voltage payload (expected 3 bytes)");

    return formatVoltageLine(label, reading);
}

QString ResponseParser::parseStInputIoText(const Frame &frame)
{
    quint8 faultFlag = 0;
    if (!parseStInputIo(frame, faultFlag))
        return QObject::tr("Invalid ST_INPUT IO payload (expected 1 byte)");

    return QObject::tr("ST_INPUT1/2 IO (CN45/CN13): %1").arg(faultText(faultFlag));
}

QString ResponseParser::summaryText(const Frame &frame)
{
    if (frame.resp != kRespUpOk)
        return ProtocolCodec::respCodeText(frame.resp);

    switch (frame.cmd) {
    case CmdQueryVersion:
        return versionText(frame);
    case CmdVccCn52Test: {
        VccCn52Reading reading;
        if (!parseVccCn52(frame, reading))
            return QString();
        return QStringList{
            formatVoltageSummary(QObject::tr("12V"), reading.vcc12v),
            formatVoltageSummary(QObject::tr("5V"), reading.vcc5v),
            formatVoltageSummary(QObject::tr("3.3V"), reading.vcc3v3),
        }.join(QStringLiteral(" | "));
    }
    case CmdPrinterCn43Test: {
        VoltageReading reading;
        if (!parseSingleVoltage(frame, reading))
            return QString();
        return formatVoltageSummary(QObject::tr("CN43"), reading);
    }
    case CmdVout5vCn39Test: {
        VoltageReading reading;
        if (!parseSingleVoltage(frame, reading))
            return QString();
        return formatVoltageSummary(QObject::tr("CN39"), reading);
    }
    case CmdVout12vCn47Test: {
        VoltageReading reading;
        if (!parseSingleVoltage(frame, reading))
            return QString();
        return formatVoltageSummary(QObject::tr("CN47"), reading);
    }
    case CmdProximityCn13Test: {
        VoltageReading reading;
        if (!parseSingleVoltage(frame, reading))
            return QString();
        return formatVoltageSummary(QObject::tr("CN13"), reading);
    }
    case CmdStInputIoTest: {
        quint8 faultFlag = 0;
        if (!parseStInputIo(frame, faultFlag))
            return QString();
        return faultText(faultFlag);
    }
    default:
        return ProtocolCodec::frameToHex(frame.info);
    }
}

} // namespace Protocol
