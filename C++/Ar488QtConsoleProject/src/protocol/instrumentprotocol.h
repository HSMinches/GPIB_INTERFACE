#pragma once

#include <QString>

#include <utility>

class InstrumentProtocol {
public:
    virtual ~InstrumentProtocol() = default;

    QString normalizeCommand(QString command) const {
        return normalizeCommandImpl(std::move(command));
    }

    bool isTransportCommand(const QString& command) const {
        return isTransportCommandImpl(command);
    }

    bool expectsTransportReply(const QString& command) const {
        return expectsTransportReplyImpl(command);
    }

    QString cleanReply(const QString& reply) const {
        return cleanReplyImpl(reply);
    }

protected:
    InstrumentProtocol() = default;

private:
    InstrumentProtocol(const InstrumentProtocol&) = delete;
    InstrumentProtocol& operator=(const InstrumentProtocol&) = delete;

    virtual QString normalizeCommandImpl(QString command) const = 0;
    virtual bool isTransportCommandImpl(const QString& command) const = 0;
    virtual bool expectsTransportReplyImpl(const QString& command) const = 0;
    virtual QString cleanReplyImpl(const QString& reply) const = 0;
};
