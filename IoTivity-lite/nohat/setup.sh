#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/nohat/devbuildmake ${CURPWD}/devbuildmake
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/nohat/nohat-config.json ${CURPWD}/${PROJNAME}-config.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/nohat/nohat.json ${CURPWD}/${PROJNAME}.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/nohat/nohat.c ${CURPWD}/src/${PROJNAME}.c
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/nohat/nohat-gen.c ${CURPWD}/src/${PROJNAME}-old.c
