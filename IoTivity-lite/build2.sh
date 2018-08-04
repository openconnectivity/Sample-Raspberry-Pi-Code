#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

MY_COMMAND="cd ${OCFPATH}/iotivity-constrained/port/linux"
eval ${MY_COMMAND}

#TODO change this to compile from the project source direcotry, but temporarily copy the souce code over.
MY_COMMAND="cp $CURPWD/src/$PROJNAME.c ${OCFPATH}/iotivity-constrained/apps/device_builder_server.c"
eval ${MY_COMMAND}

#scons resource/examples
MY_COMMAND="make DYNAMIC=1 IPV4=1 device_builder_server"
eval ${MY_COMMAND}

#TODO remove this command once the above problem is fixed
MY_COMMAND="cp ./device_builder_server /$CURPWD/bin/$PROJNAME"
eval ${MY_COMMAND}

cd $CURPWD
