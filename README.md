# Sample Code for Raspberry Pi

This repository contains code for creating an IoTivity server running on the Raspberry Pi. The examples include support for three daughter boards from Pimoroni.
- automation-hat: This board has support for several digital inputs and outputs and well as 4 analog to digital converters and 3 relays. There is also support for serial interfaces and a number of LEDs.
- automation-phat: This board uses the same libraries as the Automation Hat, but it has a smaller form factor, only supports one relay and doesn't have any LEDs.
- enviro-phat: The Enviro pHat has several sensors including temperature, pressure, color, acceleration and direction. It also supports 3 analog-to digital converters and has an LED light.
All of these boards use a Python to provide their interface, so the server code includes interface libraries that communicate between C++ (the language for the IoTivity code) and the Pimoroni Python libraries

In addition to the Pimoroni sample code, there are two other samples.
- example: This is the most basic sample which supports a single binary switch. It has no interface to any hardware, but you can see it is working by watching the debug statements on the server monitor screen.
- mraa-example: This is similar to example, but it supports the MRAA libraries to control GPIO pin 7 on the Raspberry Pi. You can see it switch the light by wiring an LED and resistor in series between pin 7 and ground. It turns out this same pin is used on the Enviro pHat board for its light, so this sample will control the light on the Enviro pHat.

# Installation

To install all the samples described above, just type the following command. Typically, this would be installed in the ~/iot directory.

- curl https://openconnectivity.github.io/Sample-Raspberry-Pi-Code/pi-boards/install.sh | bash

This command will do a number of things:

- Clone the Sample-Raspberry-Pi-Code repository (which includes all the sample code described above). Each sample subdirectory includes the following files:
  - SConscript: A scons description file that will manage the compiling and linking of the project.
  - <sample>.json: The JSON device description file that is the input to DeviceBuilder and describes all the resources available on the device. Each of these resources will show up in the source code and introspection file created by DeviceBuilder.
  - <sample>.cpp: The C++ code that includes all the resources plus the code to interface to the hardware. This code can be copied over the C++ code created by DeviceBuilder when the gen.sh script is run in order to control the hardware. If you run gen.sh again, <sample>.cpp will be overwritten. So if you change anything in <sample>.cpp, be sure to make a backup or avoid running gen.sh again. Otherwise, you will lose your work.
  - <sample>.py: The Python code that connects the C++ code to the Pimoroni python libraries. This code will be copied to the executable directory so it will be available at runtime to control the hardware.

One other note:

The PYTHONPATH environment variable needs to be set so that the C++ server code can find the Python interface code. When the curl command is run, the ~/.bashrc file will be modified so that PYTHONPATH is set on bootup. The system will need to be booted in order for ~/.bashrc to be run.
- sudo reboot

# Building and Running the samples

A number of convenience scripts have been written to make the development cycle easier. These should be run from the ~/iot directory.
1. Change to the development directory where the scripts are found:
    1. cd ~/iot
2. Run the following development cycle scripts as needed
    1. ./gen.sh - generate the code, introspection file, PICS file, onboarding file
        1. (optional) To load new a new example: ./gen.sh automation-phat (or whatever example you want) This will copy the correct device description code and generate the .cpp code, introspection file, etc. The currently available examples are: automation-phat, automation-hat, enviro-phat, mraa-example, and example (original simple single switch with no board interface code). It will also copy the completed sample code to run the board hardware and the Python interface code. So this is everything you need to run the sample. If you run gen.sh without a parameter, it will generate new code from the existing example.json device description file in the ~/iot directory. This is useful if you want new clean code to be generated (but it won't run the hardware).
    2. ./build.sh - compile and link everything
    3. ./edit_code.sh - edit the server source code. NOTE: This edits whatever server.cpp (or device_builder_server.c for IoTivity-lite) code is currently active. It does NOT edit the code in the example directories.
    4. ./reset.sh - reset the sever to RFOTM by copying a fresh onboarding file
    5. ./run.sh - run the currently compiled server in the appropriate directory
3. When doing the client control described below, you should see nice debug messages in the server terminal confirming that the messages are getting through.
    1. Discover, onboard and control the server using OTGC
    2. Discover, onboard and control the server using DeviceSpy
    3. Test the server using CTT
