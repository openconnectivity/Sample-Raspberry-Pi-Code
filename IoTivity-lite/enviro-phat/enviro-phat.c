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
 input_file            : /home/pi/workspace/envirophat/device_output/out_codegeneration_merged.swagger.json
 version of input_file : 20190222
 title of input_file   : EnviroPhat
*/

#include "/usr/include/python2.7/Python.h"

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

//Python calling stuff
#define MAX_BRIGHTNESS 65535  // maximum brightness of light sensor

static PyObject *pName, *pModule, *pFunc;
static PyObject *pArgs, *pValue;

int myParamArgs[2];
long returnLong = 0;
double returnDouble = 0.0;
double returnDoubleArray[3];

/*
* Funcion to call Pimoroni python libraries
*/
int CallPythonFunction(char moduleName[], char functionName[], int numArgs, int args[])
{
    int i;

    printf("module = %s\n", moduleName);
    pName = PyString_FromString(moduleName);
    /* Error checking of pName left out */

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        printf("function = %s\n", functionName);
        pFunc = PyObject_GetAttrString(pModule, functionName);
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_New(numArgs);
            for (i = 0; i < numArgs; ++i) {
                printf("arg%d = %d\n", i+1, args[i]);
                pValue = PyInt_FromLong(args[i]);
                if (!pValue) {
                    Py_DECREF(pArgs);
                    Py_DECREF(pModule);
                    fprintf(stderr, "Cannot convert argument\n");
                    return 1;
                }
                /* pValue reference stolen here: */
                PyTuple_SetItem(pArgs, i, pValue);
            }
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL) {
                if (PyList_Check(pValue)) {
                  printf("x: %f\n", returnDoubleArray[0] = PyFloat_AsDouble(PyList_GetItem(pValue, 0)));
                  printf("y: %f\n", returnDoubleArray[1] = PyFloat_AsDouble(PyList_GetItem(pValue, 1)));
                  printf("z: %f\n", returnDoubleArray[2] = PyFloat_AsDouble(PyList_GetItem(pValue, 2)));
                } else if (PyFloat_Check(pValue)) {
                    returnDouble = PyFloat_AsDouble(pValue);
                    printf("Result of call: %f\n", returnDouble);
                } else {
                    returnLong = PyInt_AsLong(pValue);
                    printf("Result of call: %ld\n", PyInt_AsLong(pValue));
                }
                Py_DECREF(pValue);
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return 1;
            }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", functionName);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", moduleName);
        return 1;
    }

    return 0;
}

/* global property variables for path: "/brightness" */
static char g_brightness_RESOURCE_PROPERTY_NAME_brightness[] = "brightness"; /* the name for the attribute */
int g_brightness_brightness = 50; /* current value of property "brightness" The Quantized representation in the range 0-100 of the current sensed or set value for Brig
htness. */
/* global property variables for path: "/color" */
static char g_color_RESOURCE_PROPERTY_NAME_rgbValue[] = "rgbValue"; /* the name for the attribute */

/* array rgbValue  The RGB value; the first item is the R, second the G, third the B. */
int g_color_rgbValue[3];
size_t g_color_rgbValue_array_size;

/* global property variables for path: "/colorSensorLight" */
static char g_colorSensorLight_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_colorSensorLight_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/heading" */
static char g_heading_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_heading_value = true; /* current value of property "value" true = sensed, false = not sensed. */
/* global property variables for path: "/pressure" */
/* global property variables for path: "/temperature" */
static char g_temperature_RESOURCE_PROPERTY_NAME_temperature[] = "temperature"; /* the name for the attribute */
double g_temperature_temperature = 20.0; /* current value of property "temperature"  The current temperature setting or measurement. */
static char g_temperature_RESOURCE_PROPERTY_NAME_units[] = "units"; /* the name for the attribute */
char g_temperature_units[ MAX_PAYLOAD_STRING ] = "C"; /* current value of property "units" The unit for the conveyed temperature value, Note that when doing an UPDATE,
 the unit on the device does NOT change, it only indicates the unit of the conveyed value during the UPDATE operation. */
