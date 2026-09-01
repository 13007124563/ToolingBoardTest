#include "CanPortTester.h"

#include <QByteArray>
#include <QDebug>
#include <QElapsedTimer>
#include <QProcess>
#include <QRegularExpression>
#include <QThread>
#include <QtGlobal>

#ifdef Q_OS_LINUX
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#endif

namespace {

quint32 requestExtId(quint8 xx)
{
    return 0x00090010u | (static_cast<quint32>(xx) << 8); // 0x0009xx10
}

quint32 responseExtId(quint8 xx)
{
    return 0x00091000u | static_cast<quint32>(xx); // 0x000910xx
}

bool parseFirmwareVersion(const QByteArray &payload, QString *outVersion)
{
    if (payload.size() < 13)
        return false;

    QByteArray verBytes = payload.left(13);
    while (!verBytes.isEmpty()) {
        const unsigned char c = static_cast<unsigned char>(verBytes.at(verBytes.size() - 1));
        if (c >= 0x21 && c <= 0x7E)
            break;
        verBytes.chop(1);
    }

    const QString ver = QString::fromLatin1(verBytes);
    static const QRegularExpression re(QStringLiteral("^V[0-9A-Za-z._\\-]{3,}$"));
    if (!re.match(ver).hasMatch())
        return false;

    if (outVersion)
        *outVersion = ver;
    return true;
}

QString toHex(const quint8 *data, int len)
{
    return QString::fromLatin1(QByteArray(reinterpret_cast<const char *>(data), len).toHex(' '));
}

#ifdef Q_OS_LINUX
bool ensureCanInterfaceUp(const QString &ifName, int bitrate, QString *log)
{
    QProcess down;
    down.start(QStringLiteral("ip"),
               QStringList() << QStringLiteral("link") << QStringLiteral("set")
                             << ifName << QStringLiteral("down"));
    down.waitForFinished(3000);

    QProcess up;
    up.start(QStringLiteral("ip"),
             QStringList() << QStringLiteral("link") << QStringLiteral("set")
                           << ifName << QStringLiteral("up")
                           << QStringLiteral("type") << QStringLiteral("can")
                           << QStringLiteral("bitrate") << QString::number(bitrate));
    if (!up.waitForFinished(5000)
        || up.exitStatus() != QProcess::NormalExit
        || up.exitCode() != 0) {
        if (log) {
            *log += QStringLiteral("ip link set %1 up bitrate %2 failed\n")
                        .arg(ifName).arg(bitrate);
            const QString err = QString::fromLocal8Bit(up.readAllStandardError()).trimmed();
            if (!err.isEmpty())
                *log += err + QLatin1Char('\n');
        }
        return false;
    }

    QThread::msleep(200);
    if (log)
        *log += QStringLiteral("ip link set %1 up type can bitrate %2 OK\n")
                    .arg(ifName).arg(bitrate);
    return true;
}
#endif

} // namespace

