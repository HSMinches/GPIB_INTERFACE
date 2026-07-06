# UC05: Datalogging

## General Description

This is the detailed description of the "Datalogging" use case of the Custom GPIB Instrument Interface System. It describes the process of repeatedly collecting measurement data from a GPIB instrument and storing the results in a CSV file. The system periodically sends SCPI queries, receives responses, extracts measurement values, and saves timestamped samples.

### Requirements

- RF13: Allow the user to configure a datalogging session.
- RF14: Repeatedly query an instrument for measurement data.
- RF15: Store collected measurement samples in a CSV file.
- RF16: Associate each measurement with a timestamp.

### Actors

- User
- USB-GPIB Adapter

### Priority

- Medium

### Preconditions

The USB-GPIB adapter must be connected, the target instrument must be available, and the user must configure the query, sampling interval, and output file.

### Frequency of Use

Medium

### Entry Condition

The user starts a datalogging session.

## Main Flow

1. The user opens the datalogging interface;
2. The user selects the target instrument or GPIB address;
3. The user defines the SCPI query to be used for measurement;
4. The user configures the sampling interval and output CSV file;
5. The user starts the datalogging session;
6. The system sends the configured SCPI query using **UC03: Inject SCPI Query**;
7. The system receives the instrument response using **UC04: Receive SCPI Query**;
8. The system parses the response into a measurement value;
9. The system records the timestamp and measurement value;
10. The system saves the sample to the CSV file;
11. The system waits for the next sampling interval;
12. Steps 6 to 11 repeat until the user stops the datalogging session.

### Alternative Flow

#### A1: Invalid measurement response

1. The system receives a response that cannot be converted into a numeric value;
2. The system marks the sample as invalid or skips it;
3. The datalogging session continues.

#### A2: CSV file cannot be written

1. The system fails to create or update the CSV file;
2. The system notifies the user;
3. The datalogging session is stopped to avoid data loss.

#### A3: User stops datalogging

1. The user requests to stop the session;
2. The system finishes the current operation;
3. The system closes the CSV file safely.

#### A4: Communication failure

1. The system fails to send the query or receive a response;
2. The system reports the communication error;
3. Depending on the configuration, the system retries or stops the session.

### Postconditions

After the execution of this use case, the collected measurement data is stored in a CSV file and can be used later for analysis or plotting.

## Business Rules

1. Each valid measurement sample must contain at least a timestamp and a value;
2. The sampling interval must be greater than zero;
3. The system must not overwrite existing log files unless the user confirms it;
4. Invalid samples should not prevent the entire session unless repeated failures occur;
5. The CSV file must remain readable by external tools.