/* global property variables for path: "/voltage0" */
static char g_voltage0_RESOURCE_PROPERTY_NAME_current[] = "current"; /* the name for the attribute */
double g_voltage0_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
static char g_voltage0_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; /* the name for the attribute */
double g_voltage0_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
static char g_voltage0_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; /* the name for the attribute */
double g_voltage0_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
static char g_voltage0_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; /* the name for the attribute */
double g_voltage0_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
static char g_voltage0_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; /* the name for the attribute */
double g_voltage0_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
static char g_voltage0_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; /* the name for the attribute */
double g_voltage0_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
/* global property variables for path: "/voltage1" */
static char g_voltage1_RESOURCE_PROPERTY_NAME_current[] = "current"; /* the name for the attribute */
double g_voltage1_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
static char g_voltage1_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; /* the name for the attribute */
double g_voltage1_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
static char g_voltage1_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; /* the name for the attribute */
double g_voltage1_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
static char g_voltage1_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; /* the name for the attribute */
double g_voltage1_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
static char g_voltage1_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; /* the name for the attribute */
double g_voltage1_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
static char g_voltage1_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; /* the name for the attribute */
double g_voltage1_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
/* global property variables for path: "/voltage2" */
static char g_voltage2_RESOURCE_PROPERTY_NAME_current[] = "current"; /* the name for the attribute */
double g_voltage2_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
static char g_voltage2_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; /* the name for the attribute */
double g_voltage2_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
static char g_voltage2_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; /* the name for the attribute */
double g_voltage2_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
static char g_voltage2_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; /* the name for the attribute */
double g_voltage2_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
static char g_voltage2_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; /* the name for the attribute */
double g_voltage2_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
static char g_voltage2_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; /* the name for the attribute */
double g_voltage2_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
/* global property variables for path: "/voltage3" */
static char g_voltage3_RESOURCE_PROPERTY_NAME_current[] = "current"; /* the name for the attribute */
double g_voltage3_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
static char g_voltage3_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; /* the name for the attribute */
double g_voltage3_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
static char g_voltage3_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; /* the name for the attribute */
double g_voltage3_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
static char g_voltage3_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; /* the name for the attribute */
double g_voltage3_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
static char g_voltage3_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; /* the name for the attribute */
double g_voltage3_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
static char g_voltage3_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; /* the name for the attribute */
double g_voltage3_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
/* global property variables for path: "/xmotion" */
static char g_xmotion_RESOURCE_PROPERTY_NAME_acceleration[] = "acceleration"; /* the name for the attribute */
double g_xmotion_acceleration = 0.5; /* current value of property "acceleration"  The sensed acceleration experienced in 'g'. */
/* global property variables for path: "/ymotion" */
static char g_ymotion_RESOURCE_PROPERTY_NAME_acceleration[] = "acceleration"; /* the name for the attribute */
double g_ymotion_acceleration = 0.5; /* current value of property "acceleration"  The sensed acceleration experienced in 'g'. */
/* global property variables for path: "/zmotion" */
static char g_zmotion_RESOURCE_PROPERTY_NAME_acceleration[] = "acceleration"; /* the name for the attribute */
double g_zmotion_acceleration = 0.5; /* current value of property "acceleration"  The sensed acceleration experienced in 'g'. *//* registration data variables for the
resources */

/* global resource variables for path: /brightness */
static char g_brightness_RESOURCE_ENDPOINT[] = "/brightness"; /* used path for this resource */
static char g_brightness_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.light.brightness"}; /* rt value (as an array) */
int g_brightness_nr_resource_types = 1;
static char g_brightness_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_brightness_nr_resource_interfaces = 2;

/* global resource variables for path: /color */
static char g_color_RESOURCE_ENDPOINT[] = "/color"; /* used path for this resource */
static char g_color_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.colour.rgb"}; /* rt value (as an array) */
int g_color_nr_resource_types = 1;
static char g_color_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_color_nr_resource_interfaces = 2;

/* global resource variables for path: /colorSensorLight */
static char g_colorSensorLight_RESOURCE_ENDPOINT[] = "/colorSensorLight"; /* used path for this resource */
static char g_colorSensorLight_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_colorSensorLight_nr_resource_types = 1;
static char g_colorSensorLight_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_colorSensorLight_nr_resource_interfaces = 2;

/* global resource variables for path: /heading */
static char g_heading_RESOURCE_ENDPOINT[] = "/heading"; /* used path for this resource */
static char g_heading_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor"}; /* rt value (as an array) */
int g_heading_nr_resource_types = 1;
static char g_heading_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_heading_nr_resource_interfaces = 2;

/* global resource variables for path: /pressure */
static char g_pressure_RESOURCE_ENDPOINT[] = "/pressure"; /* used path for this resource */
static char g_pressure_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor"}; /* rt value (as an array) */
int g_pressure_nr_resource_types = 1;
static char g_pressure_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_pressure_nr_resource_interfaces = 2;

/* global resource variables for path: /temperature */
static char g_temperature_RESOURCE_ENDPOINT[] = "/temperature"; /* used path for this resource */
static char g_temperature_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.temperature"}; /* rt value (as an array) */
int g_temperature_nr_resource_types = 1;
static char g_temperature_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_temperature_nr_resource_interfaces = 2;

/* global resource variables for path: /voltage0 */
static char g_voltage0_RESOURCE_ENDPOINT[] = "/voltage0"; /* used path for this resource */
static char g_voltage0_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; /* rt value (as an array) */
int g_voltage0_nr_resource_types = 1;
static char g_voltage0_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_voltage0_nr_resource_interfaces = 2;

/* global resource variables for path: /voltage1 */
static char g_voltage1_RESOURCE_ENDPOINT[] = "/voltage1"; /* used path for this resource */
static char g_voltage1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; /* rt value (as an array) */
int g_voltage1_nr_resource_types = 1;
static char g_voltage1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_voltage1_nr_resource_interfaces = 2;

/* global resource variables for path: /voltage2 */
static char g_voltage2_RESOURCE_ENDPOINT[] = "/voltage2"; /* used path for this resource */
static char g_voltage2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; /* rt value (as an array) */
int g_voltage2_nr_resource_types = 1;
static char g_voltage2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_voltage2_nr_resource_interfaces = 2;

/* global resource variables for path: /voltage3 */
static char g_voltage3_RESOURCE_ENDPOINT[] = "/voltage3"; /* used path for this resource */
static char g_voltage3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; /* rt value (as an array) */
int g_voltage3_nr_resource_types = 1;
static char g_voltage3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_voltage3_nr_resource_interfaces = 2;

