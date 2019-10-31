#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/mraa-example/devbuildmake ${CURPWD}/devbuildmake
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/mraa-example/mraa-example-config.json ${CURPWD}/${PROJNAME}-config.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/mraa-example/mraa-example.json ${CURPWD}/${PROJNAME}.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/mraa-example/mraa-example.c ${CURPWD}/src/${PROJNAME}.c
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/mraa-example/mraa-example-gen.c ${CURPWD}/src/${PROJNAME}-old.c
