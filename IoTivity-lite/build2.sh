#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

MY_COMMAND="cd ${OCFPATH}/iotivity"
eval ${MY_COMMAND}

#TODO change this to compile from the project source direcotry, but temporarily copy the souce code over.
MY_COMMAND="mkdir ${OCFPATH}/iotivity/examples/$PROJNAME"
eval ${MY_COMMAND}
MY_COMMAND="cp $CURPWD/src/$PROJNAME.cpp ${OCFPATH}/iotivity/examples/$PROJNAME/server.cpp"
eval ${MY_COMMAND}

#scons resource/examples
MY_COMMAND="scons examples/$PROJNAME"
eval ${MY_COMMAND}

#TODO remove this command once the above problem is fixed
MY_COMMAND="cp ${OCFPATH}/iotivity/out/linux/armv7l/release/examples/OCFDeviceBuilder/server /$CURPWD/bin/$PROJNAME"
eval ${MY_COMMAND}
MY_COMMAND="cp ${OCFPATH}/iotivity/out/linux/armv7l/release/examples/OCFDeviceBuilder/server_introspection.dat /$CURPWD/bin/"
eval ${MY_COMMAND}
MY_COMMAND="cp ${OCFPATH}/iotivity/out/linux/armv7l/release/examples/OCFDeviceBuilder/server_security.dat /$CURPWD/bin/"
eval ${MY_COMMAND}

cd $CURPWD
