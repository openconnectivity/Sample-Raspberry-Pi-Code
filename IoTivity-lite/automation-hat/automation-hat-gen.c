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
 input_file            : /home/pi/workspace/automationhat/device_output/out_codegeneration_merged.swagger.json
 version of input_file : 20190215
 title of input_file   : Energy
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

#define MAX_STRING 30           /* max size of the strings. */
#define MAX_PAYLOAD_STRING 65   /* max size strings in the payload */
#define MAX_ARRAY 10            /* max size of the array */
/* Note: Magic numbers are derived from the resource definition, either from the example or the definition.*/

volatile int quit = 0;          /* stop variable, used by handle_signal */


/* global property variables for path: "/ADC1" */
static char g_ADC1_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; /* the name for the attribute */
double g_ADC1_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
static char g_ADC1_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; /* the name for the attribute */
double g_ADC1_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
static char g_ADC1_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; /* the name for the attribute */
double g_ADC1_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
static char g_ADC1_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; /* the name for the attribute */
double g_ADC1_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
static char g_ADC1_RESOURCE_PROPERTY_NAME_current[] = "current"; /* the name for the attribute */
double g_ADC1_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
static char g_ADC1_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; /* the name for the attribute */
double g_ADC1_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
/* global property variables for path: "/ADC2" */
static char g_ADC2_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; /* the name for the attribute */
double g_ADC2_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
static char g_ADC2_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; /* the name for the attribute */
double g_ADC2_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
static char g_ADC2_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; /* the name for the attribute */
double g_ADC2_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
static char g_ADC2_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; /* the name for the attribute */
double g_ADC2_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
static char g_ADC2_RESOURCE_PROPERTY_NAME_current[] = "current"; /* the name for the attribute */
double g_ADC2_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
static char g_ADC2_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; /* the name for the attribute */
double g_ADC2_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
/* global property variables for path: "/ADC3" */
static char g_ADC3_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; /* the name for the attribute */
double g_ADC3_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
static char g_ADC3_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; /* the name for the attribute */
double g_ADC3_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
static char g_ADC3_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; /* the name for the attribute */
double g_ADC3_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
static char g_ADC3_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; /* the name for the attribute */
double g_ADC3_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
static char g_ADC3_RESOURCE_PROPERTY_NAME_current[] = "current"; /* the name for the attribute */
double g_ADC3_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
static char g_ADC3_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; /* the name for the attribute */
double g_ADC3_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
/* global property variables for path: "/ADC4" */
static char g_ADC4_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; /* the name for the attribute */
double g_ADC4_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
static char g_ADC4_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; /* the name for the attribute */
double g_ADC4_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
static char g_ADC4_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; /* the name for the attribute */
double g_ADC4_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
static char g_ADC4_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; /* the name for the attribute */
double g_ADC4_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
static char g_ADC4_RESOURCE_PROPERTY_NAME_current[] = "current"; /* the name for the attribute */
double g_ADC4_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
static char g_ADC4_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; /* the name for the attribute */
double g_ADC4_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
/* global property variables for path: "/CommsLight" */
static char g_CommsLight_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_CommsLight_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/PowerLight" */
static char g_PowerLight_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_PowerLight_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/WarnLight" */
static char g_WarnLight_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_WarnLight_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/input1" */
static char g_input1_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_input1_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/input2" */
static char g_input2_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_input2_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/input3" */
static char g_input3_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_input3_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/output1" */
static char g_output1_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_output1_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/output2" */
static char g_output2_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_output2_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/output3" */
static char g_output3_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_output3_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/relay1" */
static char g_relay1_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_relay1_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/relay2" */
static char g_relay2_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_relay2_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/relay3" */
static char g_relay3_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_relay3_value = false; /* current value of property "value" The status of the switch. *//* registration data variables for the resources */

/* global resource variables for path: /ADC1 */
static char g_ADC1_RESOURCE_ENDPOINT[] = "/ADC1"; /* used path for this resource */
static char g_ADC1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; /* rt value (as an array) */
int g_ADC1_nr_resource_types = 1;
static char g_ADC1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_ADC1_nr_resource_interfaces = 2;

