/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Copyright 2017-2019 Open Connectivity Foundation
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

/* Application Design
*
* support functions:
* app_init
*  initializes the oic/p and oic/d values.
* register_resources
*  function that registers all endpoints, e.g. sets the RETRIEVE/UPDATE handlers for each end point
*
* main 
*  starts the stack, with the registered resources.
*
* Each resource has:
*  global property variables (per resource path) for:
*    the property name
*       naming convention: g_<path>_RESOURCE_PROPERTY_NAME_<propertyname>
*    the actual value of the property, which is typed from the json data type
*      naming convention: g_<path>_<propertyname>
*  global resource variables (per path) for:
*    the path in a variable:
*      naming convention: g_<path>_RESOURCE_ENDPOINT
*    array of interfaces, where by the first will be set as default interface
*      naming convention g_<path>_RESOURCE_INTERFACE
*
*  handlers for the implemented methods (get/post)
*   get_<path>
*     function that is being called when a RETRIEVE is called on <path>
*     set the global variables in the output
*   post_<path>
*     function that is being called when a UPDATE is called on <path>
*     checks the input data
*     if input data is correct
*       updates the global variables
*
*/
/*
 tool_version          : 20171123
 input_file            : /home/pi/workspace/test2/device_output/out_codegeneration_merged.swagger.json
 version of input_file : 20190215
 title of input_file   : ExplorerHatPro
*/

#include "oc_api.h"
#include "port/oc_clock.h"
#include <signal.h>

#ifdef __linux__
/* linux specific code */
#include <pthread.h>
static pthread_mutex_t mutex;
static pthread_cond_t cv;
static struct timespec ts;
#endif

#ifdef WIN32
/* windows specific code */
#include <windows.h>
static CONDITION_VARIABLE cv;   /* event loop variable */
static CRITICAL_SECTION cs;     /* event loop variable */
#endif

#define btoa(x) ((x)?"true":"false")

#define MAX_STRING 30           /* max size of the strings. */
#define MAX_PAYLOAD_STRING 65   /* max size strings in the payload */
#define MAX_ARRAY 10            /* max size of the array */
/* Note: Magic numbers are derived from the resource definition, either from the example or the definition.*/

volatile int quit = 0;          /* stop variable, used by handle_signal */


/* global property variables for path: "/analog1" */
static char g_analog1_RESOURCE_PROPERTY_NAME_current[] = "current"; /* the name for the attribute */
double g_analog1_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
static char g_analog1_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; /* the name for the attribute */
double g_analog1_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
static char g_analog1_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; /* the name for the attribute */
double g_analog1_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
static char g_analog1_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; /* the name for the attribute */
double g_analog1_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
static char g_analog1_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; /* the name for the attribute */
double g_analog1_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
static char g_analog1_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; /* the name for the attribute */
double g_analog1_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
/* global property variables for path: "/analog2" */
static char g_analog2_RESOURCE_PROPERTY_NAME_current[] = "current"; /* the name for the attribute */
double g_analog2_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
static char g_analog2_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; /* the name for the attribute */
double g_analog2_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
static char g_analog2_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; /* the name for the attribute */
double g_analog2_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
static char g_analog2_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; /* the name for the attribute */
double g_analog2_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
static char g_analog2_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; /* the name for the attribute */
double g_analog2_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
static char g_analog2_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; /* the name for the attribute */
double g_analog2_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
/* global property variables for path: "/analog3" */
static char g_analog3_RESOURCE_PROPERTY_NAME_current[] = "current"; /* the name for the attribute */
double g_analog3_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
static char g_analog3_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; /* the name for the attribute */
double g_analog3_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
static char g_analog3_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; /* the name for the attribute */
double g_analog3_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
static char g_analog3_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; /* the name for the attribute */
double g_analog3_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
static char g_analog3_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; /* the name for the attribute */
double g_analog3_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
static char g_analog3_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; /* the name for the attribute */
double g_analog3_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
/* global property variables for path: "/analog4" */
static char g_analog4_RESOURCE_PROPERTY_NAME_current[] = "current"; /* the name for the attribute */
double g_analog4_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
static char g_analog4_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; /* the name for the attribute */
double g_analog4_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
static char g_analog4_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; /* the name for the attribute */
double g_analog4_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
static char g_analog4_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; /* the name for the attribute */
double g_analog4_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
static char g_analog4_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; /* the name for the attribute */
double g_analog4_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
static char g_analog4_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; /* the name for the attribute */
double g_analog4_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
/* global property variables for path: "/input1" */
static char g_input1_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_input1_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/input2" */
static char g_input2_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_input2_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/input3" */
static char g_input3_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_input3_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/input4" */
static char g_input4_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_input4_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/light1" */
static char g_light1_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_light1_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/light2" */
static char g_light2_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_light2_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/light3" */
static char g_light3_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_light3_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/light4" */
static char g_light4_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_light4_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/output1" */
static char g_output1_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_output1_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/output2" */
static char g_output2_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_output2_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/output3" */
static char g_output3_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_output3_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/output4" */
static char g_output4_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_output4_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/touch1" */
static char g_touch1_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_touch1_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. */
/* global property variables for path: "/touch2" */
static char g_touch2_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_touch2_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. */
/* global property variables for path: "/touch3" */
static char g_touch3_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_touch3_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. */
/* global property variables for path: "/touch4" */
static char g_touch4_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_touch4_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. */
/* global property variables for path: "/touch5" */
static char g_touch5_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_touch5_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. */
/* global property variables for path: "/touch6" */
static char g_touch6_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_touch6_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. */
/* global property variables for path: "/touch7" */
static char g_touch7_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_touch7_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. */
/* global property variables for path: "/touch8" */
static char g_touch8_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_touch8_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. *//* registration data variables for the resources */

/* global resource variables for path: /analog1 */
static char g_analog1_RESOURCE_ENDPOINT[] = "/analog1"; /* used path for this resource */
static char g_analog1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; /* rt value (as an array) */
int g_analog1_nr_resource_types = 1;
static char g_analog1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_analog1_nr_resource_interfaces = 2;

/* global resource variables for path: /analog2 */
static char g_analog2_RESOURCE_ENDPOINT[] = "/analog2"; /* used path for this resource */
static char g_analog2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; /* rt value (as an array) */
int g_analog2_nr_resource_types = 1;
static char g_analog2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_analog2_nr_resource_interfaces = 2;

/* global resource variables for path: /analog3 */
static char g_analog3_RESOURCE_ENDPOINT[] = "/analog3"; /* used path for this resource */
static char g_analog3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; /* rt value (as an array) */
int g_analog3_nr_resource_types = 1;
static char g_analog3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_analog3_nr_resource_interfaces = 2;

/* global resource variables for path: /analog4 */
static char g_analog4_RESOURCE_ENDPOINT[] = "/analog4"; /* used path for this resource */
static char g_analog4_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; /* rt value (as an array) */
int g_analog4_nr_resource_types = 1;
static char g_analog4_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_analog4_nr_resource_interfaces = 2;

/* global resource variables for path: /input1 */
static char g_input1_RESOURCE_ENDPOINT[] = "/input1"; /* used path for this resource */
static char g_input1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_input1_nr_resource_types = 1;
static char g_input1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_input1_nr_resource_interfaces = 2;

