#include "csvutils.h"

QString CsvUtils::escapeField(const QString& value) {
    return quoteEscaped(value);
}

QStringList CsvUtils::parseLine(const QString& line) {
    QStringList result;
    QString field;
    bool inQuotes = false;

    for (int i = 0; i < line.size(); ++i) {
        const QChar ch = line.at(i);

        if (isQuote(ch)) {
            if (inQuotes && i + 1 < line.size() && isQuote(line.at(i + 1))) {
                field += '"';
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
            continue;
        }

        if (isFieldSeparator(ch) && !inQuotes) {
            result << field;
            field.clear();
            continue;
        }

        field += ch;
    }

    result << field;
    return result;
}

QString CsvUtils::sanitizeFileComponent(QString text) {
    for (int i = 0; i < text.size(); ++i) {
        if (isInvalidFileComponentChar(text.at(i))) {
            text[i] = '_';
        }
    }

    const QString sanitized = collapseRepeatedUnderscores(text).trimmed();
    return sanitized.isEmpty() ? QString("log") : sanitized;
}

QString CsvUtils::quoteEscaped(QString value) {
    value.replace("\"", "\"\"");
    return "\"" + value + "\"";
}

bool CsvUtils::isQuote(QChar ch) {
    return ch == '"';
}

bool CsvUtils::isFieldSeparator(QChar ch) {
    return ch == ',';
}

bool CsvUtils::isInvalidFileComponentChar(QChar ch) {
    static const QString invalid = "\\/:*?\"<>| ,";
    return invalid.contains(ch);
}

QString CsvUtils::collapseRepeatedUnderscores(QString text) {
    while (text.contains("__")) {
        text.replace("__", "_");
    }
    return text;
}
