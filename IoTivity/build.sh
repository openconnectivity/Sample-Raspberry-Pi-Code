#!/bin/bash

if [ $# -gt 0 ]
then
FILE1=$1
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/IoTivity/$FILE1/$FILE1.cpp ./iotivity/examples/OCFDeviceBuilder/server.cpp"
eval ${MY_COMMAND}
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/IoTivity/$FILE1/SConscript ./iotivity/examples/OCFDeviceBuilder/"
eval ${MY_COMMAND}
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/pi-boards/$FILE1/*.py ./iotivity/out/linux/armv7l/release/examples/OCFDeviceBuilder/"
eval ${MY_COMMAND}
fi

cd iotivity
#scons resource/examples
scons examples/OCFDeviceBuilder
cd ..
