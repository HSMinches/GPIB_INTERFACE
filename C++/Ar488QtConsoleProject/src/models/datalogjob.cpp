#include "datalogjob.h"

bool DataLogJob::isContinuous() const {
    return mode == "Continuous";
}

bool DataLogJob::isTriggerBased() const {
    return mode == "Trigger Based";
}

bool DataLogJob::isComputerTimeBased() const {
    return mode == "Computer Time Based";
}

bool DataLogJob::hasSampleLimitReached() const {
    return maxSamples > 0 && samplesWritten >= maxSamples;
}

QString DataLogJob::statusText(bool datalogRunning) const {
    if (stopped) {
        return "Stopped";
    }
    if (finished) {
        return "Finished";
    }
    if (!datalogRunning) {
        return "Configured";
    }
    if (isTriggerBased()) {
        return triggerPending ? "Trigger Pending" : "Armed";
    }
    if (isComputerTimeBased() && QDateTime::currentDateTime() < startTime) {
        return "Waiting Start";
    }
    return "Running";
}
