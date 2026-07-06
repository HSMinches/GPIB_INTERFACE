#include "protocol/batchaction.h"

#include <utility>

BatchAction::BatchAction(int sourceLineNumber)
    : sourceLineNumber_(sourceLineNumber) {
}

int BatchAction::sourceLineNumber() const {
    return sourceLineNumber_;
}

SkipBatchAction::SkipBatchAction(int sourceLineNumber)
    : BatchAction(sourceLineNumber) {
}

void SkipBatchAction::execute(BatchExecutionContext&) const {
}

WaitBatchAction::WaitBatchAction(int sourceLineNumber, int delayMs)
    : BatchAction(sourceLineNumber), delayMs_(delayMs) {
}

void WaitBatchAction::execute(BatchExecutionContext& context) const {
    context.waitBatch(delayMs_);
}

CommandBatchAction::CommandBatchAction(int sourceLineNumber, QString command, bool forceWrite, bool forceQuery)
    : BatchAction(sourceLineNumber),
      command_(std::move(command)),
      forceWrite_(forceWrite),
      forceQuery_(forceQuery) {
}

void CommandBatchAction::execute(BatchExecutionContext& context) const {
    context.executeBatchCommand(command_, forceWrite_, forceQuery_);
}
