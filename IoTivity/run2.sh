#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

env LD_LIBRARY_PATH=/home/pi/iot/IOTivity-setup/mraa/build/src
sudo ldconfig
cd ./bin

MY_COMMAND="./$PROJNAME"
eval ${MY_COMMAND}
cd $CURPWD
