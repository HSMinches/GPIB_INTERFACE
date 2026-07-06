#pragma once

#include <QString>

#include "protocol/instrumentprotocol.h"

class Ar488Protocol final : public InstrumentProtocol {
public:
    Ar488Protocol() = default;
    ~Ar488Protocol() override = default;

private:
    QString normalizeCommandImpl(QString command) const override;
    bool isTransportCommandImpl(const QString& command) const override;
    bool expectsTransportReplyImpl(const QString& command) const override;
    QString cleanReplyImpl(const QString& reply) const override;

    static QString stripCommandLineEnding(QString text);
    static bool isRawCommand(const QString& command);
    static bool commandExpectsReply(const QString& command);
    static bool isAdapterNoiseLine(const QString& line);
    static QString sanitizeReply(const QString& reply);
};
