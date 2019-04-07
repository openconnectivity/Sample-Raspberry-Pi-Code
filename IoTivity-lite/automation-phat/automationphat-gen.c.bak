//******************************************************************
//
// Copyright 2017 Open Connectivity Foundation
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=/

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
* Each endpoint has:
*  global variables for:
*    the property name
*       naming convention: g_<path>_RESOURCE_PROPERTY_NAME_<propertyname>
*    the actual value of the property, which is typed from the json data type
*      naming convention: g_<path>_<propertyname>
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
 input_file            : /home/pi/tmp/automationphatlite/device_output/out_codegeneration_merged.swagger.json
 version of input_file : v1.1.0-20170815
 title of input_file   : Energy
*/

#include "oc_api.h"
#include "port/oc_clock.h"
#include <signal.h>

#ifdef __linux__
// linux specific code
#include <pthread.h>
static pthread_mutex_t mutex;
static pthread_cond_t cv;
static struct timespec ts;
#endif

#ifdef WIN32
// windows specific code
#include <windows.h>
static CONDITION_VARIABLE cv;   // event loop variable
static CRITICAL_SECTION cs;     // event loop variable
#endif

#define MAX_STRING 65   // max size of the strings.
volatile int quit = 0;  // stop variable, used by handle_signal

// global variables for path: /ADC1
static char g_ADC1_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
double g_ADC1_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_ADC1_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
double g_ADC1_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_ADC1_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
double g_ADC1_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
static char g_ADC1_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
double g_ADC1_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_ADC1_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
double g_ADC1_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_ADC1_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
double g_ADC1_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
// global variables for path: /ADC2
static char g_ADC2_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
double g_ADC2_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_ADC2_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
double g_ADC2_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_ADC2_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
double g_ADC2_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
static char g_ADC2_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
double g_ADC2_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_ADC2_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
double g_ADC2_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_ADC2_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
double g_ADC2_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
// global variables for path: /ADC3
static char g_ADC3_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
double g_ADC3_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_ADC3_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
double g_ADC3_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_ADC3_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
double g_ADC3_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
static char g_ADC3_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
double g_ADC3_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_ADC3_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
double g_ADC3_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_ADC3_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
double g_ADC3_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
// global variables for path: /ADC4
static char g_ADC4_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
double g_ADC4_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_ADC4_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
double g_ADC4_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_ADC4_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
double g_ADC4_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
static char g_ADC4_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
double g_ADC4_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_ADC4_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
double g_ADC4_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_ADC4_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
double g_ADC4_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
// global variables for path: /input1
static char g_input1_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_input1_value = false; // current value of property "value" Status of the switch
// global variables for path: /input2
static char g_input2_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_input2_value = false; // current value of property "value" Status of the switch
// global variables for path: /input3
static char g_input3_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_input3_value = false; // current value of property "value" Status of the switch
// global variables for path: /output1
static char g_output1_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_output1_value = false; // current value of property "value" Status of the switch
// global variables for path: /output2
static char g_output2_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_output2_value = false; // current value of property "value" Status of the switch
// global variables for path: /output3
static char g_output3_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_output3_value = false; // current value of property "value" Status of the switch
// global variables for path: /relay
static char g_relay_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_relay_value = false; // current value of property "value" Status of the switch// registration data variables for the resources

static char g_ADC1_RESOURCE_ENDPOINT[] = "/ADC1";  // used path for this resource
static char g_ADC1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; // rt value (as an array)
int g_ADC1_nr_resource_types = 1;
static char g_ADC1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_ADC1_nr_resource_interfaces = 2;

static char g_ADC2_RESOURCE_ENDPOINT[] = "/ADC2";  // used path for this resource
static char g_ADC2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; // rt value (as an array)
int g_ADC2_nr_resource_types = 1;
static char g_ADC2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_ADC2_nr_resource_interfaces = 2;