/* global resource variables for path: /input2 */
static char g_input2_RESOURCE_ENDPOINT[] = "/input2"; /* used path for this resource */
static char g_input2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_input2_nr_resource_types = 1;
static char g_input2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_input2_nr_resource_interfaces = 2;

/* global resource variables for path: /input3 */
static char g_input3_RESOURCE_ENDPOINT[] = "/input3"; /* used path for this resource */
static char g_input3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_input3_nr_resource_types = 1;
static char g_input3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_input3_nr_resource_interfaces = 2;

/* global resource variables for path: /input4 */
static char g_input4_RESOURCE_ENDPOINT[] = "/input4"; /* used path for this resource */
static char g_input4_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_input4_nr_resource_types = 1;
static char g_input4_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_input4_nr_resource_interfaces = 2;

/* global resource variables for path: /light1 */
static char g_light1_RESOURCE_ENDPOINT[] = "/light1"; /* used path for this resource */
static char g_light1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_light1_nr_resource_types = 1;
static char g_light1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_light1_nr_resource_interfaces = 2;

/* global resource variables for path: /light2 */
static char g_light2_RESOURCE_ENDPOINT[] = "/light2"; /* used path for this resource */
static char g_light2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_light2_nr_resource_types = 1;
static char g_light2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_light2_nr_resource_interfaces = 2;

/* global resource variables for path: /light3 */
static char g_light3_RESOURCE_ENDPOINT[] = "/light3"; /* used path for this resource */
static char g_light3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_light3_nr_resource_types = 1;
static char g_light3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_light3_nr_resource_interfaces = 2;

/* global resource variables for path: /light4 */
static char g_light4_RESOURCE_ENDPOINT[] = "/light4"; /* used path for this resource */
static char g_light4_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_light4_nr_resource_types = 1;
static char g_light4_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_light4_nr_resource_interfaces = 2;

/* global resource variables for path: /output1 */
static char g_output1_RESOURCE_ENDPOINT[] = "/output1"; /* used path for this resource */
static char g_output1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_output1_nr_resource_types = 1;
static char g_output1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_output1_nr_resource_interfaces = 2;

/* global resource variables for path: /output2 */
static char g_output2_RESOURCE_ENDPOINT[] = "/output2"; /* used path for this resource */
static char g_output2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_output2_nr_resource_types = 1;
static char g_output2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_output2_nr_resource_interfaces = 2;

/* global resource variables for path: /output3 */
static char g_output3_RESOURCE_ENDPOINT[] = "/output3"; /* used path for this resource */
static char g_output3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_output3_nr_resource_types = 1;
static char g_output3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_output3_nr_resource_interfaces = 2;

/* global resource variables for path: /output4 */
static char g_output4_RESOURCE_ENDPOINT[] = "/output4"; /* used path for this resource */
static char g_output4_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_output4_nr_resource_types = 1;
static char g_output4_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_output4_nr_resource_interfaces = 2;

/* global resource variables for path: /touch1 */
static char g_touch1_RESOURCE_ENDPOINT[] = "/touch1"; /* used path for this resource */
static char g_touch1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; /* rt value (as an array) */
int g_touch1_nr_resource_types = 1;
static char g_touch1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_touch1_nr_resource_interfaces = 2;

/* global resource variables for path: /touch2 */
static char g_touch2_RESOURCE_ENDPOINT[] = "/touch2"; /* used path for this resource */
static char g_touch2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; /* rt value (as an array) */
int g_touch2_nr_resource_types = 1;
static char g_touch2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_touch2_nr_resource_interfaces = 2;

/* global resource variables for path: /touch3 */
static char g_touch3_RESOURCE_ENDPOINT[] = "/touch3"; /* used path for this resource */
static char g_touch3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; /* rt value (as an array) */
int g_touch3_nr_resource_types = 1;
static char g_touch3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_touch3_nr_resource_interfaces = 2;

/* global resource variables for path: /touch4 */
static char g_touch4_RESOURCE_ENDPOINT[] = "/touch4"; /* used path for this resource */
static char g_touch4_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; /* rt value (as an array) */
int g_touch4_nr_resource_types = 1;
static char g_touch4_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_touch4_nr_resource_interfaces = 2;

/* global resource variables for path: /touch5 */
static char g_touch5_RESOURCE_ENDPOINT[] = "/touch5"; /* used path for this resource */
static char g_touch5_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; /* rt value (as an array) */
int g_touch5_nr_resource_types = 1;
static char g_touch5_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_touch5_nr_resource_interfaces = 2;

/* global resource variables for path: /touch6 */
static char g_touch6_RESOURCE_ENDPOINT[] = "/touch6"; /* used path for this resource */
static char g_touch6_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; /* rt value (as an array) */
int g_touch6_nr_resource_types = 1;
static char g_touch6_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_touch6_nr_resource_interfaces = 2;

/* global resource variables for path: /touch7 */
static char g_touch7_RESOURCE_ENDPOINT[] = "/touch7"; /* used path for this resource */
static char g_touch7_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; /* rt value (as an array) */
int g_touch7_nr_resource_types = 1;
static char g_touch7_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_touch7_nr_resource_interfaces = 2;

/* global resource variables for path: /touch8 */
static char g_touch8_RESOURCE_ENDPOINT[] = "/touch8"; /* used path for this resource */
static char g_touch8_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; /* rt value (as an array) */
int g_touch8_nr_resource_types = 1;
static char g_touch8_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_touch8_nr_resource_interfaces = 2;
/**
* function to set up the device.
*
*/
static int
app_init(void)
{
  int ret = oc_init_platform("ocf", NULL, NULL);
  /* the settings determine the appearance of the device on the network
     can be OCF1.3.1 or OCF2.0.0 (or even higher)
     supplied values are for OCF1.3.1 */
  ret |= oc_add_device("/oic/d", "oic.d.explorerhatpro", "ExplorerHatPro", 
                       "ocf.1.0.0", /* icv value */
                       "ocf.res.1.3.0, ocf.sh.1.3.0",  /* dmv value */
                       NULL, NULL);
  return ret;
}

/**
* helper function to convert the interface string definition to the constant defintion used by the stack.
* @param interface the interface string e.g. "oic.if.a"
* @return the stack constant for the interface
*/
static int
convert_if_string(char *interface_name)
{
  if (strcmp(interface_name, "oic.if.baseline") == 0) return OC_IF_BASELINE;  /* baseline interface */
  if (strcmp(interface_name, "oic.if.rw") == 0) return OC_IF_RW;              /* read write interface */
  if (strcmp(interface_name, "oic.if.r" )== 0) return OC_IF_R;                /* read interface */
  if (strcmp(interface_name, "oic.if.s") == 0) return OC_IF_S;                /* sensor interface */
  if (strcmp(interface_name, "oic.if.a") == 0) return OC_IF_A;                /* actuator interface */
  if (strcmp(interface_name, "oic.if.b") == 0) return OC_IF_B;                /* batch interface */
  if (strcmp(interface_name, "oic.if.ll") == 0) return OC_IF_LL;              /* linked list interface */
  return OC_IF_A;
}

