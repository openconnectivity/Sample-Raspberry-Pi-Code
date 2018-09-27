#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/explorer-hat-pro/Makefile ${CURPWD}/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/explorer-hat-pro/explorer-hat-pro.json ${CURPWD}/${PROJNAME}.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/explorer-hat-pro/explorer-hat-pro.c ${CURPWD}/src/${PROJNAME}.c
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity-lite/explorer-hat-pro/explorer-hat-pro-gen.c ${CURPWD}/src/${PROJNAME}-gen.c
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/explorer-hat-pro/explorer-hat-pro.py ${CURPWD}/bin/
