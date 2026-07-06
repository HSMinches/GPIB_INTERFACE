#include "batchscriptparser.h"

#include "protocol/batchaction.h"

#include <QStringList>

#include <stdexcept>

namespace {
[[noreturn]] void fail(const QString& message) {
    throw std::runtime_error(message.toStdString());
}

bool isCommentLine(const QString& line) {
    return line.startsWith('#') ||
           line.startsWith("//") ||
           line.startsWith(';');
}

bool tryParseDelayLine(const QString& line, int& delayMs, bool& matched) {
    matched = false;

    const QString trimmed = line.trimmed();
    const QStringList keywords = { "WAIT", "DELAY", "SLEEP" };

    for (const QString& keyword : keywords) {
        if (trimmed.compare(keyword, Qt::CaseInsensitive) == 0 ||
            trimmed.startsWith(keyword + QLatin1Char(' '), Qt::CaseInsensitive)) {
            matched = true;
            const QString value = trimmed.mid(keyword.size()).trimmed();
            bool ok = false;
            const int ms = value.toInt(&ok);
            if (!ok || ms < 0) {
                return false;
            }
            delayMs = ms;
            return true;
        }
    }

    return false;
}
}

BatchScriptParser::ParsedLine BatchScriptParser::parseLine(
    const QString& originalLine,
    int sourceLineNumber
    ) {
    ParsedLine parsed;
    parsed.sourceLineNumber = sourceLineNumber;

    const QString line = originalLine.trimmed();
    if (line.isEmpty() || isCommentLine(line)) {
        parsed.type = LineType::Skip;
        return parsed;
    }

    int delayMs = 0;
    bool matchedDelay = false;
    if (tryParseDelayLine(line, delayMs, matchedDelay)) {
        parsed.type = LineType::Wait;
        parsed.delayMs = delayMs;
        return parsed;
    }
    if (matchedDelay) {
        fail(QString("Invalid delay on line %1: %2").arg(sourceLineNumber).arg(line));
    }

    QString command = line;
    if (command.startsWith("WRITE ", Qt::CaseInsensitive)) {
        parsed.forceWrite = true;
        command = command.mid(6).trimmed();
    } else if (command.startsWith("QUERY ", Qt::CaseInsensitive)) {
        parsed.forceQuery = true;
        command = command.mid(6).trimmed();
    }

    if (command.isEmpty()) {
        fail(QString("Empty command on line %1").arg(sourceLineNumber));
    }

    parsed.type = LineType::Command;
    parsed.command = command;
    return parsed;
}


std::unique_ptr<BatchAction> BatchScriptParser::parseAction(
    const QString& originalLine,
    int sourceLineNumber
    ) {
    const ParsedLine parsed = parseLine(originalLine, sourceLineNumber);

    switch (parsed.type) {
    case LineType::Skip:
        return std::make_unique<SkipBatchAction>(parsed.sourceLineNumber);
    case LineType::Wait:
        return std::make_unique<WaitBatchAction>(parsed.sourceLineNumber, parsed.delayMs);
    case LineType::Command:
        return std::make_unique<CommandBatchAction>(
            parsed.sourceLineNumber,
            parsed.command,
            parsed.forceWrite,
            parsed.forceQuery
            );
    }

    return std::make_unique<SkipBatchAction>(sourceLineNumber);
}
