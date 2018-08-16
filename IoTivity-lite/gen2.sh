#!/bin/bash
CURPWD=`pwd`
PROJNAME=${PWD##*/}

MY_COMMAND="cd ${OCFPATH}/DeviceBuilder"
eval ${MY_COMMAND}
MY_COMMAND="sh ./DeviceBuilder_IotivityLiteServer.sh $CURPWD/$PROJNAME.json  $CURPWD/device_output \"oic.d.light\""
eval ${MY_COMMAND}

# copying the introspection file to the executable folder
MY_COMMAND="cp $CURPWD/device_output/code/server_introspection.dat.h ${OCFPATH}/iotivity-constrained/include/"
eval ${MY_COMMAND}

MY_COMMAND="mkdir $CURPWD/bin/device_builder_server_creds"
eval ${MY_COMMAND}

if [ -e $CURPWD/src/$PROJNAME.c ]
then
  echo "It appears that you have modified the automatically generated source file. Use a tool like diff3 if you want to merge in any changes."
else
  MY_COMMAND="cp $CURPWD/device_output/code/simpleserver.c $CURPWD/src/$PROJNAME.c"
  eval ${MY_COMMAND}
fi

cd $CURPWD
