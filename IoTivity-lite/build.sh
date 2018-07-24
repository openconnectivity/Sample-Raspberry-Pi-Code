#!/bin/bash
if [ $# -gt 0 ]
then
FILE1=$1
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/IoTivity-lite/$FILE1/$FILE1.c ./iotivity-constrained/apps/device_builder_server.c"
eval ${MY_COMMAND}
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/IoTivity-lite/$FILE1/Makefile ./iotivity-constrained/port/linux/"
eval ${MY_COMMAND}
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/pi-boards/$FILE1/*.py ./iotivity-constrained/port/linux/"
eval ${MY_COMMAND}
fi

cd iotivity-constrained/port/linux
#uncomment next line for building without security
#scons examples/OCFDeviceBuilder SECURED=0
#make device_builder_server
make DEBUG=1 DYNAMIC=1 IPV4=1 device_builder_server
cd ../../..
