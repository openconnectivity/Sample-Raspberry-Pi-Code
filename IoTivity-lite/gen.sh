#!/bin/bash
if [ $# -gt 0 ]
then
FILE1=$1
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/pi-boards/$FILE1/$FILE1.json ./example.json"
eval ${MY_COMMAND}
fi

cd DeviceBuilder
sh ./DeviceBuilder_IotivityLiteServer.sh ../example.json  ../device_output "oic.d.light"
cd ..
# copying source code to compile location
cp ./device_output/code/simpleserver.c ./iotivity-constrained/apps/device_builder_server.c
cp ./device_output/code/server_introspection.dat.h ./iotivity-constrained/include/server_introspection.dat.h
