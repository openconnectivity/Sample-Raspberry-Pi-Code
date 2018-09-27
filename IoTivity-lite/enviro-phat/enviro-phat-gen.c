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
 input_file            : /home/pi/tmp/envirophatlite/device_output/out_codegeneration_merged.swagger.json
 version of input_file : v1.1.0-20160519
 title of input_file   : Binary Switch
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

// global variables for path: /brightness
static char g_brightness_RESOURCE_PROPERTY_NAME_brightness[] = "brightness"; // the name for the attribute
int g_brightness_brightness = 50; // current value of property "brightness" Quantized representation in the range 0-100 of the current sensed or set value for Brightness
// global variables for path: /color
static char g_color_RESOURCE_PROPERTY_NAME_rgbValue[] = "rgbValue"; // the name for the attribute
// array not handled rgbValue  RGB value; the first item is the R, second the G, third the B.
// global variables for path: /colorSensorLight
static char g_colorSensorLight_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_colorSensorLight_value = false; // current value of property "value" Status of the switch
// global variables for path: /heading
static char g_heading_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
// array not handled value  Array containing Hx, Hy, Hz.
// global variables for path: /pressure
static char g_pressure_RESOURCE_PROPERTY_NAME_id[] = "id"; // the name for the attribute
char g_pressure_id[MAX_STRING] = "unique_example_id"; // current value of property "id" Instance ID of this specific resource
static char g_pressure_RESOURCE_PROPERTY_NAME_atmosphericPressure[] = "atmosphericPressure"; // the name for the attribute
double g_pressure_atmosphericPressure = 1000.4; // current value of property "atmosphericPressure"  Current atmospheric pressure in hPa.
// global variables for path: /temperature
static char g_temperature_RESOURCE_PROPERTY_NAME_units[] = "units"; // the name for the attribute
char g_temperature_units[MAX_STRING] = "C"; // current value of property "units" Units for the temperature value
static char g_temperature_RESOURCE_PROPERTY_NAME_id[] = "id"; // the name for the attribute
char g_temperature_id[MAX_STRING] = "unique_example_id"; // current value of property "id" Instance ID of this specific resource
static char g_temperature_RESOURCE_PROPERTY_NAME_temperature[] = "temperature"; // the name for the attribute
double g_temperature_temperature = 20.0; // current value of property "temperature"  Current temperature setting or measurement
// global variables for path: /voltage0
static char g_voltage0_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
double g_voltage0_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_voltage0_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
double g_voltage0_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
static char g_voltage0_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
double g_voltage0_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_voltage0_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
double g_voltage0_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_voltage0_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
double g_voltage0_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_voltage0_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
double g_voltage0_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
// global variables for path: /voltage1
static char g_voltage1_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
double g_voltage1_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_voltage1_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
double g_voltage1_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
static char g_voltage1_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
double g_voltage1_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_voltage1_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
double g_voltage1_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_voltage1_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
double g_voltage1_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_voltage1_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
double g_voltage1_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
// global variables for path: /voltage2
static char g_voltage2_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
double g_voltage2_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_voltage2_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
double g_voltage2_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
static char g_voltage2_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
double g_voltage2_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_voltage2_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
double g_voltage2_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_voltage2_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
double g_voltage2_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_voltage2_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
double g_voltage2_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
// global variables for path: /voltage3
static char g_voltage3_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
double g_voltage3_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_voltage3_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
double g_voltage3_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
static char g_voltage3_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
double g_voltage3_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_voltage3_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
double g_voltage3_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_voltage3_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
double g_voltage3_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_voltage3_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
double g_voltage3_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
// global variables for path: /xmotion
static char g_xmotion_RESOURCE_PROPERTY_NAME_acceleration[] = "acceleration"; // the name for the attribute
double g_xmotion_acceleration = 0.5; // current value of property "acceleration"  sensed acceleration experienced in 'g'.
// global variables for path: /ymotion
static char g_ymotion_RESOURCE_PROPERTY_NAME_acceleration[] = "acceleration"; // the name for the attribute
double g_ymotion_acceleration = 0.5; // current value of property "acceleration"  sensed acceleration experienced in 'g'.
// global variables for path: /zmotion
static char g_zmotion_RESOURCE_PROPERTY_NAME_acceleration[] = "acceleration"; // the name for the attribute
double g_zmotion_acceleration = 0.5; // current value of property "acceleration"  sensed acceleration experienced in 'g'.// registration data variables for the resources

