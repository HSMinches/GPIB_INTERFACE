# GPIB_INTERFACE 

## Description
Development of a GPIB (General Purpose Interface Bus) based interface to communicate with benchtop lab equipment through SCPI commands.

As basys, this project consists of C++ with QT interface, to be able to generate SCPI queries for multiples workbench instruments, thus automating measurements workflow.

In this case, the interface will use a USB-GPIB adapter "AR488", like shown below, although there can be other variations like e.g. ethernet-GPIB, this will be the cheapest and quickest development path to be taken. 

For more info about the adapter, check out this repo: https://github.com/Twilight-Logic/AR488

