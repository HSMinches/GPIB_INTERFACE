QT += core gui widgets serialport charts
CONFIG += c++17
TEMPLATE = app
TARGET = Ar488QtConsole

INCLUDEPATH += src

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/ar488controller.cpp \
    src/core/csvutils.cpp \
    src/core/scpiutils.cpp \
    src/models/datalogjob.cpp \
    src/protocol/ar488protocol.cpp \
    src/protocol/batchscriptparser.cpp \
    src/ui/appstyle.cpp \
    src/ui/interactivechartview.cpp

HEADERS += \
    src/mainwindow.h \
    src/ar488controller.h \
    src/core/constants.h \
    src/core/csvutils.h \
    src/core/scpiutils.h \
    src/models/datalogjob.h \
    src/models/equipment.h \
    src/protocol/ar488protocol.h \
    src/protocol/batchscriptparser.h \
    src/ui/appstyle.h \
    src/ui/interactivechartview.h

FORMS +=
