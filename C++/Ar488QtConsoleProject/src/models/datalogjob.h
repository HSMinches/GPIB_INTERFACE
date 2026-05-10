#pragma once

#include <QDateTime>
#include <QString>

struct DataLogJob {
    int address = 10;
    QString equipmentCode;
    QString equipmentType;
    QString query;
    QString mode;
    int intervalMs = 1000;
    int maxSamples = 0;
    QDateTime startTime;
    QDateTime endTime;
    QString csvPath;

    int samplesWritten = 0;
    QDateTime nextDueTime;
    bool finished = false;
    bool stopped = false;
    bool triggerPending = false;

    bool isContinuous() const;
    bool isTriggerBased() const;
    bool isComputerTimeBased() const;
    bool hasSampleLimitReached() const;
    QString statusText(bool datalogRunning) const;
};
