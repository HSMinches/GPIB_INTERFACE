#include "datalogjob.h"

#include <utility>

int DataLogJob::address() const {
    return address_;
}

const QString& DataLogJob::equipmentCode() const {
    return equipmentCode_;
}

const QString& DataLogJob::equipmentType() const {
    return equipmentType_;
}

const QString& DataLogJob::query() const {
    return query_;
}

const QString& DataLogJob::mode() const {
    return mode_;
}

int DataLogJob::intervalMs() const {
    return intervalMs_;
}

int DataLogJob::maxSamples() const {
    return maxSamples_;
}

const QDateTime& DataLogJob::startTime() const {
    return startTime_;
}

const QDateTime& DataLogJob::endTime() const {
    return endTime_;
}

const QString& DataLogJob::csvPath() const {
    return csvPath_;
}

int DataLogJob::samplesWritten() const {
    return samplesWritten_;
}

const QDateTime& DataLogJob::nextDueTime() const {
    return nextDueTime_;
}

bool DataLogJob::isFinished() const {
    return finished_;
}

bool DataLogJob::isStopped() const {
    return stopped_;
}

bool DataLogJob::isTriggerPending() const {
    return triggerPending_;
}

void DataLogJob::setAddress(int address) {
    address_ = address;
}

void DataLogJob::setEquipmentCode(QString equipmentCode) {
    equipmentCode_ = std::move(equipmentCode);
}

void DataLogJob::setEquipmentType(QString equipmentType) {
    equipmentType_ = std::move(equipmentType);
}

void DataLogJob::setQuery(QString query) {
    query_ = std::move(query);
}

void DataLogJob::setMode(QString mode) {
    mode_ = std::move(mode);
}

void DataLogJob::setIntervalMs(int intervalMs) {
    intervalMs_ = intervalMs;
}

void DataLogJob::setMaxSamples(int maxSamples) {
    maxSamples_ = maxSamples;
}

void DataLogJob::setStartTime(const QDateTime& startTime) {
    startTime_ = startTime;
}

void DataLogJob::setEndTime(const QDateTime& endTime) {
    endTime_ = endTime;
}

void DataLogJob::setCsvPath(QString csvPath) {
    csvPath_ = std::move(csvPath);
}

void DataLogJob::setSamplesWritten(int samplesWritten) {
    samplesWritten_ = samplesWritten;
}

void DataLogJob::incrementSamplesWritten() {
    ++samplesWritten_;
}

void DataLogJob::setNextDueTime(const QDateTime& nextDueTime) {
    nextDueTime_ = nextDueTime;
}

void DataLogJob::setFinished(bool finished) {
    finished_ = finished;
}

void DataLogJob::setStopped(bool stopped) {
    stopped_ = stopped;
}

void DataLogJob::setTriggerPending(bool triggerPending) {
    triggerPending_ = triggerPending;
}

void DataLogJob::resetRuntimeState() {
    samplesWritten_ = 0;
    finished_ = false;
    stopped_ = false;
    triggerPending_ = false;
    nextDueTime_ = QDateTime();
}

bool DataLogJob::isContinuous() const {
    return mode_ == "Continuous";
}

bool DataLogJob::isTriggerBased() const {
    return mode_ == "Trigger Based";
}

bool DataLogJob::isComputerTimeBased() const {
    return mode_ == "Computer Time Based";
}

bool DataLogJob::hasSampleLimitReached() const {
    return maxSamples_ > 0 && samplesWritten_ >= maxSamples_;
}

QString DataLogJob::statusText(bool datalogRunning) const {
    if (stopped_) {
        return "Stopped";
    }
    if (finished_) {
        return "Finished";
    }
    if (!datalogRunning) {
        return "Configured";
    }
    if (isTriggerBased()) {
        return triggerPending_ ? "Trigger Pending" : "Armed";
    }
    if (isComputerTimeBased() && QDateTime::currentDateTime() < startTime_) {
        return "Waiting Start";
    }
    return "Running";
}