/**
* helper function to check if the POST input document contains 
* the common readOnly properties or the resouce readOnly properties
* @param name the name of the property
* @return the error_status, e.g. if error_status is true, then the input document contains something illegal
*/
static bool
check_on_readonly_common_resource_properties(oc_string_t name, bool error_state)
{
  if (strcmp ( oc_string(name), "n") == 0) {
    error_state = true;
    PRINT ("   property \"n\" is ReadOnly \n");
  }else if (strcmp ( oc_string(name), "if") == 0) {
    error_state = true;
    PRINT ("   property \"if\" is ReadOnly \n");
  } else if (strcmp ( oc_string(name), "rt") == 0) {
    error_state = true;
    PRINT ("   property \"rt\" is ReadOnly \n");
  } else if (strcmp ( oc_string(name), "id") == 0) {
    error_state = true;
    PRINT ("   property \"id\" is ReadOnly \n");
  } else if (strcmp ( oc_string(name), "id") == 0) {
    error_state = true;
    PRINT ("   property \"id\" is ReadOnly \n");
  } else if (strcmp ( oc_string(name), "range") == 0) {
    error_state = true;
    PRINT ("   property \"range\" is ReadOnly \n");
  } else if (strcmp ( oc_string(name), "step") == 0) {
    error_state = true;
    PRINT ("   property \"step\" is ReadOnly \n");
  } else if (strcmp ( oc_string(name), "precision") == 0) {
    error_state = true;
    PRINT ("   property \"precision\" is ReadOnly \n");
  }
  return error_state;
}


 
/**
* get method for "/analog1" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_analog1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_analog1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (number) 'current' */
    oc_rep_set_double(root, current, g_analog1_current);
    PRINT("   %s : %f\n", g_analog1_RESOURCE_PROPERTY_NAME_current, g_analog1_current);
    /* property (number) 'desiredcurrent' */
    oc_rep_set_double(root, desiredcurrent, g_analog1_desiredcurrent);
    PRINT("   %s : %f\n", g_analog1_RESOURCE_PROPERTY_NAME_desiredcurrent, g_analog1_desiredcurrent);
    /* property (number) 'desiredfrequency' */
    oc_rep_set_double(root, desiredfrequency, g_analog1_desiredfrequency);
    PRINT("   %s : %f\n", g_analog1_RESOURCE_PROPERTY_NAME_desiredfrequency, g_analog1_desiredfrequency);
    /* property (number) 'desiredvoltage' */
    oc_rep_set_double(root, desiredvoltage, g_analog1_desiredvoltage);
    PRINT("   %s : %f\n", g_analog1_RESOURCE_PROPERTY_NAME_desiredvoltage, g_analog1_desiredvoltage);
    /* property (number) 'frequency' */
    oc_rep_set_double(root, frequency, g_analog1_frequency);
    PRINT("   %s : %f\n", g_analog1_RESOURCE_PROPERTY_NAME_frequency, g_analog1_frequency);
    /* property (number) 'voltage' */
    oc_rep_set_double(root, voltage, g_analog1_voltage);
    PRINT("   %s : %f\n", g_analog1_RESOURCE_PROPERTY_NAME_voltage, g_analog1_voltage);
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_analog1\n");
}
 
/**
* get method for "/analog2" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_analog2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_analog2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (number) 'current' */
    oc_rep_set_double(root, current, g_analog2_current);
    PRINT("   %s : %f\n", g_analog2_RESOURCE_PROPERTY_NAME_current, g_analog2_current);
    /* property (number) 'desiredcurrent' */
    oc_rep_set_double(root, desiredcurrent, g_analog2_desiredcurrent);
    PRINT("   %s : %f\n", g_analog2_RESOURCE_PROPERTY_NAME_desiredcurrent, g_analog2_desiredcurrent);
    /* property (number) 'desiredfrequency' */
    oc_rep_set_double(root, desiredfrequency, g_analog2_desiredfrequency);
    PRINT("   %s : %f\n", g_analog2_RESOURCE_PROPERTY_NAME_desiredfrequency, g_analog2_desiredfrequency);
    /* property (number) 'desiredvoltage' */
    oc_rep_set_double(root, desiredvoltage, g_analog2_desiredvoltage);
    PRINT("   %s : %f\n", g_analog2_RESOURCE_PROPERTY_NAME_desiredvoltage, g_analog2_desiredvoltage);
    /* property (number) 'frequency' */
    oc_rep_set_double(root, frequency, g_analog2_frequency);
    PRINT("   %s : %f\n", g_analog2_RESOURCE_PROPERTY_NAME_frequency, g_analog2_frequency);
    /* property (number) 'voltage' */
    oc_rep_set_double(root, voltage, g_analog2_voltage);
    PRINT("   %s : %f\n", g_analog2_RESOURCE_PROPERTY_NAME_voltage, g_analog2_voltage);
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_analog2\n");
}
 
/**
* get method for "/analog3" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_analog3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_analog3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (number) 'current' */
    oc_rep_set_double(root, current, g_analog3_current);
    PRINT("   %s : %f\n", g_analog3_RESOURCE_PROPERTY_NAME_current, g_analog3_current);
    /* property (number) 'desiredcurrent' */
    oc_rep_set_double(root, desiredcurrent, g_analog3_desiredcurrent);
    PRINT("   %s : %f\n", g_analog3_RESOURCE_PROPERTY_NAME_desiredcurrent, g_analog3_desiredcurrent);
    /* property (number) 'desiredfrequency' */
    oc_rep_set_double(root, desiredfrequency, g_analog3_desiredfrequency);
    PRINT("   %s : %f\n", g_analog3_RESOURCE_PROPERTY_NAME_desiredfrequency, g_analog3_desiredfrequency);
    /* property (number) 'desiredvoltage' */
    oc_rep_set_double(root, desiredvoltage, g_analog3_desiredvoltage);
    PRINT("   %s : %f\n", g_analog3_RESOURCE_PROPERTY_NAME_desiredvoltage, g_analog3_desiredvoltage);
    /* property (number) 'frequency' */
    oc_rep_set_double(root, frequency, g_analog3_frequency);
    PRINT("   %s : %f\n", g_analog3_RESOURCE_PROPERTY_NAME_frequency, g_analog3_frequency);
    /* property (number) 'voltage' */
    oc_rep_set_double(root, voltage, g_analog3_voltage);
    PRINT("   %s : %f\n", g_analog3_RESOURCE_PROPERTY_NAME_voltage, g_analog3_voltage);
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_analog3\n");
}
 
/**
* get method for "/analog4" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_analog4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_analog4: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (number) 'current' */
    oc_rep_set_double(root, current, g_analog4_current);
    PRINT("   %s : %f\n", g_analog4_RESOURCE_PROPERTY_NAME_current, g_analog4_current);
    /* property (number) 'desiredcurrent' */
    oc_rep_set_double(root, desiredcurrent, g_analog4_desiredcurrent);
    PRINT("   %s : %f\n", g_analog4_RESOURCE_PROPERTY_NAME_desiredcurrent, g_analog4_desiredcurrent);
    /* property (number) 'desiredfrequency' */
    oc_rep_set_double(root, desiredfrequency, g_analog4_desiredfrequency);
    PRINT("   %s : %f\n", g_analog4_RESOURCE_PROPERTY_NAME_desiredfrequency, g_analog4_desiredfrequency);
    /* property (number) 'desiredvoltage' */
    oc_rep_set_double(root, desiredvoltage, g_analog4_desiredvoltage);
    PRINT("   %s : %f\n", g_analog4_RESOURCE_PROPERTY_NAME_desiredvoltage, g_analog4_desiredvoltage);
    /* property (number) 'frequency' */
    oc_rep_set_double(root, frequency, g_analog4_frequency);
    PRINT("   %s : %f\n", g_analog4_RESOURCE_PROPERTY_NAME_frequency, g_analog4_frequency);
    /* property (number) 'voltage' */
    oc_rep_set_double(root, voltage, g_analog4_voltage);
    PRINT("   %s : %f\n", g_analog4_RESOURCE_PROPERTY_NAME_voltage, g_analog4_voltage);
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_analog4\n");
}
 
