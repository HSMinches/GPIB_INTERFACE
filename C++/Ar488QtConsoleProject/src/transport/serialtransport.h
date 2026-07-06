#pragma once

#include <QByteArray>
#include <QIODevice>
#include <QSerialPort>
#include <QString>

class SerialTransport {
public:
    virtual ~SerialTransport() = default;

    bool isOpen() const {
        return isOpenImpl();
    }

    void setPortName(const QString& portName) {
        setPortNameImpl(portName);
    }

    bool open(QIODevice::OpenMode mode) {
        return openImpl(mode);
    }

    void close() {
        closeImpl();
    }

    bool setBaudRate(qint32 baudRate) {
        return setBaudRateImpl(baudRate);
    }

    bool setDataBits(QSerialPort::DataBits dataBits) {
        return setDataBitsImpl(dataBits);
    }

    bool setParity(QSerialPort::Parity parity) {
        return setParityImpl(parity);
    }

    bool setStopBits(QSerialPort::StopBits stopBits) {
        return setStopBitsImpl(stopBits);
    }

    bool setFlowControl(QSerialPort::FlowControl flowControl) {
        return setFlowControlImpl(flowControl);
    }

    bool clear(QSerialPort::Directions directions) {
        return clearImpl(directions);
    }

    qint64 write(const QByteArray& payload) {
        return writeImpl(payload);
    }

    bool waitForBytesWritten(int msecs) {
        return waitForBytesWrittenImpl(msecs);
    }

    bool flush() {
        return flushImpl();
    }

    qint64 bytesAvailable() const {
        return bytesAvailableImpl();
    }

    QByteArray read(qint64 maxSize) {
        return readImpl(maxSize);
    }

    QByteArray readAll() {
        return readAllImpl();
    }

    bool waitForReadyRead(int msecs) {
        return waitForReadyReadImpl(msecs);
    }

    QSerialPort::SerialPortError error() const {
        return errorImpl();
    }

    QString errorString() const {
        return errorStringImpl();
    }

protected:
    SerialTransport() = default;

private:
    SerialTransport(const SerialTransport&) = delete;
    SerialTransport& operator=(const SerialTransport&) = delete;

    virtual bool isOpenImpl() const = 0;
    virtual void setPortNameImpl(const QString& portName) = 0;
    virtual bool openImpl(QIODevice::OpenMode mode) = 0;
    virtual void closeImpl() = 0;

    virtual bool setBaudRateImpl(qint32 baudRate) = 0;
    virtual bool setDataBitsImpl(QSerialPort::DataBits dataBits) = 0;
    virtual bool setParityImpl(QSerialPort::Parity parity) = 0;
    virtual bool setStopBitsImpl(QSerialPort::StopBits stopBits) = 0;
    virtual bool setFlowControlImpl(QSerialPort::FlowControl flowControl) = 0;

    virtual bool clearImpl(QSerialPort::Directions directions) = 0;
    virtual qint64 writeImpl(const QByteArray& payload) = 0;
    virtual bool waitForBytesWrittenImpl(int msecs) = 0;
    virtual bool flushImpl() = 0;
    virtual qint64 bytesAvailableImpl() const = 0;
    virtual QByteArray readImpl(qint64 maxSize) = 0;
    virtual QByteArray readAllImpl() = 0;
    virtual bool waitForReadyReadImpl(int msecs) = 0;

    virtual QSerialPort::SerialPortError errorImpl() const = 0;
    virtual QString errorStringImpl() const = 0;
};
