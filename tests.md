# Tests

> [!NOTE]  
> This section describes the testing procedure used to validate the Custom GPIB Instrument Interface System.  
> The tests focus on serial communication, SCPI command execution, batch execution, datalogging, plotting, file handling, and error handling.

## General Overview

The testing process was designed to verify whether the system correctly performs its main responsibilities:

- Connect to a USB-GPIB adapter;
- Send SCPI commands to GPIB instruments;
- Receive and display instrument responses;
- Execute command batches;
- Perform datalogging;
- Save measurement data to CSV files;
- Plot live or stored measurement data;
- Handle common communication and user-input errors.

Because the project communicates with external laboratory equipment, the tests are divided into two main categories:

1. **Tests with real hardware**, using an AR488-compatible USB-GPIB adapter and a GPIB instrument;
2. **Tests without real hardware**, using simulated inputs, mock responses, or manual validation of internal logic.

## Test Environment

The expected test environment is composed of:

- A computer running the application;
- Qt installed with the required modules;
- A USB-GPIB adapter compatible with AR488;
- At least one SCPI-compatible GPIB instrument;
- A valid serial port connection;
- A build system such as CMake or qmake.

### Software Requirements

- C++ compiler;
- Qt framework;
- Qt Widgets module;
- Qt Serial Port module;
- Qt Charts module;
- CMake or qmake;
- GitHub repository with the project source code.

### Hardware Requirements

- USB-GPIB adapter;
- GPIB cable;
- SCPI-compatible instrument, such as:
  - Digital multimeter;
  - Oscilloscope;
  - Power supply;
  - Signal generator.

## Test Strategy

The system should be tested at different levels:

### Unit Tests

Unit tests should validate isolated pieces of logic, such as:

- SCPI command formatting;
- AR488 command construction;
- Batch script parsing;
- CSV parsing;
- Numeric response extraction;
- Equipment data validation.

### Integration Tests

Integration tests should verify communication between different modules, such as:

- User interface and controller;
- Controller and serial communication;
- Serial communication and AR488 adapter;
- Datalogging and CSV storage;
- Plotting and measurement data parsing.

### System Tests

System tests should verify the complete behavior of the application from the user perspective.

Examples include:

- Sending a command from the interface and receiving a response;
- Running a batch script;
- Starting and stopping a datalogging session;
- Loading and plotting a CSV file.

## Test Cases

---

# TC01: Application Startup

## Objective

Verify whether the application starts correctly and loads the main interface without errors.

## Preconditions

The project must be compiled successfully.

## Test Steps

1. Open the application executable;
2. Wait for the main window to load;
3. Verify whether all main interface sections are visible;
4. Check if the application does not display unexpected error messages.

## Expected Result

The application starts successfully and displays the main interface.

## Status

- DONE

20:46:54: The command "E:\Ar488QtConsoleProject_OOP_refactor\Ar488QtConsoleProject\build\Desktop_Qt_6_11_0_llvm_mingw_64_bit-Debug\debug\Ar488QtConsole.exe" finished successfully.

20:46:56: Starting E:\Ar488QtConsoleProject_OOP_refactor\Ar488QtConsoleProject\build\Desktop_Qt_6_11_0_llvm_mingw_64_bit-Debug\debug\Ar488QtConsole.exe...

<img width="1920" height="1047" alt="image" src="https://github.com/user-attachments/assets/a446425a-6354-4e16-a16d-36bab0bd9f7a" />

---

# TC02: Serial Port Detection

## Objective

Verify whether the system can detect available serial ports.

## Preconditions

At least one serial device must be connected to the computer.

## Test Steps

1. Connect the USB-GPIB adapter to the computer;
2. Open the application;
3. Access the serial connection section;
4. Refresh or list the available serial ports;
5. Verify whether the adapter port appears in the list.

## Expected Result

The serial port associated with the USB-GPIB adapter is listed by the application.

## Status

- DONE 

<img width="314" height="87" alt="image" src="https://github.com/user-attachments/assets/34aef511-fab4-4cc1-ba64-ac5009646c93" />


---

# TC03: Serial Connection

## Objective

Verify whether the system can open a serial connection with the USB-GPIB adapter.

## Preconditions

The USB-GPIB adapter must be connected and its serial port must be available.

## Test Steps

1. Open the application;
2. Select the correct serial port;
3. Select the correct baud rate;
4. Click the connection button;
5. Verify whether the system indicates that the connection was opened.

## Expected Result

