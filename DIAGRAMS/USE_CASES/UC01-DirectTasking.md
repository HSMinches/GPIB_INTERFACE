# UC01: Direct Tasking

## General Description

This is the detailed description of the "Direct Tasking" use case of the Custom GPIB Instrument Interface System. It describes the manual execution of a single SCPI command/query by the user. In this mode, the user enters a command and sends it directly to a selected GPIB instrument through the USB-GPIB adapter.

### Requirements

- RF01: Allow the user to manually send SCPI commands to an instrument.
- RF02: Allow the system to communicate with a USB-GPIB adapter.
- RF03: Allow the user to view the response returned by the instrument.

### Actors

- User
- USB-GPIB Adapter

### Priority

- High

### Preconditions

The system must be initialized, the USB-GPIB adapter must be connected, and the target instrument must be available on the GPIB bus.

### Frequency of Use

High

### Entry Condition

The user chooses to manually send a SCPI command/query to an instrument.

## Main Flow

1. The user opens the direct tasking interface;
2. The system displays the command input area;
3. The user enters a SCPI command/query;
4. The user selects or confirms the target GPIB instrument address;
5. The system validates the command and the selected GPIB address;
6. The system includes the use case **UC03: Inject SCPI Query**;
7. The USB-GPIB adapter transmits the command to the target instrument;
8. If the command is a query, the system waits for a response;
9. The system includes the use case **UC04: Receive SCPI Query**;
10. The system displays the response or execution status to the user.

### Alternative Flow

#### A1: Invalid command

1. The user enters an empty or invalid command;
2. The system notifies the user that the command cannot be sent;
3. The flow returns to step 3 of the main flow.

#### A2: Communication failure

1. The command cannot be transmitted through the adapter;
2. The system displays a communication error message;
3. The operation is canceled.

### Postconditions

After the execution of this use case, the command has either been successfully transmitted to the instrument or the user has been informed of the error.

## Business Rules

1. A command cannot be sent if no serial connection is active;
2. A valid GPIB address must be selected before transmission;
3. Commands ending with `?` are treated as queries and should expect a response;
4. Communication errors must be shown to the user.
