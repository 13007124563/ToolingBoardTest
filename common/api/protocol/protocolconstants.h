#ifndef PROTOCOLCONSTANTS_H
#define PROTOCOLCONSTANTS_H

#include <QtGlobal>
#include <QByteArray>

namespace Protocol {

static const quint8 kSign       = 0x7F;
static const quint8 kRespDown   = 0xFF;
static const quint8 kRespUpOk   = 0x00;
static const int    kDefaultTimeoutMs = 500;
static const int    kDefaultBaudRate  = 9600;
static const int    kMaxCompartment   = 22;

enum Command : quint8 {
    CmdQueryVersion      = 0x01,
    CmdQueryCompStatus   = 0x04,
    CmdCompLight         = 0x08,
    CmdQueryUnlockTime   = 0x11,
    CmdQueryUnlockDelay  = 0x13,
    CmdDoorLock          = 0x1E
};

inline quint8 makeAddress(quint8 boardAddr)
{
    return static_cast<quint8>(boardAddr & 0x7F);
}

inline quint8 boardFromAddress(quint8 addr)
{
    return static_cast<quint8>(addr & 0x7F);
}

} // namespace Protocol

#endif // PROTOCOLCONSTANTS_H