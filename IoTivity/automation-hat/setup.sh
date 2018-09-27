#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/automation-hat/SConscript ${CURPWD}/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/automation-hat/automation-hat.json ${CURPWD}/${PROJNAME}.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/automation-hat/automation-hat.cpp ${CURPWD}/src/${PROJNAME}.cpp
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/automation-hat/automation-hat-gen.cpp ${CURPWD}/src/${PROJNAME}-gen.cpp
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/automation-hat/automation-hat.py ${CURPWD}/bin/
