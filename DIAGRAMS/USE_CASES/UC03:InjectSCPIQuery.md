# UC03: Inject SCPI Query

## General Description

This is the detailed description of the "Inject SCPI Query" use case of the Custom GPIB Instrument Interface System. It describes the process of sending a SCPI command/query from the application to the instrument through the USB-GPIB adapter. This use case is included by both direct tasking and batch tasking.

### Requirements

- RF07: Send SCPI commands to the USB-GPIB adapter.
- RF08: Format commands according to the AR488/GPIB communication rules.
- RF09: Allow commands to be sent to a selected GPIB address.

### Actors

- User
- USB-GPIB Adapter

### Priority

- High

### Preconditions

The USB-GPIB adapter must be connected and configured. A target GPIB address and a SCPI command/query must be available.

### Frequency of Use

High

### Entry Condition

The system receives a request to transmit a SCPI command/query.

## Main Flow

1. The system receives the SCPI command/query from direct tasking or batch tasking;
2. The system verifies the selected GPIB address;
3. The system formats the command according to the expected adapter protocol;
4. The system sends the command to the USB-GPIB adapter through the serial connection;
5. The USB-GPIB adapter forwards the command to the target GPIB instrument;
6. The system records or displays that the command was sent.

### Alternative Flow

#### A1: Missing GPIB address

1. The system detects that no target address was selected;
2. The system cancels the transmission;
3. The system notifies the user.

#### A2: Serial port unavailable

1. The system detects that the serial connection is closed or unavailable;
2. The command is not sent;
3. The system displays a communication error.

#### A3: Transmission timeout

1. The adapter does not confirm or complete the transmission in the expected time;
2. The system reports a timeout error.

### Postconditions

After the execution of this use case, the SCPI command/query has been sent to the adapter or the system has informed the user that the transmission failed.

## Business Rules

1. Every transmitted command must be associated with a valid GPIB address;
2. The command must not be sent if the adapter is disconnected;
3. Adapter-specific formatting must be applied before transmission;
4. The system should keep a history or log of sent commands when applicable.
