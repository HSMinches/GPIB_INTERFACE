#pragma once

#include "transport/serialtransport.h"

class QObject;

class QtSerialTransport final : public SerialTransport {
public:
    explicit QtSerialTransport(QObject* parent = nullptr);
    ~QtSerialTransport() override = default;

private:
    bool isOpenImpl() const override;
    void setPortNameImpl(const QString& portName) override;
    bool openImpl(QIODevice::OpenMode mode) override;
    void closeImpl() override;

    bool setBaudRateImpl(qint32 baudRate) override;
    bool setDataBitsImpl(QSerialPort::DataBits dataBits) override;
    bool setParityImpl(QSerialPort::Parity parity) override;
    bool setStopBitsImpl(QSerialPort::StopBits stopBits) override;
    bool setFlowControlImpl(QSerialPort::FlowControl flowControl) override;

    bool clearImpl(QSerialPort::Directions directions) override;
    qint64 writeImpl(const QByteArray& payload) override;
    bool waitForBytesWrittenImpl(int msecs) override;
    bool flushImpl() override;
    qint64 bytesAvailableImpl() const override;
    QByteArray readImpl(qint64 maxSize) override;
    QByteArray readAllImpl() override;
    bool waitForReadyReadImpl(int msecs) override;

    QSerialPort::SerialPortError errorImpl() const override;
    QString errorStringImpl() const override;

    QSerialPort serial_;
};
