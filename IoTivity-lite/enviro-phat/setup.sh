#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/enviro-phat/Makefile $CURPWD/src/"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/enviro-phat/enviro-phat.json $CURPWD/$PROJNAME.json"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/enviro-phat/enviro-phat.c $CURPWD/src/$PROJNAME.c"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/enviro-phat/enviro-phat.py $CURPWD/bin/"
eval ${MY_COMMAND}
