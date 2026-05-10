// file: src/ar488controller.cpp
#include "ar488controller.h"

#include "core/constants.h"
#include "protocol/ar488protocol.h"
#include "protocol/batchscriptparser.h"

#include <QElapsedTimer>
#include <QIODevice>
#include <QStringList>
#include <QThread>

#include <algorithm>
#include <stdexcept>

namespace {
[[noreturn]] void fail(const QString& message) {
    throw std::runtime_error(message.toStdString());
}
}  // namespace

Ar488Controller::Ar488Controller(QObject* parent)
    : QObject(parent), serial_(this) {
}

void Ar488Controller::setBusy(bool busy) {
    if (busy_ == busy) {
        return;
    }
    busy_ = busy;
    emit busyChanged(busy_);
}

void Ar488Controller::ensureOpen() const {
    if (!serial_.isOpen()) {
        fail("Serial port is not open.");
    }
}

void Ar488Controller::clearBuffers() {
    ensureOpen();
    serial_.clear(QSerialPort::AllDirections);
}

void Ar488Controller::writeLine(const QString& line) {
    ensureOpen();

    const QByteArray payload = Ar488Protocol::stripCommandLineEnding(line).toUtf8() + '\n';
    const qint64 written = serial_.write(payload);

    if (written < 0) {
        fail("Serial write failed: " + serial_.errorString());
    }

    if (!serial_.waitForBytesWritten(1000)) {
        fail("Serial write timeout: " + serial_.errorString());
    }

    serial_.flush();
}

QString Ar488Controller::readUntilIdle(int totalTimeoutMs, int idleTimeoutMs) {
    ensureOpen();

    QByteArray data;
    QElapsedTimer timer;
    timer.start();
    qint64 lastRxMs = timer.elapsed();

    while (timer.elapsed() < totalTimeoutMs) {
        const qint64 waiting = serial_.bytesAvailable();
        if (waiting > 0) {
            const QByteArray chunk = serial_.read(waiting);
            if (!chunk.isEmpty()) {
                data += chunk;
                lastRxMs = timer.elapsed();
                continue;
            }
        }

        if (serial_.waitForReadyRead(20)) {
            const QByteArray chunk = serial_.read(1);
            if (!chunk.isEmpty()) {
                data += chunk;
                lastRxMs = timer.elapsed();
                continue;
            }
        }

        if (!data.isEmpty() && (timer.elapsed() - lastRxMs) >= idleTimeoutMs) {
            break;
        }

        if (serial_.error() != QSerialPort::NoError &&
            serial_.error() != QSerialPort::TimeoutError) {
            fail("Serial read failed: " + serial_.errorString());
        }
    }

    if (serial_.bytesAvailable() > 0) {
        data += serial_.readAll();
    }

    return QString::fromUtf8(data).trimmed();
}

QString Ar488Controller::ar488Cmd(const QString& command, bool expectReply, int timeoutMs) {
    clearBuffers();
    writeLine(command);
    if (expectReply) {
        return readUntilIdle(timeoutMs, 250);
    }
    return QString();
}

void Ar488Controller::initAdapter(int gpibAddress) {
    ar488Cmd("++auto 0");
    ar488Cmd("++eoi 1");
    ar488Cmd("++eos 2");
    ar488Cmd("++eot_enable 1");
    ar488Cmd("++eot_char 10");
    ar488Cmd(QString("++read_tmo_ms %1").arg(AppConstants::DefaultReadTimeoutMs));
    ar488Cmd("++ifc");
    ar488Cmd(QString("++addr %1").arg(gpibAddress));
}

void Ar488Controller::setGpibAddress(int gpibAddress) {
    currentGpibAddress_ = std::clamp(gpibAddress, AppConstants::MinGpibAddress, AppConstants::MaxGpibAddress);
    ar488Cmd(QString("++addr %1").arg(currentGpibAddress_));
}

void Ar488Controller::writeScpiInternal(int gpibAddress, const QString& command) {
    setGpibAddress(gpibAddress);
    clearBuffers();
    writeLine(command);
}

QString Ar488Controller::queryPythonStyleInternal(
    int gpibAddress,
    const QString& command,
    int timeoutMs,
    int settleMs
    ) {
    setGpibAddress(gpibAddress);

    clearBuffers();
    writeLine(command);
    QThread::msleep(static_cast<unsigned long>(settleMs));
    writeLine("++read eoi");

    QString reply = Ar488Protocol::sanitizeReply(readUntilIdle(timeoutMs, 250));
    if (!reply.isEmpty()) {
        return reply;
    }

    clearBuffers();
    writeLine(command);
    QThread::msleep(static_cast<unsigned long>(settleMs));
    writeLine("++read");

    return Ar488Protocol::sanitizeReply(readUntilIdle(timeoutMs, 250));
}

