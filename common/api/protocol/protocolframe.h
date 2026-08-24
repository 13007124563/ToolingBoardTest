#ifndef PROTOCOLFRAME_H
#define PROTOCOLFRAME_H

#include "protocolconstants.h"

#include <QByteArray>
#include <QString>

namespace Protocol {

struct Frame
{
    quint8 addr = 0;
    quint8 sign = kSign;
    quint8 len  = 0;
    quint8 seq  = 0;
    quint8 cmd  = 0;
    quint8 resp = 0;
    QByteArray info;

    bool crcValid = false;
    QByteArray raw;
};

class ProtocolCodec
{
public:
    QByteArray buildRequest(quint8 boardAddr, quint8 seq,
                            quint8 cmd, const QByteArray &info = QByteArray()) const;

    bool tryParseFrame(const QByteArray &buffer, Frame &frame, int &consumedBytes) const;
    static QString frameToHex(const QByteArray &data);
    static QString respCodeText(quint8 resp);
};

} // namespace Protocol

#endif // PROTOCOLFRAME_H
