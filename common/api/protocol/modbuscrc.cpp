#include "modbuscrc.h"

namespace Protocol {

quint16 modbusCrc16(const quint8 *data, int size, quint16 init)
{
    quint16 crc16 = init;
    for (int i = 0; i < size; ++i) {
        crc16 ^= data[i];
        for (int j = 0; j < 8; ++j) {
            if (crc16 & 0x01)
                crc16 = (crc16 >> 1) ^ 0xA001;
            else
                crc16 >>= 1;
        }
    }
    return crc16;
}

quint16 modbusCrc16(const QByteArray &data, quint16 init)
{
    return modbusCrc16(reinterpret_cast<const quint8 *>(data.constData()), data.size(), init);
}

bool verifyModbusCrc16(const QByteArray &frame)
{
    if (frame.size() < 3)
        return false;
    const int payloadLen = static_cast<quint8>(frame.at(2));
    if (frame.size() < payloadLen)
        return false;
    const QByteArray body = frame.left(payloadLen - 2);
    const quint16 expected = static_cast<quint8>(frame.at(payloadLen - 2))
                           | (static_cast<quint16>(static_cast<quint8>(frame.at(payloadLen - 1))) << 8);
    return modbusCrc16(body) == expected;
}

} // namespace Protocol