/* global resource variables for path: /xmotion */
static char g_xmotion_RESOURCE_ENDPOINT[] = "/xmotion"; /* used path for this resource */
static char g_xmotion_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.acceleration"}; /* rt value (as an array) */
int g_xmotion_nr_resource_types = 1;
static char g_xmotion_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_xmotion_nr_resource_interfaces = 2;

/* global resource variables for path: /ymotion */
static char g_ymotion_RESOURCE_ENDPOINT[] = "/ymotion"; /* used path for this resource */
static char g_ymotion_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.acceleration"}; /* rt value (as an array) */
int g_ymotion_nr_resource_types = 1;
static char g_ymotion_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_ymotion_nr_resource_interfaces = 2;

/* global resource variables for path: /zmotion */
static char g_zmotion_RESOURCE_ENDPOINT[] = "/zmotion"; /* used path for this resource */
static char g_zmotion_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.acceleration"}; /* rt value (as an array) */
int g_zmotion_nr_resource_types = 1;
static char g_zmotion_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; /* interface if (as an array) */
int g_zmotion_nr_resource_interfaces = 2;
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
  ret |= oc_add_device("/oic/d", "oic.d.envirophat", "EnviroPhat",
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
  }
  return error_state;
}



/**
* get method for "/brightness" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes the brightness of a light or lamp.
* The Property "brightness" is an integer showing the current brightness level as a quantized representation in the range 0-100.
* A brightness of 0 is the minimum for the resource.
* A brightness of 100 is the maximum for the resource.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_brightness(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;

  myParamArgs[0] = 0;
  CallPythonFunction((char *)"enviro-phat", (char *)"readBrightness", 0, myParamArgs);
  g_brightness_brightness = returnLong * 100 / MAX_BRIGHTNESS;

  PRINT("-- Begin get_brightness: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);

    /* property (integer) 'brightness' */
    oc_rep_set_int(root, brightness, g_brightness_brightness);
    PRINT("   %s : %d\n", g_brightness_RESOURCE_PROPERTY_NAME_brightness, g_brightness_brightness);
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
  PRINT("-- End get_brightness\n");
}

/**
* get method for "/color" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource specifies the actual colour in the RGB space represented as an array of integers.
* Each colour value is described with a Red, Green, Blue component.
* These colour values are encoded as an array of integer values ([R,G,B]).
* The minimum and maximum colour value per component may be described by the Property "range".
* When "range" is omitted, then the "range" is [0,255].
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_color(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;


  PRINT("-- Begin get_color: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);


    /* property (array of integers) 'rgbValue' */
    oc_rep_set_array(root, rgbValue);
    PRINT("   %s int = [ ", g_color_RESOURCE_PROPERTY_NAME_rgbValue);
    for (int i=0; i< (int)g_color_rgbValue_array_size; i++) {
      oc_rep_add_int(rgbValue, g_color_rgbValue[i]);
      PRINT("   %d ", g_color_rgbValue[i]);
    }
    oc_rep_close_array(root, rgbValue);
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
  PRINT("-- End get_color\n");
}

/**
* get method for "/colorSensorLight" resource.
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
get_colorSensorLight(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;


  PRINT("-- Begin get_colorSensorLight: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);

    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_colorSensorLight_value);
    PRINT("   %s : %s\n", g_colorSensorLight_RESOURCE_PROPERTY_NAME_value,  btoa(g_colorSensorLight_value));
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
  PRINT("-- End get_colorSensorLight\n");
}

/**
* get method for "/heading" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes whether some value or property or entity has been sensed or not.
* The Property "value" is a boolean.
* A value of 'true' means that the target has been sensed.
* A value of 'false' means that the target has not been sensed.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_heading(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;


  PRINT("-- Begin get_heading: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);

    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_heading_value);
    PRINT("   %s : %s\n", g_heading_RESOURCE_PROPERTY_NAME_value,  btoa(g_heading_value));
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
  PRINT("-- End get_heading\n");
}

/**
* get method for "/pressure" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes whether some value or property or entity has been sensed or not.
* The Property "value" is a boolean.
* A value of 'true' means that the target has been sensed.
* A value of 'false' means that the target has not been sensed.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_pressure(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;


  PRINT("-- Begin get_pressure: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);

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
  PRINT("-- End get_pressure\n");
}

/**
* get method for "/temperature" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes a sensed or actuated Temperature value.
* The Property "temperature" describes the current value measured.
* The Property "units" is a single value that is one of "C", "F" or "K".
* It provides the unit of measurement for the "temperature" value.
* It is a read-only value that is provided by the server.
* If the "units" Property is missing the default is Celsius [C].
* When the Property "range" is omitted the default is +/- MAXINT.
* A client can specify the units for the requested temperature by use of a query parameter.
* If no query parameter is provided the server provides its default measure or set value.
* It is recommended to return always the units Property in the result.
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_temperature(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;

  myParamArgs[0] = 0;
  CallPythonFunction((char *)"enviro-phat", (char *)"readTemperature", 0, myParamArgs);
  g_temperature_temperature = returnDouble;

  /* query name 'units' type: 'string', enum: ['C', 'F', 'K']*/
  char *_units = NULL; /* not null terminated Units */
  int _units_len = oc_get_query_value(request, "units", &_units);
  if (_units_len != -1) {
    PRINT (" query value 'units': %.*s\n", _units_len, _units);
    bool query_ok = false;

    if ( strncmp (_units, "C", _units_len) == 0)  query_ok = true;
    if ( strncmp (_units, "F", _units_len) == 0)  query_ok = true;
    if ( strncmp (_units, "K", _units_len) == 0)  query_ok = true;
    if (query_ok == false) error_state = true;

    /* TODO: use the query value to tailer the response*/
  }

  PRINT("-- Begin get_temperature: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);

    /* property (number) 'temperature' */
    oc_rep_set_double(root, temperature, g_temperature_temperature);
    PRINT("   %s : %f\n", g_temperature_RESOURCE_PROPERTY_NAME_temperature, g_temperature_temperature);
    /* property (string) 'units' */
    oc_rep_set_text_string(root, units, g_temperature_units);
    PRINT("   %s : %s\n", g_temperature_RESOURCE_PROPERTY_NAME_units, g_temperature_units);
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
  PRINT("-- End get_temperature\n");
}

