#pragma once

#include <QDateTime>
#include <QString>

class DataLogJob {
public:
    DataLogJob() = default;

    int address() const;
    const QString& equipmentCode() const;
    const QString& equipmentType() const;
    const QString& query() const;
    const QString& mode() const;
    int intervalMs() const;
    int maxSamples() const;
    const QDateTime& startTime() const;
    const QDateTime& endTime() const;
    const QString& csvPath() const;
    int samplesWritten() const;
    const QDateTime& nextDueTime() const;
    bool isFinished() const;
    bool isStopped() const;
    bool isTriggerPending() const;

    void setAddress(int address);
    void setEquipmentCode(QString equipmentCode);
    void setEquipmentType(QString equipmentType);
    void setQuery(QString query);
    void setMode(QString mode);
    void setIntervalMs(int intervalMs);
    void setMaxSamples(int maxSamples);
    void setStartTime(const QDateTime& startTime);
    void setEndTime(const QDateTime& endTime);
    void setCsvPath(QString csvPath);
    void setSamplesWritten(int samplesWritten);
    void incrementSamplesWritten();
    void setNextDueTime(const QDateTime& nextDueTime);
    void setFinished(bool finished);
    void setStopped(bool stopped);
    void setTriggerPending(bool triggerPending);
    void resetRuntimeState();

    bool isContinuous() const;
    bool isTriggerBased() const;
    bool isComputerTimeBased() const;
    bool hasSampleLimitReached() const;
    QString statusText(bool datalogRunning) const;

private:
    int address_ = 10;
    QString equipmentCode_;
    QString equipmentType_;
    QString query_;
    QString mode_;
    int intervalMs_ = 1000;
    int maxSamples_ = 0;
    QDateTime startTime_;
    QDateTime endTime_;
    QString csvPath_;

    int samplesWritten_ = 0;
    QDateTime nextDueTime_;
    bool finished_ = false;
    bool stopped_ = false;
    bool triggerPending_ = false;
};
