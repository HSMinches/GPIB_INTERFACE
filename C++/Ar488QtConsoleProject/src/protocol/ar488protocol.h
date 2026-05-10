#pragma once

#include <QString>

class Ar488Protocol final {
public:
    Ar488Protocol() = delete;

    static QString stripCommandLineEnding(QString text);
    static bool isRawCommand(const QString& command);
    static bool commandExpectsReply(const QString& command);
    static bool isAdapterNoiseLine(const QString& line);
    static QString sanitizeReply(const QString& reply);
};
