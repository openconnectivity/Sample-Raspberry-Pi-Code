#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/explorer-hat-pro-example/SConscript ${CURPWD}/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/explorer-hat-pro-example/explorer-hat-pro-example-config.json ${CURPWD}/${PROJNAME}-config.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/explorer-hat-pro-example/explorer-hat-pro-example.json ${CURPWD}/${PROJNAME}.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/explorer-hat-pro-example/explorer-hat-pro-example.cpp ${CURPWD}/src/${PROJNAME}.cpp
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/explorer-hat-pro-example/explorer-hat-pro-example-gen.cpp ${CURPWD}/src/${PROJNAME}-gen.cpp
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/explorer-hat-pro-example/observer.cpp ${CURPWD}/src/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/explorer-hat-pro-example/observer.h ${CURPWD}/src/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/explorer-hat-pro-example/namedefs.h ${CURPWD}/src/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/explorer-hat-pro-example/explorer-hat-pro.py ${CURPWD}/bin/
