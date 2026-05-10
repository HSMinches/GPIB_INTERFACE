#pragma once

#include <QString>

class BatchScriptParser final {
public:
    enum class LineType {
        Skip,
        Wait,
        Command
    };

    struct ParsedLine {
        LineType type = LineType::Skip;
        int sourceLineNumber = 0;
        int delayMs = 0;
        QString command;
        bool forceWrite = false;
        bool forceQuery = false;
    };

    BatchScriptParser() = delete;

    static ParsedLine parseLine(const QString& originalLine, int sourceLineNumber);
};
