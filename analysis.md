# OBJECT ORIENTED ANALYSIS 

## General description of the problem domain
The project belongs to the laboratory instrument control and measurement automation domain.

Its purpose is to let a user control SCPI-capable electronic test equipment from a desktop Qt application through an AR488 serial-to-GPIB adapter. The application bridges a modern PC serial/USB interface with older or professional lab instruments that communicate using GPIB / IEEE-488 and accept SCPI / IEEE-488.2 commands.

In practical terms, the system helps a lab user:

  - Connect to an AR488 adapter through a serial port.
  - Select or register lab instruments by GPIB address, code, and type.
  - Send manual SCPI commands such as *IDN? or MEAS:VOLT?.
  - Execute command batches with waits/delays.
  - Query instruments repeatedly for measurements.
  - Save timestamped measurement data to CSV.
  - Display live or previously logged measurement data in a graph.

So the domain is a test-bench automation tool where the main concepts are instruments, addresses, commands, replies, measurement samples, CSV logs, and visualization.

## Use Case Diagram
![USE CASE DIAGRAM](/DIAGRAMS/USE_CASES/UPD_UC.png)

- [UC1: Direct Tasking](DIAGRAMS/USE_CASES/UC01:DirectTasking)
- [UC2: Batch Tasking](DIAGRAMS/USE_CASES/UC02:BatchTasking)
- [UC3: Inject SCPI Query](DIAGRAMS/USE_CASES/UC03:InjectSCPIQuery)
- [UC4: ReceiveSCPIQuery](DIAGRAMS/USE_CASES/UC04:ReceiveSCPIQuery)
- [UC5: Datalogging](DIAGRAMS/USE_CASES/UC05:Datalogging)
- [UC6: Plotting](DIAGRAMS/USE_CASES/UC06:Plotting)

## Problem Domain Diagram
