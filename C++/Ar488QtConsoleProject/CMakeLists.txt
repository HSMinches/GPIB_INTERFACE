cmake_minimum_required(VERSION 3.21)
project(Ar488QtConsole LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort Charts)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/ar488controller.cpp
    src/core/csvutils.cpp
    src/core/scpiutils.cpp
    src/models/datalogjob.cpp
    src/protocol/ar488protocol.cpp
    src/protocol/batchscriptparser.cpp
    src/ui/appstyle.cpp
    src/ui/interactivechartview.cpp
)

set(PROJECT_HEADERS
    src/mainwindow.h
    src/ar488controller.h
    src/core/constants.h
    src/core/csvutils.h
    src/core/scpiutils.h
    src/models/datalogjob.h
    src/models/equipment.h
    src/protocol/ar488protocol.h
    src/protocol/batchscriptparser.h
    src/ui/appstyle.h
    src/ui/interactivechartview.h
)

add_executable(Ar488QtConsole
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

target_include_directories(Ar488QtConsole PRIVATE src)

target_link_libraries(Ar488QtConsole PRIVATE
    Qt6::Widgets
    Qt6::SerialPort
    Qt6::Charts
)
