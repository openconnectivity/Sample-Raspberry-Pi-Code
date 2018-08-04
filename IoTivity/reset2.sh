#!/bin/bash
rm -f ./bin/server_security.dat
#cp ./device_output/code/server_security.dat ./iotivity/out/linux/armv7l/release/examples/OCFDeviceBuilder/server_security.dat
MY_COMMAND="cp ${OCFPATH}/iotivity/resource/csdk/security/provisioning/sample/oic_svr_db_server_justworks.dat ./bin/server_security.dat"
eval ${MY_COMMAND}

