#!/bin/bash
PROJNAME=$1
MY_COMMAND="mkdir -p ./iotivity/examples/$PROJNAME"
eval ${MY_COMMAND}
MY_COMMAND="cp ./example.json ./iotivity/examples/$PROJNAME/$PROJNAME.json"
eval ${MY_COMMAND}
MY_COMMAND="cp ./iotivity/examples/OCFDeviceBuilder/SConscript ./iotivity/examples/$PROJNAME/"
eval ${MY_COMMAND}
