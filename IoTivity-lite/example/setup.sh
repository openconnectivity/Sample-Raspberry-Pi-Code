#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/example/Makefile ${CURPWD}/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/example/example.json ${CURPWD}/${PROJNAME}.json
