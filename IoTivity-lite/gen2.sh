#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

MY_COMMAND="cd ${OCFPATH}/DeviceBuilder"
eval ${MY_COMMAND}
MY_COMMAND="sh ./DeviceBuilder_IotivityLiteServer.sh $CURPWD/$PROJNAME.json  $CURPWD/device_output \"oic.d.light\""
eval ${MY_COMMAND}

#TODO Add command to merge generated code from DeviceBuilder with modified code in the src directory
MY_COMMAND="cp $CURPWD/device_output/code/simpleserver.c $CURPWD/src/$PROJNAME.c"
eval ${MY_COMMAND}

# copying the introspection file to the executable folder
MY_COMMAND="cp $CURPWD/device_output/code/server_introspection.dat.h ${OCFPATH}/iotivity-constrained/include/"
eval ${MY_COMMAND}

MY_COMMAND="mkdir $CURPWD/bin/device_builder_server_creds"
eval ${MY_COMMAND}

cd $CURPWD
