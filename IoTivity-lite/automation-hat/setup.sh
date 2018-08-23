#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-hat/Makefile $CURPWD/"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/automation-hat/automation-hat.json $CURPWD/$PROJNAME.json"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-hat/automation-hat.c $CURPWD/src/$PROJNAME.c"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-hat/automation-hat.c.gen $CURPWD/src/$PROJNAME.c.gen"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/automation-hat/automation-hat.py $CURPWD/bin/"
eval ${MY_COMMAND}
