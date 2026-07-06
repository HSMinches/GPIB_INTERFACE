#pragma once

#include <QDateTime>
#include <QString>
#include <QStringList>

class ScpiUtils final {
public:
    static bool extractNumericReply(const QString& text, double& value);
    static bool parseGraphCsvRow(const QStringList& fields, QDateTime& timestamp, double& value);

private:
    ScpiUtils() = delete;

    static QStringList splitReplyLines(const QString& text);
    static bool tryParseFirstNumber(const QString& line, double& value);
    static bool parseTimestamp(const QString& text, QDateTime& timestamp);
    static bool parseValueField(const QString& text, double& value);
};
