#ifndef RESPONSEPARSER_H
#define RESPONSEPARSER_H

#include "protocolframe.h"

#include <QString>
#include <QVector>

namespace Protocol {

struct VersionInfo {
    quint8 major = 0;
    quint8 minor = 0;
};

struct CompartmentStatus {
    quint8 statusFlags = 0;
    quint8 faultFlags = 0;
};

struct UnlockTimeEntry {
    int compartment = 0;
    int resultCode = 0;
    int timeMs = 0;
};

struct UnlockDelayEntry {
    int compartment = 0;
    int resultCode = 0;
    int delayMs = 0;
};

class ResponseParser
{
public:
    static QString parse(const Frame &frame);

    static void setDoorMagneticInverted(bool inverted);
    static bool doorMagneticInverted();

private:
    static bool s_doorMagneticInverted;
    static QString parseVersion(const Frame &frame);
    static QString parseCompartmentStatus(const Frame &frame);
    static QString parseSimpleAck(const Frame &frame, const QString &title);
    static QString parseUnlockTime(const Frame &frame);
    static QString parseUnlockDelay(const Frame &frame);
    static QString parseDoorLock(const Frame &frame);

    static QString formatStatusBits(quint8 flags);
    static QString formatFaultBits(quint8 flags);
    static QString formatUnlockResult(int code);
};

} // namespace Protocol

#endif // RESPONSEPARSER_H