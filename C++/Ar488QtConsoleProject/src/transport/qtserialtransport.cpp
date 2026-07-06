#include "transport/qtserialtransport.h"

#include <QObject>

QtSerialTransport::QtSerialTransport(QObject* parent)
    : serial_(parent) {
}

bool QtSerialTransport::isOpenImpl() const {
    return serial_.isOpen();
}

void QtSerialTransport::setPortNameImpl(const QString& portName) {
    serial_.setPortName(portName);
}

bool QtSerialTransport::openImpl(QIODevice::OpenMode mode) {
    return serial_.open(mode);
}

void QtSerialTransport::closeImpl() {
    serial_.close();
}

bool QtSerialTransport::setBaudRateImpl(qint32 baudRate) {
    return serial_.setBaudRate(baudRate);
}

bool QtSerialTransport::setDataBitsImpl(QSerialPort::DataBits dataBits) {
    return serial_.setDataBits(dataBits);
}

bool QtSerialTransport::setParityImpl(QSerialPort::Parity parity) {
    return serial_.setParity(parity);
}

bool QtSerialTransport::setStopBitsImpl(QSerialPort::StopBits stopBits) {
    return serial_.setStopBits(stopBits);
}

bool QtSerialTransport::setFlowControlImpl(QSerialPort::FlowControl flowControl) {
    return serial_.setFlowControl(flowControl);
}

bool QtSerialTransport::clearImpl(QSerialPort::Directions directions) {
    return serial_.clear(directions);
}

qint64 QtSerialTransport::writeImpl(const QByteArray& payload) {
    return serial_.write(payload);
}

bool QtSerialTransport::waitForBytesWrittenImpl(int msecs) {
    return serial_.waitForBytesWritten(msecs);
}

bool QtSerialTransport::flushImpl() {
    return serial_.flush();
}

qint64 QtSerialTransport::bytesAvailableImpl() const {
    return serial_.bytesAvailable();
}

QByteArray QtSerialTransport::readImpl(qint64 maxSize) {
    return serial_.read(maxSize);
}

QByteArray QtSerialTransport::readAllImpl() {
    return serial_.readAll();
}

bool QtSerialTransport::waitForReadyReadImpl(int msecs) {
    return serial_.waitForReadyRead(msecs);
}

QSerialPort::SerialPortError QtSerialTransport::errorImpl() const {
    return serial_.error();
}

QString QtSerialTransport::errorStringImpl() const {
    return serial_.errorString();
}
