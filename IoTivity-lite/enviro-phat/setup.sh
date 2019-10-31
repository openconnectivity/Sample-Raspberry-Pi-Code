#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/enviro-phat/devbuildmake ${CURPWD}/devbuildmake
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/enviro-phat/enviro-phat-config.json ${CURPWD}/${PROJNAME}-config.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/enviro-phat/enviro-phat.json ${CURPWD}/${PROJNAME}.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/enviro-phat/enviro-phat.c ${CURPWD}/src/${PROJNAME}.c
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/enviro-phat/enviro-phat-gen.c ${CURPWD}/src/${PROJNAME}-old.c
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/enviro-phat/enviro-phat.py ${CURPWD}/bin/
