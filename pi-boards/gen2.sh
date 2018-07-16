#!/bin/bash
FILE1=$1
cd DeviceBuilder
MY_COMMAND="sh ./DeviceBuilder_C++IotivityServer.sh ../iotivity/examples/$FILE1/$FILE1.json  ../iotivity/examples/$FILE1/device_output \"oic.d.light\""
eval ${MY_COMMAND}
MY_COMMAND="cd ../iotivity/examples/$FILE1/device_output/code"
eval ${MY_COMMAND}
# copying source code to compile location
cp ./server.cpp ../../
cd ../../../../../
# making executable folder
MY_COMMAND="mkdir -p ./iotivity/out/linux/armv7l/release/examples/$FILE1 >/dev/null 2>&1"
eval ${MY_COMMAND}
# copying the introspection file to the executable folder
MY_COMMAND="cp ./iotivity/examples/$FILE1/device_output/code/server_introspection.dat ./iotivity/out/linux/armv7l/release/examples/$FILE1/"
eval ${MY_COMMAND}
# quick fix: using the iotivity supplied oic_svr_db_server_justworks.dat file
MY_COMMAND="cp ./iotivity/resource/csdk/security/provisioning/sample/oic_svr_db_server_justworks.dat ./iotivity/out/linux/armv7l/release/examples/$FILE1/server_security.dat"
eval ${MY_COMMAND}