The application successfully connects to the USB-GPIB adapter.

## Status

- DONE

<img width="1481" height="264" alt="image" src="https://github.com/user-attachments/assets/00d7510f-d13a-4beb-ace2-f13786de2d13" />


---

# TC04: Serial Disconnection

## Objective

Verify whether the system can close an active serial connection.

## Preconditions

The application must already be connected to the USB-GPIB adapter.

## Test Steps

1. Connect to the USB-GPIB adapter;
2. Click the disconnect button;
3. Verify whether the system closes the serial connection;
4. Try sending a command after disconnection.

## Expected Result

The connection is closed, and the system prevents commands from being sent while disconnected.

## Status

- DONE
<img width="98" height="28" alt="image" src="https://github.com/user-attachments/assets/a780a27e-604e-4252-a76d-d6ca6961f84a" />


---

# TC05: Direct SCPI Command Execution

## Objective

Verify whether the user can send a manual SCPI command to an instrument.

## Preconditions

The USB-GPIB adapter must be connected, and a GPIB instrument must be available.

## Test Steps

1. Open the direct tasking interface;
2. Select or enter the target GPIB address;
3. Enter a valid SCPI command, such as `*IDN?`;
4. Send the command;
5. Wait for the instrument response;
6. Verify whether the response is displayed.

## Expected Result

The command is sent successfully, and the instrument response is displayed to the user.

## Status

- DONE
<img width="1514" height="269" alt="image" src="https://github.com/user-attachments/assets/64bfc346-7709-476f-937a-82fa03ec10f0" />


---

# TC06: Invalid Direct Command

## Objective

Verify whether the system handles invalid or empty commands correctly.

## Preconditions

The application must be running.

## Test Steps

1. Open the direct tasking interface;
2. Leave the command field empty;
3. Attempt to send the command;
4. Enter an invalid command;
5. Attempt to send the command again.

## Expected Result

The system does not send empty commands and displays a validation message when necessary.

## Status

- DONE

---

# TC07: Invalid GPIB Address

## Objective

Verify whether the system prevents communication with an invalid GPIB address.

## Preconditions

The application must be running.

## Test Steps

1. Open the direct tasking interface;
2. Enter an invalid GPIB address, such as an empty address or an out-of-range value;
3. Enter a valid SCPI command;
4. Attempt to send the command.

## Expected Result

The system blocks the operation and informs the user that the GPIB address is invalid.

## Status

- DONE

---

# TC08: Receive SCPI Response

## Objective

Verify whether the system can receive and display a response from an instrument.

## Preconditions

A SCPI query must be sent to a connected instrument.

## Test Steps

1. Connect to the USB-GPIB adapter;
2. Select the correct GPIB address;
3. Send a query command, such as `*IDN?`;
4. Wait for the response;
5. Observe the response area.

## Expected Result

The instrument response is received and displayed correctly.

## Status

- TBD ( ADAPTER NEEDS TO BE UPDATED )

---

# TC9: Batch Script Execution

## Objective

Verify whether the system can execute multiple SCPI commands in sequence.

## Preconditions

The USB-GPIB adapter and target instrument must be connected.

## Test Steps

1. Open the batch tasking interface;
2. Enter a batch script with multiple commands, for example:

```text
*IDN?
WAIT 1000
MEAS:VOLT?
```

3. Start the batch execution;
4. Observe each command being executed;
5. Verify whether responses are displayed when expected.

## Expected Result

The system executes the commands in order and respects the wait instruction.

## Status

- DONE

---


# TC10: Stop Batch Execution

## Objective

Verify whether the user can interrupt a running batch task.

## Preconditions

A batch script with multiple commands or delays must be running.

## Test Steps

1. Open the batch tasking interface;
2. Start a long batch script;
3. Click the stop button during execution;
4. Verify whether the system interrupts the batch.

## Expected Result

The batch execution stops safely, and the system displays the partial execution status.

## Status

- DONE

---

# TC11: Datalogging Start

## Objective

Verify whether the system can start a datalogging session.

## Preconditions

The USB-GPIB adapter and target instrument must be connected.

## Test Steps

1. Open the datalogging interface;
2. Select the target GPIB address;
3. Enter a valid SCPI query that returns a numeric value;
4. Configure the sampling interval;
5. Select the output CSV file;
6. Start the datalogging session.

## Expected Result

The system starts collecting measurement data periodically.

## Status