/* global resource variables for path: /ADC2 */
static char g_ADC2_RESOURCE_ENDPOINT[] = "/ADC2"; /* used path for this resource */
static char g_ADC2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; /* rt value (as an array) */
int g_ADC2_nr_resource_types = 1;
static char g_ADC2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_ADC2_nr_resource_interfaces = 2;

/* global resource variables for path: /ADC3 */
static char g_ADC3_RESOURCE_ENDPOINT[] = "/ADC3"; /* used path for this resource */
static char g_ADC3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; /* rt value (as an array) */
int g_ADC3_nr_resource_types = 1;
static char g_ADC3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_ADC3_nr_resource_interfaces = 2;

/* global resource variables for path: /ADC4 */
static char g_ADC4_RESOURCE_ENDPOINT[] = "/ADC4"; /* used path for this resource */
static char g_ADC4_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; /* rt value (as an array) */
int g_ADC4_nr_resource_types = 1;
static char g_ADC4_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_ADC4_nr_resource_interfaces = 2;

/* global resource variables for path: /CommsLight */
static char g_CommsLight_RESOURCE_ENDPOINT[] = "/CommsLight"; /* used path for this resource */
static char g_CommsLight_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_CommsLight_nr_resource_types = 1;
static char g_CommsLight_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_CommsLight_nr_resource_interfaces = 2;

/* global resource variables for path: /PowerLight */
static char g_PowerLight_RESOURCE_ENDPOINT[] = "/PowerLight"; /* used path for this resource */
static char g_PowerLight_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_PowerLight_nr_resource_types = 1;
static char g_PowerLight_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_PowerLight_nr_resource_interfaces = 2;

/* global resource variables for path: /WarnLight */
static char g_WarnLight_RESOURCE_ENDPOINT[] = "/WarnLight"; /* used path for this resource */
static char g_WarnLight_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_WarnLight_nr_resource_types = 1;
static char g_WarnLight_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_WarnLight_nr_resource_interfaces = 2;

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

/* global resource variables for path: /relay1 */
static char g_relay1_RESOURCE_ENDPOINT[] = "/relay1"; /* used path for this resource */
static char g_relay1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_relay1_nr_resource_types = 1;
static char g_relay1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_relay1_nr_resource_interfaces = 2;

/* global resource variables for path: /relay2 */
static char g_relay2_RESOURCE_ENDPOINT[] = "/relay2"; /* used path for this resource */
static char g_relay2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_relay2_nr_resource_types = 1;
static char g_relay2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_relay2_nr_resource_interfaces = 2;

