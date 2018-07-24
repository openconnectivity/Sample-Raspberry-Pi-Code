#!/bin/bash
FILE1=$1

cd iotivity
#scons resource/examples
MY_COMMAND="scons examples/$FILE1"
eval ${MY_COMMAND}
cd ..
