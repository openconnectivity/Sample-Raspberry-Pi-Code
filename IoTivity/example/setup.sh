#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/IoTivity/example/SConscript $CURPWD/"
eval ${MY_COMMAND}
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/pi-boards/example/example.json $CURPWD/$PROJNAME.json"
eval ${MY_COMMAND}
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/IoTivity/example/example.cpp $CURPWD/src/$PROJNAME.cpp"
eval ${MY_COMMAND}
