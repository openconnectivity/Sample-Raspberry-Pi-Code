#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/tutorial/SConscript ${CURPWD}/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/tutorial/tutorial-config.json ${CURPWD}/${PROJNAME}-config.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/tutorial/tutorial.json ${CURPWD}/${PROJNAME}.json
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/tutorial/tutorial-gen.cpp ${CURPWD}/src/${PROJNAME}-gen.cpp
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/tutorial/*.cpp ${CURPWD}/src/
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/IoTivity/tutorial/*.h ${CURPWD}/src/
mv -i $CURPWD/src/tutorial.cpp ${CURPWD}/src/${PROJNAME}.cpp
cp ${EXAMPLEPATH}/Sample-Raspberry-Pi-Code/pi-boards/enviro-phat/enviro-phat.py ${CURPWD}/bin/
