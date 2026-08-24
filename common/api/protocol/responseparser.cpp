#include "responseparser.h"

#include <QObject>

namespace Protocol {

bool ResponseParser::s_doorMagneticInverted = false;

void ResponseParser::setDoorMagneticInverted(bool inverted)
{
    s_doorMagneticInverted = inverted;
}

bool ResponseParser::doorMagneticInverted()
{
    return s_doorMagneticInverted;
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

    if (frame.resp != kRespUpOk) {
        return header;
    }

    switch (frame.cmd) {
    case CmdQueryVersion:     return header + "\n" + parseVersion(frame);
    case CmdQueryCompStatus:  return header + "\n" + parseCompartmentStatus(frame);
    case CmdCompLight:        return header + "\n" + parseSimpleAck(frame, QObject::tr("Compartment light"));
    case CmdQueryUnlockTime:  return header + "\n" + parseUnlockTime(frame);
    case CmdQueryUnlockDelay: return header + "\n" + parseUnlockDelay(frame);
    case CmdDoorLock:         return header + "\n" + parseDoorLock(frame);
    default:
        return header + "\n" + QObject::tr("INFO: %1")
                            .arg(ProtocolCodec::frameToHex(frame.info));
    }
}

QString ResponseParser::parseVersion(const Frame &frame)
{
    if (frame.info.size() < 2)
        return QObject::tr("Invalid version payload");

    const quint8 data1 = static_cast<quint8>(frame.info.at(0));
    const quint8 minor = static_cast<quint8>(frame.info.at(1));
    const quint8 major = static_cast<quint8>(data1 & 0x0F);

    return QObject::tr("Protocol version: V%1.%2").arg(major).arg(minor);
}

QString ResponseParser::parseCompartmentStatus(const Frame &frame)
{
    if (frame.info.size() < 3)
        return QObject::tr("Invalid compartment status payload");

    const quint8 status = static_cast<quint8>(frame.info.at(1));
    const quint8 fault  = static_cast<quint8>(frame.info.at(2));

    return QObject::tr("Status: %1\nFault: %2")
        .arg(formatStatusBits(status))
        .arg(formatFaultBits(fault));
}

QString ResponseParser::parseSimpleAck(const Frame &frame, const QString &title)
{
    Q_UNUSED(frame);
    return QObject::tr("%1 command accepted").arg(title);
}

QString ResponseParser::parseUnlockTime(const Frame &frame)
{
    if (frame.info.isEmpty())
        return QObject::tr("No unlock time data");

    if (frame.info.size() == 2) {
        const quint8 data1 = static_cast<quint8>(frame.info.at(0));
        const quint8 data2 = static_cast<quint8>(frame.info.at(1));
        const int result = (data1 >> 4) & 0x0F;
        const int high = data1 & 0x0F;
        const int timeMs = (high << 8) | data2;
        return QObject::tr("Unlock time: %1 ms\nResult: %2")
            .arg(timeMs)
            .arg(formatUnlockResult(result));
    }

    QStringList lines;
    const int count = frame.info.size() / 2;
    for (int i = 0; i < count; ++i) {
        const quint8 data1 = static_cast<quint8>(frame.info.at(i * 2));
        const quint8 data2 = static_cast<quint8>(frame.info.at(i * 2 + 1));
        const int result = (data1 >> 4) & 0x0F;
        const int high = data1 & 0x0F;
        const int timeMs = (high << 8) | data2;
        lines << QObject::tr("Compartment %1: %2 ms (%3)")
                     .arg(i + 1)
                     .arg(timeMs)
                     .arg(formatUnlockResult(result));
    }
    return lines.join('\n');
}

QString ResponseParser::parseUnlockDelay(const Frame &frame)
{
    if (frame.info.isEmpty())
        return QObject::tr("No unlock delay data");

    if (frame.info.size() == 2) {
        const quint8 data1 = static_cast<quint8>(frame.info.at(0));
        const quint8 data2 = static_cast<quint8>(frame.info.at(1));
        const int result = (data1 >> 4) & 0x0F;
        const int high = data1 & 0x0F;
        const int delayMs = (high << 8) | data2;
        return QObject::tr("Unlock comm delay: %1 ms\nResult: %2")
            .arg(delayMs)
            .arg(formatUnlockResult(result));
    }

    QStringList lines;
    const int count = frame.info.size() / 2;
    for (int i = 0; i < count; ++i) {
        const quint8 data1 = static_cast<quint8>(frame.info.at(i * 2));
        const quint8 data2 = static_cast<quint8>(frame.info.at(i * 2 + 1));
        const int result = (data1 >> 4) & 0x0F;
        const int high = data1 & 0x0F;
        const int delayMs = (high << 8) | data2;
        lines << QObject::tr("Compartment %1: %2 ms (%3)")
                     .arg(i + 1)
                     .arg(delayMs)
                     .arg(formatUnlockResult(result));
    }
    return lines.join('\n');
}

QString ResponseParser::parseDoorLock(const Frame &frame)
{
    if (frame.resp == 0x01 && !frame.info.isEmpty()
        && static_cast<quint8>(frame.info.at(0)) == 1) {
        return QObject::tr("Request ID duplicated, door not opened again");
    }
    if (frame.resp == kRespUpOk) {
        if (!frame.info.isEmpty() && static_cast<quint8>(frame.info.at(0)) == 1)
            return QObject::tr("Request ID duplicated, door not opened again");
        return QObject::tr("Door lock command accepted");
    }
    return QString();
}

QString ResponseParser::formatStatusBits(quint8 flags)
{
    QStringList parts;
    const bool doorOpen = (flags & 0x01) != 0;
    const bool showOpen = s_doorMagneticInverted ? !doorOpen : doorOpen;
    parts << (showOpen ? QObject::tr("Door open") : QObject::tr("Door closed"));
    parts << (flags & 0x02 ? QObject::tr("Light on") : QObject::tr("Light off"));
    if (flags & 0x40)
        parts << QObject::tr("Door opened before");
    return parts.join(QStringLiteral(", "));
}

QString ResponseParser::formatFaultBits(quint8 flags)
{
    if (flags == 0)
        return QObject::tr("None");

    QStringList parts;
    if (flags & 0x01)
        parts << QObject::tr("Door lock fault");
    if (flags & 0x02)
        parts << QObject::tr("Light fault");
    return parts.join(", ");
}

QString ResponseParser::formatUnlockResult(int code)
{
    switch (code) {
    case 0:  return QObject::tr("Normal");
    case 1:  return QObject::tr("Door not closed");
    case 14: return QObject::tr("Test not started");
    case 15: return QObject::tr("Test not finished");
    default:
        if (code >= 2 && code <= 7)
            return QObject::tr("Invalid or open failed");
        return QObject::tr("Code %1").arg(code);
    }
}

} // namespace Protocol