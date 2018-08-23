#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

MY_COMMAND="cd ${OCFPATH}/iotivity-constrained/port/linux"
eval ${MY_COMMAND}

#TODO change this to compile from the project source direcotry, but temporarily copy the souce code over.
MY_COMMAND="cp $CURPWD/src/$PROJNAME.c ${OCFPATH}/iotivity-constrained/apps/"
eval ${MY_COMMAND}

# Copying the Makefile file to the executable folder
MY_COMMAND="cp $CURPWD/Makefile ${OCFPATH}/iotivity-constrained/port/linux/"
eval ${MY_COMMAND}

#make resource/examples
MY_COMMAND="make DYNAMIC=1 IPV4=1 $PROJNAME"
eval ${MY_COMMAND}

#TODO remove this command once the above problem is fixed
MY_COMMAND="rm -rf ${OCFPATH}/iotivity-constrained/port/linux/${PROJNAME}_creds"
eval ${MY_COMMAND}
MY_COMMAND="rm ${OCFPATH}/iotivity-constrained/apps/${PROJNAME}.c"
eval ${MY_COMMAND}
MY_COMMAND="mv ./$PROJNAME /$CURPWD/bin/"
eval ${MY_COMMAND}

cd $CURPWD
