# UC06: Plotting

## General Description

This is the detailed description of the "Plotting" use case of the Custom GPIB Instrument Interface System. It describes the visualization of measurement data in a graph. The system can plot values received from live instrument queries or values loaded from a CSV file generated during datalogging.

### Requirements

- RF17: Display measurement data graphically.
- RF18: Allow the user to visualize live measurement trends.
- RF19: Allow the user to load and plot previously stored CSV data.

### Actors

- User
- USB-GPIB Adapter

### Priority

- Medium

### Preconditions

For live plotting, the USB-GPIB adapter and target instrument must be connected. For offline plotting, a valid CSV file containing measurement data must be available.

### Frequency of Use

Medium

### Entry Condition

The user requests to plot measurement data.

## Main Flow

1. The user opens the plotting interface;
2. The user chooses between live plotting or CSV-based plotting;
3. If live plotting is selected, the user configures the instrument address, SCPI query, and update interval;
4. The system sends the query using **UC03: Inject SCPI Query**;
5. The system receives the response using **UC04: Receive SCPI Query**;
6. The system parses the response into a numeric measurement;
7. The system adds the value to the graph;
8. The graph is updated to show the latest measurement;
9. Steps 4 to 8 repeat until the user stops live plotting;
10. If CSV-based plotting is selected, the user selects a CSV file;
11. The system reads the measurement data from the CSV file;
12. The system plots the stored values on the graph.

### Alternative Flow

#### A1: CSV file is invalid

1. The user selects a CSV file with missing or invalid data;
2. The system notifies the user that the file cannot be plotted;
3. The operation is canceled.

#### A2: Live response is not numeric

1. The system receives a response that cannot be plotted as a number;
2. The system ignores the invalid value;
3. Live plotting continues.

#### A3: User stops plotting

1. The user requests to stop live plotting;
2. The system stops sending periodic queries;
3. The graph remains available for viewing.

#### A4: Communication failure during live plotting

1. The system fails to send a query or receive a response;
2. The system displays an error message;
3. Live plotting is stopped or paused.

### Postconditions

After the execution of this use case, the user can view the measurement data graphically, either from live acquisition or from a previously stored CSV file.

## Business Rules

1. Only numeric data can be plotted as measurement values;
2. Live plotting must respect the configured update interval;
3. The graph should preserve previously collected points while plotting is active;
4. CSV data must contain compatible columns, such as timestamp and value;
5. The system should not freeze the interface while updating the graph.
