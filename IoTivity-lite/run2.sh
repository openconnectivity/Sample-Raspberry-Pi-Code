#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

cd ./bin

MY_COMMAND="./$PROJNAME"
eval ${MY_COMMAND}
cd $CURPWD
