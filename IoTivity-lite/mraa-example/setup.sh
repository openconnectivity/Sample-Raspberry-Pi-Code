#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/mraa-example/Makefile $CURPWD/src/"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/mraa-example/mraa-example.json $CURPWD/$PROJNAME.json"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/mraa-example/mraa-example.c $CURPWD/src/$PROJNAME.c"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/mraa-example/mraa-example.c.gen $CURPWD/src/$PROJNAME.c.gen"
eval ${MY_COMMAND}
