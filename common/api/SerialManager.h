#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include "protocol/protocolframe.h"

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QByteArray>

class SerialManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialManager(QObject *parent = nullptr);

    QStringList availablePorts() const;
    bool isOpen() const;

    void setTimeoutMs(int ms);
    int timeoutMs() const;

    bool openPort(const QString &portName, int baudRate = Protocol::kDefaultBaudRate);
    void closePort();

    bool sendFrame(const QByteArray &frame, quint8 expectedCmd, quint8 expectedSeq,
                   quint8 expectedBoardAddr);

signals:
    void portOpened(const QString &portName);
    void portClosed();
    void errorOccurred(const QString &message);
    void frameSent(const QByteArray &frame);
    void frameReceived(const Protocol::Frame &frame, const QString &parsedText);
    void passiveFrameReceived(const Protocol::Frame &frame, const QString &reason);
    void operationTimeout();

private slots:
    void onReadyRead();
    void onTimeout();

private:
    void resetReceiveState();
    void processIncomingFrames();
    bool isExpectedResponse(const Protocol::Frame &frame) const;
    QString passiveReason(const Protocol::Frame &frame) const;

    QSerialPort m_port;
    QTimer m_timeoutTimer;
    QByteArray m_rxBuffer;
    Protocol::ProtocolCodec m_codec;
    quint8 m_pendingCmd = 0;
    quint8 m_pendingSeq = 0;
    quint8 m_pendingBoardAddr = 0;
    bool m_waitingResponse = false;
    int m_timeoutMs = Protocol::kDefaultTimeoutMs;
};

#endif // SERIALMANAGER_H
