#!/bin/bash
set -x #echo on
#############################
#
# copyright 2018 Open Connectivity Foundation, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#############################

cd ~
curl https://get.pimoroni.com/automationhat | bash
curl https://get.pimoroni.com/envirophat | bash

git clone https://github.com/openconnectivity/Sample-Raspberry-Pi-Code.git

cp ~/Sample-Raspberry-Pi-Code/pi-boards/gen.sh ~/iot/
cp ~/Sample-Raspberry-Pi-Code/pi-boards/build.sh ~/iot/

if [[ ! -v PYTHONPATH ]]; then
    export PYTHONPATH='.'
    echo "export PYTHONPATH='.'" >> ~/.bashrc
fi

cd ~/iot