static char g_ADC3_RESOURCE_ENDPOINT[] = "/ADC3";  // used path for this resource
static char g_ADC3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; // rt value (as an array)
int g_ADC3_nr_resource_types = 1;
static char g_ADC3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_ADC3_nr_resource_interfaces = 2;

static char g_ADC4_RESOURCE_ENDPOINT[] = "/ADC4";  // used path for this resource
static char g_ADC4_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; // rt value (as an array)
int g_ADC4_nr_resource_types = 1;
static char g_ADC4_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_ADC4_nr_resource_interfaces = 2;

static char g_input1_RESOURCE_ENDPOINT[] = "/input1";  // used path for this resource
static char g_input1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_input1_nr_resource_types = 1;
static char g_input1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array) 
int g_input1_nr_resource_interfaces = 2;

static char g_input2_RESOURCE_ENDPOINT[] = "/input2";  // used path for this resource
static char g_input2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_input2_nr_resource_types = 1;
static char g_input2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array) 
int g_input2_nr_resource_interfaces = 2;

static char g_input3_RESOURCE_ENDPOINT[] = "/input3";  // used path for this resource
static char g_input3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_input3_nr_resource_types = 1;
static char g_input3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array) 
int g_input3_nr_resource_interfaces = 2;

static char g_output1_RESOURCE_ENDPOINT[] = "/output1";  // used path for this resource
static char g_output1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_output1_nr_resource_types = 1;
static char g_output1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array) 
int g_output1_nr_resource_interfaces = 2;

static char g_output2_RESOURCE_ENDPOINT[] = "/output2";  // used path for this resource
static char g_output2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_output2_nr_resource_types = 1;
static char g_output2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array) 
int g_output2_nr_resource_interfaces = 2;

static char g_output3_RESOURCE_ENDPOINT[] = "/output3";  // used path for this resource
static char g_output3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_output3_nr_resource_types = 1;
static char g_output3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array) 
int g_output3_nr_resource_interfaces = 2;

static char g_relay_RESOURCE_ENDPOINT[] = "/relay";  // used path for this resource
static char g_relay_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_relay_nr_resource_types = 1;
static char g_relay_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array) 
int g_relay_nr_resource_interfaces = 2;
/**
* function to set up the device.
*
*/
static int
app_init(void)
{
  int ret = oc_init_platform("ocf", NULL, NULL);
  ret |= oc_add_device("/oic/d", "oic.d.light", "Energy", 
                       "ocf.1.0.0", // icv value
                       "ocf.res.1.3.0, ocf.sh.1.3.0",  // dmv value
                       NULL, NULL);
  return ret;
}

