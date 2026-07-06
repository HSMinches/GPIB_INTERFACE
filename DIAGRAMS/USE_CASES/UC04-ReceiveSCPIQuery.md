# UC04: Receive SCPI Query

## General Description

This is the detailed description of the "Receive SCPI Query" use case of the Custom GPIB Instrument Interface System. Although the diagram names it "Receive SCPI Query", in the context of the system this represents receiving the response/result returned by the queried instrument.

### Requirements

- RF10: Receive responses from the USB-GPIB adapter.
- RF11: Display instrument responses to the user.
- RF12: Parse received responses when numerical measurement data is expected.

### Actors

- User
- USB-GPIB Adapter

### Priority

- High

### Preconditions

A SCPI query must have been sent successfully, and the instrument must be able to return a response through the USB-GPIB adapter.

### Frequency of Use

High

### Entry Condition

The system waits for a response after sending a SCPI query.

## Main Flow

1. The system starts listening for incoming data from the USB-GPIB adapter;
2. The target instrument sends its response through the GPIB bus;
3. The USB-GPIB adapter forwards the response to the application through the serial connection;
4. The system receives the raw response;
5. The system validates whether the response is complete;
6. The system displays the received response to the user;
7. If the response contains measurement data, the system parses the value for possible datalogging or plotting.

### Alternative Flow

#### A1: No response received

1. The system waits until the configured timeout is reached;
2. No response is received;
3. The system reports a timeout error to the user.

#### A2: Invalid or incomplete response

1. The system receives malformed or incomplete data;
2. The system notifies the user that the response could not be interpreted;
3. The raw response may still be displayed for debugging.

#### A3: Non-numeric response during measurement

1. The system expects a numeric value;
2. The received response cannot be converted into a numeric measurement;
3. The system ignores the sample or marks it as invalid.

### Postconditions

After the execution of this use case, the instrument response has been received, displayed, and optionally parsed for logging or plotting.

## Business Rules

1. A response should only be expected after a query command;
2. Query commands are usually identified by the `?` character;
3. The system must handle timeout situations gracefully;
4. Numeric parsing must not crash the application if the response contains text or invalid data.
