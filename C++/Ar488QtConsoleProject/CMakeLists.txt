cmake_minimum_required(VERSION 3.21)
project(Ar488QtConsole LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)

add_executable(Ar488QtConsole
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/ar488controller.h
    src/ar488controller.cpp
)

target_link_libraries(Ar488QtConsole PRIVATE
    Qt6::Widgets
    Qt6::SerialPort
)
