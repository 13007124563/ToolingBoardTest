#include "Rs232PortTester.h"

#include <QElapsedTimer>
#include <QThread>
#include <QtGlobal>

const char *Rs232PortTester::kDefaultCn35Port = "ttyLP2";
const char *Rs232PortTester::kDefaultCn36Port = "ttyLP7";

void Rs232PortTester::closePorts()
{
    if (m_rs232PortCn35.isOpen())
        m_rs232PortCn35.close();
    if (m_rs232PortCn36.isOpen())
        m_rs232PortCn36.close();
}

bool Rs232PortTester::configureAndOpen(QSerialPort &port, const QString &name, int baudRate, QString *log)
{
    if (port.isOpen())
        port.close();

    port.setPortName(name);
    port.setBaudRate(baudRate);
    port.setDataBits(QSerialPort::Data8);
    port.setParity(QSerialPort::NoParity);
    port.setStopBits(QSerialPort::OneStop);
    port.setFlowControl(QSerialPort::NoFlowControl);

    if (!port.open(QIODevice::ReadWrite)) {
        if (log) {
            *log += QStringLiteral("Open %1 failed: %2\n")
                        .arg(name, port.errorString());
        }
        return false;
    }

    port.clear(QSerialPort::AllDirections);
    if (log)
        *log += QStringLiteral("Open %1 OK baud=%2 8N1\n").arg(name).arg(baudRate);
    return true;
}

QByteArray Rs232PortTester::readExact(QSerialPort &port, int expectLen, int timeoutMs, QString *log)
{
    QByteArray buf;
    QElapsedTimer timer;
    timer.start();
    const int waitMs = qMax(200, timeoutMs);

    while (buf.size() < expectLen && timer.elapsed() < waitMs) {
        const int remain = waitMs - static_cast<int>(timer.elapsed());
        if (port.waitForReadyRead(qMin(200, remain)))
            buf.append(port.readAll());
        else if (port.bytesAvailable() > 0)
            buf.append(port.readAll());
    }

    if (log) {
        *log += QStringLiteral("RX %1 got %2 bytes: %3\n")
                    .arg(port.portName())
                    .arg(buf.size())
                    .arg(QString::fromLatin1(buf.toHex(' ')));
    }
    return buf;
}

bool Rs232PortTester::sendAndExpect(QSerialPort &tx,
                                    QSerialPort &rx,
                                    const QByteArray &payload,
                                    int timeoutMs,
                                    QString *log)
{
    rx.clear(QSerialPort::Input);

    const qint64 written = tx.write(payload);
    if (written != payload.size()) {
        if (log) {
            *log += QStringLiteral("TX %1 write failed (%2/%3): %4\n")
                        .arg(tx.portName())
                        .arg(written)
                        .arg(payload.size())
                        .arg(tx.errorString());
        }
        return false;
    }
    tx.flush();
    if (log) {
        *log += QStringLiteral("TX %1 -> %2 data=%3\n")
                    .arg(tx.portName(), rx.portName(),
                         QString::fromLatin1(payload.toHex(' ')));
    }

    QThread::msleep(20);

    const QByteArray got = readExact(rx, payload.size(), timeoutMs, log);
    if (got != payload) {
        if (log) {
            *log += QStringLiteral("Mismatch expect=%1 actual=%2\n")
                        .arg(QString::fromLatin1(payload.toHex(' ')),
                             QString::fromLatin1(got.toHex(' ')));
        }
        return false;
    }
    if (log)
        *log += QStringLiteral("Match OK (%1 -> %2)\n").arg(tx.portName(), rx.portName());
    return true;
}

Rs232CrossTalkResult Rs232PortTester::crossTalk(const QString &portA,
                                                const QString &portB,
                                                int baudRate,
                                                int timeoutMs)
{
    Rs232CrossTalkResult result;

    QString a = portA.trimmed();
    QString b = portB.trimmed();
    if (a.isEmpty())
        a = QString::fromLatin1(kDefaultCn35Port);
    if (b.isEmpty())
        b = QString::fromLatin1(kDefaultCn36Port);

    if (a == b) {
        result.detail = QStringLiteral("Port A and B must be different");
        return result;
    }
    if (baudRate <= 0)
        baudRate = kDefaultBaudRate;
    if (timeoutMs <= 0)
        timeoutMs = kDefaultTimeoutMs;

    result.detail += QStringLiteral("Cross-talk A=%1 B=%2 baud=%3 timeout=%4ms\n")
                         .arg(a, b)
                         .arg(baudRate)
                         .arg(timeoutMs);

    // 始终用本对象独立串口，不碰治具协议口
    closePorts();

    if (!configureAndOpen(m_rs232PortCn35, a, baudRate, &result.detail))
        return result;
    if (!configureAndOpen(m_rs232PortCn36, b, baudRate, &result.detail)) {
        closePorts();
        return result;
    }

    const QByteArray aToB = QByteArrayLiteral("CN35->CN36#RS232\r\n");
    const QByteArray bToA = QByteArrayLiteral("CN36->CN35#RS232\r\n");

    const bool okAb = sendAndExpect(m_rs232PortCn35, m_rs232PortCn36, aToB, timeoutMs, &result.detail);
    const bool okBa = okAb
        && sendAndExpect(m_rs232PortCn36, m_rs232PortCn35, bToA, timeoutMs, &result.detail);

    closePorts();

    if (okAb && okBa) {
        result.ok = true;
        result.summary = QStringLiteral("%1↔%2 OK").arg(a, b);
        result.detail += QStringLiteral("Both directions OK\n");
    } else {
        result.summary = QStringLiteral("%1↔%2 FAIL").arg(a, b);
        if (!okAb)
            result.detail += QStringLiteral("Direction A->B failed\n");
        else
            result.detail += QStringLiteral("Direction B->A failed\n");
    }
    return result;
}