/**
*  function to convert the interface string definition to the constant
* @param interface the interface string e.g. "oic.if.a"
*/
static int
convert_if_string(char *interface_name)
{
  if (strcmp(interface_name, "oic.if.baseline") == 0) return OC_IF_BASELINE;
  if (strcmp(interface_name, "oic.if.rw") == 0) return OC_IF_RW;
  if (strcmp(interface_name, "oic.if.r" )== 0) return OC_IF_R;
  if (strcmp(interface_name, "oic.if.s") == 0) return OC_IF_S;
  if (strcmp(interface_name, "oic.if.a") == 0) return OC_IF_A;
  if (strcmp(interface_name, "oic.if.b") == 0) return OC_IF_B;
  //if strcmp(interface_name, "oic.if.lb") == 0) return OC_IF_LB;
  if (strcmp(interface_name, "oic.if.ll") == 0) return OC_IF_LL;
  return OC_IF_A;
}

 
/**
* get method for "/ADC1" endpoint to intialize the returned values from the global values
* This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
* Retrieves the current energy.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_ADC1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_ADC1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_double(root, voltage, g_ADC1_voltage ); 
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_voltage, g_ADC1_voltage );
    
    oc_rep_set_double(root, desiredcurrent, g_ADC1_desiredcurrent ); 
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_desiredcurrent, g_ADC1_desiredcurrent );
    
    oc_rep_set_double(root, desiredfrequency, g_ADC1_desiredfrequency ); 
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_desiredfrequency, g_ADC1_desiredfrequency );
    
    oc_rep_set_double(root, desiredvoltage, g_ADC1_desiredvoltage ); 
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_desiredvoltage, g_ADC1_desiredvoltage );
    
    oc_rep_set_double(root, current, g_ADC1_current ); 
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_current, g_ADC1_current );
    
    oc_rep_set_double(root, frequency, g_ADC1_frequency ); 
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_frequency, g_ADC1_frequency );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/ADC2" endpoint to intialize the returned values from the global values
* This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
* Retrieves the current energy.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_ADC2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_ADC2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_double(root, voltage, g_ADC2_voltage ); 
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_voltage, g_ADC2_voltage );
    
    oc_rep_set_double(root, desiredcurrent, g_ADC2_desiredcurrent ); 
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_desiredcurrent, g_ADC2_desiredcurrent );
    
    oc_rep_set_double(root, desiredfrequency, g_ADC2_desiredfrequency ); 
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_desiredfrequency, g_ADC2_desiredfrequency );
    
    oc_rep_set_double(root, desiredvoltage, g_ADC2_desiredvoltage ); 
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_desiredvoltage, g_ADC2_desiredvoltage );
    
    oc_rep_set_double(root, current, g_ADC2_current ); 
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_current, g_ADC2_current );
    
    oc_rep_set_double(root, frequency, g_ADC2_frequency ); 
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_frequency, g_ADC2_frequency );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/ADC3" endpoint to intialize the returned values from the global values
* This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
* Retrieves the current energy.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_ADC3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_ADC3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_double(root, voltage, g_ADC3_voltage ); 
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_voltage, g_ADC3_voltage );
    
    oc_rep_set_double(root, desiredcurrent, g_ADC3_desiredcurrent ); 
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_desiredcurrent, g_ADC3_desiredcurrent );
    
    oc_rep_set_double(root, desiredfrequency, g_ADC3_desiredfrequency ); 
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_desiredfrequency, g_ADC3_desiredfrequency );
    
    oc_rep_set_double(root, desiredvoltage, g_ADC3_desiredvoltage ); 
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_desiredvoltage, g_ADC3_desiredvoltage );
    
    oc_rep_set_double(root, current, g_ADC3_current ); 
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_current, g_ADC3_current );
    
    oc_rep_set_double(root, frequency, g_ADC3_frequency ); 
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_frequency, g_ADC3_frequency );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/ADC4" endpoint to intialize the returned values from the global values
* This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
* Retrieves the current energy.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_ADC4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_ADC4: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_double(root, voltage, g_ADC4_voltage ); 
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_voltage, g_ADC4_voltage );
    
    oc_rep_set_double(root, desiredcurrent, g_ADC4_desiredcurrent ); 
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_desiredcurrent, g_ADC4_desiredcurrent );
    
    oc_rep_set_double(root, desiredfrequency, g_ADC4_desiredfrequency ); 
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_desiredfrequency, g_ADC4_desiredfrequency );
    
    oc_rep_set_double(root, desiredvoltage, g_ADC4_desiredvoltage ); 
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_desiredvoltage, g_ADC4_desiredvoltage );
    
    oc_rep_set_double(root, current, g_ADC4_current ); 
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_current, g_ADC4_current );
    
    oc_rep_set_double(root, frequency, g_ADC4_frequency ); 
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_frequency, g_ADC4_frequency );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/input1" endpoint to intialize the returned values from the global values
* This resource describes a binary switch (on/off).
* The value is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_input1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_input1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_input1_value); 
    PRINT("   %s : %d\n", g_input1_RESOURCE_PROPERTY_NAME_value,  g_input1_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/input2" endpoint to intialize the returned values from the global values
* This resource describes a binary switch (on/off).
* The value is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_input2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_input2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_input2_value); 
    PRINT("   %s : %d\n", g_input2_RESOURCE_PROPERTY_NAME_value,  g_input2_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/input3" endpoint to intialize the returned values from the global values
* This resource describes a binary switch (on/off).
* The value is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_input3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_input3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_input3_value); 
    PRINT("   %s : %d\n", g_input3_RESOURCE_PROPERTY_NAME_value,  g_input3_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/output1" endpoint to intialize the returned values from the global values
* This resource describes a binary switch (on/off).
* The value is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_output1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_output1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_output1_value); 
    PRINT("   %s : %d\n", g_output1_RESOURCE_PROPERTY_NAME_value,  g_output1_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/output2" endpoint to intialize the returned values from the global values
* This resource describes a binary switch (on/off).
* The value is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_output2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_output2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_output2_value); 
    PRINT("   %s : %d\n", g_output2_RESOURCE_PROPERTY_NAME_value,  g_output2_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/output3" endpoint to intialize the returned values from the global values
* This resource describes a binary switch (on/off).
* The value is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_output3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_output3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_output3_value); 
    PRINT("   %s : %d\n", g_output3_RESOURCE_PROPERTY_NAME_value,  g_output3_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/relay" endpoint to intialize the returned values from the global values
* This resource describes a binary switch (on/off).
* The value is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_relay(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_relay: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_relay_value); 
    PRINT("   %s : %d\n", g_relay_RESOURCE_PROPERTY_NAME_value,  g_relay_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* post method for "/output1" endpoint to assign the returned values to the global values.

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
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_output1_RESOURCE_PROPERTY_NAME_value) == 0)
    {
      // value exist in payload
      
      if (rep->type != OC_REP_BOOL)
      {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }
    
    
    rep = rep->next;
  }
  if (error_state == false)
  {
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s ", oc_string(rep->name));
      // no error: assign the variables
      if (strcmp ( oc_string(rep->name), g_output1_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_output1_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_output1_value); 
    oc_rep_end_root_object();
    
    // TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
    // one can use the global variables as input to those calls
    // the global values have been updated already with the data from the request
    
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}
 
/**
* post method for "/output2" endpoint to assign the returned values to the global values.

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
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_output2_RESOURCE_PROPERTY_NAME_value) == 0)
    {
      // value exist in payload
      
      if (rep->type != OC_REP_BOOL)
      {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }
    
    
    rep = rep->next;
  }
  if (error_state == false)
  {
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s ", oc_string(rep->name));
      // no error: assign the variables
      if (strcmp ( oc_string(rep->name), g_output2_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_output2_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_output2_value); 
    oc_rep_end_root_object();
    
    // TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
    // one can use the global variables as input to those calls
    // the global values have been updated already with the data from the request
    
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}
 
/**
* post method for "/output3" endpoint to assign the returned values to the global values.

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
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_output3_RESOURCE_PROPERTY_NAME_value) == 0)
    {
      // value exist in payload
      
      if (rep->type != OC_REP_BOOL)
      {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }
    
    
    rep = rep->next;
  }
  if (error_state == false)
  {
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s ", oc_string(rep->name));
      // no error: assign the variables
      if (strcmp ( oc_string(rep->name), g_output3_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_output3_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_output3_value); 
    oc_rep_end_root_object();
    
    // TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
    // one can use the global variables as input to those calls
    // the global values have been updated already with the data from the request
    
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}
 
/**
* post method for "/relay" endpoint to assign the returned values to the global values.

* @param requestRep the request representation.
*/
static void
post_relay(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_relay:\n");
  oc_rep_t *rep = request->request_payload;
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_relay_RESOURCE_PROPERTY_NAME_value) == 0)
    {
      // value exist in payload
      
      if (rep->type != OC_REP_BOOL)
      {
        error_state = true;
        PRINT ("   property 'value' is not of type bool %d \n", rep->type);
      }
    }
    
    
    rep = rep->next;
  }
  if (error_state == false)
  {
    oc_rep_t *rep = request->request_payload;
    while (rep != NULL) {
      PRINT("key: (assign) %s ", oc_string(rep->name));
      // no error: assign the variables
      if (strcmp ( oc_string(rep->name), g_relay_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_relay_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_relay_value); 
    oc_rep_end_root_object();
    
    // TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
    // one can use the global variables as input to those calls
    // the global values have been updated already with the data from the request
    
    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}
/**
*  register all the resources
*/
static void
register_resources(void)
{

  PRINT("register resource with path /ADC1\n");
  oc_resource_t *res_ADC1 = oc_new_resource(NULL, g_ADC1_RESOURCE_ENDPOINT, g_ADC1_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_ADC1_nr_resource_types);
  for( int a = 0; a < g_ADC1_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_ADC1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_ADC1,g_ADC1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_ADC1_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_ADC1, convert_if_string(g_ADC1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_ADC1, convert_if_string(g_ADC1_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_ADC1_RESOURCE_INTERFACE[0]), g_ADC1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_ADC1, true);
  oc_resource_set_periodic_observable(res_ADC1, 1);
   
  oc_resource_set_request_handler(res_ADC1, OC_GET, get_ADC1, NULL);
  oc_add_resource(res_ADC1);

  PRINT("register resource with path /ADC2\n");
  oc_resource_t *res_ADC2 = oc_new_resource(NULL, g_ADC2_RESOURCE_ENDPOINT, g_ADC2_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_ADC2_nr_resource_types);
  for( int a = 0; a < g_ADC2_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_ADC2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_ADC2,g_ADC2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_ADC2_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_ADC2, convert_if_string(g_ADC2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_ADC2, convert_if_string(g_ADC2_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_ADC2_RESOURCE_INTERFACE[0]), g_ADC2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_ADC2, true);
  oc_resource_set_periodic_observable(res_ADC2, 1);
   
  oc_resource_set_request_handler(res_ADC2, OC_GET, get_ADC2, NULL);
  oc_add_resource(res_ADC2);

  PRINT("register resource with path /ADC3\n");
  oc_resource_t *res_ADC3 = oc_new_resource(NULL, g_ADC3_RESOURCE_ENDPOINT, g_ADC3_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_ADC3_nr_resource_types);
  for( int a = 0; a < g_ADC3_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_ADC3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_ADC3,g_ADC3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_ADC3_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_ADC3, convert_if_string(g_ADC3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_ADC3, convert_if_string(g_ADC3_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_ADC3_RESOURCE_INTERFACE[0]), g_ADC3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_ADC3, true);
  oc_resource_set_periodic_observable(res_ADC3, 1);
   
  oc_resource_set_request_handler(res_ADC3, OC_GET, get_ADC3, NULL);
  oc_add_resource(res_ADC3);

  PRINT("register resource with path /ADC4\n");
  oc_resource_t *res_ADC4 = oc_new_resource(NULL, g_ADC4_RESOURCE_ENDPOINT, g_ADC4_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_ADC4_nr_resource_types);
  for( int a = 0; a < g_ADC4_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_ADC4_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_ADC4,g_ADC4_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_ADC4_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_ADC4, convert_if_string(g_ADC4_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_ADC4, convert_if_string(g_ADC4_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_ADC4_RESOURCE_INTERFACE[0]), g_ADC4_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_ADC4, true);
  oc_resource_set_periodic_observable(res_ADC4, 1);
   
  oc_resource_set_request_handler(res_ADC4, OC_GET, get_ADC4, NULL);
  oc_add_resource(res_ADC4);

  PRINT("register resource with path /input1\n");
  oc_resource_t *res_input1 = oc_new_resource(NULL, g_input1_RESOURCE_ENDPOINT, g_input1_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_input1_nr_resource_types);
  for( int a = 0; a < g_input1_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_input1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_input1,g_input1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_input1_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_input1, convert_if_string(g_input1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_input1, convert_if_string(g_input1_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_input1_RESOURCE_INTERFACE[0]), g_input1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_input1, true);
  oc_resource_set_periodic_observable(res_input1, 1);
   
  oc_resource_set_request_handler(res_input1, OC_GET, get_input1, NULL);
  oc_add_resource(res_input1);

  PRINT("register resource with path /input2\n");
  oc_resource_t *res_input2 = oc_new_resource(NULL, g_input2_RESOURCE_ENDPOINT, g_input2_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_input2_nr_resource_types);
  for( int a = 0; a < g_input2_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_input2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_input2,g_input2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_input2_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_input2, convert_if_string(g_input2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_input2, convert_if_string(g_input2_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_input2_RESOURCE_INTERFACE[0]), g_input2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_input2, true);
  oc_resource_set_periodic_observable(res_input2, 1);
   
  oc_resource_set_request_handler(res_input2, OC_GET, get_input2, NULL);
  oc_add_resource(res_input2);

  PRINT("register resource with path /input3\n");
  oc_resource_t *res_input3 = oc_new_resource(NULL, g_input3_RESOURCE_ENDPOINT, g_input3_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_input3_nr_resource_types);
  for( int a = 0; a < g_input3_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_input3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_input3,g_input3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_input3_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_input3, convert_if_string(g_input3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_input3, convert_if_string(g_input3_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_input3_RESOURCE_INTERFACE[0]), g_input3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_input3, true);
  oc_resource_set_periodic_observable(res_input3, 1);
   
  oc_resource_set_request_handler(res_input3, OC_GET, get_input3, NULL);
  oc_add_resource(res_input3);

  PRINT("register resource with path /output1\n");
  oc_resource_t *res_output1 = oc_new_resource(NULL, g_output1_RESOURCE_ENDPOINT, g_output1_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_output1_nr_resource_types);
  for( int a = 0; a < g_output1_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_output1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_output1,g_output1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_output1_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_output1, convert_if_string(g_output1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_output1, convert_if_string(g_output1_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_output1_RESOURCE_INTERFACE[0]), g_output1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_output1, true);
  oc_resource_set_periodic_observable(res_output1, 1);
   
  oc_resource_set_request_handler(res_output1, OC_GET, get_output1, NULL);
   
  oc_resource_set_request_handler(res_output1, OC_POST, post_output1, NULL);
  oc_add_resource(res_output1);

  PRINT("register resource with path /output2\n");
  oc_resource_t *res_output2 = oc_new_resource(NULL, g_output2_RESOURCE_ENDPOINT, g_output2_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_output2_nr_resource_types);
  for( int a = 0; a < g_output2_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_output2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_output2,g_output2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_output2_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_output2, convert_if_string(g_output2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_output2, convert_if_string(g_output2_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_output2_RESOURCE_INTERFACE[0]), g_output2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_output2, true);
  oc_resource_set_periodic_observable(res_output2, 1);
   
  oc_resource_set_request_handler(res_output2, OC_GET, get_output2, NULL);
   
  oc_resource_set_request_handler(res_output2, OC_POST, post_output2, NULL);
  oc_add_resource(res_output2);

  PRINT("register resource with path /output3\n");
  oc_resource_t *res_output3 = oc_new_resource(NULL, g_output3_RESOURCE_ENDPOINT, g_output3_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_output3_nr_resource_types);
  for( int a = 0; a < g_output3_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_output3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_output3,g_output3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_output3_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_output3, convert_if_string(g_output3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_output3, convert_if_string(g_output3_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_output3_RESOURCE_INTERFACE[0]), g_output3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_output3, true);
  oc_resource_set_periodic_observable(res_output3, 1);
   
  oc_resource_set_request_handler(res_output3, OC_GET, get_output3, NULL);
   
  oc_resource_set_request_handler(res_output3, OC_POST, post_output3, NULL);
  oc_add_resource(res_output3);

  PRINT("register resource with path /relay\n");
  oc_resource_t *res_relay = oc_new_resource(NULL, g_relay_RESOURCE_ENDPOINT, g_relay_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_relay_nr_resource_types);
  for( int a = 0; a < g_relay_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_relay_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_relay,g_relay_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_relay_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_relay, convert_if_string(g_relay_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_relay, convert_if_string(g_relay_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_relay_RESOURCE_INTERFACE[0]), g_relay_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_relay, true);
  oc_resource_set_periodic_observable(res_relay, 1);
   
  oc_resource_set_request_handler(res_relay, OC_GET, get_relay, NULL);
   
  oc_resource_set_request_handler(res_relay, OC_POST, post_relay, NULL);
  oc_add_resource(res_relay);
}

#ifdef WIN32
/**
* signal the event loop
*/
static void
signal_event_loop(void)
{
  WakeConditionVariable(&cv);
}
#endif
#ifdef __linux__
/**
* signal the event loop
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
  // windows specific
  InitializeCriticalSection(&cs);
  InitializeConditionVariable(&cv);
  // install Ctrl-C
  signal(SIGINT, handle_signal);
#endif
#ifdef __linux__
  // linux specific
  struct sigaction sa;
  sigfillset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handle_signal;
  // install Ctrl-C
  sigaction(SIGINT, &sa, NULL);
#endif
  // initialize global variables for endpoint "/ADC1"
  g_ADC1_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_ADC1_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_ADC1_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
  g_ADC1_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_ADC1_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_ADC1_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
  
  // initialize global variables for endpoint "/ADC2"
  g_ADC2_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_ADC2_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_ADC2_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
  g_ADC2_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_ADC2_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_ADC2_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
  
  // initialize global variables for endpoint "/ADC3"
  g_ADC3_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_ADC3_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_ADC3_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
  g_ADC3_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_ADC3_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_ADC3_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
  
  // initialize global variables for endpoint "/ADC4"
  g_ADC4_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_ADC4_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_ADC4_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
  g_ADC4_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_ADC4_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_ADC4_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
  
  // initialize global variables for endpoint "/input1"
  g_input1_value = false; // current value of property "value" Status of the switch
  
  // initialize global variables for endpoint "/input2"
  g_input2_value = false; // current value of property "value" Status of the switch
  
  // initialize global variables for endpoint "/input3"
  g_input3_value = false; // current value of property "value" Status of the switch
  
  // initialize global variables for endpoint "/output1"
  g_output1_value = false; // current value of property "value" Status of the switch
  
  // initialize global variables for endpoint "/output2"
  g_output2_value = false; // current value of property "value" Status of the switch
  
  // initialize global variables for endpoint "/output3"
  g_output3_value = false; // current value of property "value" Status of the switch
  
  // initialize global variables for endpoint "/relay"
  g_relay_value = false; // current value of property "value" Status of the switch
   
  
  // no oic/con resource.
  oc_set_con_res_announced(false);

  // initializes the handlers structure
  static const oc_handler_t handler = {.init = app_init,
                                       .signal_event_loop = signal_event_loop,
                                       .register_resources = register_resources
#ifdef OC_CLIENT
                                       ,
                                       .requests_entry = 0 
#endif
                                       };
  oc_clock_time_t next_event;
  
  PRINT("file : /home/pi/tmp/automationphatlite/device_output/out_codegeneration_merged.swagger.json\n");
  PRINT("title: Energy\n");

#ifdef OC_SECURITY
  PRINT("intialize secure resources\n");
  oc_storage_config("./device_builder_server_creds/");
#endif /* OC_SECURITY */

  // start the stack
  init = oc_main_init(&handler);
  if (init < 0)
    return init;

#ifdef WIN32
  // windows specific loop
  while (quit != 1) {
    next_event = oc_main_poll();
    if (next_event == 0) {
      SleepConditionVariableCS(&cv, &cs, INFINITE);
    } else {
      SleepConditionVariableCS(&cv, &cs,
                               (DWORD)(next_event / (1000 * OC_CLOCK_SECOND)));
    }
  }
#endif
  
#ifdef __linux__
  // linux specific loop
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

  // shut down the stack
  oc_main_shutdown();
  return 0;
}