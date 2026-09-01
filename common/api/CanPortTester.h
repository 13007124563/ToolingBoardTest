#ifndef CANPORTTESTER_H
#define CANPORTTESTER_H

#include <QString>

/** SocketCAN 固件版本查询（协议 5.9：IPC↔LCB EXTID）。 */
struct CanVersionQueryResult {
    bool ok = false;
    QString version;   // 解析到的固件版本，如 V1.0.0.240417
    QString detail;    // 过程日志（TX/RX/错误）
};

class CanPortTester
{
public:
    static const int kDefaultBitrate = 500000; // 与开发板实测一致

    /**
     * 通过开发板 CAN 口查询受控板固件版本。
     * 请求 EXTID=0x0009xx10，8 字节全 0；
     * 应答 EXTID=0x000910xx，16 字节（13 字节版本 + 预留 + CRC）。
     * 版本号解析成功则 ok=true。
     * 测试前会按 bitrate 拉起接口（默认 500000）。
     */
    static CanVersionQueryResult queryFirmwareVersion(
        const QString &ifName = QStringLiteral("can0"),
        quint8 boardXx = 0x30,
        int timeoutMs = 2000,
        int bitrate = kDefaultBitrate);
};

#endif // CANPORTTESTER_H