static char g_brightness_RESOURCE_ENDPOINT[] = "/brightness";  // used path for this resource
static char g_brightness_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.light.brightness"}; // rt value (as an array)
int g_brightness_nr_resource_types = 1;
static char g_brightness_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_brightness_nr_resource_interfaces = 2;

static char g_color_RESOURCE_ENDPOINT[] = "/color";  // used path for this resource
static char g_color_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.colour.rgb"}; // rt value (as an array)
int g_color_nr_resource_types = 1;
static char g_color_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_color_nr_resource_interfaces = 2;

static char g_colorSensorLight_RESOURCE_ENDPOINT[] = "/colorSensorLight";  // used path for this resource
static char g_colorSensorLight_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_colorSensorLight_nr_resource_types = 1;
static char g_colorSensorLight_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array) 
int g_colorSensorLight_nr_resource_interfaces = 2;

static char g_heading_RESOURCE_ENDPOINT[] = "/heading";  // used path for this resource
static char g_heading_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.magneticfielddirection"}; // rt value (as an array)
int g_heading_nr_resource_types = 1;
static char g_heading_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_heading_nr_resource_interfaces = 2;

static char g_pressure_RESOURCE_ENDPOINT[] = "/pressure";  // used path for this resource
static char g_pressure_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.atmosphericpressure"}; // rt value (as an array)
int g_pressure_nr_resource_types = 1;
static char g_pressure_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_pressure_nr_resource_interfaces = 2;

static char g_temperature_RESOURCE_ENDPOINT[] = "/temperature";  // used path for this resource
static char g_temperature_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.temperature"}; // rt value (as an array)
int g_temperature_nr_resource_types = 1;
static char g_temperature_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_temperature_nr_resource_interfaces = 2;

static char g_voltage0_RESOURCE_ENDPOINT[] = "/voltage0";  // used path for this resource
static char g_voltage0_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; // rt value (as an array)
int g_voltage0_nr_resource_types = 1;
static char g_voltage0_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_voltage0_nr_resource_interfaces = 2;

static char g_voltage1_RESOURCE_ENDPOINT[] = "/voltage1";  // used path for this resource
static char g_voltage1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; // rt value (as an array)
int g_voltage1_nr_resource_types = 1;
static char g_voltage1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_voltage1_nr_resource_interfaces = 2;

static char g_voltage2_RESOURCE_ENDPOINT[] = "/voltage2";  // used path for this resource
static char g_voltage2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; // rt value (as an array)
int g_voltage2_nr_resource_types = 1;
static char g_voltage2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_voltage2_nr_resource_interfaces = 2;

static char g_voltage3_RESOURCE_ENDPOINT[] = "/voltage3";  // used path for this resource
static char g_voltage3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; // rt value (as an array)
int g_voltage3_nr_resource_types = 1;
static char g_voltage3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_voltage3_nr_resource_interfaces = 2;

static char g_xmotion_RESOURCE_ENDPOINT[] = "/xmotion";  // used path for this resource
static char g_xmotion_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.acceleration"}; // rt value (as an array)
int g_xmotion_nr_resource_types = 1;
static char g_xmotion_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_xmotion_nr_resource_interfaces = 2;

static char g_ymotion_RESOURCE_ENDPOINT[] = "/ymotion";  // used path for this resource
static char g_ymotion_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.acceleration"}; // rt value (as an array)
int g_ymotion_nr_resource_types = 1;
static char g_ymotion_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_ymotion_nr_resource_interfaces = 2;

