#include "SerialManager.h"
#include "protocol/responseparser.h"

#include <QSerialPortInfo>

SerialManager::SerialManager(QObject *parent)
    : QObject(parent)
{
    m_timeoutTimer.setSingleShot(true);
    connect(&m_port, &QSerialPort::readyRead, this, &SerialManager::onReadyRead);
    connect(&m_timeoutTimer, &QTimer::timeout, this, &SerialManager::onTimeout);
}

QStringList SerialManager::availablePorts() const
{
    QStringList ports;
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts())
        ports << info.portName();
    return ports;
}

bool SerialManager::isOpen() const
{
    return m_port.isOpen();
}

QString SerialManager::portName() const
{
    return m_port.portName();
}

int SerialManager::baudRate() const
{
    return m_port.baudRate();
}

void SerialManager::setTimeoutMs(int ms)
{
    m_timeoutMs = qMax(50, ms);
}

int SerialManager::timeoutMs() const
{
    return m_timeoutMs;
}

bool SerialManager::openPort(const QString &portName, int baudRate)
{
    closePort();

    m_port.setPortName(portName);
    m_port.setBaudRate(baudRate);
    m_port.setDataBits(QSerialPort::Data8);
    m_port.setParity(QSerialPort::NoParity);
    m_port.setStopBits(QSerialPort::OneStop);
    m_port.setFlowControl(QSerialPort::NoFlowControl);

    if (!m_port.open(QIODevice::ReadWrite)) {
        emit errorOccurred(tr("Failed to open port %1: %2").arg(portName, m_port.errorString()));
        return false;
    }

    resetReceiveState();
    emit portOpened(portName);
    return true;
}

void SerialManager::closePort()
{
    if (m_port.isOpen()) {
        m_timeoutTimer.stop();
        m_port.close();
        resetReceiveState();
        emit portClosed();
    }
}

bool SerialManager::sendFrame(const QByteArray &frame, quint8 expectedCmd, quint8 expectedSeq,
                              quint8 expectedBoardAddr)
{
    if (!m_port.isOpen()) {
        emit errorOccurred(tr("Serial port is not open"));
        return false;
    }

    m_pendingCmd = expectedCmd;
    m_pendingSeq = expectedSeq;
    m_pendingBoardAddr = expectedBoardAddr;
    m_waitingResponse = true;

    const qint64 written = m_port.write(frame);
    if (written != frame.size()) {
        m_waitingResponse = false;
        emit errorOccurred(tr("Failed to write complete frame"));
        return false;
    }
    m_port.flush();

    emit frameSent(frame);
    m_timeoutTimer.start(m_timeoutMs);
    return true;
}

bool SerialManager::isExpectedResponse(const Protocol::Frame &frame) const
{
    return Protocol::boardFromAddress(frame.addr) == m_pendingBoardAddr
        && frame.cmd == m_pendingCmd
        && frame.seq == m_pendingSeq;
}

QString SerialManager::passiveReason(const Protocol::Frame &frame) const
{
    if (!m_waitingResponse)
        return tr("unsolicited");

    QStringList parts;
    if (Protocol::boardFromAddress(frame.addr) != m_pendingBoardAddr)
        parts << tr("addr %1").arg(frame.addr, 2, 16, QChar('0'));
    if (frame.cmd != m_pendingCmd)
        parts << tr("cmd 0x%1").arg(frame.cmd, 2, 16, QChar('0'));
    if (frame.seq != m_pendingSeq)
        parts << tr("seq %1").arg(frame.seq);

    if (parts.isEmpty())
        return tr("ignored");

    return tr("mismatch: %1").arg(parts.join(QStringLiteral(", ")));
}

void SerialManager::processIncomingFrames()
{
    while (true) {
        Protocol::Frame frame;
        int consumed = 0;
        if (!m_codec.tryParseFrame(m_rxBuffer, frame, consumed)) {
            if (consumed > 0)
                m_rxBuffer.remove(0, consumed);
            break;
        }

        m_rxBuffer.remove(0, consumed);

        if (m_waitingResponse && isExpectedResponse(frame)) {
            m_waitingResponse = false;
            m_timeoutTimer.stop();

            const QString parsed = Protocol::ResponseParser::parse(frame);
            emit frameReceived(frame, parsed);
            continue;
        }

        emit passiveFrameReceived(frame, passiveReason(frame));
    }
}

void SerialManager::onReadyRead()
{
    m_rxBuffer.append(m_port.readAll());
    processIncomingFrames();
}

void SerialManager::onTimeout()
{
    if (!m_waitingResponse)
        return;

    processIncomingFrames();

    if (!m_waitingResponse)
        return;

    m_waitingResponse = false;
    emit operationTimeout();
}

void SerialManager::resetReceiveState()
{
    m_rxBuffer.clear();
    m_waitingResponse = false;
    m_timeoutTimer.stop();
}
