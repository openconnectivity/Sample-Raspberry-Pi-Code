#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-hat/Makefile ${CURPWD}/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-hat/automation-hat-config.json ${CURPWD}/${PROJNAME}-config.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/automation-hat/automation-hat.json ${CURPWD}/${PROJNAME}.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-hat/automation-hat.c ${CURPWD}/src/${PROJNAME}.c
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-hat/automation-hat-gen.c ${CURPWD}/src/${PROJNAME}-gen.c
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/automation-hat/automation-hat.py ${CURPWD}/bin/
