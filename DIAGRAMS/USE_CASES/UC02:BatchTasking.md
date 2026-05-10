# UC02: Batch Tasking

## General Description

This is the detailed description of the "Batch Tasking" use case of the Custom GPIB Instrument Interface System. It describes the execution of multiple SCPI commands in sequence. The user can define a batch of commands, including delays or wait instructions, and the system sends them automatically to the instrument through the USB-GPIB adapter.

### Requirements

- RF04: Allow the user to execute multiple SCPI commands sequentially.
- RF05: Allow delay/wait operations between commands.
- RF06: Allow the user to monitor the execution of a batch task.

### Actors

- User
- USB-GPIB Adapter

### Priority

- High

### Preconditions

The system must be initialized, the USB-GPIB adapter must be connected, and the batch script must contain at least one valid instruction.

### Frequency of Use

Medium

### Entry Condition

The user starts the execution of a batch task.

## Main Flow

1. The user opens the batch tasking interface;
2. The user writes or loads a batch script containing SCPI commands and optional wait instructions;
3. The system validates the batch script structure;
4. The user starts the batch execution;
5. The system reads the first instruction from the batch;
6. If the instruction is a SCPI command, the system includes **UC03: Inject SCPI Query**;
7. If the command expects a response, the system includes **UC04: Receive SCPI Query**;
8. If the instruction is a wait/delay command, the system waits for the configured time;
9. The system moves to the next instruction;
10. Steps 5 to 9 are repeated until all batch instructions are completed;
11. The system displays the final execution status to the user.

### Alternative Flow

#### A1: Invalid batch instruction

1. The system detects an invalid command or unsupported instruction;
2. The system notifies the user of the invalid line;
3. Depending on the configuration, the system either skips the instruction or stops the batch execution.

#### A2: User stops batch execution

1. The user requests to stop the batch process;
2. The system interrupts the current batch;
3. The system displays the partial execution status.

#### A3: Adapter communication error

1. The USB-GPIB adapter fails to send or receive data;
2. The system stops the batch execution;
3. The system displays an error message.

### Postconditions

After the execution of this use case, all valid batch instructions have been executed or the batch has been interrupted due to user action or an error.

## Business Rules

1. Batch commands must be executed in the order defined by the user;
2. Delay instructions must pause execution before the next command is sent;
3. The system must not continue a batch task after a critical communication failure;
4. The user must be able to identify which command caused an error.
