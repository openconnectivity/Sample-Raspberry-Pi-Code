#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

MY_COMMAND="cd ${OCFPATH}/DeviceBuilder"
eval ${MY_COMMAND}
MY_COMMAND="sh ./DeviceBuilder_C++IotivityServer.sh $CURPWD/$PROJNAME.json  $CURPWD/device_output \"oic.d.light\""
eval ${MY_COMMAND}

#TODO Add command to merge generated code from DeviceBuilder with modified code in the src directory
MY_COMMAND="cp $CURPWD/device_output/code/server.cpp $CURPWD/src/$PROJNAME.cpp"
eval ${MY_COMMAND}

# copying the introspection file to the executable folder
MY_COMMAND="cp $CURPWD/device_output/code/server_introspection.dat $CURPWD/bin/"
eval ${MY_COMMAND}
# quick fix: using the iotivity supplied oic_svr_db_server_justworks.dat file
MY_COMMAND="cp ${OCFPATH}/iotivity/resource/csdk/security/provisioning/sample/oic_svr_db_server_justworks.dat $CURPWD/bin/server_security.dat"
eval ${MY_COMMAND}
cd $CURPWD
