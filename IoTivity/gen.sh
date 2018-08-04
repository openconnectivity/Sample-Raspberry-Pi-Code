#!/bin/bash

if [ $# -gt 0 ]; then
FILE1=$1
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/pi-boards/$FILE1/$FILE1.json ./example.json"
eval ${MY_COMMAND}
fi

cd DeviceBuilder
sh ./DeviceBuilder_C++IotivityServer.sh ../example.json  ../device_output "oic.d.light"
cd ..
# copying source code to compile location
cp ./device_output/code/server.cpp ./iotivity/examples/OCFDeviceBuilder/server.cpp
# making executable folder
mkdir -p ./iotivity/out/linux/armv7l/release/examples/OCFDeviceBuilder >/dev/null 2>&1
# copying the introspection file to the executable folder
cp ./device_output/code/server_introspection.dat ./iotivity/out/linux/armv7l/release/examples/OCFDeviceBuilder/server_introspection.dat
# quick fix: using the iotivity supplied oic_svr_db_server_justworks.dat file
cp ./iotivity/resource/csdk/security/provisioning/sample/oic_svr_db_server_justworks.dat     ./iotivity/out/linux/armv7l/release/examples/OCFDeviceBuilder/server_security.dat

if [ $# -gt 0 ]; then
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/IoTivity/$FILE1/$FILE1.cpp ./iotivity/examples/OCFDeviceBuilder/server.cpp"
eval ${MY_COMMAND}
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/IoTivity/$FILE1/SConscript ./iotivity/examples/OCFDeviceBuilder/"
eval ${MY_COMMAND}
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/pi-boards/$FILE1/*.py ./iotivity/out/linux/armv7l/release/examples/OCFDeviceBuilder/"
eval ${MY_COMMAND}
fi
