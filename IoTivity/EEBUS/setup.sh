#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/EEBUS/SConscript ${CURPWD}/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/EEBUS/eebus.json ${CURPWD}/${PROJNAME}.json