/* global resource variables for path: /relay3 */
static char g_relay3_RESOURCE_ENDPOINT[] = "/relay3"; /* used path for this resource */
static char g_relay3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_relay3_nr_resource_types = 1;
static char g_relay3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_relay3_nr_resource_interfaces = 2;
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
  ret |= oc_add_device("/oic/d", "oic.d.switchdevice", "Energy", 
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
* get method for "/ADC1" resource.
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
get_ADC1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_ADC1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "desiredfrequency" */
    oc_rep_set_double(root, desiredfrequency, g_ADC1_desiredfrequency ); 
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_desiredfrequency, g_ADC1_desiredfrequency );  /* not handled desiredfrequency *//* property "desiredvoltage" */
    oc_rep_set_double(root, desiredvoltage, g_ADC1_desiredvoltage ); 
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_desiredvoltage, g_ADC1_desiredvoltage );  /* not handled desiredvoltage *//* property "desiredcurrent" */
    oc_rep_set_double(root, desiredcurrent, g_ADC1_desiredcurrent ); 
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_desiredcurrent, g_ADC1_desiredcurrent );  /* not handled desiredcurrent *//* property "frequency" */
    oc_rep_set_double(root, frequency, g_ADC1_frequency ); 
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_frequency, g_ADC1_frequency );  /* not handled frequency *//* property "current" */
    oc_rep_set_double(root, current, g_ADC1_current ); 
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_current, g_ADC1_current );  /* not handled current *//* property "voltage" */
    oc_rep_set_double(root, voltage, g_ADC1_voltage ); 
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_voltage, g_ADC1_voltage );  /* not handled voltage */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/ADC2" resource.
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
get_ADC2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_ADC2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "desiredfrequency" */
    oc_rep_set_double(root, desiredfrequency, g_ADC2_desiredfrequency ); 
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_desiredfrequency, g_ADC2_desiredfrequency );  /* not handled desiredfrequency *//* property "desiredvoltage" */
    oc_rep_set_double(root, desiredvoltage, g_ADC2_desiredvoltage ); 
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_desiredvoltage, g_ADC2_desiredvoltage );  /* not handled desiredvoltage *//* property "desiredcurrent" */
    oc_rep_set_double(root, desiredcurrent, g_ADC2_desiredcurrent ); 
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_desiredcurrent, g_ADC2_desiredcurrent );  /* not handled desiredcurrent *//* property "frequency" */
    oc_rep_set_double(root, frequency, g_ADC2_frequency ); 
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_frequency, g_ADC2_frequency );  /* not handled frequency *//* property "current" */
    oc_rep_set_double(root, current, g_ADC2_current ); 
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_current, g_ADC2_current );  /* not handled current *//* property "voltage" */
    oc_rep_set_double(root, voltage, g_ADC2_voltage ); 
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_voltage, g_ADC2_voltage );  /* not handled voltage */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/ADC3" resource.
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
get_ADC3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_ADC3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "desiredfrequency" */
    oc_rep_set_double(root, desiredfrequency, g_ADC3_desiredfrequency ); 
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_desiredfrequency, g_ADC3_desiredfrequency );  /* not handled desiredfrequency *//* property "desiredvoltage" */
    oc_rep_set_double(root, desiredvoltage, g_ADC3_desiredvoltage ); 
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_desiredvoltage, g_ADC3_desiredvoltage );  /* not handled desiredvoltage *//* property "desiredcurrent" */
    oc_rep_set_double(root, desiredcurrent, g_ADC3_desiredcurrent ); 
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_desiredcurrent, g_ADC3_desiredcurrent );  /* not handled desiredcurrent *//* property "frequency" */
    oc_rep_set_double(root, frequency, g_ADC3_frequency ); 
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_frequency, g_ADC3_frequency );  /* not handled frequency *//* property "current" */
    oc_rep_set_double(root, current, g_ADC3_current ); 
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_current, g_ADC3_current );  /* not handled current *//* property "voltage" */
    oc_rep_set_double(root, voltage, g_ADC3_voltage ); 
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_voltage, g_ADC3_voltage );  /* not handled voltage */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/ADC4" resource.
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
get_ADC4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_ADC4: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "desiredfrequency" */
    oc_rep_set_double(root, desiredfrequency, g_ADC4_desiredfrequency ); 
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_desiredfrequency, g_ADC4_desiredfrequency );  /* not handled desiredfrequency *//* property "desiredvoltage" */
    oc_rep_set_double(root, desiredvoltage, g_ADC4_desiredvoltage ); 
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_desiredvoltage, g_ADC4_desiredvoltage );  /* not handled desiredvoltage *//* property "desiredcurrent" */
    oc_rep_set_double(root, desiredcurrent, g_ADC4_desiredcurrent ); 
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_desiredcurrent, g_ADC4_desiredcurrent );  /* not handled desiredcurrent *//* property "frequency" */
    oc_rep_set_double(root, frequency, g_ADC4_frequency ); 
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_frequency, g_ADC4_frequency );  /* not handled frequency *//* property "current" */
    oc_rep_set_double(root, current, g_ADC4_current ); 
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_current, g_ADC4_current );  /* not handled current *//* property "voltage" */
    oc_rep_set_double(root, voltage, g_ADC4_voltage ); 
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_voltage, g_ADC4_voltage );  /* not handled voltage */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/CommsLight" resource.
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
get_CommsLight(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_CommsLight: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "value" */
    oc_rep_set_boolean(root, value, g_CommsLight_value); 
    PRINT("   %s : %d\n", g_CommsLight_RESOURCE_PROPERTY_NAME_value,  g_CommsLight_value );  /* not handled value */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/PowerLight" resource.
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
get_PowerLight(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_PowerLight: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "value" */
    oc_rep_set_boolean(root, value, g_PowerLight_value); 
    PRINT("   %s : %d\n", g_PowerLight_RESOURCE_PROPERTY_NAME_value,  g_PowerLight_value );  /* not handled value */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/WarnLight" resource.
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
get_WarnLight(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_WarnLight: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "value" */
    oc_rep_set_boolean(root, value, g_WarnLight_value); 
    PRINT("   %s : %d\n", g_WarnLight_RESOURCE_PROPERTY_NAME_value,  g_WarnLight_value );  /* not handled value */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
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
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_input1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "value" */
    oc_rep_set_boolean(root, value, g_input1_value); 
    PRINT("   %s : %d\n", g_input1_RESOURCE_PROPERTY_NAME_value,  g_input1_value );  /* not handled value */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
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
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_input2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "value" */
    oc_rep_set_boolean(root, value, g_input2_value); 
    PRINT("   %s : %d\n", g_input2_RESOURCE_PROPERTY_NAME_value,  g_input2_value );  /* not handled value */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
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
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_input3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "value" */
    oc_rep_set_boolean(root, value, g_input3_value); 
    PRINT("   %s : %d\n", g_input3_RESOURCE_PROPERTY_NAME_value,  g_input3_value );  /* not handled value */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
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
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_output1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "value" */
    oc_rep_set_boolean(root, value, g_output1_value); 
    PRINT("   %s : %d\n", g_output1_RESOURCE_PROPERTY_NAME_value,  g_output1_value );  /* not handled value */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
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
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_output2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "value" */
    oc_rep_set_boolean(root, value, g_output2_value); 
    PRINT("   %s : %d\n", g_output2_RESOURCE_PROPERTY_NAME_value,  g_output2_value );  /* not handled value */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
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
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_output3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "value" */
    oc_rep_set_boolean(root, value, g_output3_value); 
    PRINT("   %s : %d\n", g_output3_RESOURCE_PROPERTY_NAME_value,  g_output3_value );  /* not handled value */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/relay1" resource.
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
get_relay1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_relay1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "value" */
    oc_rep_set_boolean(root, value, g_relay1_value); 
    PRINT("   %s : %d\n", g_relay1_RESOURCE_PROPERTY_NAME_value,  g_relay1_value );  /* not handled value */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/relay2" resource.
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
get_relay2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_relay2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "value" */
    oc_rep_set_boolean(root, value, g_relay2_value); 
    PRINT("   %s : %d\n", g_relay2_RESOURCE_PROPERTY_NAME_value,  g_relay2_value );  /* not handled value */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/relay3" resource.
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
get_relay3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  
  PRINT("get_relay3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    /* property "value" */
    oc_rep_set_boolean(root, value, g_relay3_value); 
    PRINT("   %s : %d\n", g_relay3_RESOURCE_PROPERTY_NAME_value,  g_relay3_value );  /* not handled value */
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* post method for "/CommsLight" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param requestRep the request representation.
*/
static void
post_CommsLight(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_CommsLight:\n");
  oc_rep_t *rep = request->request_payload;
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_CommsLight_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }
    
    rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      if (strcmp ( oc_string(rep->name), g_CommsLight_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        g_CommsLight_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_CommsLight_value); 
    oc_rep_end_root_object();
    
    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}
 
/**
* post method for "/PowerLight" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param requestRep the request representation.
*/
static void
post_PowerLight(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_PowerLight:\n");
  oc_rep_t *rep = request->request_payload;
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_PowerLight_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }
    
    rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      if (strcmp ( oc_string(rep->name), g_PowerLight_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        g_PowerLight_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_PowerLight_value); 
    oc_rep_end_root_object();
    
    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}
 
/**
* post method for "/WarnLight" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param requestRep the request representation.
*/
static void
post_WarnLight(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_WarnLight:\n");
  oc_rep_t *rep = request->request_payload;
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_WarnLight_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }
    
    rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      if (strcmp ( oc_string(rep->name), g_WarnLight_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        g_WarnLight_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_WarnLight_value); 
    oc_rep_end_root_object();
    
    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}
 
/**
* post method for "/output1" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param requestRep the request representation.
*/
static void
post_output1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_output1:\n");
  oc_rep_t *rep = request->request_payload;
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_output1_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }
    
    rep = rep->next;
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
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}
 
/**
* post method for "/output2" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param requestRep the request representation.
*/
static void
post_output2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_output2:\n");
  oc_rep_t *rep = request->request_payload;
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_output2_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }
    
    rep = rep->next;
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
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}
 