CanVersionQueryResult CanPortTester::queryFirmwareVersion(const QString &ifName,
                                                          quint8 boardXx,
                                                          int timeoutMs,
                                                          int bitrate)
{
    CanVersionQueryResult result;
    qDebug() << "[CAN]" << "queryFirmwareVersion"
             << "ifName=" << ifName
             << "boardXx=0x" << QString::number(boardXx, 16)
             << "timeoutMs=" << timeoutMs
             << "bitrate=" << bitrate;

#if !defined(Q_OS_LINUX)
    Q_UNUSED(ifName);
    Q_UNUSED(boardXx);
    Q_UNUSED(timeoutMs);
    Q_UNUSED(bitrate);
    result.detail = QStringLiteral("CAN test is only supported on Linux");
    return result;
#else
    if (ifName.trimmed().isEmpty() || boardXx < 0x30 || boardXx > 0x60)
        return result;
    if (bitrate <= 0)
        bitrate = kDefaultBitrate;
    if (!ensureCanInterfaceUp(ifName, bitrate, &result.detail))
        return result;

    const quint32 reqId = requestExtId(boardXx);
    const quint32 rspId = responseExtId(boardXx);
    result.detail += QStringLiteral("IF=%1 xx=0x%2 req=0x%3 rsp=0x%4 bitrate=%5\n")
                         .arg(ifName)
                         .arg(boardXx, 2, 16, QLatin1Char('0'))
                         .arg(reqId, 8, 16, QLatin1Char('0'))
                         .arg(rspId, 8, 16, QLatin1Char('0'))
                         .arg(bitrate);

    const int sock = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        result.detail += QStringLiteral("socket failed: %1").arg(QString::fromLocal8Bit(::strerror(errno)));
        return result;
    }

    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::snprintf(ifr.ifr_name, IFNAMSIZ, "%s", ifName.toLatin1().constData());
    if (::ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
        result.detail += QStringLiteral("ioctl failed: %1").arg(QString::fromLocal8Bit(::strerror(errno)));
        ::close(sock);
        return result;
    }

    // 放宽过滤：匹配 0x000910xx（忽略低 8 位 xx）
    struct can_filter filter;
    filter.can_id = 0x00091000u | CAN_EFF_FLAG;
    filter.can_mask = CAN_EFF_FLAG | CAN_RTR_FLAG | 0x1FFFFF00u;
    (void)::setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, sizeof(filter));

    struct sockaddr_can addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (::bind(sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
        result.detail += QStringLiteral("bind failed: %1").arg(QString::fromLocal8Bit(::strerror(errno)));
        ::close(sock);
        return result;
    }

    {
        const int flags = ::fcntl(sock, F_GETFL, 0);
        ::fcntl(sock, F_SETFL, flags | O_NONBLOCK);
        struct can_frame discard;
        while (::read(sock, &discard, sizeof(discard)) > 0) {
        }
        ::fcntl(sock, F_SETFL, flags);
    }

    struct can_frame tx;
    std::memset(&tx, 0, sizeof(tx));
    tx.can_id = reqId | CAN_EFF_FLAG;
    tx.can_dlc = 8;
    if (::write(sock, &tx, sizeof(tx)) != static_cast<ssize_t>(sizeof(tx))) {
        result.detail += QStringLiteral("write failed: %1").arg(QString::fromLocal8Bit(::strerror(errno)));
        ::close(sock);
        return result;
    }
    result.detail += QStringLiteral("TX EXTID=0x%1 data=%2\n")
                         .arg(reqId, 8, 16, QLatin1Char('0'))
                         .arg(toHex(tx.data, 8));

    QByteArray payload;
    QElapsedTimer timer;
    timer.start();
    const int waitMs = qMax(500, timeoutMs);

    while (payload.size() < 16 && timer.elapsed() < waitMs) {
        const int remain = waitMs - static_cast<int>(timer.elapsed());
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);
        struct timeval tv = { remain / 1000, (remain % 1000) * 1000 };
        const int sel = ::select(sock + 1, &rfds, nullptr, nullptr, &tv);
        if (sel <= 0)
            break;

        struct can_frame rx;
        if (::read(sock, &rx, sizeof(rx)) != static_cast<ssize_t>(sizeof(rx)))
            continue;

        const quint32 id = rx.can_id & CAN_EFF_MASK;
        const int len = qBound(0, static_cast<int>(rx.can_dlc), 8);
        result.detail += QStringLiteral("RX EXTID=0x%1 len=%2 data=%3\n")
                             .arg(id, 8, 16, QLatin1Char('0'))
                             .arg(len)
                             .arg(toHex(rx.data, len));
        payload.append(reinterpret_cast<const char *>(rx.data), len);

        if (payload.size() >= 13) {
            QString version;
            if (parseFirmwareVersion(payload, &version)) {
                ::close(sock);
                result.ok = true;
                result.version = version;
                result.detail += QStringLiteral("Version OK: %1").arg(version);
                return result;
            }
        }
    }

    ::close(sock);
    result.detail += QStringLiteral("Timeout waiting for response (got %1 bytes)").arg(payload.size());
    return result;
#endif
}
