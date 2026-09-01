#ifndef RS232PORTTESTER_H
#define RS232PORTTESTER_H

#include <QSerialPort>
#include <QString>

/** 两路 RS232 交叉互联后的互发自收结果。 */
struct Rs232CrossTalkResult {
    bool ok = false;
    QString summary; // 成功时简要结果，如 CN35↔CN36 OK
    QString detail;  // 过程日志（打开口 / TX / RX / 错误）
};

/**
 * 本机双串口互发自收（CN35 TX↔CN36 RX，CN35 RX↔CN36 TX）。
 * 自带独立 QSerialPort，不复用治具协议串口。
 */
class Rs232PortTester
{
public:
    static const int kDefaultBaudRate = 9600;
    static const int kDefaultTimeoutMs = 2000;

    // 开发板上 CN35/CN36 默认设备名（不含 /dev/）
    static const char *kDefaultCn35Port; // ttyLP2
    static const char *kDefaultCn36Port; // ttyLP7

    Rs232CrossTalkResult crossTalk(
        const QString &portA = QString(),
        const QString &portB = QString(),
        int baudRate = kDefaultBaudRate,
        int timeoutMs = kDefaultTimeoutMs);

    void closePorts();

private:
    bool configureAndOpen(QSerialPort &port, const QString &name, int baudRate, QString *log);
    QByteArray readExact(QSerialPort &port, int expectLen, int timeoutMs, QString *log);
    bool sendAndExpect(QSerialPort &tx, QSerialPort &rx, const QByteArray &payload,
                       int timeoutMs, QString *log);

    QSerialPort m_rs232PortCn35;
    QSerialPort m_rs232PortCn36;
};

#endif // RS232PORTTESTER_H