- DONE
<img width="1492" height="864" alt="image" src="https://github.com/user-attachments/assets/3ad45e60-5b43-446d-98f7-0f8ca8cfc1a1" />

---

# TC12: Datalogging CSV Output

## Objective

Verify whether measurement data is correctly saved to a CSV file.

## Preconditions

A datalogging session must be running.

## Test Steps

1. Start a datalogging session;
2. Allow the system to collect several samples;
3. Stop the datalogging session;
4. Open the generated CSV file;
5. Verify whether the file contains timestamped measurement records.

## Expected Result

The CSV file is generated correctly and contains valid measurement data.

## Status

- DONE
<img width="640" height="165" alt="image" src="https://github.com/user-attachments/assets/5be2c75c-0e01-4e81-9c6a-1ecf26bc9559" />

---

# TC13: Live Plotting

## Objective

Verify whether the system can plot live measurement data.

## Preconditions

The USB-GPIB adapter and target instrument must be connected.

## Test Steps

1. Open the plotting interface;
2. Select live plotting mode;
3. Configure the target GPIB address;
4. Enter a SCPI query that returns a numeric value;
5. Configure the update interval;
6. Start live plotting;
7. Observe the graph as new values are received.

## Expected Result

The graph updates periodically with live measurement values.

## Status

- DONE
- 
<img width="1488" height="910" alt="image" src="https://github.com/user-attachments/assets/6ec0f98e-2b5e-40a0-98aa-645fa7ac9a1c" />


---

# TC15: Stop Live Plotting

## Objective

Verify whether the user can stop live plotting safely.

## Preconditions

Live plotting must be active.

## Test Steps

1. Start live plotting;
2. Wait until several points are displayed;
3. Click the stop button;
4. Observe the graph.

## Expected Result

Live data acquisition stops, and the graph remains visible with the collected points.

## Status

- Not executed

---

# TC16: Plot CSV File

## Objective

Verify whether the system can load and plot data from a CSV file.

## Preconditions

A valid CSV file with measurement data must be available.

## Test Steps

1. Open the plotting interface;
2. Select the option to load data from a CSV file;
3. Choose a valid CSV file generated by datalogging;
4. Load the file;
5. Observe the graph.

## Expected Result

The system reads the CSV file and displays the stored measurement data on the graph.

## Status

- Not executed

---

# TC17: Equipment Registry

## Objective

Verify whether the system can manage registered equipment information.

## Preconditions

The application must be running.

## Test Steps

1. Open the equipment management section;
2. Add a new instrument with name, type, code, and GPIB address;
3. Save the equipment information;
4. Verify whether the equipment appears in the list;
5. Restart the application if persistence is implemented;
6. Verify whether the equipment data is still available.

## Expected Result

The equipment is registered correctly and can be used by other parts of the system.

## Status

- Not executed

---

# TC18: User Interface Responsiveness

## Objective

Verify whether the graphical interface remains responsive during communication operations.

## Preconditions

The application must be running.

## Test Steps

1. Start a long operation, such as batch execution, datalogging, or live plotting;
2. Interact with the interface during the operation;
3. Verify whether the window responds normally;
4. Attempt to stop the operation.

## Expected Result

The interface remains responsive and allows the user to stop running operations.

## Status

- DONE

---

# TC23: Application Error Recovery

## Objective

Verify whether the application can recover from a communication error.

## Preconditions

The system must be connected to the USB-GPIB adapter.

## Test Steps

1. Connect to the USB-GPIB adapter;
2. Start a communication operation;
3. Disconnect the adapter physically or select an unavailable instrument;
4. Observe the error message;
5. Reconnect the adapter;
6. Attempt to connect again through the application.

## Expected Result

The system detects the error, informs the user, and allows a new connection attempt.

## Status

- DONE

---

# TC25: Build Test with qmake

## Objective

Verify whether the project can be built using qmake.

## Preconditions

Qt and qmake must be installed.

## Test Steps

1. Open the project in Qt Creator or use the terminal;
2. Run qmake on the `.pro` file;
3. Build the project;
4. Verify whether the executable is generated.

## Expected Result

The project builds successfully using qmake.

## Status

- Not executed

---

## Final Considerations

The test procedure verifies both the internal behavior and the external communication behavior of the system. The most critical tests are related to serial communication, SCPI command execution, response handling, and datalogging, because these features depend directly on external hardware.

The system should only be considered validated after the main communication flow has been tested with real equipment. However, many internal parts of the application, such as command parsing, CSV handling, and plotting, can also be tested without physical hardware.


