#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-phat/devbuildmake ${CURPWD}/devbuildmake
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-phat/automation-phat-config.json ${CURPWD}/${PROJNAME}-config.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/automation-phat/automation-phat.json ${CURPWD}/${PROJNAME}.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-phat/automation-phat.c ${CURPWD}/src/${PROJNAME}.c
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/automation-phat/automation-phat-gen.c ${CURPWD}/src/${PROJNAME}-old.c
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/automation-phat/automation-hat.py ${CURPWD}/bin/
