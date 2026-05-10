#pragma once

#include <QString>
#include <QStringList>

class CsvUtils final {
public:
    CsvUtils() = delete;

    static QString escapeField(const QString& value);
    static QStringList parseLine(const QString& line);
    static QString sanitizeFileComponent(QString text);
};
