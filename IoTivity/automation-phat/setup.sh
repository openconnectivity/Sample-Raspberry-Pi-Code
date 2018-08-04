#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/IoTivity/automation-phat/SConscript $CURPWD/"
eval ${MY_COMMAND}
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/pi-boards/automation-phat/automation-phat.json $CURPWD/$PROJNAME.json"
eval ${MY_COMMAND}
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/IoTivity/automation-phat/automation-phat.cpp $CURPWD/src/$PROJNAME.cpp"
eval ${MY_COMMAND}
MY_COMMAND="cp ~/Sample-Raspberry-Pi-Code/pi-boards/automation-phat/automation-hat.py $CURPWD/bin/"
eval ${MY_COMMAND}
