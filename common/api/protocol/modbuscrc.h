#ifndef MODBUSCRC_H
#define MODBUSCRC_H

#include <QtGlobal>
#include <QByteArray>

namespace Protocol {

quint16 modbusCrc16(const quint8 *data, int size, quint16 init = 0xFFFF);
quint16 modbusCrc16(const QByteArray &data, quint16 init = 0xFFFF);
bool verifyModbusCrc16(const QByteArray &frame);

} // namespace Protocol

#endif // MODBUSCRC_H
