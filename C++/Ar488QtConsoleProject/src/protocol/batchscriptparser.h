#pragma once

#include <QString>

#include <memory>

class BatchAction;

class BatchScriptParser final {
public:
    static std::unique_ptr<BatchAction> parseAction(const QString& originalLine, int sourceLineNumber);

private:
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
