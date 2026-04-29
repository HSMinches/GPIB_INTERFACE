#pragma once

#include <QObject>
#include <QSerialPort>
#include <QString>

class Ar488Controller final : public QObject {
    Q_OBJECT

public:
    explicit Ar488Controller(QObject* parent = nullptr);

public slots:
    void openPort(const QString& portName, int baudRate, int gpibAddress);
    void closePort();
    void detectAdapter();
    void writeScpi(int gpibAddress, const QString& command);
    void queryScpi(int gpibAddress, const QString& command);
    void runBatch(int gpibAddress, const QString& script);

signals:
    void logMessage(const QString& message);
    void errorOccurred(const QString& message);
    void connectionChanged(bool connected, const QString& message);
    void adapterDetected(bool ok, const QString& text);
    void queryCompleted(const QString& command, const QString& reply);
    void writeCompleted(const QString& command);
    void busyChanged(bool busy);
    void batchStarted();
    void batchFinished();
    void batchLineResult(const QString& command, const QString& reply);

private:
    QSerialPort serial_;
    bool busy_ = false;
    int currentGpibAddress_ = 10;

    void setBusy(bool busy);
    void ensureOpen() const;
    void clearBuffers();
    void writeLine(const QString& line);
    QString readUntilIdle(int totalTimeoutMs = 4000, int idleTimeoutMs = 250);
    QString ar488Cmd(const QString& command, bool expectReply = false, int timeoutMs = 2000);
    void initAdapter(int gpibAddress);
    void setGpibAddress(int gpibAddress);

    bool isRawAr488Command(const QString& command) const;
    bool ar488CommandShouldExpectReply(const QString& command) const;
    bool isAdapterNoiseLine(const QString& line) const;
    QString sanitizeReply(const QString& reply) const;

    void writeScpiInternal(int gpibAddress, const QString& command);
    QString queryPythonStyleInternal(int gpibAddress, const QString& command, int timeoutMs, int settleMs);
    QString queryScpiInternal(int gpibAddress, const QString& command, int timeoutMs, int settleMs);
    QString queryIdnInternal(int gpibAddress);
};