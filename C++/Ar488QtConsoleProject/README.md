# AR488 Qt Console

Qt Widgets desktop app for controlling SCPI instruments through an AR488 serial-to-GPIB adapter.

Open either `CMakeLists.txt` or `Ar488QtConsole.pro` in Qt Creator.

## Refactored structure

The project was reorganized to keep the UI, protocol handling, parsing, and small data models separate:

```text
src/
  main.cpp                  Application entry point only
  mainwindow.*              Main Qt window and signal wiring
  ar488controller.*         Worker object for serial/GPIB operations
  core/                     Reusable constants, CSV helpers, SCPI parsing helpers
  models/                   Plain data models such as Equipment and DataLogJob
  protocol/                 AR488 protocol helper and batch script parser
  ui/                       App style and reusable UI widgets
```

## Build notes

The CMake file now explicitly links `Qt6::Charts`, because the graph tab uses Qt Charts. qmake already includes `charts` through `QT += core gui widgets serialport charts`.

