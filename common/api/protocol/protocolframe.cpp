#include "protocolframe.h"
#include "modbuscrc.h"

#include <QDebug>
#include <QObject>

namespace Protocol {

QByteArray ProtocolCodec::buildRequest(quint8 boardAddr, quint8 seq,
                                       quint8 cmd, const QByteArray &info) const
{
    const quint8 addr = makeAddress(boardAddr);
    QByteArray payload;
    payload.append(static_cast<char>(seq));
    payload.append(static_cast<char>(cmd));
    payload.append(static_cast<char>(kRespDown));
    payload.append(info);

    QByteArray frame;
    frame.append(static_cast<char>(addr));
    frame.append(static_cast<char>(kSign));

    const quint8 len = static_cast<quint8>(3 + payload.size() + 2);
    frame.append(static_cast<char>(len));
    frame.append(payload);

    const quint16 crc = modbusCrc16(frame);
    frame.append(static_cast<char>(crc & 0xFF));
    frame.append(static_cast<char>((crc >> 8) & 0xFF));
    return frame;
}

bool ProtocolCodec::tryParseFrame(const QByteArray &buffer, Frame &frame, int &consumedBytes) const
{
    consumedBytes = 0;
    if (buffer.size() < 5)
        return false;

    int start = 0;
    while (start < buffer.size()) {
        // 至少要有 addr + sign 才能判断帧头；末尾单字节不能访问 start+1
        if (start + 1 >= buffer.size()) {
            qDebug() << "[DEBUG] tryParseFrame: trailing byte, skip scan"
                     << "start=" << start << "bufferSize=" << buffer.size();
            break;
        }

        if (static_cast<quint8>(buffer.at(start + 1)) != kSign) {
            ++start;
            continue;
        }

        // 找到 sign 后还需 len 字段；数据不完整则等待更多字节
        if (start + 2 >= buffer.size()) {
            qDebug() << "[DEBUG] tryParseFrame: incomplete frame header, wait for more data"
                     << "start=" << start << "bufferSize=" << buffer.size();
            return false;
        }

        const quint8 len = static_cast<quint8>(buffer.at(start + 2));
        if (len < 5) {
            ++start;
            continue;
        }
        if (buffer.size() - start < len)
            return false;

        const QByteArray raw = buffer.mid(start, len);
        frame.raw = raw;
        frame.addr = static_cast<quint8>(raw.at(0));
        frame.sign = static_cast<quint8>(raw.at(1));
        frame.len = len;
        frame.crcValid = verifyModbusCrc16(raw);

        const QByteArray plain = raw.mid(3, len - 5);
        if (plain.size() < 3)
            return false;

        frame.seq  = static_cast<quint8>(plain.at(0));
        frame.cmd  = static_cast<quint8>(plain.at(1));
        frame.resp = static_cast<quint8>(plain.at(2));
        frame.info = plain.mid(3);

        if (!frame.crcValid) {
            consumedBytes = start + 1;
            return false;
        }

        consumedBytes = start + len;
        return true;
    }

    if (start > 0)
        consumedBytes = start;
    return false;
}

QString ProtocolCodec::frameToHex(const QByteArray &data)
{
    QString hex;
    hex.reserve(data.size() * 3);
    for (int i = 0; i < data.size(); ++i) {
        if (i > 0)
            hex.append(' ');
        hex.append(QString("%1").arg(static_cast<quint8>(data.at(i)), 2, 16, QChar('0')).toUpper());
    }
    return hex;
}

QString ProtocolCodec::respCodeText(quint8 resp)
{
    switch (resp) {
    case 0x00: return QObject::tr("OK");
    case 0x01: return QObject::tr("Unclassified error");
    case 0x02: return QObject::tr("Reserved");
    case 0x03: return QObject::tr("Out of range");
    case 0x04: return QObject::tr("Not supported");
    case 0x05: return QObject::tr("Data not ready");
    case 0xE0: return QObject::tr("CRC failed");
    default:   return QObject::tr("Unknown (0x%1)").arg(resp, 2, 16, QChar('0'));
    }
}

} // namespace Protocol