QString Ar488Controller::queryScpiInternal(
    int gpibAddress,
    const QString& command,
    int timeoutMs,
    int settleMs
    ) {
    return queryPythonStyleInternal(gpibAddress, command, timeoutMs, settleMs);
}

QString Ar488Controller::queryIdnInternal(int gpibAddress) {
    return queryScpiInternal(gpibAddress, "*IDN?", 4000, 200);
}

void Ar488Controller::openPort(const QString& portName, int baudRate, int gpibAddress) {
    if (busy_) {
        emit logMessage("Busy; connect ignored.");
        return;
    }

    setBusy(true);

    try {
        if (serial_.isOpen()) {
            serial_.close();
        }

        serial_.setPortName(portName);
        serial_.setBaudRate(baudRate);
        serial_.setDataBits(QSerialPort::Data8);
        serial_.setParity(QSerialPort::NoParity);
        serial_.setStopBits(QSerialPort::OneStop);
        serial_.setFlowControl(QSerialPort::NoFlowControl);

        if (!serial_.open(QIODevice::ReadWrite)) {
            fail(QString("Open failed for %1: %2").arg(portName, serial_.errorString()));
        }

        QThread::msleep(2000);

        currentGpibAddress_ = std::clamp(gpibAddress, AppConstants::MinGpibAddress, AppConstants::MaxGpibAddress);
        clearBuffers();
        initAdapter(currentGpibAddress_);
        clearBuffers();

        emit connectionChanged(true, QString("Connected to %1 @ %2 baud").arg(portName).arg(baudRate));
        emit logMessage("AR488 initialized.");

        const QString version = ar488Cmd("++ver", true, 1000);
        if (!version.isEmpty()) {
            emit adapterDetected(true, version);
            emit logMessage("AR488: " + version);
        }
    } catch (const std::exception& ex) {
        if (serial_.isOpen()) {
            serial_.close();
        }
        emit errorOccurred(QString::fromUtf8(ex.what()));
        emit connectionChanged(false, "Disconnected");
    }

    setBusy(false);
}

void Ar488Controller::closePort() {
    if (busy_) {
        emit logMessage("Busy; disconnect ignored.");
        return;
    }

    setBusy(true);

    if (serial_.isOpen()) {
        serial_.close();
        emit logMessage("Serial port closed.");
    }

    emit connectionChanged(false, "Disconnected");
    setBusy(false);
}

void Ar488Controller::detectAdapter() {
    if (busy_) {
        emit logMessage("Busy; detect ignored.");
        return;
    }

    setBusy(true);

    try {
        ensureOpen();
        const QString version = ar488Cmd("++ver", true, 1000);
        if (version.isEmpty()) {
            emit adapterDetected(false, "No response to ++ver");
        } else {
            emit adapterDetected(true, version);
        }
    } catch (const std::exception& ex) {
        emit adapterDetected(false, QString::fromUtf8(ex.what()));
        emit errorOccurred(QString::fromUtf8(ex.what()));
    }

    setBusy(false);
}

void Ar488Controller::writeScpi(int gpibAddress, const QString& command) {
    if (busy_) {
        emit logMessage("Busy; write ignored.");
        return;
    }

    setBusy(true);

    try {
        const QString trimmed = command.trimmed();
        if (trimmed.isEmpty()) {
            fail("Command is empty.");
        }

        if (Ar488Protocol::isRawCommand(trimmed)) {
            ar488Cmd(trimmed, false);
            emit logMessage(QString("AR488 TX  %1").arg(trimmed));
            emit writeCompleted(trimmed);
            setBusy(false);
            return;
        }

        writeScpiInternal(gpibAddress, trimmed);
        emit logMessage(QString("SCPI TX  [%1] %2").arg(gpibAddress).arg(trimmed));
        emit writeCompleted(trimmed);
    } catch (const std::exception& ex) {
        emit errorOccurred(QString::fromUtf8(ex.what()));
    }

    setBusy(false);
}

