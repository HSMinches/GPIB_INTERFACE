#include "csvutils.h"

QString CsvUtils::escapeField(const QString& value) {
    QString escaped = value;
    escaped.replace("\"", "\"\"");
    return "\"" + escaped + "\"";
}

QStringList CsvUtils::parseLine(const QString& line) {
    QStringList result;
    QString field;
    bool inQuotes = false;

    for (int i = 0; i < line.size(); ++i) {
        const QChar ch = line.at(i);

        if (ch == '"') {
            if (inQuotes && i + 1 < line.size() && line.at(i + 1) == '"') {
                field += '"';
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
            continue;
        }

        if (ch == ',' && !inQuotes) {
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
    static const QString invalid = "\\/:*?\"<>| ,";
    for (const QChar ch : invalid) {
        text.replace(ch, '_');
    }
    while (text.contains("__")) {
        text.replace("__", "_");
    }
    return text.trimmed().isEmpty() ? QString("log") : text.trimmed();
}
