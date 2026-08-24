#include "responseparser.h"

#include <QObject>

namespace Protocol {

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
    default:
        return header + "\n" + QObject::tr("INFO: %1")
                            .arg(ProtocolCodec::frameToHex(frame.info));
    }
}

QString ResponseParser::versionText(const Frame &frame)
{
    if (frame.info.isEmpty())
        return QString();

    // 测试治具固件：INFO 为 ASCII，例 "V1.0.0.260530"
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

} // namespace Protocol
