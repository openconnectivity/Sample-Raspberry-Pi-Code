#!/bin/bash
PROJNAME=$1
MY_COMMAND="mkdir -p ./$PROJNAME"
eval ${MY_COMMAND}
MY_COMMAND="mkdir -p ./$PROJNAME/src"
eval ${MY_COMMAND}
MY_COMMAND="mkdir -p ./$PROJNAME/bin"
eval ${MY_COMMAND}
MY_COMMAND="cp ${OCFPATH}/example.json ./$PROJNAME/$PROJNAME.json"
eval ${MY_COMMAND}
MY_COMMAND="cp ${OCFPATH}/iotivity-constrained/port/linux/Makefile ./$PROJNAME/"
eval ${MY_COMMAND}
