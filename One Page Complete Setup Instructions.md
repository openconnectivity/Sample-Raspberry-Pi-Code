# Set up the OS and connectivity on the Raspberry Pi:
1. Initialize SD card and install Raspian Stretch lite or desktop https://www.raspberrypi.org/downloads/raspbian/
3. Copy empty ssh file to root directory on SD card (will enable ssh on boot). Empty ssh file is in this repository.
4. (optional) For Wi-Fi, copy wpa_supplicant.conf to root directory on SD card (will automatically log in to Wi-Fi on boot). Sample wpa_supplicant.conf file is in this repository. (You will need to edit this file to add your own SSID and password.)
5. Insert the SD card into the Raspberry Pi and connect power. It should boot and connect to the network (either Ethernet or Wi-Fi).
6. Create SSH connection to Raspberry Pi. (Use Connect to Server… menu on Mac) You could also just connect a keyboard and monitor directly to the Raspberry Pi.
    1. If you get the message that it can’t connect due to security, edit ~/.ssh/known_hosts (on the Mac) and delete the line associated with the IP address causing trouble. Or, you can use this command:
        1. ssh-keygen -R [IP address or hostname]
        
# Install IoTivity or IoTivity-lite and its development system (This works on most Linux systems and Windows):
1. Install the development system (for IoTivity, IoTivity-lite or both) using the following commands on the Raspberry Pi (or other development system):
    1. For IoTivity:
        1. curl https://openconnectivity.github.io/IOTivity-setup/install.sh | bash
    2. For IoTivity-lite
        1. curl https://openconnectivity.github.io/IOTivity-Lite-setup/install.sh | bash
2. At this point, we have a working server that will show up on OTGC, implement all the resources and manipulate those resources. Now we just need to add the code to interface with the hardware board. (That’s what we’ll do next.)

# For Raspberry Pi and Pimoroni board control stuff:
1. Install the Pimoroni libraries and examples for the Automation Hat (and pHat) and the Enviro pHat along with the Python interfaces to attach the modified C++ IoTivity code (or modified C IoTivity-lite code) to the Pimoroni Python libraries. This also installs the MRAA example to control the Enviro pHat light directly.:
    1. curl https://openconnectivity.github.io/Sample-Raspberry-Pi-Code/pi-boards/install.sh | bash
    2. source ~/.bashrc

# Development cycle:
1. Change to the development directory where the scripts are found:
    1. cd ~/iot (for IoTivity) or cd ~/iot-lite (for IoTivity-lite)
2. Run the following development cycle scripts as needed
    1. ./gen.sh - generate the code, introspection file, onboarding file, etc.
        1. (optional) To load new a new example: ./gen.sh automation-phat (or whatever example you want) This will copy the correct device description code and generate the .cpp code, introspection file, etc. The currently available examples are: automation-phat, automation-hat, enviro-phat, mraa-example, and example (original simple single switch with no board interface code).
    2. ./build.sh - compile and link everything
    3. ./edit_code.sh - edit the server source code. NOTE: This edits whatever server.cpp code is currently active. It does NOT edit the code in the example directories.
    4. ./reset.sh - reset the sever to RFOTM by copying a fresh onboarding file
    5. ./run.sh - run the currently compiled server in the appropriate directory
3. When doing the client control described below, you should see nice debug messages in the server terminal confirming that the messages are getting through.
    1. Discover, onboard and control the server using OTGC
    2. Discover, onboard and control the server using DeviceSpy
    3. Test the server using CTT
