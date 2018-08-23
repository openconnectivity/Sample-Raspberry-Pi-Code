#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-phat/Makefile $CURPWD/"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/automation-phat/automation-phat.json $CURPWD/$PROJNAME.json"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-phat/automation-phat.c $CURPWD/src/$PROJNAME.c"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-phat/automation-phat.c.gen $CURPWD/src/$PROJNAME.c.gen"
eval ${MY_COMMAND}
MY_COMMAND="cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/automation-phat/automation-hat.py $CURPWD/bin/"
eval ${MY_COMMAND}
