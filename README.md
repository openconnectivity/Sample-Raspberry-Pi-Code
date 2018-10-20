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
  - SConscript or Makefile: A scons (or Makefile) description file that will manage the compiling and linking of the project.
  - <sample>.json: The JSON device description file that is the input to DeviceBuilder and describes all the resources available on the device. Each of these resources will show up in the source code and introspection file created by DeviceBuilder.
  - <sample>.cpp (or .c): The C++ code that includes all the resources plus the code to interface to the hardware. If necessary, .h files needed to support the sample code are also included.
  - <sample>.py: The Python code that connects the C++ code to the Pimoroni python libraries. This code will be copied to the executable directory so it will be available at runtime to control the hardware.

One other note:

The PYTHONPATH environment variable needs to be set so that the C++ server code can find the Python interface code. When the curl command is run, the ~/.bashrc file will be modified so that PYTHONPATH is set on bootup. The system will need to be booted in order for ~/.bashrc to be run.
- sudo reboot

# Setting the PATH to find the Scripts

The scripts are normally stored in the Project-Scripts directory under the HOME directory, but just add the Project-Scripts directory to the PATH. The ~/.bashrc file should have been modified provide the correct path every time you log in, but to get the PATH fixed immediately, the following command should do it:

- source ~/.bashrc

This should also work:

- cd ~/Project-Scripts
- source ./set_path.sh

# Building and Running Projects

With the PATH set, the following tool chain should work.

A number of convenience scripts have been written to make the development cycle easier.
1. Run the following development cycle as needed
    1. create_project project_name - Create a new project in a new directory directly under the current directory (working in a development directory (e.g. cd ~/workspace) is a good way to organize projects) and name it anything you want.
    2. This isn't a script, but you need to "cd project_name" to run all the other scripts.
    3. (optional) Copy the setup.sh from the directory of the sample you want to install into the current project directory.
    4. (optional) ./setup.sh - This will load all the necessary stuff to build and run the particular sample project.
    5. edit_config.sh - Edit the project configuration file (project_name-config.json) if necessary. (e.g. to change the IoTivity version or target OS). NOTE: Temporarily, only the first entry in the configuration implementation and platform arrays is used.
    6. edit_input.sh - Edit the input file (e.g. example.json). This is for convenience only as the input file is simply created from the config file.
    7. gen.sh - Generate the code, introspection file, PICS file, and onboarding file from the device description file. The IoTivity version specified in the config file will be used.
    8. build.sh - Compile and link everything. The IoTivity version in the config file will be used.
    9. edit_code.sh - Edit the server source code if necessary.
    10. reset.sh - Reset the sever to RFOTM.
    11. run.sh - Run the currently compiled server in the appropriate directory.
2. To test the project, you will need to run a client. Here are some options.
    1. Discover, onboard and control the server using OTGC.
    2. Discover, onboard and control the server using DeviceSpy.
    3. Test the server using CTT.