static char g_zmotion_RESOURCE_ENDPOINT[] = "/zmotion";  // used path for this resource
static char g_zmotion_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.acceleration"}; // rt value (as an array)
int g_zmotion_nr_resource_types = 1;
static char g_zmotion_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array) 
int g_zmotion_nr_resource_interfaces = 2;
/**
* function to set up the device.
*
*/
static int
app_init(void)
{
  int ret = oc_init_platform("ocf", NULL, NULL);
  ret |= oc_add_device("/oic/d", "oic.d.light", "Binary Switch", 
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
* get method for "/brightness" endpoint to intialize the returned values from the global values
* This resource describes the brightness of a light or lamp.
* brightness is an integer showing the current brightness level as a quantized representation in the range 0-100.
* A brightness of 0 is the minimum for the resource.
* A brightness of 100 is the maximum for the resource.
* Retrieves the current brightness level.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_brightness(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_brightness: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_int(root, brightness, g_brightness_brightness ); 
    PRINT("   %s : %d\n", g_brightness_RESOURCE_PROPERTY_NAME_brightness, g_brightness_brightness );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/color" endpoint to intialize the returned values from the global values
* This resource specifies the actual colour in the RGB space represented as an array of integers.
* Each colour value is described with a Red, Green, Blue component.
* These colour values are encoded as an array of integer values ([R,G,B]).
* The minimum and maximum colour value per component may be described by range (from oic.r.baseresource).
* When range (from oic.r.baseresource) is omitted, then the range is [0,255].
* Retrieves the current colour in RGB.
* Value is an array of integer values in the order R,G,B.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_color(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_color: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    PRINT("   ARRAY NOT handled%s : %s\n", g_color_RESOURCE_PROPERTY_NAME_rgbValue );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/colorSensorLight" endpoint to intialize the returned values from the global values
* This resource describes a binary switch (on/off).
* The value is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_colorSensorLight(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_colorSensorLight: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_colorSensorLight_value); 
    PRINT("   %s : %d\n", g_colorSensorLight_RESOURCE_PROPERTY_NAME_value,  g_colorSensorLight_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/heading" endpoint to intialize the returned values from the global values
* This resource describes the direction of the Earth's magnetic field at the observer's current point in space.
* Typical use case includes measurement of compass readings on a personal device.
* The value is an array containing Hx, Hy, Hz (in that order) each of which are floats.
* Each of Hx, Hy and Hz are expressed in A/m (Amperes per metre)
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_heading(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_heading: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    PRINT("   ARRAY NOT handled%s : %s\n", g_heading_RESOURCE_PROPERTY_NAME_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/pressure" endpoint to intialize the returned values from the global values
* This resource provides a measurement of Mean Sea Level Pressure experienced at the measuring point expressed in millibars.
* The value is float which describes the atmospheric pressure in hPa (hectoPascals).
* Note that hPa and the also commonly used unit of millibars (mbar) are numerically equivalent.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_pressure(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_pressure: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_text_string(root, id, g_pressure_id ); 
    PRINT("   %s : %s\n", g_pressure_RESOURCE_PROPERTY_NAME_id, g_pressure_id );
    
    oc_rep_set_double(root, atmosphericPressure, g_pressure_atmosphericPressure ); 
    PRINT("   %s : %f\n", g_pressure_RESOURCE_PROPERTY_NAME_atmosphericPressure, g_pressure_atmosphericPressure );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/temperature" endpoint to intialize the returned values from the global values
* This resource describes a sensed or actuated Temperature value.
* The temperature describes the current value measured.
* The units is a single value that is one of C, F or K.
* It provides the unit of measurement for the temperature value.
* It is a read-only value that is provided by the server.
* If the units Property is missing the default is Celsius [C].
* When range (from oic.r.baseresource) is omitted the default is +/- MAXINT.
* Retrieves the current temperature value.
* A client can specify the units for the requested temperature by use of a query parameter.
* If no query parameter is provided the server provides its default measure or set value.
* It is recommended to return always the units Property in the result.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_temperature(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_temperature: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_text_string(root, units, g_temperature_units ); 
    PRINT("   %s : %s\n", g_temperature_RESOURCE_PROPERTY_NAME_units, g_temperature_units );
    
    oc_rep_set_text_string(root, id, g_temperature_id ); 
    PRINT("   %s : %s\n", g_temperature_RESOURCE_PROPERTY_NAME_id, g_temperature_id );
    
    oc_rep_set_double(root, temperature, g_temperature_temperature ); 
    PRINT("   %s : %f\n", g_temperature_RESOURCE_PROPERTY_NAME_temperature, g_temperature_temperature );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/voltage0" endpoint to intialize the returned values from the global values
* This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
* Retrieves the current energy.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_voltage0(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_voltage0: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_double(root, desiredvoltage, g_voltage0_desiredvoltage ); 
    PRINT("   %s : %f\n", g_voltage0_RESOURCE_PROPERTY_NAME_desiredvoltage, g_voltage0_desiredvoltage );
    
    oc_rep_set_double(root, frequency, g_voltage0_frequency ); 
    PRINT("   %s : %f\n", g_voltage0_RESOURCE_PROPERTY_NAME_frequency, g_voltage0_frequency );
    
    oc_rep_set_double(root, current, g_voltage0_current ); 
    PRINT("   %s : %f\n", g_voltage0_RESOURCE_PROPERTY_NAME_current, g_voltage0_current );
    
    oc_rep_set_double(root, desiredcurrent, g_voltage0_desiredcurrent ); 
    PRINT("   %s : %f\n", g_voltage0_RESOURCE_PROPERTY_NAME_desiredcurrent, g_voltage0_desiredcurrent );
    
    oc_rep_set_double(root, voltage, g_voltage0_voltage ); 
    PRINT("   %s : %f\n", g_voltage0_RESOURCE_PROPERTY_NAME_voltage, g_voltage0_voltage );
    
    oc_rep_set_double(root, desiredfrequency, g_voltage0_desiredfrequency ); 
    PRINT("   %s : %f\n", g_voltage0_RESOURCE_PROPERTY_NAME_desiredfrequency, g_voltage0_desiredfrequency );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/voltage1" endpoint to intialize the returned values from the global values
* This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
* Retrieves the current energy.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_voltage1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_voltage1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_double(root, desiredvoltage, g_voltage1_desiredvoltage ); 
    PRINT("   %s : %f\n", g_voltage1_RESOURCE_PROPERTY_NAME_desiredvoltage, g_voltage1_desiredvoltage );
    
    oc_rep_set_double(root, frequency, g_voltage1_frequency ); 
    PRINT("   %s : %f\n", g_voltage1_RESOURCE_PROPERTY_NAME_frequency, g_voltage1_frequency );
    
    oc_rep_set_double(root, current, g_voltage1_current ); 
    PRINT("   %s : %f\n", g_voltage1_RESOURCE_PROPERTY_NAME_current, g_voltage1_current );
    
    oc_rep_set_double(root, desiredcurrent, g_voltage1_desiredcurrent ); 
    PRINT("   %s : %f\n", g_voltage1_RESOURCE_PROPERTY_NAME_desiredcurrent, g_voltage1_desiredcurrent );
    
    oc_rep_set_double(root, voltage, g_voltage1_voltage ); 
    PRINT("   %s : %f\n", g_voltage1_RESOURCE_PROPERTY_NAME_voltage, g_voltage1_voltage );
    
    oc_rep_set_double(root, desiredfrequency, g_voltage1_desiredfrequency ); 
    PRINT("   %s : %f\n", g_voltage1_RESOURCE_PROPERTY_NAME_desiredfrequency, g_voltage1_desiredfrequency );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/voltage2" endpoint to intialize the returned values from the global values
* This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
* Retrieves the current energy.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_voltage2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_voltage2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_double(root, desiredvoltage, g_voltage2_desiredvoltage ); 
    PRINT("   %s : %f\n", g_voltage2_RESOURCE_PROPERTY_NAME_desiredvoltage, g_voltage2_desiredvoltage );
    
    oc_rep_set_double(root, frequency, g_voltage2_frequency ); 
    PRINT("   %s : %f\n", g_voltage2_RESOURCE_PROPERTY_NAME_frequency, g_voltage2_frequency );
    
    oc_rep_set_double(root, current, g_voltage2_current ); 
    PRINT("   %s : %f\n", g_voltage2_RESOURCE_PROPERTY_NAME_current, g_voltage2_current );
    
    oc_rep_set_double(root, desiredcurrent, g_voltage2_desiredcurrent ); 
    PRINT("   %s : %f\n", g_voltage2_RESOURCE_PROPERTY_NAME_desiredcurrent, g_voltage2_desiredcurrent );
    
    oc_rep_set_double(root, voltage, g_voltage2_voltage ); 
    PRINT("   %s : %f\n", g_voltage2_RESOURCE_PROPERTY_NAME_voltage, g_voltage2_voltage );
    
    oc_rep_set_double(root, desiredfrequency, g_voltage2_desiredfrequency ); 
    PRINT("   %s : %f\n", g_voltage2_RESOURCE_PROPERTY_NAME_desiredfrequency, g_voltage2_desiredfrequency );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/voltage3" endpoint to intialize the returned values from the global values
* This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
* Retrieves the current energy.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_voltage3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_voltage3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_double(root, desiredvoltage, g_voltage3_desiredvoltage ); 
    PRINT("   %s : %f\n", g_voltage3_RESOURCE_PROPERTY_NAME_desiredvoltage, g_voltage3_desiredvoltage );
    
    oc_rep_set_double(root, frequency, g_voltage3_frequency ); 
    PRINT("   %s : %f\n", g_voltage3_RESOURCE_PROPERTY_NAME_frequency, g_voltage3_frequency );
    
    oc_rep_set_double(root, current, g_voltage3_current ); 
    PRINT("   %s : %f\n", g_voltage3_RESOURCE_PROPERTY_NAME_current, g_voltage3_current );
    
    oc_rep_set_double(root, desiredcurrent, g_voltage3_desiredcurrent ); 
    PRINT("   %s : %f\n", g_voltage3_RESOURCE_PROPERTY_NAME_desiredcurrent, g_voltage3_desiredcurrent );
    
    oc_rep_set_double(root, voltage, g_voltage3_voltage ); 
    PRINT("   %s : %f\n", g_voltage3_RESOURCE_PROPERTY_NAME_voltage, g_voltage3_voltage );
    
    oc_rep_set_double(root, desiredfrequency, g_voltage3_desiredfrequency ); 
    PRINT("   %s : %f\n", g_voltage3_RESOURCE_PROPERTY_NAME_desiredfrequency, g_voltage3_desiredfrequency );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/xmotion" endpoint to intialize the returned values from the global values
* This resource provides a measure of proper acceleration (g force) as opposed to co-ordinate acceleration
* (which is dependent on the co-ordinate system and the observer).
* The value is a float which describes the acceleration experienced by the object in "g".
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_xmotion(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_xmotion: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_double(root, acceleration, g_xmotion_acceleration ); 
    PRINT("   %s : %f\n", g_xmotion_RESOURCE_PROPERTY_NAME_acceleration, g_xmotion_acceleration );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/ymotion" endpoint to intialize the returned values from the global values
* This resource provides a measure of proper acceleration (g force) as opposed to co-ordinate acceleration
* (which is dependent on the co-ordinate system and the observer).
* The value is a float which describes the acceleration experienced by the object in "g".
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_ymotion(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_ymotion: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_double(root, acceleration, g_ymotion_acceleration ); 
    PRINT("   %s : %f\n", g_ymotion_RESOURCE_PROPERTY_NAME_acceleration, g_ymotion_acceleration );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* get method for "/zmotion" endpoint to intialize the returned values from the global values
* This resource provides a measure of proper acceleration (g force) as opposed to co-ordinate acceleration
* (which is dependent on the co-ordinate system and the observer).
* The value is a float which describes the acceleration experienced by the object in "g".
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_zmotion(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  
  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the calls needs to fill in the global variable before it is returned.
  // alternative is to have a callback from the hardware that sets the global variables
  
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  
  PRINT("get_zmotion: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_double(root, acceleration, g_zmotion_acceleration ); 
    PRINT("   %s : %f\n", g_zmotion_RESOURCE_PROPERTY_NAME_acceleration, g_zmotion_acceleration );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
* post method for "/colorSensorLight" endpoint to assign the returned values to the global values.

* @param requestRep the request representation.
*/
static void
post_colorSensorLight(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_colorSensorLight:\n");
  oc_rep_t *rep = request->request_payload;
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_colorSensorLight_RESOURCE_PROPERTY_NAME_value) == 0)
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
      if (strcmp ( oc_string(rep->name), g_colorSensorLight_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_colorSensorLight_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_colorSensorLight_value); 
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

  PRINT("register resource with path /brightness\n");
  oc_resource_t *res_brightness = oc_new_resource(NULL, g_brightness_RESOURCE_ENDPOINT, g_brightness_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_brightness_nr_resource_types);
  for( int a = 0; a < g_brightness_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_brightness_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_brightness,g_brightness_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_brightness_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_brightness, convert_if_string(g_brightness_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_brightness, convert_if_string(g_brightness_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_brightness_RESOURCE_INTERFACE[0]), g_brightness_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_brightness, true);
  oc_resource_set_periodic_observable(res_brightness, 1);
   
  oc_resource_set_request_handler(res_brightness, OC_GET, get_brightness, NULL);
  oc_add_resource(res_brightness);

  PRINT("register resource with path /color\n");
  oc_resource_t *res_color = oc_new_resource(NULL, g_color_RESOURCE_ENDPOINT, g_color_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_color_nr_resource_types);
  for( int a = 0; a < g_color_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_color_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_color,g_color_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_color_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_color, convert_if_string(g_color_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_color, convert_if_string(g_color_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_color_RESOURCE_INTERFACE[0]), g_color_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_color, true);
  oc_resource_set_periodic_observable(res_color, 1);
   
  oc_resource_set_request_handler(res_color, OC_GET, get_color, NULL);
  oc_add_resource(res_color);

  PRINT("register resource with path /colorSensorLight\n");
  oc_resource_t *res_colorSensorLight = oc_new_resource(NULL, g_colorSensorLight_RESOURCE_ENDPOINT, g_colorSensorLight_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_colorSensorLight_nr_resource_types);
  for( int a = 0; a < g_colorSensorLight_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_colorSensorLight_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_colorSensorLight,g_colorSensorLight_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_colorSensorLight_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_colorSensorLight, convert_if_string(g_colorSensorLight_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_colorSensorLight, convert_if_string(g_colorSensorLight_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_colorSensorLight_RESOURCE_INTERFACE[0]), g_colorSensorLight_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_colorSensorLight, true);
  oc_resource_set_periodic_observable(res_colorSensorLight, 1);
   
  oc_resource_set_request_handler(res_colorSensorLight, OC_GET, get_colorSensorLight, NULL);
   
  oc_resource_set_request_handler(res_colorSensorLight, OC_POST, post_colorSensorLight, NULL);
  oc_add_resource(res_colorSensorLight);

  PRINT("register resource with path /heading\n");
  oc_resource_t *res_heading = oc_new_resource(NULL, g_heading_RESOURCE_ENDPOINT, g_heading_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_heading_nr_resource_types);
  for( int a = 0; a < g_heading_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_heading_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_heading,g_heading_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_heading_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_heading, convert_if_string(g_heading_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_heading, convert_if_string(g_heading_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_heading_RESOURCE_INTERFACE[0]), g_heading_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_heading, true);
  oc_resource_set_periodic_observable(res_heading, 1);
   
  oc_resource_set_request_handler(res_heading, OC_GET, get_heading, NULL);
  oc_add_resource(res_heading);

  PRINT("register resource with path /pressure\n");
  oc_resource_t *res_pressure = oc_new_resource(NULL, g_pressure_RESOURCE_ENDPOINT, g_pressure_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_pressure_nr_resource_types);
  for( int a = 0; a < g_pressure_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_pressure_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_pressure,g_pressure_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_pressure_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_pressure, convert_if_string(g_pressure_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_pressure, convert_if_string(g_pressure_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_pressure_RESOURCE_INTERFACE[0]), g_pressure_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_pressure, true);
  oc_resource_set_periodic_observable(res_pressure, 1);
   
  oc_resource_set_request_handler(res_pressure, OC_GET, get_pressure, NULL);
  oc_add_resource(res_pressure);

  PRINT("register resource with path /temperature\n");
  oc_resource_t *res_temperature = oc_new_resource(NULL, g_temperature_RESOURCE_ENDPOINT, g_temperature_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_temperature_nr_resource_types);
  for( int a = 0; a < g_temperature_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_temperature_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_temperature,g_temperature_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_temperature_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_temperature, convert_if_string(g_temperature_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_temperature, convert_if_string(g_temperature_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_temperature_RESOURCE_INTERFACE[0]), g_temperature_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_temperature, true);
  oc_resource_set_periodic_observable(res_temperature, 1);
   
  oc_resource_set_request_handler(res_temperature, OC_GET, get_temperature, NULL);
  oc_add_resource(res_temperature);

  PRINT("register resource with path /voltage0\n");
  oc_resource_t *res_voltage0 = oc_new_resource(NULL, g_voltage0_RESOURCE_ENDPOINT, g_voltage0_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_voltage0_nr_resource_types);
  for( int a = 0; a < g_voltage0_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_voltage0_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_voltage0,g_voltage0_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_voltage0_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_voltage0, convert_if_string(g_voltage0_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_voltage0, convert_if_string(g_voltage0_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_voltage0_RESOURCE_INTERFACE[0]), g_voltage0_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_voltage0, true);
  oc_resource_set_periodic_observable(res_voltage0, 1);
   
  oc_resource_set_request_handler(res_voltage0, OC_GET, get_voltage0, NULL);
  oc_add_resource(res_voltage0);

  PRINT("register resource with path /voltage1\n");
  oc_resource_t *res_voltage1 = oc_new_resource(NULL, g_voltage1_RESOURCE_ENDPOINT, g_voltage1_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_voltage1_nr_resource_types);
  for( int a = 0; a < g_voltage1_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_voltage1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_voltage1,g_voltage1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_voltage1_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_voltage1, convert_if_string(g_voltage1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_voltage1, convert_if_string(g_voltage1_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_voltage1_RESOURCE_INTERFACE[0]), g_voltage1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_voltage1, true);
  oc_resource_set_periodic_observable(res_voltage1, 1);
   
  oc_resource_set_request_handler(res_voltage1, OC_GET, get_voltage1, NULL);
  oc_add_resource(res_voltage1);

  PRINT("register resource with path /voltage2\n");
  oc_resource_t *res_voltage2 = oc_new_resource(NULL, g_voltage2_RESOURCE_ENDPOINT, g_voltage2_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_voltage2_nr_resource_types);
  for( int a = 0; a < g_voltage2_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_voltage2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_voltage2,g_voltage2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_voltage2_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_voltage2, convert_if_string(g_voltage2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_voltage2, convert_if_string(g_voltage2_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_voltage2_RESOURCE_INTERFACE[0]), g_voltage2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_voltage2, true);
  oc_resource_set_periodic_observable(res_voltage2, 1);
   
  oc_resource_set_request_handler(res_voltage2, OC_GET, get_voltage2, NULL);
  oc_add_resource(res_voltage2);

  PRINT("register resource with path /voltage3\n");
  oc_resource_t *res_voltage3 = oc_new_resource(NULL, g_voltage3_RESOURCE_ENDPOINT, g_voltage3_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_voltage3_nr_resource_types);
  for( int a = 0; a < g_voltage3_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_voltage3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_voltage3,g_voltage3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_voltage3_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_voltage3, convert_if_string(g_voltage3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_voltage3, convert_if_string(g_voltage3_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_voltage3_RESOURCE_INTERFACE[0]), g_voltage3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_voltage3, true);
  oc_resource_set_periodic_observable(res_voltage3, 1);
   
  oc_resource_set_request_handler(res_voltage3, OC_GET, get_voltage3, NULL);
  oc_add_resource(res_voltage3);

  PRINT("register resource with path /xmotion\n");
  oc_resource_t *res_xmotion = oc_new_resource(NULL, g_xmotion_RESOURCE_ENDPOINT, g_xmotion_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_xmotion_nr_resource_types);
  for( int a = 0; a < g_xmotion_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_xmotion_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_xmotion,g_xmotion_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_xmotion_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_xmotion, convert_if_string(g_xmotion_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_xmotion, convert_if_string(g_xmotion_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_xmotion_RESOURCE_INTERFACE[0]), g_xmotion_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_xmotion, true);
  oc_resource_set_periodic_observable(res_xmotion, 1);
   
  oc_resource_set_request_handler(res_xmotion, OC_GET, get_xmotion, NULL);
  oc_add_resource(res_xmotion);

  PRINT("register resource with path /ymotion\n");
  oc_resource_t *res_ymotion = oc_new_resource(NULL, g_ymotion_RESOURCE_ENDPOINT, g_ymotion_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_ymotion_nr_resource_types);
  for( int a = 0; a < g_ymotion_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_ymotion_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_ymotion,g_ymotion_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_ymotion_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_ymotion, convert_if_string(g_ymotion_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_ymotion, convert_if_string(g_ymotion_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_ymotion_RESOURCE_INTERFACE[0]), g_ymotion_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_ymotion, true);
  oc_resource_set_periodic_observable(res_ymotion, 1);
   
  oc_resource_set_request_handler(res_ymotion, OC_GET, get_ymotion, NULL);
  oc_add_resource(res_ymotion);

  PRINT("register resource with path /zmotion\n");
  oc_resource_t *res_zmotion = oc_new_resource(NULL, g_zmotion_RESOURCE_ENDPOINT, g_zmotion_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_zmotion_nr_resource_types);
  for( int a = 0; a < g_zmotion_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_zmotion_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_zmotion,g_zmotion_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_zmotion_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_zmotion, convert_if_string(g_zmotion_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_zmotion, convert_if_string(g_zmotion_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_zmotion_RESOURCE_INTERFACE[0]), g_zmotion_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_zmotion, true);
  oc_resource_set_periodic_observable(res_zmotion, 1);
   
  oc_resource_set_request_handler(res_zmotion, OC_GET, get_zmotion, NULL);
  oc_add_resource(res_zmotion);
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
  // initialize global variables for endpoint "/brightness"
  g_brightness_brightness = 50; // current value of property "brightness" Quantized representation in the range 0-100 of the current sensed or set value for Brightness
  
  // initialize global variables for endpoint "/color"
  // initialize vector rgbValue  RGB value; the first item is the R, second the G, third the B.
  
  //m_var_value_rgbValue.push_back(255);
  
  //m_var_value_rgbValue.push_back(255);
  
  //m_var_value_rgbValue.push_back(255);
  
  // initialize global variables for endpoint "/colorSensorLight"
  g_colorSensorLight_value = false; // current value of property "value" Status of the switch
  
  // initialize global variables for endpoint "/heading"
  // initialize vector value  Array containing Hx, Hy, Hz.
  
  //m_var_value_value.push_back(100.0);
  
  //m_var_value_value.push_back(15.0);
  
  //m_var_value_value.push_back(90.0);
  
  // initialize global variables for endpoint "/pressure"
  strcpy(g_pressure_id,"unique_example_id");  // current value of property "id" Instance ID of this specific resource
  g_pressure_atmosphericPressure = 1000.4; // current value of property "atmosphericPressure"  Current atmospheric pressure in hPa.
  
  // initialize global variables for endpoint "/temperature"
  strcpy(g_temperature_units,"C");  // current value of property "units" Units for the temperature value
  strcpy(g_temperature_id,"unique_example_id");  // current value of property "id" Instance ID of this specific resource
  g_temperature_temperature = 20.0; // current value of property "temperature"  Current temperature setting or measurement
  
  // initialize global variables for endpoint "/voltage0"
  g_voltage0_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_voltage0_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
  g_voltage0_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_voltage0_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_voltage0_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_voltage0_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
  
  // initialize global variables for endpoint "/voltage1"
  g_voltage1_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_voltage1_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
  g_voltage1_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_voltage1_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_voltage1_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_voltage1_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
  
  // initialize global variables for endpoint "/voltage2"
  g_voltage2_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_voltage2_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
  g_voltage2_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_voltage2_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_voltage2_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_voltage2_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
  
  // initialize global variables for endpoint "/voltage3"
  g_voltage3_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_voltage3_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
  g_voltage3_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_voltage3_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_voltage3_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_voltage3_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
  
  // initialize global variables for endpoint "/xmotion"
  g_xmotion_acceleration = 0.5; // current value of property "acceleration"  sensed acceleration experienced in 'g'.
  
  // initialize global variables for endpoint "/ymotion"
  g_ymotion_acceleration = 0.5; // current value of property "acceleration"  sensed acceleration experienced in 'g'.
  
  // initialize global variables for endpoint "/zmotion"
  g_zmotion_acceleration = 0.5; // current value of property "acceleration"  sensed acceleration experienced in 'g'.
   
  
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
  
  PRINT("file : /home/pi/tmp/envirophatlite/device_output/out_codegeneration_merged.swagger.json\n");
  PRINT("title: Binary Switch\n");

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