#include "scpiutils.h"

#include <QRegularExpression>

bool ScpiUtils::extractNumericReply(const QString& text, double& value) {
    const QStringList lines = splitReplyLines(text);

    for (const QString& rawLine : lines) {
        const QString line = rawLine.trimmed();
        if (line.isEmpty()) {
            continue;
        }

        if (tryParseFirstNumber(line, value)) {
            return true;
        }
    }

    return false;
}

bool ScpiUtils::parseGraphCsvRow(const QStringList& fields, QDateTime& timestamp, double& value) {
    if (fields.size() >= 6) {
        return parseTimestamp(fields.at(0), timestamp) && parseValueField(fields.at(5), value);
    }

    if (fields.size() >= 2) {
        return parseTimestamp(fields.at(0), timestamp) && parseValueField(fields.at(1), value);
    }

    return false;
}

QStringList ScpiUtils::splitReplyLines(const QString& text) {
    return text.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
}

bool ScpiUtils::tryParseFirstNumber(const QString& line, double& value) {
    const QRegularExpression rx(R"(([+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?))");
    const QRegularExpressionMatch match = rx.match(line);
    if (!match.hasMatch()) {
        return false;
    }

    bool ok = false;
    const double parsed = match.captured(1).toDouble(&ok);
    if (!ok) {
        return false;
    }

    value = parsed;
    return true;
}

bool ScpiUtils::parseTimestamp(const QString& text, QDateTime& timestamp) {
    timestamp = QDateTime::fromString(text.trimmed(), Qt::ISODateWithMs);
    if (!timestamp.isValid()) {
        timestamp = QDateTime::fromString(text.trimmed(), Qt::ISODate);
    }
    return timestamp.isValid();
}

bool ScpiUtils::parseValueField(const QString& text, double& value) {
    bool ok = false;
    const double parsed = text.trimmed().toDouble(&ok);
    if (!ok) {
        return false;
    }

    value = parsed;
    return true;
}
