#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-phat-example/Makefile ${CURPWD}/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-phat-example/automation-phat-example-config.json ${CURPWD}/${PROJNAME}-config.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/automation-phat-example/automation-phat-example.json ${CURPWD}/${PROJNAME}.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-phat-example/automation-phat-example.c ${CURPWD}/src/${PROJNAME}.c
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-phat-example/automation-phat-example-gen.c ${CURPWD}/src/${PROJNAME}-gen.c
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/automation-phat-example/automation-hat.py ${CURPWD}/bin/