/**
* get method for "/voltage0" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (r
ead-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_voltage0(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;

  myParamArgs[0] = 1;
  CallPythonFunction((char *)"enviro-phat", (char *)"readADC", 1, myParamArgs);
  g_voltage0_voltage = returnDouble;

  PRINT("-- Begin get_voltage0: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);

    /* property (number) 'current' */
    oc_rep_set_double(root, current, g_voltage0_current);
    PRINT("   %s : %f\n", g_voltage0_RESOURCE_PROPERTY_NAME_current, g_voltage0_current);
    /* property (number) 'desiredcurrent' */
    oc_rep_set_double(root, desiredcurrent, g_voltage0_desiredcurrent);
    PRINT("   %s : %f\n", g_voltage0_RESOURCE_PROPERTY_NAME_desiredcurrent, g_voltage0_desiredcurrent);
    /* property (number) 'desiredfrequency' */
    oc_rep_set_double(root, desiredfrequency, g_voltage0_desiredfrequency);
    PRINT("   %s : %f\n", g_voltage0_RESOURCE_PROPERTY_NAME_desiredfrequency, g_voltage0_desiredfrequency);
    /* property (number) 'desiredvoltage' */
    oc_rep_set_double(root, desiredvoltage, g_voltage0_desiredvoltage);
    PRINT("   %s : %f\n", g_voltage0_RESOURCE_PROPERTY_NAME_desiredvoltage, g_voltage0_desiredvoltage);
    /* property (number) 'frequency' */
    oc_rep_set_double(root, frequency, g_voltage0_frequency);
    PRINT("   %s : %f\n", g_voltage0_RESOURCE_PROPERTY_NAME_frequency, g_voltage0_frequency);
    /* property (number) 'voltage' */
    oc_rep_set_double(root, voltage, g_voltage0_voltage);
    PRINT("   %s : %f\n", g_voltage0_RESOURCE_PROPERTY_NAME_voltage, g_voltage0_voltage);
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
  PRINT("-- End get_voltage0\n");
}

/**
* get method for "/voltage1" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (r
ead-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_voltage1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;

  myParamArgs[0] = 2;
  CallPythonFunction((char *)"enviro-phat", (char *)"readADC", 1, myParamArgs);
  g_voltage1_voltage = returnDouble;

  PRINT("-- Begin get_voltage1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);

    /* property (number) 'current' */
    oc_rep_set_double(root, current, g_voltage1_current);
    PRINT("   %s : %f\n", g_voltage1_RESOURCE_PROPERTY_NAME_current, g_voltage1_current);
    /* property (number) 'desiredcurrent' */
    oc_rep_set_double(root, desiredcurrent, g_voltage1_desiredcurrent);
    PRINT("   %s : %f\n", g_voltage1_RESOURCE_PROPERTY_NAME_desiredcurrent, g_voltage1_desiredcurrent);
    /* property (number) 'desiredfrequency' */
    oc_rep_set_double(root, desiredfrequency, g_voltage1_desiredfrequency);
    PRINT("   %s : %f\n", g_voltage1_RESOURCE_PROPERTY_NAME_desiredfrequency, g_voltage1_desiredfrequency);
    /* property (number) 'desiredvoltage' */
    oc_rep_set_double(root, desiredvoltage, g_voltage1_desiredvoltage);
    PRINT("   %s : %f\n", g_voltage1_RESOURCE_PROPERTY_NAME_desiredvoltage, g_voltage1_desiredvoltage);
    /* property (number) 'frequency' */
    oc_rep_set_double(root, frequency, g_voltage1_frequency);
    PRINT("   %s : %f\n", g_voltage1_RESOURCE_PROPERTY_NAME_frequency, g_voltage1_frequency);
    /* property (number) 'voltage' */
    oc_rep_set_double(root, voltage, g_voltage1_voltage);
    PRINT("   %s : %f\n", g_voltage1_RESOURCE_PROPERTY_NAME_voltage, g_voltage1_voltage);
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
  PRINT("-- End get_voltage1\n");
}

