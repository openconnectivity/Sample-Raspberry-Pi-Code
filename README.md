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

To install all the samples described above, just type the following command. Typically, this would be installed in the HOME (~) directory.

- curl https://openconnectivity.github.io/Sample-Raspberry-Pi-Code/pi-boards/install.sh | bash

This command will do a number of things:

- Clone the Sample-Raspberry-Pi-Code repository (which includes all the sample code described above for both IoTivity and IoTivity-lite). Each sample subdirectory includes the following files:
  - SConscript: A scons description file that will manage the compiling and linking of the project.
  - <sample>.json: The JSON device description file that is the input to DeviceBuilder and describes all the resources available on the device. Each of these resources will show up in the source code and introspection file created by DeviceBuilder.
  - <sample>.cpp: The C++ code that includes all the resources plus the code to interface to the hardware.
  - <sample>.py: The Python code that connects the C++ code to the Pimoroni python libraries. This code will be copied to the executable directory so it will be available at runtime to control the hardware.

One other note:

The PYTHONPATH environment variable needs to be set so that the C++ server code can find the Python interface code. When the curl command is run, the ~/.bashrc file will be modified so that PYTHONPATH is set on bootup. The system will need to be booted in order for ~/.bashrc to be run.
- sudo reboot

# Setting the OCF Implementation to use

- The following variables MUST be set to point to the directories related to the OCF implementation you want to use in order to run the convenience scripts described below.
  - OCFPATH - This is the absolute directory where the OCF implementation is installed and where all the convenience scripts are found. (This is typically /home/my_user_name/iot or /home/my_user_name/iot-lite)
  - OCFSUBPATH - This is just the final part of OCFPATH, but it is necessary as a way to get the set_implementaion.sh script to work correctly. (This is typically /iot or /iot-lite)
  - PATH - This is the normal search PATH, but it will have OCFPATH prepended to it so the scripts can be found.

- source set_implementation.sh (/iot or /iot-lite) - This will switch the implementation of OCF to use (IoTivity or IoTivity-lite right now). It sets some environment variables that MUST be set for all of the scripts. IT IS CRITICAL to use the "source" command to run this script. That makes it work in the current bash context rather that a temporary one.

# Building and Running Projects

A number of convenience scripts have been written to make the development cycle easier.
1. Run the following development cycle as needed
    1. create_project project_name - create a new project and name it anything you want.
    2. This isn't a script, but you need to "cd project_name" to run all the other scripts.
    3. Copy the setup.sh from the directory of the sample you want to install into the current project directory. (In the Sample-Raspberry-Pi-Code repository)
    4. ./setup.sh - This will load all the necessary stuff to build and run the sample project.
    5. edit_input2.sh - edit the device description input file (project_name.json) if necessary.
    6. gen2.sh - generate the code, introspection file, PICS file, and onboarding file from the device description file.
    7. build2.sh - compile and link everything
    8. edit_code2.sh - edit the server source code if necessary.
    9. reset2.sh - reset the sever to RFOTM by copying a fresh onboarding file
    10. run2.sh - run the currently compiled server in the appropriate directory
2. To test the project, you will need to run a client. Here are some options.
    1. Discover, onboard and control the server using OTGC
    2. Discover, onboard and control the server using DeviceSpy
    3. Test the server using CTT
