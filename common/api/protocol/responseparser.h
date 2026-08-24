#ifndef RESPONSEPARSER_H
#define RESPONSEPARSER_H

#include "protocolframe.h"

#include <QString>

namespace Protocol {

class ResponseParser
{
public:
    static QString parse(const Frame &frame);

    // 从 0x01 应答 INFO 提取版本字符串（ASCII）
    static QString versionText(const Frame &frame);

private:
    static QString parseVersion(const Frame &frame);
};

} // namespace Protocol

#endif // RESPONSEPARSER_H