/**
* post method for "/output3" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param requestRep the request representation.
*/
static void
post_output3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_output3:\n");
  oc_rep_t *rep = request->request_payload;
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_output3_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }
    
    rep = rep->next;
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
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}
 
/**
* post method for "/relay1" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param requestRep the request representation.
*/
static void
post_relay1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_relay1:\n");
  oc_rep_t *rep = request->request_payload;
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_relay1_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }
    
    rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      if (strcmp ( oc_string(rep->name), g_relay1_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        g_relay1_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_relay1_value); 
    oc_rep_end_root_object();
    
    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}
 
/**
* post method for "/relay2" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param requestRep the request representation.
*/
static void
post_relay2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_relay2:\n");
  oc_rep_t *rep = request->request_payload;
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_relay2_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }
    
    rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      if (strcmp ( oc_string(rep->name), g_relay2_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        g_relay2_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_relay2_value); 
    oc_rep_end_root_object();
    
    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}
 
/**
* post method for "/relay3" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:

*
* @param requestRep the request representation.
*/
static void
post_relay3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_relay3:\n");
  oc_rep_t *rep = request->request_payload;
  /* loop over the request document to check if all inputs are ok */
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_relay3_RESOURCE_PROPERTY_NAME_value) == 0) {
      /* property "value" of type boolean exist in payload */
      if (rep->type != OC_REP_BOOL) {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }
    
    rep = rep->next;
  }
  /* if the input is ok, then process the input document and assign the global variables */
  if (error_state == false)
  {
    /* loop over all the properties in the input document */
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s \n", oc_string(rep->name));
      /* no error: assign the variables */
      if (strcmp ( oc_string(rep->name), g_relay3_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        g_relay3_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_relay3_value); 
    oc_rep_end_root_object();
    
    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    /* TODO: add error response, if any */
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
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

  PRINT("Register Resource with local path \"/ADC1\"\n");
  oc_resource_t *res_ADC1 = oc_new_resource(NULL, g_ADC1_RESOURCE_ENDPOINT, g_ADC1_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_ADC1_nr_resource_types);
  for( int a = 0; a < g_ADC1_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_ADC1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_ADC1,g_ADC1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_ADC1_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_ADC1, convert_if_string(g_ADC1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_ADC1, convert_if_string(g_ADC1_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_ADC1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_ADC1, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_ADC1, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_ADC1, true); */
   
  oc_resource_set_request_handler(res_ADC1, OC_GET, get_ADC1, NULL);
  oc_add_resource(res_ADC1);

  PRINT("Register Resource with local path \"/ADC2\"\n");
  oc_resource_t *res_ADC2 = oc_new_resource(NULL, g_ADC2_RESOURCE_ENDPOINT, g_ADC2_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_ADC2_nr_resource_types);
  for( int a = 0; a < g_ADC2_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_ADC2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_ADC2,g_ADC2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_ADC2_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_ADC2, convert_if_string(g_ADC2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_ADC2, convert_if_string(g_ADC2_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_ADC2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_ADC2, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_ADC2, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_ADC2, true); */
   
  oc_resource_set_request_handler(res_ADC2, OC_GET, get_ADC2, NULL);
  oc_add_resource(res_ADC2);

  PRINT("Register Resource with local path \"/ADC3\"\n");
  oc_resource_t *res_ADC3 = oc_new_resource(NULL, g_ADC3_RESOURCE_ENDPOINT, g_ADC3_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_ADC3_nr_resource_types);
  for( int a = 0; a < g_ADC3_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_ADC3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_ADC3,g_ADC3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_ADC3_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_ADC3, convert_if_string(g_ADC3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_ADC3, convert_if_string(g_ADC3_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_ADC3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_ADC3, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_ADC3, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_ADC3, true); */
   
  oc_resource_set_request_handler(res_ADC3, OC_GET, get_ADC3, NULL);
  oc_add_resource(res_ADC3);

  PRINT("Register Resource with local path \"/ADC4\"\n");
  oc_resource_t *res_ADC4 = oc_new_resource(NULL, g_ADC4_RESOURCE_ENDPOINT, g_ADC4_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_ADC4_nr_resource_types);
  for( int a = 0; a < g_ADC4_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_ADC4_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_ADC4,g_ADC4_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_ADC4_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_ADC4, convert_if_string(g_ADC4_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_ADC4, convert_if_string(g_ADC4_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_ADC4_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_ADC4, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_ADC4, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_ADC4, true); */
   
  oc_resource_set_request_handler(res_ADC4, OC_GET, get_ADC4, NULL);
  oc_add_resource(res_ADC4);

  PRINT("Register Resource with local path \"/CommsLight\"\n");
  oc_resource_t *res_CommsLight = oc_new_resource(NULL, g_CommsLight_RESOURCE_ENDPOINT, g_CommsLight_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_CommsLight_nr_resource_types);
  for( int a = 0; a < g_CommsLight_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_CommsLight_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_CommsLight,g_CommsLight_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_CommsLight_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_CommsLight, convert_if_string(g_CommsLight_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_CommsLight, convert_if_string(g_CommsLight_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_CommsLight_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_CommsLight, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_CommsLight, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_CommsLight, true); */
   
  oc_resource_set_request_handler(res_CommsLight, OC_GET, get_CommsLight, NULL);
   
  oc_resource_set_request_handler(res_CommsLight, OC_POST, post_CommsLight, NULL);
  oc_add_resource(res_CommsLight);

  PRINT("Register Resource with local path \"/PowerLight\"\n");
  oc_resource_t *res_PowerLight = oc_new_resource(NULL, g_PowerLight_RESOURCE_ENDPOINT, g_PowerLight_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_PowerLight_nr_resource_types);
  for( int a = 0; a < g_PowerLight_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_PowerLight_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_PowerLight,g_PowerLight_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_PowerLight_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_PowerLight, convert_if_string(g_PowerLight_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_PowerLight, convert_if_string(g_PowerLight_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_PowerLight_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_PowerLight, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_PowerLight, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_PowerLight, true); */
   
  oc_resource_set_request_handler(res_PowerLight, OC_GET, get_PowerLight, NULL);
   
  oc_resource_set_request_handler(res_PowerLight, OC_POST, post_PowerLight, NULL);
  oc_add_resource(res_PowerLight);

  PRINT("Register Resource with local path \"/WarnLight\"\n");
  oc_resource_t *res_WarnLight = oc_new_resource(NULL, g_WarnLight_RESOURCE_ENDPOINT, g_WarnLight_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_WarnLight_nr_resource_types);
  for( int a = 0; a < g_WarnLight_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_WarnLight_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_WarnLight,g_WarnLight_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_WarnLight_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_WarnLight, convert_if_string(g_WarnLight_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_WarnLight, convert_if_string(g_WarnLight_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_WarnLight_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_WarnLight, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_WarnLight, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_WarnLight, true); */
   
  oc_resource_set_request_handler(res_WarnLight, OC_GET, get_WarnLight, NULL);
   
  oc_resource_set_request_handler(res_WarnLight, OC_POST, post_WarnLight, NULL);
  oc_add_resource(res_WarnLight);

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

  PRINT("Register Resource with local path \"/relay1\"\n");
  oc_resource_t *res_relay1 = oc_new_resource(NULL, g_relay1_RESOURCE_ENDPOINT, g_relay1_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_relay1_nr_resource_types);
  for( int a = 0; a < g_relay1_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_relay1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_relay1,g_relay1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_relay1_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_relay1, convert_if_string(g_relay1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_relay1, convert_if_string(g_relay1_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_relay1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_relay1, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_relay1, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_relay1, true); */
   
  oc_resource_set_request_handler(res_relay1, OC_GET, get_relay1, NULL);
   
  oc_resource_set_request_handler(res_relay1, OC_POST, post_relay1, NULL);
  oc_add_resource(res_relay1);

  PRINT("Register Resource with local path \"/relay2\"\n");
  oc_resource_t *res_relay2 = oc_new_resource(NULL, g_relay2_RESOURCE_ENDPOINT, g_relay2_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_relay2_nr_resource_types);
  for( int a = 0; a < g_relay2_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_relay2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_relay2,g_relay2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_relay2_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_relay2, convert_if_string(g_relay2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_relay2, convert_if_string(g_relay2_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_relay2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_relay2, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_relay2, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_relay2, true); */
   
  oc_resource_set_request_handler(res_relay2, OC_GET, get_relay2, NULL);
   
  oc_resource_set_request_handler(res_relay2, OC_POST, post_relay2, NULL);
  oc_add_resource(res_relay2);

  PRINT("Register Resource with local path \"/relay3\"\n");
  oc_resource_t *res_relay3 = oc_new_resource(NULL, g_relay3_RESOURCE_ENDPOINT, g_relay3_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_relay3_nr_resource_types);
  for( int a = 0; a < g_relay3_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_relay3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_relay3,g_relay3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_relay3_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_relay3, convert_if_string(g_relay3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_relay3, convert_if_string(g_relay3_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_relay3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_relay3, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_relay3, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_relay3, true); */
   
  oc_resource_set_request_handler(res_relay3, OC_GET, get_relay3, NULL);
   
  oc_resource_set_request_handler(res_relay3, OC_POST, post_relay3, NULL);
  oc_add_resource(res_relay3);
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
  /* initialize global variables for resource "/ADC1" */
  g_ADC1_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
  g_ADC1_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
  g_ADC1_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
  g_ADC1_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
  g_ADC1_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
  g_ADC1_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
  
  /* initialize global variables for resource "/ADC2" */
  g_ADC2_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
  g_ADC2_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
  g_ADC2_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
  g_ADC2_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
  g_ADC2_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
  g_ADC2_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
  
  /* initialize global variables for resource "/ADC3" */
  g_ADC3_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
  g_ADC3_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
  g_ADC3_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
  g_ADC3_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
  g_ADC3_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
  g_ADC3_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
  
  /* initialize global variables for resource "/ADC4" */
  g_ADC4_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
  g_ADC4_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
  g_ADC4_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
  g_ADC4_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
  g_ADC4_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
  g_ADC4_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
  
  /* initialize global variables for resource "/CommsLight" */
  g_CommsLight_value = false; /* current value of property "value" The status of the switch. */
  
  /* initialize global variables for resource "/PowerLight" */
  g_PowerLight_value = false; /* current value of property "value" The status of the switch. */
  
  /* initialize global variables for resource "/WarnLight" */
  g_WarnLight_value = false; /* current value of property "value" The status of the switch. */
  
  /* initialize global variables for resource "/input1" */
  g_input1_value = false; /* current value of property "value" The status of the switch. */
  
  /* initialize global variables for resource "/input2" */
  g_input2_value = false; /* current value of property "value" The status of the switch. */
  
  /* initialize global variables for resource "/input3" */
  g_input3_value = false; /* current value of property "value" The status of the switch. */
  
  /* initialize global variables for resource "/output1" */
  g_output1_value = false; /* current value of property "value" The status of the switch. */
  
  /* initialize global variables for resource "/output2" */
  g_output2_value = false; /* current value of property "value" The status of the switch. */
  
  /* initialize global variables for resource "/output3" */
  g_output3_value = false; /* current value of property "value" The status of the switch. */
  
  /* initialize global variables for resource "/relay1" */
  g_relay1_value = false; /* current value of property "value" The status of the switch. */
  
  /* initialize global variables for resource "/relay2" */
  g_relay2_value = false; /* current value of property "value" The status of the switch. */
  
  /* initialize global variables for resource "/relay3" */
  g_relay3_value = false; /* current value of property "value" The status of the switch. */
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
  
  PRINT("Used input file : \"/home/pi/workspace/automationhat/device_output/out_codegeneration_merged.swagger.json\"\n");
  PRINT("OCF Server name : \"Energy\"\n");

#ifdef OC_SECURITY
  PRINT("Intialize Secure Resources\n");
  oc_storage_config("./device_builder_server_creds/");
#endif /* OC_SECURITY */


  /* start the stack */
  init = oc_main_init(&handler);
  if (init < 0)
    return init;

  PRINT("OCF server \"Energy\" running, waiting on incomming connections.\n");
    
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