/**
* get method for "/voltage2" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (r
ead-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_voltage2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;

  myParamArgs[0] = 3;
  CallPythonFunction((char *)"enviro-phat", (char *)"readADC", 1, myParamArgs);
  g_voltage2_voltage = returnDouble;

  PRINT("-- Begin get_voltage2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);

    /* property (number) 'current' */
    oc_rep_set_double(root, current, g_voltage2_current);
    PRINT("   %s : %f\n", g_voltage2_RESOURCE_PROPERTY_NAME_current, g_voltage2_current);
    /* property (number) 'desiredcurrent' */
    oc_rep_set_double(root, desiredcurrent, g_voltage2_desiredcurrent);
    PRINT("   %s : %f\n", g_voltage2_RESOURCE_PROPERTY_NAME_desiredcurrent, g_voltage2_desiredcurrent);
    /* property (number) 'desiredfrequency' */
    oc_rep_set_double(root, desiredfrequency, g_voltage2_desiredfrequency);
    PRINT("   %s : %f\n", g_voltage2_RESOURCE_PROPERTY_NAME_desiredfrequency, g_voltage2_desiredfrequency);
    /* property (number) 'desiredvoltage' */
    oc_rep_set_double(root, desiredvoltage, g_voltage2_desiredvoltage);
    PRINT("   %s : %f\n", g_voltage2_RESOURCE_PROPERTY_NAME_desiredvoltage, g_voltage2_desiredvoltage);
    /* property (number) 'frequency' */
    oc_rep_set_double(root, frequency, g_voltage2_frequency);
    PRINT("   %s : %f\n", g_voltage2_RESOURCE_PROPERTY_NAME_frequency, g_voltage2_frequency);
    /* property (number) 'voltage' */
    oc_rep_set_double(root, voltage, g_voltage2_voltage);
    PRINT("   %s : %f\n", g_voltage2_RESOURCE_PROPERTY_NAME_voltage, g_voltage2_voltage);
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
  PRINT("-- End get_voltage2\n");
}

/**
* get method for "/voltage3" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (r
ead-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_voltage3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;

  myParamArgs[0] = 4;
  CallPythonFunction((char *)"enviro-phat", (char *)"readADC", 1, myParamArgs);
  g_voltage3_voltage = returnDouble;

  PRINT("-- Begin get_voltage3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);

    /* property (number) 'current' */
    oc_rep_set_double(root, current, g_voltage3_current);
    PRINT("   %s : %f\n", g_voltage3_RESOURCE_PROPERTY_NAME_current, g_voltage3_current);
    /* property (number) 'desiredcurrent' */
    oc_rep_set_double(root, desiredcurrent, g_voltage3_desiredcurrent);
    PRINT("   %s : %f\n", g_voltage3_RESOURCE_PROPERTY_NAME_desiredcurrent, g_voltage3_desiredcurrent);
    /* property (number) 'desiredfrequency' */
    oc_rep_set_double(root, desiredfrequency, g_voltage3_desiredfrequency);
    PRINT("   %s : %f\n", g_voltage3_RESOURCE_PROPERTY_NAME_desiredfrequency, g_voltage3_desiredfrequency);
    /* property (number) 'desiredvoltage' */
    oc_rep_set_double(root, desiredvoltage, g_voltage3_desiredvoltage);
    PRINT("   %s : %f\n", g_voltage3_RESOURCE_PROPERTY_NAME_desiredvoltage, g_voltage3_desiredvoltage);
    /* property (number) 'frequency' */
    oc_rep_set_double(root, frequency, g_voltage3_frequency);
    PRINT("   %s : %f\n", g_voltage3_RESOURCE_PROPERTY_NAME_frequency, g_voltage3_frequency);
    /* property (number) 'voltage' */
    oc_rep_set_double(root, voltage, g_voltage3_voltage);
    PRINT("   %s : %f\n", g_voltage3_RESOURCE_PROPERTY_NAME_voltage, g_voltage3_voltage);
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
  PRINT("-- End get_voltage3\n");
}

/**
* get method for "/xmotion" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource provides a measure of proper acceleration (g force) as opposed to co-ordinate acceleration (which is dependent on the co-ordinate system and the observ
er).
* The Property "value" is a float which describes the acceleration experienced by the object in "g".
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_xmotion(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;

  myParamArgs[0] = 0;
  CallPythonFunction((char *)"enviro-phat", (char *)"readAccelerometer", 0, myParamArgs);
  g_xmotion_acceleration = returnDoubleArray[0];
  g_ymotion_acceleration = returnDoubleArray[1];
  g_zmotion_acceleration = returnDoubleArray[2];

  PRINT("-- Begin get_xmotion: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);

    /* property (number) 'acceleration' */
    oc_rep_set_double(root, acceleration, g_xmotion_acceleration);
    PRINT("   %s : %f\n", g_xmotion_RESOURCE_PROPERTY_NAME_acceleration, g_xmotion_acceleration);
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
  PRINT("-- End get_xmotion\n");
}

/**
* get method for "/ymotion" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource provides a measure of proper acceleration (g force) as opposed to co-ordinate acceleration (which is dependent on the co-ordinate system and the observ
er).
* The Property "value" is a float which describes the acceleration experienced by the object in "g".
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_ymotion(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;


  PRINT("-- Begin get_ymotion: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);

    /* property (number) 'acceleration' */
    oc_rep_set_double(root, acceleration, g_ymotion_acceleration);
    PRINT("   %s : %f\n", g_ymotion_RESOURCE_PROPERTY_NAME_acceleration, g_ymotion_acceleration);
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
  PRINT("-- End get_ymotion\n");
}

