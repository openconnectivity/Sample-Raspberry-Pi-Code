#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp -f ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/example/devbuildmake ${CURPWD}/devbuildmake
cp -f ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/example/example-config.json ${CURPWD}/${PROJNAME}-config.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/example/example.json ${CURPWD}/${PROJNAME}.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/example/example.c ${CURPWD}/src/${PROJNAME}.c
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/example/example-gen.c ${CURPWD}/src/${PROJNAME}-old.c
