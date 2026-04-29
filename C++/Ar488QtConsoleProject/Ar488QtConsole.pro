QT += core gui widgets serialport charts
CONFIG += c++17
TEMPLATE = app
TARGET = Ar488QtConsole

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/ar488controller.cpp

HEADERS += \
    src/mainwindow.h \
    src/ar488controller.h

FORMS +=
