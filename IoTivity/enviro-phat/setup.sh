#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/enviro-phat/SConscript ${CURPWD}/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/enviro-phat/enviro-phat.json ${CURPWD}/${PROJNAME}.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/enviro-phat/enviro-phat-gen.cpp ${CURPWD}/src/${PROJNAME}-gen.cpp
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/enviro-phat/*.cpp ${CURPWD}/src/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/enviro-phat/*.h ${CURPWD}/src/
mv -i $CURPWD/src/enviro-phat.cpp ${CURPWD}/src/${PROJNAME}.cpp
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/enviro-phat/enviro-phat.py ${CURPWD}/bin/
