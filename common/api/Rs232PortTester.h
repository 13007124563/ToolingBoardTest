#ifndef RS232PORTTESTER_H
#define RS232PORTTESTER_H

#include <QSerialPort>
#include <QString>

/** 两路 RS232 交叉互联后的互发自收结果。 */
struct Rs232CrossTalkResult {
    bool ok = false;
    QString summary; // 成功时简要结果，如 ttyLP2↔ttyLP7 OK
    QString detail;  // 过程日志（打开口 / TX / RX / 错误）
};

/**
 * 本机双串口互发自收（TX↔RX 交叉接线）。
 * 自带独立 QSerialPort，不复用治具协议串口。
 */
class Rs232PortTester
{
public:
    static const int kDefaultTimeoutMs = 2000;

    // CN35/CN36：ttyLP2 / ttyLP7 @ 9600
    static const int kBaudCn35Cn36 = 9600;
    static const char *kDefaultCn35Port;
    static const char *kDefaultCn36Port;

    // CN37/CN38：ttyLP3 / ttyLP5 @ 115200
    static const int kBaudCn37Cn38 = 115200;
    static const char *kDefaultCn37Port;
    static const char *kDefaultCn38Port;

    Rs232CrossTalkResult crossTalk(
        const QString &portA,
        const QString &portB,
        int baudRate,
        int timeoutMs = kDefaultTimeoutMs);

    void closePorts();

private:
    bool configureAndOpen(QSerialPort &port, const QString &name, int baudRate, QString *log);
    QByteArray readExact(QSerialPort &port, int expectLen, int timeoutMs, QString *log);
    bool sendAndExpect(QSerialPort &tx, QSerialPort &rx, const QByteArray &payload,
                       int timeoutMs, QString *log);

    QSerialPort m_rs232PortA;
    QSerialPort m_rs232PortB;
};

#endif // RS232PORTTESTER_H
