#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

MY_COMMAND="cd ${OCFPATH}/iotivity"
eval ${MY_COMMAND}

#TODO change this to compile from the project source direcotry, but temporarily copy the souce code over.
MY_COMMAND="cp $CURPWD/src/$PROJNAME.cpp ${OCFPATH}/iotivity/examples/OCFDeviceBuilder/server.cpp"
eval ${MY_COMMAND}

# copying the SConscript file to the source folder
MY_COMMAND="cp $CURPWD/SConscript ${OCFPATH}/iotivity/examples/OCFDeviceBuilder/"
eval ${MY_COMMAND}

#scons resource/examples
MY_COMMAND="scons examples/OCFDeviceBuilder"
eval ${MY_COMMAND}

#TODO remove this command once the above problem is fixed
MY_COMMAND="cp ${OCFPATH}/iotivity/out/linux/armv7l/release/examples/OCFDeviceBuilder/server /$CURPWD/bin/$PROJNAME"
eval ${MY_COMMAND}

cd $CURPWD
