#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/automation-phat/SConscript ${CURPWD}/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/automation-phat/automation-phat.json ${CURPWD}/${PROJNAME}.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/automation-phat/automation-phat.cpp ${CURPWD}/src/${PROJNAME}.cpp
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/automation-phat/automation-phat-gen.cpp ${CURPWD}/src/${PROJNAME}-gen.cpp
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/automation-phat/automation-hat.py ${CURPWD}/bin/