/**
* get method for "/zmotion" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource provides a measure of proper acceleration (g force) as opposed to co-ordinate acceleration (which is dependent on the co-ordinate system and the observ
er).
* The Property "value" is a float which describes the acceleration experienced by the object in "g".
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_zmotion(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.

     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;


  PRINT("-- Begin get_zmotion: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);

    /* property (number) 'acceleration' */
    oc_rep_set_double(root, acceleration, g_zmotion_acceleration);
    PRINT("   %s : %f\n", g_zmotion_RESOURCE_PROPERTY_NAME_acceleration, g_zmotion_acceleration);
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
  PRINT("-- End get_zmotion\n");
}

/**
* post method for "/colorSensorLight" resource.
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
post_colorSensorLight(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_colorSensorLight:\n");
  oc_rep_t *rep = request->request_payload;

  /* loop over the request document for each required input field to check if all required input fields are present */
  bool var_in_request= false;
  rep = request->request_payload;
  while (rep != NULL) {
    if (strcmp ( oc_string(rep->name), g_colorSensorLight_RESOURCE_PROPERTY_NAME_value) == 0) {
      var_in_request = true;
    }
    rep = rep->next;
  }
  if ( var_in_request == false)
  {
      error_state = true;
      PRINT (" required property: 'value' not in request\n");
  }
  /* loop over the request document to check if all inputs are ok */
  rep = request->request_payload;
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));

    error_state = check_on_readonly_common_resource_properties(rep->name, error_state);
    if (strcmp ( oc_string(rep->name), g_colorSensorLight_RESOURCE_PROPERTY_NAME_value) == 0) {
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

      if (strcmp ( oc_string(rep->name), g_colorSensorLight_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        PRINT ("  property 'value' : %s\n", btoa(rep->value.boolean));
        g_colorSensorLight_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_colorSensorLight_value);

    oc_rep_end_root_object();
    /* TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
       one can use the global variables as input to those calls
       the global values have been updated already with the data from the request */
    myParamArgs[0] = g_colorSensorLight_value ? 1 : 0;
    CallPythonFunction((char *)"enviro-phat", (char *)"writeLEDs", 1, myParamArgs);

    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    PRINT("  Returning Error \n");
    /* TODO: add error response, if any */
    //oc_send_response(request, OC_STATUS_NOT_MODIFIED);
    oc_send_response(request, OC_STATUS_BAD_REQUEST);
  }
  PRINT("-- End post_colorSensorLight\n");
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

  PRINT("Register Resource with local path \"/brightness\"\n");
  oc_resource_t *res_brightness = oc_new_resource(NULL, g_brightness_RESOURCE_ENDPOINT, g_brightness_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_brightness_nr_resource_types);
  for( int a = 0; a < g_brightness_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_brightness_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_brightness,g_brightness_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_brightness_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_brightness, convert_if_string(g_brightness_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_brightness, convert_if_string(g_brightness_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_brightness_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_brightness, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_brightness, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_brightness, true); */

  oc_resource_set_request_handler(res_brightness, OC_GET, get_brightness, NULL);
  oc_add_resource(res_brightness);

  PRINT("Register Resource with local path \"/color\"\n");
  oc_resource_t *res_color = oc_new_resource(NULL, g_color_RESOURCE_ENDPOINT, g_color_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_color_nr_resource_types);
  for( int a = 0; a < g_color_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_color_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_color,g_color_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_color_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_color, convert_if_string(g_color_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_color, convert_if_string(g_color_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_color_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_color, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_color, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_color, true); */

  oc_resource_set_request_handler(res_color, OC_GET, get_color, NULL);
  oc_add_resource(res_color);

  PRINT("Register Resource with local path \"/colorSensorLight\"\n");
  oc_resource_t *res_colorSensorLight = oc_new_resource(NULL, g_colorSensorLight_RESOURCE_ENDPOINT, g_colorSensorLight_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_colorSensorLight_nr_resource_types);
  for( int a = 0; a < g_colorSensorLight_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_colorSensorLight_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_colorSensorLight,g_colorSensorLight_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_colorSensorLight_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_colorSensorLight, convert_if_string(g_colorSensorLight_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_colorSensorLight, convert_if_string(g_colorSensorLight_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_colorSensorLight_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_colorSensorLight, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_colorSensorLight, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_colorSensorLight, true); */

  oc_resource_set_request_handler(res_colorSensorLight, OC_GET, get_colorSensorLight, NULL);

  oc_resource_set_request_handler(res_colorSensorLight, OC_POST, post_colorSensorLight, NULL);
  oc_add_resource(res_colorSensorLight);

  PRINT("Register Resource with local path \"/heading\"\n");
  oc_resource_t *res_heading = oc_new_resource(NULL, g_heading_RESOURCE_ENDPOINT, g_heading_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_heading_nr_resource_types);
  for( int a = 0; a < g_heading_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_heading_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_heading,g_heading_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_heading_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_heading, convert_if_string(g_heading_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_heading, convert_if_string(g_heading_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_heading_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_heading, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_heading, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_heading, true); */

  oc_resource_set_request_handler(res_heading, OC_GET, get_heading, NULL);
  oc_add_resource(res_heading);

  PRINT("Register Resource with local path \"/pressure\"\n");
  oc_resource_t *res_pressure = oc_new_resource(NULL, g_pressure_RESOURCE_ENDPOINT, g_pressure_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_pressure_nr_resource_types);
  for( int a = 0; a < g_pressure_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_pressure_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_pressure,g_pressure_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_pressure_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_pressure, convert_if_string(g_pressure_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_pressure, convert_if_string(g_pressure_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_pressure_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_pressure, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_pressure, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_pressure, true); */

  oc_resource_set_request_handler(res_pressure, OC_GET, get_pressure, NULL);
  oc_add_resource(res_pressure);

  PRINT("Register Resource with local path \"/temperature\"\n");
  oc_resource_t *res_temperature = oc_new_resource(NULL, g_temperature_RESOURCE_ENDPOINT, g_temperature_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_temperature_nr_resource_types);
  for( int a = 0; a < g_temperature_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_temperature_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_temperature,g_temperature_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_temperature_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_temperature, convert_if_string(g_temperature_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_temperature, convert_if_string(g_temperature_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_temperature_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_temperature, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_temperature, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_temperature, true); */

  oc_resource_set_request_handler(res_temperature, OC_GET, get_temperature, NULL);
  oc_add_resource(res_temperature);

  PRINT("Register Resource with local path \"/voltage0\"\n");
  oc_resource_t *res_voltage0 = oc_new_resource(NULL, g_voltage0_RESOURCE_ENDPOINT, g_voltage0_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_voltage0_nr_resource_types);
  for( int a = 0; a < g_voltage0_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_voltage0_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_voltage0,g_voltage0_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_voltage0_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_voltage0, convert_if_string(g_voltage0_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_voltage0, convert_if_string(g_voltage0_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_voltage0_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_voltage0, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_voltage0, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_voltage0, true); */

  oc_resource_set_request_handler(res_voltage0, OC_GET, get_voltage0, NULL);
  oc_add_resource(res_voltage0);

  PRINT("Register Resource with local path \"/voltage1\"\n");
  oc_resource_t *res_voltage1 = oc_new_resource(NULL, g_voltage1_RESOURCE_ENDPOINT, g_voltage1_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_voltage1_nr_resource_types);
  for( int a = 0; a < g_voltage1_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_voltage1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_voltage1,g_voltage1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_voltage1_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_voltage1, convert_if_string(g_voltage1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_voltage1, convert_if_string(g_voltage1_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_voltage1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_voltage1, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_voltage1, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_voltage1, true); */

  oc_resource_set_request_handler(res_voltage1, OC_GET, get_voltage1, NULL);
  oc_add_resource(res_voltage1);

  PRINT("Register Resource with local path \"/voltage2\"\n");
  oc_resource_t *res_voltage2 = oc_new_resource(NULL, g_voltage2_RESOURCE_ENDPOINT, g_voltage2_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_voltage2_nr_resource_types);
  for( int a = 0; a < g_voltage2_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_voltage2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_voltage2,g_voltage2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_voltage2_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_voltage2, convert_if_string(g_voltage2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_voltage2, convert_if_string(g_voltage2_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_voltage2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_voltage2, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_voltage2, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_voltage2, true); */

  oc_resource_set_request_handler(res_voltage2, OC_GET, get_voltage2, NULL);
  oc_add_resource(res_voltage2);

  PRINT("Register Resource with local path \"/voltage3\"\n");
  oc_resource_t *res_voltage3 = oc_new_resource(NULL, g_voltage3_RESOURCE_ENDPOINT, g_voltage3_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_voltage3_nr_resource_types);
  for( int a = 0; a < g_voltage3_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_voltage3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_voltage3,g_voltage3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_voltage3_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_voltage3, convert_if_string(g_voltage3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_voltage3, convert_if_string(g_voltage3_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_voltage3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_voltage3, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_voltage3, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_voltage3, true); */

  oc_resource_set_request_handler(res_voltage3, OC_GET, get_voltage3, NULL);
  oc_add_resource(res_voltage3);

  PRINT("Register Resource with local path \"/xmotion\"\n");
  oc_resource_t *res_xmotion = oc_new_resource(NULL, g_xmotion_RESOURCE_ENDPOINT, g_xmotion_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_xmotion_nr_resource_types);
  for( int a = 0; a < g_xmotion_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_xmotion_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_xmotion,g_xmotion_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_xmotion_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_xmotion, convert_if_string(g_xmotion_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_xmotion, convert_if_string(g_xmotion_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_xmotion_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_xmotion, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_xmotion, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_xmotion, true); */

  oc_resource_set_request_handler(res_xmotion, OC_GET, get_xmotion, NULL);
  oc_add_resource(res_xmotion);

  PRINT("Register Resource with local path \"/ymotion\"\n");
  oc_resource_t *res_ymotion = oc_new_resource(NULL, g_ymotion_RESOURCE_ENDPOINT, g_ymotion_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_ymotion_nr_resource_types);
  for( int a = 0; a < g_ymotion_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_ymotion_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_ymotion,g_ymotion_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_ymotion_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_ymotion, convert_if_string(g_ymotion_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_ymotion, convert_if_string(g_ymotion_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_ymotion_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_ymotion, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_ymotion, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_ymotion, true); */

  oc_resource_set_request_handler(res_ymotion, OC_GET, get_ymotion, NULL);
  oc_add_resource(res_ymotion);

  PRINT("Register Resource with local path \"/zmotion\"\n");
  oc_resource_t *res_zmotion = oc_new_resource(NULL, g_zmotion_RESOURCE_ENDPOINT, g_zmotion_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_zmotion_nr_resource_types);
  for( int a = 0; a < g_zmotion_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_zmotion_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_zmotion,g_zmotion_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_zmotion_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_zmotion, convert_if_string(g_zmotion_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_zmotion, convert_if_string(g_zmotion_RESOURCE_INTERFACE[0]));
  PRINT("     Default OCF Interface: \"%s\"\n", g_zmotion_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_zmotion, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_zmotion, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_zmotion, true); */

  oc_resource_set_request_handler(res_zmotion, OC_GET, get_zmotion, NULL);
  oc_add_resource(res_zmotion);
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
  /* initialize global variables for resource "/brightness" */
  g_brightness_brightness = 50; /* current value of property "brightness" The Quantized representation in the range 0-100 of the current sensed or set value for Bright
ness. */
  /* initialize global variables for resource "/color" */
  /* initialize array "rgbValue" : The RGB value; the first item is the R, second the G, third the B. */g_color_rgbValue[0] = 255;
  g_color_rgbValue[1] = 255;
  g_color_rgbValue[2] = 255;
  g_color_rgbValue_array_size = 3;

  /* initialize global variables for resource "/colorSensorLight" */  g_colorSensorLight_value = false; /* current value of property "value" The status of the switch.
*/
  /* initialize global variables for resource "/heading" */  g_heading_value = true; /* current value of property "value" true = sensed, false = not sensed. */
  /* initialize global variables for resource "/pressure" */
  /* initialize global variables for resource "/temperature" */
  g_temperature_temperature = 20.0; /* current value of property "temperature"  The current temperature setting or measurement. */
  strcpy(g_temperature_units, "C");  /* current value of property "units" The unit for the conveyed temperature value, Note that when doing an UPDATE, the unit on the
device does NOT change, it only indicates the unit of the conveyed value during the UPDATE operation. */
  /* initialize global variables for resource "/voltage0" */
  g_voltage0_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
  g_voltage0_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
  g_voltage0_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
  g_voltage0_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
  g_voltage0_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
  g_voltage0_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
  /* initialize global variables for resource "/voltage1" */
  g_voltage1_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
  g_voltage1_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
  g_voltage1_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
  g_voltage1_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
  g_voltage1_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
  g_voltage1_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
  /* initialize global variables for resource "/voltage2" */
  g_voltage2_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
  g_voltage2_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
  g_voltage2_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
  g_voltage2_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
  g_voltage2_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
  g_voltage2_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
  /* initialize global variables for resource "/voltage3" */
  g_voltage3_current = 5.0; /* current value of property "current"  The electric current in Amps (A). */
  g_voltage3_desiredcurrent = 0; /* current value of property "desiredcurrent"  The desired electric current in Amps (A). */
  g_voltage3_desiredfrequency = 0; /* current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz). */
  g_voltage3_desiredvoltage = 0; /* current value of property "desiredvoltage"  The desired electric voltage in Volts (V). */
  g_voltage3_frequency = 60.0; /* current value of property "frequency"  The electric frequency in Hertz (Hz). */
  g_voltage3_voltage = 120.0; /* current value of property "voltage"  The electric voltage in Volts (V). */
  /* initialize global variables for resource "/xmotion" */
  g_xmotion_acceleration = 0.5; /* current value of property "acceleration"  The sensed acceleration experienced in 'g'. */
  /* initialize global variables for resource "/ymotion" */
  g_ymotion_acceleration = 0.5; /* current value of property "acceleration"  The sensed acceleration experienced in 'g'. */
  /* initialize global variables for resource "/zmotion" */
  g_zmotion_acceleration = 0.5; /* current value of property "acceleration"  The sensed acceleration experienced in 'g'. */

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

  PRINT("Used input file : \"/home/pi/workspace/envirophat/device_output/out_codegeneration_merged.swagger.json\"\n");
  PRINT("OCF Server name : \"EnviroPhat\"\n");

#ifdef OC_SECURITY
  PRINT("Intialize Secure Resources\n");
  oc_storage_config("./devicebuilderserver_creds");
#endif /* OC_SECURITY */

#ifdef OC_SECURITY
  /* please comment out if the server:
    - have no display capabilities to display the PIN value
    - server does not require to implement RANDOM PIN (oic.sec.doxm.rdp) onboarding mechanism
  */
  oc_set_random_pin_callback(random_pin_cb, NULL);
#endif /* OC_SECURITY */

  oc_set_factory_presets_cb(factory_presets_cb, NULL);

  Py_Initialize();

  /* start the stack */
  init = oc_main_init(&handler);

  if (init < 0)
    return init;

  PRINT("OCF server \"EnviroPhat\" running, waiting on incoming connections.\n");

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

  Py_Finalize();

  return 0;
}
