#include "scpiutils.h"

#include <QRegularExpression>

bool ScpiUtils::extractNumericReply(const QString& text, double& value) {
    const QStringList lines = text.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
    const QRegularExpression rx(R"(([+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?))");

    for (const QString& rawLine : lines) {
        const QString line = rawLine.trimmed();
        if (line.isEmpty()) {
            continue;
        }

        const QRegularExpressionMatch match = rx.match(line);
        if (!match.hasMatch()) {
            continue;
        }

        bool ok = false;
        const double parsed = match.captured(1).toDouble(&ok);
        if (ok) {
            value = parsed;
            return true;
        }
    }

    return false;
}

bool ScpiUtils::parseGraphCsvRow(const QStringList& fields, QDateTime& timestamp, double& value) {
    if (fields.size() >= 6) {
        timestamp = QDateTime::fromString(fields.at(0).trimmed(), Qt::ISODateWithMs);
        if (!timestamp.isValid()) {
            timestamp = QDateTime::fromString(fields.at(0).trimmed(), Qt::ISODate);
        }

        bool ok = false;
        value = fields.at(5).trimmed().toDouble(&ok);
        return timestamp.isValid() && ok;
    }

    if (fields.size() >= 2) {
        timestamp = QDateTime::fromString(fields.at(0).trimmed(), Qt::ISODateWithMs);
        if (!timestamp.isValid()) {
            timestamp = QDateTime::fromString(fields.at(0).trimmed(), Qt::ISODate);
        }

        bool ok = false;
        value = fields.at(1).trimmed().toDouble(&ok);
        return timestamp.isValid() && ok;
    }

    return false;
}
