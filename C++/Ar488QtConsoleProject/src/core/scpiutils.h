#pragma once

#include <QDateTime>
#include <QString>
#include <QStringList>

class ScpiUtils final {
public:
    ScpiUtils() = delete;

    static bool extractNumericReply(const QString& text, double& value);
    static bool parseGraphCsvRow(const QStringList& fields, QDateTime& timestamp, double& value);
};