void Ar488Controller::queryScpi(int gpibAddress, const QString& command) {
    if (busy_) {
        emit logMessage("Busy; query ignored.");
        return;
    }

    setBusy(true);

    try {
        const QString trimmed = command.trimmed();
        if (trimmed.isEmpty()) {
            fail("Command is empty.");
        }

        if (Ar488Protocol::isRawCommand(trimmed)) {
            const bool expectReply = Ar488Protocol::commandExpectsReply(trimmed);
            emit logMessage(QString("AR488 TX? %1").arg(trimmed));

            const QString reply = Ar488Protocol::sanitizeReply(ar488Cmd(trimmed, expectReply, 1500));

            if (expectReply) {
                emit logMessage(QString("AR488 RX  %1").arg(reply.isEmpty() ? "<empty>" : reply));
            } else {
                emit logMessage("AR488 RX  <no adapter reply expected>");
            }

            emit queryCompleted(trimmed, expectReply ? reply : QString());
            setBusy(false);
            return;
        }

        if (trimmed.compare("*IDN?", Qt::CaseInsensitive) == 0) {
            emit logMessage(QString("SCPI TX? [%1] %2").arg(gpibAddress).arg(trimmed));
            const QString reply = queryIdnInternal(gpibAddress);
            emit logMessage(QString("SCPI RX  [%1] %2").arg(gpibAddress).arg(reply.isEmpty() ? "<empty>" : reply));
            emit queryCompleted(trimmed, reply);
            setBusy(false);
            return;
        }

        emit logMessage(QString("SCPI TX? [%1] %2").arg(gpibAddress).arg(trimmed));
        const QString reply = queryScpiInternal(gpibAddress, trimmed, 4000, 150);
        emit logMessage(QString("SCPI RX  [%1] %2").arg(gpibAddress).arg(reply.isEmpty() ? "<empty>" : reply));
        emit queryCompleted(trimmed, reply);
    } catch (const std::exception& ex) {
        emit errorOccurred(QString::fromUtf8(ex.what()));
    }

    setBusy(false);
}

void Ar488Controller::runBatch(int gpibAddress, const QString& script) {
    if (busy_) {
        emit logMessage("Busy; batch ignored.");
        return;
    }

    setBusy(true);

    try {
        ensureOpen();

        const QStringList lines = script.split('\n', Qt::KeepEmptyParts);
        emit batchStarted();
        emit logMessage(QString("Batch started at GPIB address %1").arg(gpibAddress));

        for (int index = 0; index < lines.size(); ++index) {
            const auto parsed = BatchScriptParser::parseLine(lines.at(index), index + 1);

            if (parsed.type == BatchScriptParser::LineType::Skip) {
                continue;
            }

            if (parsed.type == BatchScriptParser::LineType::Wait) {
                emit logMessage(QString("BATCH WAIT %1 ms").arg(parsed.delayMs));
                QThread::msleep(static_cast<unsigned long>(parsed.delayMs));
                continue;
            }

            const QString command = parsed.command;

            if (Ar488Protocol::isRawCommand(command)) {
                const bool expectReply = parsed.forceQuery || Ar488Protocol::commandExpectsReply(command);

                emit logMessage(QString("BATCH AR488 TX%1 %2")
                                    .arg(expectReply ? "?" : "")
                                    .arg(command));

                const QString reply = Ar488Protocol::sanitizeReply(ar488Cmd(command, expectReply, 2000));

                if (expectReply) {
                    emit logMessage(QString("BATCH AR488 RX %1").arg(reply.isEmpty() ? "<empty>" : reply));
                    emit batchLineResult(command, reply);
                }

                continue;
            }

            const bool isQuery = parsed.forceQuery || (!parsed.forceWrite && command.endsWith('?'));

            if (!isQuery) {
                writeScpiInternal(gpibAddress, command);
                emit logMessage(QString("BATCH SCPI TX  [%1] %2").arg(gpibAddress).arg(command));
                continue;
            }

            emit logMessage(QString("BATCH SCPI TX? [%1] %2").arg(gpibAddress).arg(command));

            QString reply;
            if (command.compare("*IDN?", Qt::CaseInsensitive) == 0) {
                reply = queryIdnInternal(gpibAddress);
            } else {
                reply = queryScpiInternal(gpibAddress, command, 4000, 150);
            }

            emit logMessage(QString("BATCH SCPI RX  [%1] %2").arg(gpibAddress).arg(reply.isEmpty() ? "<empty>" : reply));
            emit batchLineResult(command, reply);
        }

        emit logMessage("Batch finished.");
        emit batchFinished();
    } catch (const std::exception& ex) {
        emit errorOccurred(QString::fromUtf8(ex.what()));
        emit batchFinished();
    }

    setBusy(false);
}