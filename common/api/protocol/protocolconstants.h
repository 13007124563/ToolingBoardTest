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

// B60058A / B1024-01 测试板：板地址固定 0x02，包序号固定 0x01
static const quint8 kTestBoardAddr = 0x02;
static const quint8 kTestBoardSeq  = 0x01;

// B1024-01 测试治具固件指令
enum Command : quint8 {
    CmdQueryVersion      = 0x01,  // 获取测试板版本号
    CmdVccCn52Test       = 0x02,  // VCC_12V/5V/3.3V (CN52)
    CmdPrinterCn43Test   = 0x03,  // 打印机电源 (CN43)
    CmdVout5vCn39Test    = 0x04,  // 5V控制输出 (CN39)
    CmdVout12vCn47Test   = 0x05,  // 12V受控输出 (CN47)
    CmdProximityCn13Test = 0x06,  // 5V人体接近开关 (CN13)
    CmdStInputIoTest     = 0x07,  // ST_INPUT1/2 IO (CN45/CN13)
};

// V1.1：0x03–0x06 下行 INFO 读电平选择
static const quint8 kReadVoltageLow  = 0x00;
static const quint8 kReadVoltageHigh = 0x01;

inline bool commandUsesReadLevelInfo(quint8 cmd)
{
    return cmd >= CmdPrinterCn43Test && cmd <= CmdProximityCn13Test;
}

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
