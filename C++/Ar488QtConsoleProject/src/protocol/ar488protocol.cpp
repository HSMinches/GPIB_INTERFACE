#include "ar488protocol.h"

#include <QRegularExpression>
#include <QStringList>

#include <utility>


QString Ar488Protocol::normalizeCommandImpl(QString command) const {
    return stripCommandLineEnding(std::move(command));
}

bool Ar488Protocol::isTransportCommandImpl(const QString& command) const {
    return isRawCommand(command);
}

bool Ar488Protocol::expectsTransportReplyImpl(const QString& command) const {
    return commandExpectsReply(command);
}

QString Ar488Protocol::cleanReplyImpl(const QString& reply) const {
    return sanitizeReply(reply);
}

QString Ar488Protocol::stripCommandLineEnding(QString text) {
    while (!text.isEmpty() && (text.endsWith('\n') || text.endsWith('\r'))) {
        text.chop(1);
    }
    return text;
}

bool Ar488Protocol::isRawCommand(const QString& command) {
    return command.trimmed().startsWith("++");
}

bool Ar488Protocol::commandExpectsReply(const QString& command) {
    const QString trimmed = command.trimmed().toLower();

    if (trimmed == "++ver") {
        return true;
    }

    if (trimmed == "++read" || trimmed.startsWith("++read ")) {
        return true;
    }

    return false;
}

bool Ar488Protocol::isAdapterNoiseLine(const QString& line) {
    const QString text = line.trimmed();
    if (text.isEmpty()) {
        return true;
    }

    return text.contains("Unrecognized command", Qt::CaseInsensitive) ||
           text.contains("Unknown command", Qt::CaseInsensitive) ||
           text.contains("AR488", Qt::CaseInsensitive);
}

QString Ar488Protocol::sanitizeReply(const QString& reply) {
    const QStringList rawLines = reply.split(QRegularExpression("[\r\n]+"), Qt::SkipEmptyParts);
    QStringList cleaned;

    for (const QString& rawLine : rawLines) {
        const QString line = rawLine.trimmed();
        if (line.isEmpty()) {
            continue;
        }
        if (isAdapterNoiseLine(line)) {
            continue;
        }
        cleaned << line;
    }

    return cleaned.join('\n').trimmed();
}
