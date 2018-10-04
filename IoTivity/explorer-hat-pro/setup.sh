#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/explorer-hat-pro/SConscript ${CURPWD}/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/explorer-hat-pro/explorer-hat-pro.json ${CURPWD}/${PROJNAME}.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/explorer-hat-pro/explorer-hat-pro-gen.cpp ${CURPWD}/src/${PROJNAME}-gen.cpp
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/explorer-hat-pro/observer.cpp ${CURPWD}/src/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/explorer-hat-pro/observer.h ${CURPWD}/src/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/explorer-hat-pro/namedefs.h ${CURPWD}/src/
mv -i $CURPWD/src/explorer-hat-pro.cpp ${CURPWD}/src/${PROJNAME}.cpp
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/explorer-hat-pro/explorer-hat-pro.py ${CURPWD}/bin/
