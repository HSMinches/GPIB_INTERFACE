#pragma once

#include <QChar>
#include <QString>
#include <QStringList>

class CsvUtils final {
public:
    static QString escapeField(const QString& value);
    static QStringList parseLine(const QString& line);
    static QString sanitizeFileComponent(QString text);

private:
    CsvUtils() = delete;

    static QString quoteEscaped(QString value);
    static bool isQuote(QChar ch);
    static bool isFieldSeparator(QChar ch);
    static bool isInvalidFileComponentChar(QChar ch);
    static QString collapseRepeatedUnderscores(QString text);
};
