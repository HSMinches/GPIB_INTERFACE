# Implementation

> [!NOTE]  
> This section reports the implementation process of the problem, including the main tools, technologies, libraries, architectural decisions, and organization used during the development of the Custom GPIB Instrument Interface System.

## General Overview

The system was implemented as a desktop application for controlling GPIB-compatible laboratory instruments through a USB-GPIB adapter, specifically an AR488-based interface. The application allows the user to send SCPI commands directly, execute command batches, receive instrument responses, perform datalogging, and visualize measurement data through plots.

The implementation follows an object-oriented approach, separating responsibilities into different modules. This organization improves maintainability, readability, and future extensibility of the project.

The main implementation goal was to create a flexible interface capable of communicating with instruments using SCPI commands over a serial connection, while also supporting additional features such as command automation, CSV logging, and graphical data visualization.

## Programming Language

The project was implemented using:

- C++ as the main programming language;
- Object-oriented programming principles;
- Qt framework for graphical interface and system integration.

C++ was chosen because it provides good performance, strong type control, and native support for object-oriented design. Qt was used because it simplifies the development of cross-platform graphical applications and provides useful modules for serial communication, file handling, timers, and charts.

## Tools and Libraries Used

### Qt Framework

The main framework used in the implementation was Qt. The project uses several Qt modules:

- Qt Widgets: used to build the graphical user interface;
- Qt Serial Port: used to communicate with the USB-GPIB adapter through a serial port;
- Qt Charts: used to display measurement data graphically;
- Qt Core: used for timers, strings, files, data structures, and application logic.

### CMake and qmake

The project supports build configuration using:

- CMake;
- qmake through the `.pro` file.

This makes the project more flexible because it can be compiled using different Qt development workflows.

### AR488 / USB-GPIB Adapter

The system communicates with instruments through an AR488-compatible USB-GPIB adapter. The adapter works as a bridge between the computer serial port and the GPIB bus.

The application sends commands to the adapter through the serial interface. The adapter then forwards those commands to the selected GPIB instrument.