/**
* get method for "/input1" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a binary switch (on/off).
* The Property "value" is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_input1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_input1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_input1_value);
    PRINT("   %s : %s\n", g_input1_RESOURCE_PROPERTY_NAME_value,  btoa(g_input1_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_input1\n");
}
 
/**
* get method for "/input2" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a binary switch (on/off).
* The Property "value" is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_input2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_input2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_input2_value);
    PRINT("   %s : %s\n", g_input2_RESOURCE_PROPERTY_NAME_value,  btoa(g_input2_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_input2\n");
}
 
/**
* get method for "/input3" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a binary switch (on/off).
* The Property "value" is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_input3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_input3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_input3_value);
    PRINT("   %s : %s\n", g_input3_RESOURCE_PROPERTY_NAME_value,  btoa(g_input3_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_input3\n");
}
 
/**
* get method for "/input4" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a binary switch (on/off).
* The Property "value" is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_input4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_input4: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_input4_value);
    PRINT("   %s : %s\n", g_input4_RESOURCE_PROPERTY_NAME_value,  btoa(g_input4_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_input4\n");
}
 
/**
* get method for "/light1" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a binary switch (on/off).
* The Property "value" is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_light1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_light1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_light1_value);
    PRINT("   %s : %s\n", g_light1_RESOURCE_PROPERTY_NAME_value,  btoa(g_light1_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_light1\n");
}
 
/**
* get method for "/light2" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a binary switch (on/off).
* The Property "value" is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_light2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_light2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_light2_value);
    PRINT("   %s : %s\n", g_light2_RESOURCE_PROPERTY_NAME_value,  btoa(g_light2_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_light2\n");
}
 
/**
* get method for "/light3" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a binary switch (on/off).
* The Property "value" is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_light3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_light3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_light3_value);
    PRINT("   %s : %s\n", g_light3_RESOURCE_PROPERTY_NAME_value,  btoa(g_light3_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_light3\n");
}
 
/**
* get method for "/light4" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a binary switch (on/off).
* The Property "value" is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_light4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_light4: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_light4_value);
    PRINT("   %s : %s\n", g_light4_RESOURCE_PROPERTY_NAME_value,  btoa(g_light4_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_light4\n");
}
 
/**
* get method for "/output1" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a binary switch (on/off).
* The Property "value" is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_output1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_output1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_output1_value);
    PRINT("   %s : %s\n", g_output1_RESOURCE_PROPERTY_NAME_value,  btoa(g_output1_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_output1\n");
}
 
/**
* get method for "/output2" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a binary switch (on/off).
* The Property "value" is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_output2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_output2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_output2_value);
    PRINT("   %s : %s\n", g_output2_RESOURCE_PROPERTY_NAME_value,  btoa(g_output2_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_output2\n");
}
 
/**
* get method for "/output3" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a binary switch (on/off).
* The Property "value" is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_output3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_output3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_output3_value);
    PRINT("   %s : %s\n", g_output3_RESOURCE_PROPERTY_NAME_value,  btoa(g_output3_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_output3\n");
}
 
/**
* get method for "/output4" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a binary switch (on/off).
* The Property "value" is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_output4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_output4: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_output4_value);
    PRINT("   %s : %s\n", g_output4_RESOURCE_PROPERTY_NAME_value,  btoa(g_output4_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_output4\n");
}
 
/**
* get method for "/touch1" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes whether a touch has been sensed or not.
* The Property "value" is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_touch1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_touch1_value);
    PRINT("   %s : %s\n", g_touch1_RESOURCE_PROPERTY_NAME_value,  btoa(g_touch1_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_touch1\n");
}
 
/**
* get method for "/touch2" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes whether a touch has been sensed or not.
* The Property "value" is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_touch2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_touch2_value);
    PRINT("   %s : %s\n", g_touch2_RESOURCE_PROPERTY_NAME_value,  btoa(g_touch2_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_touch2\n");
}
 
/**
* get method for "/touch3" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes whether a touch has been sensed or not.
* The Property "value" is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_touch3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_touch3_value);
    PRINT("   %s : %s\n", g_touch3_RESOURCE_PROPERTY_NAME_value,  btoa(g_touch3_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_touch3\n");
}
 
/**
* get method for "/touch4" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes whether a touch has been sensed or not.
* The Property "value" is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_touch4: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_touch4_value);
    PRINT("   %s : %s\n", g_touch4_RESOURCE_PROPERTY_NAME_value,  btoa(g_touch4_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_touch4\n");
}
 
/**
* get method for "/touch5" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes whether a touch has been sensed or not.
* The Property "value" is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch5(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_touch5: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_touch5_value);
    PRINT("   %s : %s\n", g_touch5_RESOURCE_PROPERTY_NAME_value,  btoa(g_touch5_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_touch5\n");
}
 
/**
* get method for "/touch6" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes whether a touch has been sensed or not.
* The Property "value" is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch6(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_touch6: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_touch6_value);
    PRINT("   %s : %s\n", g_touch6_RESOURCE_PROPERTY_NAME_value,  btoa(g_touch6_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_touch6\n");
}
 
/**
* get method for "/touch7" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes whether a touch has been sensed or not.
* The Property "value" is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch7(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_touch7: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_touch7_value);
    PRINT("   %s : %s\n", g_touch7_RESOURCE_PROPERTY_NAME_value,  btoa(g_touch7_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_touch7\n");
}
 
/**
* get method for "/touch8" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes whether a touch has been sensed or not.
* The Property "value" is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch8(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_touch8: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_touch8_value);
    PRINT("   %s : %s\n", g_touch8_RESOURCE_PROPERTY_NAME_value,  btoa(g_touch8_value));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  if (error_state == false) {
    oc_send_response(request, OC_STATUS_OK);
  }
  else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_touch8\n");
}
 
/**
* post method for "/light1" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param request the request representation.
* @param interfaces the used interfaces during the request.
* @param user_data the supplied user data.
*/
static void
post_light1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_light1:\n");
  oc_rep_t *rep = request->request_payload;
  
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));
    
    error_state = check_on_readonly_common_resource_properties(rep->name, error_state);
    if (strcmp ( oc_string(rep->name), g_light1_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      
      if (strcmp ( oc_string(rep->name), g_light1_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        PRINT ("  property 'value' : %s\n", btoa(rep->value.boolean));
        g_light1_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_light1_value);
    
    oc_rep_end_root_object();
    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    PRINT("  Returning Error \n");
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
  PRINT("-- End post_light1\n");
}
 
/**
* post method for "/light2" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param request the request representation.
* @param interfaces the used interfaces during the request.
* @param user_data the supplied user data.
*/
static void
post_light2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_light2:\n");
  oc_rep_t *rep = request->request_payload;
  
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));
    
    error_state = check_on_readonly_common_resource_properties(rep->name, error_state);
    if (strcmp ( oc_string(rep->name), g_light2_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      
      if (strcmp ( oc_string(rep->name), g_light2_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        PRINT ("  property 'value' : %s\n", btoa(rep->value.boolean));
        g_light2_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_light2_value);
    
    oc_rep_end_root_object();
    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    PRINT("  Returning Error \n");
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
  PRINT("-- End post_light2\n");
}
 
/**
* post method for "/light3" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param request the request representation.
* @param interfaces the used interfaces during the request.
* @param user_data the supplied user data.
*/
static void
post_light3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_light3:\n");
  oc_rep_t *rep = request->request_payload;
  
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));
    
    error_state = check_on_readonly_common_resource_properties(rep->name, error_state);
    if (strcmp ( oc_string(rep->name), g_light3_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      
      if (strcmp ( oc_string(rep->name), g_light3_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        PRINT ("  property 'value' : %s\n", btoa(rep->value.boolean));
        g_light3_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_light3_value);
    
    oc_rep_end_root_object();
    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    PRINT("  Returning Error \n");
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
  PRINT("-- End post_light3\n");
}
 
/**
* post method for "/light4" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param request the request representation.
* @param interfaces the used interfaces during the request.
* @param user_data the supplied user data.
*/
static void
post_light4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_light4:\n");
  oc_rep_t *rep = request->request_payload;
  
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));
    
    error_state = check_on_readonly_common_resource_properties(rep->name, error_state);
    if (strcmp ( oc_string(rep->name), g_light4_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      
      if (strcmp ( oc_string(rep->name), g_light4_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        PRINT ("  property 'value' : %s\n", btoa(rep->value.boolean));
        g_light4_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_light4_value);
    
    oc_rep_end_root_object();
    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    PRINT("  Returning Error \n");
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
  PRINT("-- End post_light4\n");
}
 
/**
* post method for "/output1" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param request the request representation.
* @param interfaces the used interfaces during the request.
* @param user_data the supplied user data.
*/
static void
post_output1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_output1:\n");
  oc_rep_t *rep = request->request_payload;
  
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));
    
    error_state = check_on_readonly_common_resource_properties(rep->name, error_state);
    if (strcmp ( oc_string(rep->name), g_output1_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      
      if (strcmp ( oc_string(rep->name), g_output1_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        PRINT ("  property 'value' : %s\n", btoa(rep->value.boolean));
        g_output1_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_output1_value);
    
    oc_rep_end_root_object();
    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    PRINT("  Returning Error \n");
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
  PRINT("-- End post_output1\n");
}
 
/**
* post method for "/output2" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param request the request representation.
* @param interfaces the used interfaces during the request.
* @param user_data the supplied user data.
*/
static void
post_output2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_output2:\n");
  oc_rep_t *rep = request->request_payload;
  
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));
    
    error_state = check_on_readonly_common_resource_properties(rep->name, error_state);
    if (strcmp ( oc_string(rep->name), g_output2_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      
      if (strcmp ( oc_string(rep->name), g_output2_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        PRINT ("  property 'value' : %s\n", btoa(rep->value.boolean));
        g_output2_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_output2_value);
    
    oc_rep_end_root_object();
    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    PRINT("  Returning Error \n");
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
  PRINT("-- End post_output2\n");
}
 
/**
* post method for "/output3" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param request the request representation.
* @param interfaces the used interfaces during the request.
* @param user_data the supplied user data.
*/
static void
post_output3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_output3:\n");
  oc_rep_t *rep = request->request_payload;
  
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));
    
    error_state = check_on_readonly_common_resource_properties(rep->name, error_state);
    if (strcmp ( oc_string(rep->name), g_output3_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      
      if (strcmp ( oc_string(rep->name), g_output3_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        PRINT ("  property 'value' : %s\n", btoa(rep->value.boolean));
        g_output3_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_output3_value);
    
    oc_rep_end_root_object();
    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    PRINT("  Returning Error \n");
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
  PRINT("-- End post_output3\n");
}
 
/**
* post method for "/output4" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param request the request representation.
* @param interfaces the used interfaces during the request.
* @param user_data the supplied user data.
*/
static void
post_output4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_output4:\n");
  oc_rep_t *rep = request->request_payload;
  
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));
    
    error_state = check_on_readonly_common_resource_properties(rep->name, error_state);
    if (strcmp ( oc_string(rep->name), g_output4_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      
      if (strcmp ( oc_string(rep->name), g_output4_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        PRINT ("  property 'value' : %s\n", btoa(rep->value.boolean));
        g_output4_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_output4_value);
    
    oc_rep_end_root_object();
    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    PRINT("  Returning Error \n");
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
  PRINT("-- End post_output4\n");
}
/**
* register all the resources to the stack
* this function registers all application level resources:
* - each resource path is bind to a specific function for the supported methods (GET, POST, PUT)
* - each resource is 
*   - secure
*   - observable
*   - discoverable 
*   - used interfaces (from the global variables).
*/
static void
register_resources(void)
{

  PRINT("Register Resource with local path \"/analog1\"\n");
  oc_resource_t *res_analog1 = oc_new_resource(NULL, g_analog1_RESOURCE_ENDPOINT, g_analog1_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_analog1_nr_resource_types);
  for( int a = 0; a < g_analog1_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_analog1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_analog1,g_analog1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_analog1_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_analog1, convert_if_string(g_analog1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_analog1, convert_if_string(g_analog1_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_analog1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_analog1, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_analog1, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_analog1, true); */
   
  oc_resource_set_request_handler(res_analog1, OC_GET, get_analog1, NULL);
  oc_add_resource(res_analog1);

  PRINT("Register Resource with local path \"/analog2\"\n");
  oc_resource_t *res_analog2 = oc_new_resource(NULL, g_analog2_RESOURCE_ENDPOINT, g_analog2_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_analog2_nr_resource_types);
  for( int a = 0; a < g_analog2_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_analog2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_analog2,g_analog2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_analog2_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_analog2, convert_if_string(g_analog2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_analog2, convert_if_string(g_analog2_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_analog2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_analog2, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_analog2, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_analog2, true); */
   
  oc_resource_set_request_handler(res_analog2, OC_GET, get_analog2, NULL);
  oc_add_resource(res_analog2);

  PRINT("Register Resource with local path \"/analog3\"\n");
  oc_resource_t *res_analog3 = oc_new_resource(NULL, g_analog3_RESOURCE_ENDPOINT, g_analog3_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_analog3_nr_resource_types);
  for( int a = 0; a < g_analog3_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_analog3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_analog3,g_analog3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_analog3_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_analog3, convert_if_string(g_analog3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_analog3, convert_if_string(g_analog3_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_analog3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_analog3, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_analog3, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_analog3, true); */
   
  oc_resource_set_request_handler(res_analog3, OC_GET, get_analog3, NULL);
  oc_add_resource(res_analog3);

  PRINT("Register Resource with local path \"/analog4\"\n");
  oc_resource_t *res_analog4 = oc_new_resource(NULL, g_analog4_RESOURCE_ENDPOINT, g_analog4_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_analog4_nr_resource_types);
  for( int a = 0; a < g_analog4_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_analog4_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_analog4,g_analog4_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_analog4_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_analog4, convert_if_string(g_analog4_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_analog4, convert_if_string(g_analog4_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_analog4_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_analog4, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_analog4, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_analog4, true); */
   
  oc_resource_set_request_handler(res_analog4, OC_GET, get_analog4, NULL);
  oc_add_resource(res_analog4);

  PRINT("Register Resource with local path \"/input1\"\n");
  oc_resource_t *res_input1 = oc_new_resource(NULL, g_input1_RESOURCE_ENDPOINT, g_input1_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_input1_nr_resource_types);
  for( int a = 0; a < g_input1_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_input1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_input1,g_input1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_input1_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_input1, convert_if_string(g_input1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_input1, convert_if_string(g_input1_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_input1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_input1, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_input1, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_input1, true); */
   
  oc_resource_set_request_handler(res_input1, OC_GET, get_input1, NULL);
  oc_add_resource(res_input1);

  PRINT("Register Resource with local path \"/input2\"\n");
  oc_resource_t *res_input2 = oc_new_resource(NULL, g_input2_RESOURCE_ENDPOINT, g_input2_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_input2_nr_resource_types);
  for( int a = 0; a < g_input2_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_input2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_input2,g_input2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_input2_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_input2, convert_if_string(g_input2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_input2, convert_if_string(g_input2_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_input2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_input2, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_input2, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_input2, true); */
   
  oc_resource_set_request_handler(res_input2, OC_GET, get_input2, NULL);
  oc_add_resource(res_input2);

  PRINT("Register Resource with local path \"/input3\"\n");
  oc_resource_t *res_input3 = oc_new_resource(NULL, g_input3_RESOURCE_ENDPOINT, g_input3_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_input3_nr_resource_types);
  for( int a = 0; a < g_input3_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_input3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_input3,g_input3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_input3_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_input3, convert_if_string(g_input3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_input3, convert_if_string(g_input3_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_input3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_input3, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_input3, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_input3, true); */
   
  oc_resource_set_request_handler(res_input3, OC_GET, get_input3, NULL);
  oc_add_resource(res_input3);

  PRINT("Register Resource with local path \"/input4\"\n");
  oc_resource_t *res_input4 = oc_new_resource(NULL, g_input4_RESOURCE_ENDPOINT, g_input4_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_input4_nr_resource_types);
  for( int a = 0; a < g_input4_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_input4_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_input4,g_input4_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_input4_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_input4, convert_if_string(g_input4_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_input4, convert_if_string(g_input4_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_input4_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_input4, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_input4, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_input4, true); */
   
  oc_resource_set_request_handler(res_input4, OC_GET, get_input4, NULL);
  oc_add_resource(res_input4);

  PRINT("Register Resource with local path \"/light1\"\n");
  oc_resource_t *res_light1 = oc_new_resource(NULL, g_light1_RESOURCE_ENDPOINT, g_light1_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_light1_nr_resource_types);
  for( int a = 0; a < g_light1_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_light1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_light1,g_light1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_light1_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_light1, convert_if_string(g_light1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_light1, convert_if_string(g_light1_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_light1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_light1, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_light1, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_light1, true); */
   
  oc_resource_set_request_handler(res_light1, OC_GET, get_light1, NULL);
   
  oc_resource_set_request_handler(res_light1, OC_POST, post_light1, NULL);
  oc_add_resource(res_light1);

  PRINT("Register Resource with local path \"/light2\"\n");
  oc_resource_t *res_light2 = oc_new_resource(NULL, g_light2_RESOURCE_ENDPOINT, g_light2_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_light2_nr_resource_types);
  for( int a = 0; a < g_light2_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_light2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_light2,g_light2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_light2_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_light2, convert_if_string(g_light2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_light2, convert_if_string(g_light2_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_light2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_light2, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_light2, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_light2, true); */
   
  oc_resource_set_request_handler(res_light2, OC_GET, get_light2, NULL);
   
  oc_resource_set_request_handler(res_light2, OC_POST, post_light2, NULL);
  oc_add_resource(res_light2);

  PRINT("Register Resource with local path \"/light3\"\n");
  oc_resource_t *res_light3 = oc_new_resource(NULL, g_light3_RESOURCE_ENDPOINT, g_light3_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_light3_nr_resource_types);
  for( int a = 0; a < g_light3_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_light3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_light3,g_light3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_light3_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_light3, convert_if_string(g_light3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_light3, convert_if_string(g_light3_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_light3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_light3, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_light3, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_light3, true); */
   
  oc_resource_set_request_handler(res_light3, OC_GET, get_light3, NULL);
   
  oc_resource_set_request_handler(res_light3, OC_POST, post_light3, NULL);
  oc_add_resource(res_light3);

  PRINT("Register Resource with local path \"/light4\"\n");
  oc_resource_t *res_light4 = oc_new_resource(NULL, g_light4_RESOURCE_ENDPOINT, g_light4_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_light4_nr_resource_types);
  for( int a = 0; a < g_light4_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_light4_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_light4,g_light4_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_light4_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_light4, convert_if_string(g_light4_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_light4, convert_if_string(g_light4_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_light4_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_light4, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_light4, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_light4, true); */
   
  oc_resource_set_request_handler(res_light4, OC_GET, get_light4, NULL);
   
  oc_resource_set_request_handler(res_light4, OC_POST, post_light4, NULL);
  oc_add_resource(res_light4);

  PRINT("Register Resource with local path \"/output1\"\n");
  oc_resource_t *res_output1 = oc_new_resource(NULL, g_output1_RESOURCE_ENDPOINT, g_output1_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_output1_nr_resource_types);
  for( int a = 0; a < g_output1_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_output1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_output1,g_output1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_output1_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_output1, convert_if_string(g_output1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_output1, convert_if_string(g_output1_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_output1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_output1, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_output1, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_output1, true); */
   
  oc_resource_set_request_handler(res_output1, OC_GET, get_output1, NULL);
   
  oc_resource_set_request_handler(res_output1, OC_POST, post_output1, NULL);
  oc_add_resource(res_output1);

  PRINT("Register Resource with local path \"/output2\"\n");
  oc_resource_t *res_output2 = oc_new_resource(NULL, g_output2_RESOURCE_ENDPOINT, g_output2_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_output2_nr_resource_types);
  for( int a = 0; a < g_output2_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_output2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_output2,g_output2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_output2_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_output2, convert_if_string(g_output2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_output2, convert_if_string(g_output2_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_output2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_output2, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_output2, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_output2, true); */
   
  oc_resource_set_request_handler(res_output2, OC_GET, get_output2, NULL);
   
  oc_resource_set_request_handler(res_output2, OC_POST, post_output2, NULL);
  oc_add_resource(res_output2);

  PRINT("Register Resource with local path \"/output3\"\n");
  oc_resource_t *res_output3 = oc_new_resource(NULL, g_output3_RESOURCE_ENDPOINT, g_output3_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_output3_nr_resource_types);
  for( int a = 0; a < g_output3_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_output3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_output3,g_output3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_output3_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_output3, convert_if_string(g_output3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_output3, convert_if_string(g_output3_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_output3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_output3, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_output3, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_output3, true); */
   
  oc_resource_set_request_handler(res_output3, OC_GET, get_output3, NULL);
   
  oc_resource_set_request_handler(res_output3, OC_POST, post_output3, NULL);
  oc_add_resource(res_output3);

  PRINT("Register Resource with local path \"/output4\"\n");
  oc_resource_t *res_output4 = oc_new_resource(NULL, g_output4_RESOURCE_ENDPOINT, g_output4_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_output4_nr_resource_types);
  for( int a = 0; a < g_output4_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_output4_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_output4,g_output4_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_output4_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_output4, convert_if_string(g_output4_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_output4, convert_if_string(g_output4_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_output4_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_output4, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_output4, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_output4, true); */
   
  oc_resource_set_request_handler(res_output4, OC_GET, get_output4, NULL);
   
  oc_resource_set_request_handler(res_output4, OC_POST, post_output4, NULL);
  oc_add_resource(res_output4);

  PRINT("Register Resource with local path \"/touch1\"\n");
  oc_resource_t *res_touch1 = oc_new_resource(NULL, g_touch1_RESOURCE_ENDPOINT, g_touch1_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_touch1_nr_resource_types);
  for( int a = 0; a < g_touch1_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_touch1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch1,g_touch1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch1_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_touch1, convert_if_string(g_touch1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch1, convert_if_string(g_touch1_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_touch1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch1, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_touch1, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_touch1, true); */
   
  oc_resource_set_request_handler(res_touch1, OC_GET, get_touch1, NULL);
  oc_add_resource(res_touch1);

  PRINT("Register Resource with local path \"/touch2\"\n");
  oc_resource_t *res_touch2 = oc_new_resource(NULL, g_touch2_RESOURCE_ENDPOINT, g_touch2_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_touch2_nr_resource_types);
  for( int a = 0; a < g_touch2_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_touch2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch2,g_touch2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch2_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_touch2, convert_if_string(g_touch2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch2, convert_if_string(g_touch2_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_touch2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch2, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_touch2, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_touch2, true); */
   
  oc_resource_set_request_handler(res_touch2, OC_GET, get_touch2, NULL);
  oc_add_resource(res_touch2);

  PRINT("Register Resource with local path \"/touch3\"\n");
  oc_resource_t *res_touch3 = oc_new_resource(NULL, g_touch3_RESOURCE_ENDPOINT, g_touch3_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_touch3_nr_resource_types);
  for( int a = 0; a < g_touch3_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_touch3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch3,g_touch3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch3_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_touch3, convert_if_string(g_touch3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch3, convert_if_string(g_touch3_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_touch3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch3, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_touch3, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_touch3, true); */
   
  oc_resource_set_request_handler(res_touch3, OC_GET, get_touch3, NULL);
  oc_add_resource(res_touch3);

  PRINT("Register Resource with local path \"/touch4\"\n");
  oc_resource_t *res_touch4 = oc_new_resource(NULL, g_touch4_RESOURCE_ENDPOINT, g_touch4_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_touch4_nr_resource_types);
  for( int a = 0; a < g_touch4_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_touch4_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch4,g_touch4_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch4_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_touch4, convert_if_string(g_touch4_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch4, convert_if_string(g_touch4_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_touch4_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch4, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_touch4, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_touch4, true); */
   
  oc_resource_set_request_handler(res_touch4, OC_GET, get_touch4, NULL);
  oc_add_resource(res_touch4);

  PRINT("Register Resource with local path \"/touch5\"\n");
  oc_resource_t *res_touch5 = oc_new_resource(NULL, g_touch5_RESOURCE_ENDPOINT, g_touch5_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_touch5_nr_resource_types);
  for( int a = 0; a < g_touch5_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_touch5_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch5,g_touch5_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch5_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_touch5, convert_if_string(g_touch5_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch5, convert_if_string(g_touch5_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_touch5_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch5, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_touch5, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_touch5, true); */
   
  oc_resource_set_request_handler(res_touch5, OC_GET, get_touch5, NULL);
  oc_add_resource(res_touch5);

  PRINT("Register Resource with local path \"/touch6\"\n");
  oc_resource_t *res_touch6 = oc_new_resource(NULL, g_touch6_RESOURCE_ENDPOINT, g_touch6_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_touch6_nr_resource_types);
  for( int a = 0; a < g_touch6_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_touch6_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch6,g_touch6_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch6_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_touch6, convert_if_string(g_touch6_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch6, convert_if_string(g_touch6_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_touch6_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch6, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_touch6, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_touch6, true); */
   
  oc_resource_set_request_handler(res_touch6, OC_GET, get_touch6, NULL);
  oc_add_resource(res_touch6);

  PRINT("Register Resource with local path \"/touch7\"\n");
  oc_resource_t *res_touch7 = oc_new_resource(NULL, g_touch7_RESOURCE_ENDPOINT, g_touch7_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_touch7_nr_resource_types);
  for( int a = 0; a < g_touch7_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_touch7_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch7,g_touch7_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch7_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_touch7, convert_if_string(g_touch7_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch7, convert_if_string(g_touch7_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_touch7_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch7, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_touch7, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_touch7, true); */
   
  oc_resource_set_request_handler(res_touch7, OC_GET, get_touch7, NULL);
  oc_add_resource(res_touch7);

  PRINT("Register Resource with local path \"/touch8\"\n");
  oc_resource_t *res_touch8 = oc_new_resource(NULL, g_touch8_RESOURCE_ENDPOINT, g_touch8_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_touch8_nr_resource_types);
  for( int a = 0; a < g_touch8_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_touch8_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch8,g_touch8_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch8_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_touch8, convert_if_string(g_touch8_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch8, convert_if_string(g_touch8_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_touch8_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch8, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_touch8, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_touch8, true); */
   
  oc_resource_set_request_handler(res_touch8, OC_GET, get_touch8, NULL);
  oc_add_resource(res_touch8);
}

#ifdef WIN32
/**
* signal the event loop (windows version)
* wakes up the main function to handle the next callback
*/
static void
signal_event_loop(void)
{
  WakeConditionVariable(&cv);
}
#endif
#ifdef __linux__
/**
* signal the event loop (Linux)
* wakes up the main function to handle the next callback
*/
static void
signal_event_loop(void)
{
  pthread_mutex_lock(&mutex);
  pthread_cond_signal(&cv);
  pthread_mutex_unlock(&mutex);
}
#endif

/**
* handle Ctrl-C
* @param signal the captured signal
*/
void
handle_signal(int signal)
{
  (void)signal;
  signal_event_loop();
  quit = 1;
}

#ifdef OC_SECURITY
void
random_pin_cb(const unsigned char *pin, size_t pin_len, void *data)
{
  (void)data;
  PRINT("\n====================\n");
  PRINT("Random PIN: %.*s\n", (int)pin_len, pin);
  PRINT("====================\n");
}
#endif /* OC_SECURITY */

void
factory_presets_cb(size_t device, void *data)
{
  (void)device;
  (void)data;
#if defined(OC_SECURITY) && defined(OC_PKI)
/* code to include an pki certificate and root trust anchor */
#include "oc_pki.h"
#include "pki_certs.h"
  int credid =
    oc_pki_add_mfg_cert(0, (const unsigned char *)my_cert, strlen(my_cert), (const unsigned char *)my_key, strlen(my_key));
  if (credid < 0) {
    PRINT("ERROR installing manufacturer certificate\n");
  } else {
    PRINT("Successfully installed manufacturer certificate\n");
  }

  if (oc_pki_add_mfg_intermediate_cert(0, credid, (const unsigned char *)int_ca, strlen(int_ca)) < 0) {
    PRINT("ERROR installing intermediate CA certificate\n");
  } else {
    PRINT("Successfully installed intermediate CA certificate\n");
  }

  if (oc_pki_add_mfg_trust_anchor(0, (const unsigned char *)root_ca, strlen(root_ca)) < 0) {
    PRINT("ERROR installing root certificate\n");
  } else {
    PRINT("Successfully installed root certificate\n");
  }

  oc_pki_set_security_profile(0, OC_SP_BLACK, OC_SP_BLACK, credid);
#endif /* OC_SECURITY && OC_PKI */
}

/**
* main application.
* intializes the global variables
* registers and starts the handler
* handles (in a loop) the next event.
* shuts down the stack
*/
int
main(void)
{
int init;

#ifdef WIN32
  /* windows specific */
  InitializeCriticalSection(&cs);
  InitializeConditionVariable(&cv);
  /* install Ctrl-C */
  signal(SIGINT, handle_signal);
#endif
#ifdef __linux__
  /* linux specific */
  struct sigaction sa;
  sigfillset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handle_signal;
  /* install Ctrl-C */
  sigaction(SIGINT, &sa, NULL);
#endif
  /* initialize global variables for resource "/analog1" */
  g_analog1_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
  g_analog1_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
  g_analog1_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
  g_analog1_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
  g_analog1_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
  g_analog1_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
  /* initialize global variables for resource "/analog2" */
  g_analog2_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
  g_analog2_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
  g_analog2_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
  g_analog2_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
  g_analog2_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
  g_analog2_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
  /* initialize global variables for resource "/analog3" */
  g_analog3_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
  g_analog3_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
  g_analog3_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
  g_analog3_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
  g_analog3_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
  g_analog3_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
  /* initialize global variables for resource "/analog4" */
  g_analog4_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
  g_analog4_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
  g_analog4_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
  g_analog4_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
  g_analog4_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
  g_analog4_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
  /* initialize global variables for resource "/input1" */  g_input1_value = false; /* current value of property "value" The status of the switch. */
  /* initialize global variables for resource "/input2" */  g_input2_value = false; /* current value of property "value" The status of the switch. */
  /* initialize global variables for resource "/input3" */  g_input3_value = false; /* current value of property "value" The status of the switch. */
  /* initialize global variables for resource "/input4" */  g_input4_value = false; /* current value of property "value" The status of the switch. */
  /* initialize global variables for resource "/light1" */  g_light1_value = false; /* current value of property "value" The status of the switch. */
  /* initialize global variables for resource "/light2" */  g_light2_value = false; /* current value of property "value" The status of the switch. */
  /* initialize global variables for resource "/light3" */  g_light3_value = false; /* current value of property "value" The status of the switch. */
  /* initialize global variables for resource "/light4" */  g_light4_value = false; /* current value of property "value" The status of the switch. */
  /* initialize global variables for resource "/output1" */  g_output1_value = false; /* current value of property "value" The status of the switch. */
  /* initialize global variables for resource "/output2" */  g_output2_value = false; /* current value of property "value" The status of the switch. */
  /* initialize global variables for resource "/output3" */  g_output3_value = false; /* current value of property "value" The status of the switch. */
  /* initialize global variables for resource "/output4" */  g_output4_value = false; /* current value of property "value" The status of the switch. */
  /* initialize global variables for resource "/touch1" */  g_touch1_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. */
  /* initialize global variables for resource "/touch2" */  g_touch2_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. */
  /* initialize global variables for resource "/touch3" */  g_touch3_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. */
  /* initialize global variables for resource "/touch4" */  g_touch4_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. */
  /* initialize global variables for resource "/touch5" */  g_touch5_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. */
  /* initialize global variables for resource "/touch6" */  g_touch6_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. */
  /* initialize global variables for resource "/touch7" */  g_touch7_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. */
  /* initialize global variables for resource "/touch8" */  g_touch8_value = true; /* current value of property "value" The touch sensor, true = sensed, false = not sensed. */
  
  /* set the flag for NO oic/con resource. */
  oc_set_con_res_announced(false);

  /* initializes the handlers structure */
  static const oc_handler_t handler = {.init = app_init,
                                       .signal_event_loop = signal_event_loop,
                                       .register_resources = register_resources
#ifdef OC_CLIENT
                                       ,
                                       .requests_entry = 0 
#endif
                                       };
  oc_clock_time_t next_event;
  
  PRINT("Used input file : \"/home/pi/workspace/test2/device_output/out_codegeneration_merged.swagger.json\"\n");
  PRINT("OCF Server name : \"ExplorerHatPro\"\n");

#ifdef OC_SECURITY
  PRINT("Intialize Secure Resources\n");
  oc_storage_config("./device_builder_server_creds/");
#endif /* OC_SECURITY */

#ifdef OC_SECURITY
  /* please comment out if the server:
    - have no display capabilities to display the PIN value
    - server does not require to implement RANDOM PIN (oic.sec.doxm.rdp) onboarding mechanism
  */
  oc_set_random_pin_callback(random_pin_cb, NULL);
#endif /* OC_SECURITY */

  oc_set_factory_presets_cb(factory_presets_cb, NULL);

  /* start the stack */
  init = oc_main_init(&handler);

  if (init < 0)
    return init;

  PRINT("OCF server \"ExplorerHatPro\" running, waiting on incoming connections.\n");

#ifdef WIN32
  /* windows specific loop */
  while (quit != 1) {
    next_event = oc_main_poll();
    if (next_event == 0) {
      SleepConditionVariableCS(&cv, &cs, INFINITE);
    } else {
      oc_clock_time_t now = oc_clock_time();
      if (now < next_event) {
        SleepConditionVariableCS(&cv, &cs,
                                 (DWORD)((next_event-now) * 1000 / OC_CLOCK_SECOND));
      }
    }
  }
#endif
  
#ifdef __linux__
  /* linux specific loop */
  while (quit != 1) {
    next_event = oc_main_poll();
    pthread_mutex_lock(&mutex);
    if (next_event == 0) {
      pthread_cond_wait(&cv, &mutex);
    } else {
      ts.tv_sec = (next_event / OC_CLOCK_SECOND);
      ts.tv_nsec = (next_event % OC_CLOCK_SECOND) * 1.e09 / OC_CLOCK_SECOND;
      pthread_cond_timedwait(&cv, &mutex, &ts);
    }
    pthread_mutex_unlock(&mutex);
  }
#endif

  /* shut down the stack */
  oc_main_shutdown();
  return 0;
}