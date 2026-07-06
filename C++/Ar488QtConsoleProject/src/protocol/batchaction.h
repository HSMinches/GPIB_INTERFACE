#pragma once

#include <QString>

class BatchExecutionContext {
public:
    virtual ~BatchExecutionContext() = default;

    virtual void waitBatch(int delayMs) = 0;
    virtual void executeBatchCommand(const QString& command, bool forceWrite, bool forceQuery) = 0;
};

class BatchAction {
public:
    explicit BatchAction(int sourceLineNumber);
    virtual ~BatchAction() = default;

    int sourceLineNumber() const;
    virtual void execute(BatchExecutionContext& context) const = 0;

private:
    int sourceLineNumber_ = 0;
};

class SkipBatchAction final : public BatchAction {
public:
    explicit SkipBatchAction(int sourceLineNumber);
    void execute(BatchExecutionContext& context) const override;
};

class WaitBatchAction final : public BatchAction {
public:
    WaitBatchAction(int sourceLineNumber, int delayMs);
    void execute(BatchExecutionContext& context) const override;

private:
    int delayMs_ = 0;
};

class CommandBatchAction final : public BatchAction {
public:
    CommandBatchAction(int sourceLineNumber, QString command, bool forceWrite, bool forceQuery);
    void execute(BatchExecutionContext& context) const override;

private:
    QString command_;
    bool forceWrite_ = false;
    bool forceQuery_ = false;
};
