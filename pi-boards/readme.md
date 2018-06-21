# developer-kit


see also:  https://openconnectivity.org/developer-kit


## automation phat board


The file input-AutomationPhat.json is the DeviceBuilder input file for the
the automation phat board.

https://github.com/openconnectivity/IOTivity-setup/blob/master/pi-boards/input-AutomationPhat.json

Description (from the website):

We've pulled together a great set of features into this home monitoring and automation controller. 
With a relay, analog channels, powered outputs, and buffered inputs (all 24V tolerant) you can now hook up a plethora of goodies to your Raspberry Pi all at once.

https://shop.pimoroni.com/products/automation-phat


| path  |  rt | sensor/actuator | resource | 
| ----- | ----- | -------| -------|
| /ADC1 |  "oic.r.energy.electrical"  |  sensor |  https://oneiota.org/revisions/2732 |
| /ADC2 |  "oic.r.energy.electrical"  |  sensor |  https://oneiota.org/revisions/2732 |
| /ADC3 |  "oic.r.energy.electrical"  |  sensor |  https://oneiota.org/revisions/2732 |
| /ADC4 |  "oic.r.energy.electrical"  |  sensor |  https://oneiota.org/revisions/2732 |
| /output1 |  "oic.r.switch.binary"  |  actuator |  https://oneiota.org/revisions/1393 |
| /output2 |  "oic.r.switch.binary"  |  actuator |  https://oneiota.org/revisions/1393 |
| /output3 |  "oic.r.switch.binary"  |  actuator |  https://oneiota.org/revisions/1393 |
| /input1 |  "oic.r.switch.binary"  |  sensor |  https://oneiota.org/revisions/1393 |
| /input2 |  "oic.r.switch.binary"  |  sensor |  https://oneiota.org/revisions/1393 |
| /input3 |  "oic.r.switch.binary"  |  sensor |  https://oneiota.org/revisions/1393 |
| /relay |  "oic.r.switch.binary"  |  actuator |  https://oneiota.org/revisions/1393 |
| /PowerLight |  "oic.r.switch.binary"  |  actuator |  https://oneiota.org/revisions/1393 |
| /CommsLight |  "oic.r.switch.binary"  |  actuator |  https://oneiota.org/revisions/1393 |
| /WarnLight |  "oic.r.switch.binary"  |  actuator |  https://oneiota.org/revisions/1393 |


note that the implementation needs to make the connection from the resource to the actual hardware.

To install the python libraries:

curl https://get.pimoroni.com/automationhat | bash

## enviro phat board

The file input-EnviroPhat.json is the DeviceBuilder input file for the
the enviro-phat board.

https://github.com/openconnectivity/IOTivity-setup/blob/master/pi-boards/input-EnviroPhat.json

Description (from the website):

Enviro pHAT packs 4 different sensors, letting you measure temperature, pressure, light level, colour, 3-axis motion, compass heading, and analog inputs.

https://shop.pimoroni.com/products/enviro-phat


| path  |  rt | sensor/actuator |  resource | 
| ----- | ----- | -------| -------|
| /binaryswitch |  "oic.r.switch.binary"  |  actuator | https://oneiota.org/revisions/1393 |
| /brightness |  "oic.r.light.brightness"  | sensor | https://oneiota.org/revisions/1393 |
| /colourRGB |  "oic.r.colour.rgb"  | sensor  | https://oneiota.org/revisions/1797 |
| /temperature  | "oic.r.temperature"  | sensor | https://oneiota.org/revisions/2991 | 
| /pressure          |  "oic.r.sensor.atmosphericpressure"    | sensor | https://oneiota.org/revisions/1387 |
| /xmotion  |  "oic.r.sensor.acceleration"  | sensor | https://oneiota.org/revisions/1381 |
| /ymotion  |  "oic.r.sensor.acceleration"  | sensor | https://oneiota.org/revisions/1381 |
| /zmotion  |  "oic.r.sensor.acceleration"  | sensor | https://oneiota.org/revisions/1381 |
| /heading          |  "oic.r.sensor.magneticfielddirection"  | sensor | https://oneiota.org/revisions/2041 |
| /voltage0          |  "oic.r.energy.electrical"  | sensor |  https://oneiota.org/revisions/2732 |
| /voltage1          |  "oic.r.energy.electrical"  |sensor | https://oneiota.org/revisions/2732 |
| /voltage2          |  "oic.r.energy.electrical"  | sensor | https://oneiota.org/revisions/2732 |
| /voltage3          |  "oic.r.energy.electrical"  | sensor | https://oneiota.org/revisions/2732 |

note that the implementation needs to make the connection from the resource to the actual hardware.

To install the python libraries:

curl https://get.pimoroni.com/envirophat | bash

## code examples 
some code examples can be found at:

https://github.com/openconnectivity/IOTivity-setup/tree/master/pi-boards/examplecode
