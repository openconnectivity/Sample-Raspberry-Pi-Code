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
 input_file            : /home/pi/workspace/explorerhatlite/device_output/out_codegeneration_merged.swagger.json
 version of input_file : v1.1.0-20160519
 title of input_file   : Touch Sensor
*/

#include "/usr/include/python2.7/Python.h"

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

//Python calling stuff
static PyObject *pName, *pModule, *pFunc;
static PyObject *pArgs, *pValue;

int myParamArgs[2];
long returnLong = 0;
double returnDouble = 0.0;

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
                if (PyFloat_Check(pValue)) {
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

// global property variables for path: /analog1
static char g_analog1_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
double g_analog1_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
static char g_analog1_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
double g_analog1_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_analog1_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
double g_analog1_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_analog1_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
double g_analog1_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_analog1_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
double g_analog1_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_analog1_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
double g_analog1_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
// global property variables for path: /analog2
static char g_analog2_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
double g_analog2_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
static char g_analog2_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
double g_analog2_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_analog2_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
double g_analog2_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_analog2_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
double g_analog2_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_analog2_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
double g_analog2_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_analog2_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
double g_analog2_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
// global property variables for path: /analog3
static char g_analog3_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
double g_analog3_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
static char g_analog3_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
double g_analog3_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_analog3_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
double g_analog3_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_analog3_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
double g_analog3_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_analog3_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
double g_analog3_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_analog3_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
double g_analog3_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
// global property variables for path: /analog4
static char g_analog4_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
double g_analog4_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
static char g_analog4_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
double g_analog4_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_analog4_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
double g_analog4_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_analog4_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
double g_analog4_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_analog4_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
double g_analog4_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_analog4_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
double g_analog4_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
// global property variables for path: /input1
static char g_input1_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_input1_value = false; // current value of property "value" Status of the switch
// global property variables for path: /input2
static char g_input2_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_input2_value = false; // current value of property "value" Status of the switch
// global property variables for path: /input3
static char g_input3_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_input3_value = false; // current value of property "value" Status of the switch
// global property variables for path: /input4
static char g_input4_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_input4_value = false; // current value of property "value" Status of the switch
// global property variables for path: /light1
static char g_light1_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_light1_value = false; // current value of property "value" Status of the switch
// global property variables for path: /light2
static char g_light2_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_light2_value = false; // current value of property "value" Status of the switch
// global property variables for path: /light3
static char g_light3_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_light3_value = false; // current value of property "value" Status of the switch
// global property variables for path: /light4
static char g_light4_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_light4_value = false; // current value of property "value" Status of the switch
// global property variables for path: /output1
static char g_output1_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_output1_value = false; // current value of property "value" Status of the switch
// global property variables for path: /output2
static char g_output2_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_output2_value = false; // current value of property "value" Status of the switch
// global property variables for path: /output3
static char g_output3_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_output3_value = false; // current value of property "value" Status of the switch
// global property variables for path: /output4
static char g_output4_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_output4_value = false; // current value of property "value" Status of the switch
// global property variables for path: /touch1
static char g_touch1_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_touch1_value = true; // current value of property "value" true = sensed, false = not sensed.
// global property variables for path: /touch2
static char g_touch2_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_touch2_value = true; // current value of property "value" true = sensed, false = not sensed.
// global property variables for path: /touch3
static char g_touch3_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_touch3_value = true; // current value of property "value" true = sensed, false = not sensed.
// global property variables for path: /touch4
static char g_touch4_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_touch4_value = true; // current value of property "value" true = sensed, false = not sensed.
// global property variables for path: /touch5
static char g_touch5_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_touch5_value = true; // current value of property "value" true = sensed, false = not sensed.
// global property variables for path: /touch6
static char g_touch6_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_touch6_value = true; // current value of property "value" true = sensed, false = not sensed.
// global property variables for path: /touch7
static char g_touch7_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_touch7_value = true; // current value of property "value" true = sensed, false = not sensed.
// global property variables for path: /touch8
static char g_touch8_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_touch8_value = true; // current value of property "value" true = sensed, false = not sensed.// registration data variables for the res
ources

// global resource variables for path: /analog1
static char g_analog1_RESOURCE_ENDPOINT[] = "/analog1";  // used path for this resource
static char g_analog1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; // rt value (as an array)
int g_analog1_nr_resource_types = 1;
static char g_analog1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
int g_analog1_nr_resource_interfaces = 2;

// global resource variables for path: /analog2
static char g_analog2_RESOURCE_ENDPOINT[] = "/analog2";  // used path for this resource
static char g_analog2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; // rt value (as an array)
int g_analog2_nr_resource_types = 1;
static char g_analog2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
int g_analog2_nr_resource_interfaces = 2;

// global resource variables for path: /analog3
static char g_analog3_RESOURCE_ENDPOINT[] = "/analog3";  // used path for this resource
static char g_analog3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; // rt value (as an array)
int g_analog3_nr_resource_types = 1;
static char g_analog3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
int g_analog3_nr_resource_interfaces = 2;

// global resource variables for path: /analog4
static char g_analog4_RESOURCE_ENDPOINT[] = "/analog4";  // used path for this resource
static char g_analog4_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.energy.electrical"}; // rt value (as an array)
int g_analog4_nr_resource_types = 1;
static char g_analog4_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
int g_analog4_nr_resource_interfaces = 2;

// global resource variables for path: /input1
static char g_input1_RESOURCE_ENDPOINT[] = "/input1";  // used path for this resource
static char g_input1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_input1_nr_resource_types = 1;
static char g_input1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_input1_nr_resource_interfaces = 2;

// global resource variables for path: /input2
static char g_input2_RESOURCE_ENDPOINT[] = "/input2";  // used path for this resource
static char g_input2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_input2_nr_resource_types = 1;
static char g_input2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_input2_nr_resource_interfaces = 2;

// global resource variables for path: /input3
static char g_input3_RESOURCE_ENDPOINT[] = "/input3";  // used path for this resource
static char g_input3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_input3_nr_resource_types = 1;
static char g_input3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_input3_nr_resource_interfaces = 2;

// global resource variables for path: /input4
static char g_input4_RESOURCE_ENDPOINT[] = "/input4";  // used path for this resource
static char g_input4_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_input4_nr_resource_types = 1;
static char g_input4_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_input4_nr_resource_interfaces = 2;

// global resource variables for path: /light1
static char g_light1_RESOURCE_ENDPOINT[] = "/light1";  // used path for this resource
static char g_light1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_light1_nr_resource_types = 1;
static char g_light1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_light1_nr_resource_interfaces = 2;

// global resource variables for path: /light2
static char g_light2_RESOURCE_ENDPOINT[] = "/light2";  // used path for this resource
static char g_light2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_light2_nr_resource_types = 1;
static char g_light2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_light2_nr_resource_interfaces = 2;

// global resource variables for path: /light3
static char g_light3_RESOURCE_ENDPOINT[] = "/light3";  // used path for this resource
static char g_light3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_light3_nr_resource_types = 1;
static char g_light3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_light3_nr_resource_interfaces = 2;

// global resource variables for path: /light4
static char g_light4_RESOURCE_ENDPOINT[] = "/light4";  // used path for this resource
static char g_light4_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_light4_nr_resource_types = 1;
static char g_light4_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_light4_nr_resource_interfaces = 2;

// global resource variables for path: /output1
static char g_output1_RESOURCE_ENDPOINT[] = "/output1";  // used path for this resource
static char g_output1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_output1_nr_resource_types = 1;
static char g_output1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_output1_nr_resource_interfaces = 2;

// global resource variables for path: /output2
static char g_output2_RESOURCE_ENDPOINT[] = "/output2";  // used path for this resource
static char g_output2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_output2_nr_resource_types = 1;
static char g_output2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_output2_nr_resource_interfaces = 2;

// global resource variables for path: /output3
static char g_output3_RESOURCE_ENDPOINT[] = "/output3";  // used path for this resource
static char g_output3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_output3_nr_resource_types = 1;
static char g_output3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_output3_nr_resource_interfaces = 2;

// global resource variables for path: /output4
static char g_output4_RESOURCE_ENDPOINT[] = "/output4";  // used path for this resource
static char g_output4_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_output4_nr_resource_types = 1;
static char g_output4_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_output4_nr_resource_interfaces = 2;

// global resource variables for path: /touch1
static char g_touch1_RESOURCE_ENDPOINT[] = "/touch1";  // used path for this resource
static char g_touch1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; // rt value (as an array)
int g_touch1_nr_resource_types = 1;
static char g_touch1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_touch1_nr_resource_interfaces = 2;

// global resource variables for path: /touch2
static char g_touch2_RESOURCE_ENDPOINT[] = "/touch2";  // used path for this resource
static char g_touch2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; // rt value (as an array)
int g_touch2_nr_resource_types = 1;
static char g_touch2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_touch2_nr_resource_interfaces = 2;

// global resource variables for path: /touch3
static char g_touch3_RESOURCE_ENDPOINT[] = "/touch3";  // used path for this resource
static char g_touch3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; // rt value (as an array)
int g_touch3_nr_resource_types = 1;
static char g_touch3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_touch3_nr_resource_interfaces = 2;

// global resource variables for path: /touch4
static char g_touch4_RESOURCE_ENDPOINT[] = "/touch4";  // used path for this resource
static char g_touch4_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; // rt value (as an array)
int g_touch4_nr_resource_types = 1;
static char g_touch4_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_touch4_nr_resource_interfaces = 2;

// global resource variables for path: /touch5
static char g_touch5_RESOURCE_ENDPOINT[] = "/touch5";  // used path for this resource
static char g_touch5_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; // rt value (as an array)
int g_touch5_nr_resource_types = 1;
static char g_touch5_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_touch5_nr_resource_interfaces = 2;

// global resource variables for path: /touch6
static char g_touch6_RESOURCE_ENDPOINT[] = "/touch6";  // used path for this resource
static char g_touch6_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; // rt value (as an array)
int g_touch6_nr_resource_types = 1;
static char g_touch6_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_touch6_nr_resource_interfaces = 2;

// global resource variables for path: /touch7
static char g_touch7_RESOURCE_ENDPOINT[] = "/touch7";  // used path for this resource
static char g_touch7_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; // rt value (as an array)
int g_touch7_nr_resource_types = 1;
static char g_touch7_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_touch7_nr_resource_interfaces = 2;

// global resource variables for path: /touch8
static char g_touch8_RESOURCE_ENDPOINT[] = "/touch8";  // used path for this resource
static char g_touch8_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.touch"}; // rt value (as an array)
int g_touch8_nr_resource_types = 1;
static char g_touch8_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_touch8_nr_resource_interfaces = 2;
/**
* function to set up the device.
*
*/
static int
app_init(void)
{
  int ret = oc_init_platform("ocf", NULL, NULL);
  // the settings determine the appearance of the device on the network
  // can be OCF1.3.1 or OCF2.0.0 (or even higher)
  // supplied values are for OCF1.3.1
  ret |= oc_add_device("/oic/d", "oic.d.cooldevice", "Touch Sensor",
                       "ocf.1.0.0", // icv value
                       "ocf.res.1.3.0, ocf.sh.1.3.0",  // dmv value
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
* get method for "/analog1" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-writ
e) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
* Retrieves the current energy.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_analog1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 1;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readAnalog", 1, myParamArgs);
  g_analog1_voltage = returnDouble;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_analog1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_double(root, frequency, g_analog1_frequency );
    PRINT("   %s : %f\n", g_analog1_RESOURCE_PROPERTY_NAME_frequency, g_analog1_frequency );

    oc_rep_set_double(root, desiredcurrent, g_analog1_desiredcurrent );
    PRINT("   %s : %f\n", g_analog1_RESOURCE_PROPERTY_NAME_desiredcurrent, g_analog1_desiredcurrent );

    oc_rep_set_double(root, desiredvoltage, g_analog1_desiredvoltage );
    PRINT("   %s : %f\n", g_analog1_RESOURCE_PROPERTY_NAME_desiredvoltage, g_analog1_desiredvoltage );

    oc_rep_set_double(root, current, g_analog1_current );
    PRINT("   %s : %f\n", g_analog1_RESOURCE_PROPERTY_NAME_current, g_analog1_current );

    oc_rep_set_double(root, voltage, g_analog1_voltage );
    PRINT("   %s : %f\n", g_analog1_RESOURCE_PROPERTY_NAME_voltage, g_analog1_voltage );

    oc_rep_set_double(root, desiredfrequency, g_analog1_desiredfrequency );
    PRINT("   %s : %f\n", g_analog1_RESOURCE_PROPERTY_NAME_desiredfrequency, g_analog1_desiredfrequency );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/analog2" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-writ
e) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
* Retrieves the current energy.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_analog2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 2;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readAnalog", 1, myParamArgs);
  g_analog2_voltage = returnDouble;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_analog2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_double(root, frequency, g_analog2_frequency );
    PRINT("   %s : %f\n", g_analog2_RESOURCE_PROPERTY_NAME_frequency, g_analog2_frequency );

    oc_rep_set_double(root, desiredcurrent, g_analog2_desiredcurrent );
    PRINT("   %s : %f\n", g_analog2_RESOURCE_PROPERTY_NAME_desiredcurrent, g_analog2_desiredcurrent );

    oc_rep_set_double(root, desiredvoltage, g_analog2_desiredvoltage );
    PRINT("   %s : %f\n", g_analog2_RESOURCE_PROPERTY_NAME_desiredvoltage, g_analog2_desiredvoltage );

    oc_rep_set_double(root, current, g_analog2_current );
    PRINT("   %s : %f\n", g_analog2_RESOURCE_PROPERTY_NAME_current, g_analog2_current );

    oc_rep_set_double(root, voltage, g_analog2_voltage );
    PRINT("   %s : %f\n", g_analog2_RESOURCE_PROPERTY_NAME_voltage, g_analog2_voltage );

    oc_rep_set_double(root, desiredfrequency, g_analog2_desiredfrequency );
    PRINT("   %s : %f\n", g_analog2_RESOURCE_PROPERTY_NAME_desiredfrequency, g_analog2_desiredfrequency );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/analog3" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-writ
e) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
* Retrieves the current energy.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_analog3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 3;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readAnalog", 1, myParamArgs);
  g_analog3_voltage = returnDouble;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_analog3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_double(root, frequency, g_analog3_frequency );
    PRINT("   %s : %f\n", g_analog3_RESOURCE_PROPERTY_NAME_frequency, g_analog3_frequency );

    oc_rep_set_double(root, desiredcurrent, g_analog3_desiredcurrent );
    PRINT("   %s : %f\n", g_analog3_RESOURCE_PROPERTY_NAME_desiredcurrent, g_analog3_desiredcurrent );

    oc_rep_set_double(root, desiredvoltage, g_analog3_desiredvoltage );
    PRINT("   %s : %f\n", g_analog3_RESOURCE_PROPERTY_NAME_desiredvoltage, g_analog3_desiredvoltage );

    oc_rep_set_double(root, current, g_analog3_current );
    PRINT("   %s : %f\n", g_analog3_RESOURCE_PROPERTY_NAME_current, g_analog3_current );

    oc_rep_set_double(root, voltage, g_analog3_voltage );
    PRINT("   %s : %f\n", g_analog3_RESOURCE_PROPERTY_NAME_voltage, g_analog3_voltage );

    oc_rep_set_double(root, desiredfrequency, g_analog3_desiredfrequency );
    PRINT("   %s : %f\n", g_analog3_RESOURCE_PROPERTY_NAME_desiredfrequency, g_analog3_desiredfrequency );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/analog4" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-writ
e) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
* Retrieves the current energy.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_analog4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 4;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readAnalog", 1, myParamArgs);
  g_analog4_voltage = returnDouble;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_analog4: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_double(root, frequency, g_analog4_frequency );
    PRINT("   %s : %f\n", g_analog4_RESOURCE_PROPERTY_NAME_frequency, g_analog4_frequency );

    oc_rep_set_double(root, desiredcurrent, g_analog4_desiredcurrent );
    PRINT("   %s : %f\n", g_analog4_RESOURCE_PROPERTY_NAME_desiredcurrent, g_analog4_desiredcurrent );

    oc_rep_set_double(root, desiredvoltage, g_analog4_desiredvoltage );
    PRINT("   %s : %f\n", g_analog4_RESOURCE_PROPERTY_NAME_desiredvoltage, g_analog4_desiredvoltage );

    oc_rep_set_double(root, current, g_analog4_current );
    PRINT("   %s : %f\n", g_analog4_RESOURCE_PROPERTY_NAME_current, g_analog4_current );

    oc_rep_set_double(root, voltage, g_analog4_voltage );
    PRINT("   %s : %f\n", g_analog4_RESOURCE_PROPERTY_NAME_voltage, g_analog4_voltage );

    oc_rep_set_double(root, desiredfrequency, g_analog4_desiredfrequency );
    PRINT("   %s : %f\n", g_analog4_RESOURCE_PROPERTY_NAME_desiredfrequency, g_analog4_desiredfrequency );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/input1" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
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
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 1;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readInput", 1, myParamArgs);
  g_input1_value = (bool)returnLong;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

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
* get method for "/input2" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
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
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 2;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readInput", 1, myParamArgs);
  g_input2_value = (bool)returnLong;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

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
* get method for "/input3" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
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
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 3;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readInput", 1, myParamArgs);
  g_input3_value = (bool)returnLong;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

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
* get method for "/input4" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes a binary switch (on/off).
* The value is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_input4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 4;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readInput", 1, myParamArgs);
  g_input4_value = (bool)returnLong;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_input4: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_input4_value);
    PRINT("   %s : %d\n", g_input4_RESOURCE_PROPERTY_NAME_value,  g_input4_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/light1" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes a binary switch (on/off).
* The value is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_light1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_light1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_light1_value);
    PRINT("   %s : %d\n", g_light1_RESOURCE_PROPERTY_NAME_value,  g_light1_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/light2" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes a binary switch (on/off).
* The value is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_light2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_light2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_light2_value);
    PRINT("   %s : %d\n", g_light2_RESOURCE_PROPERTY_NAME_value,  g_light2_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/light3" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes a binary switch (on/off).
* The value is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_light3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_light3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_light3_value);
    PRINT("   %s : %d\n", g_light3_RESOURCE_PROPERTY_NAME_value,  g_light3_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/light4" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes a binary switch (on/off).
* The value is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_light4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_light4: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_light4_value);
    PRINT("   %s : %d\n", g_light4_RESOURCE_PROPERTY_NAME_value,  g_light4_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/output1" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
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
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

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
* get method for "/output2" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
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
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

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
* get method for "/output3" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
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
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

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
* get method for "/output4" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes a binary switch (on/off).
* The value is a boolean.
* A value of 'true' means that the switch is on.
* A value of 'false' means that the switch is off.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_output4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_output4: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_output4_value);
    PRINT("   %s : %d\n", g_output4_RESOURCE_PROPERTY_NAME_value,  g_output4_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/touch1" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes whether touch has been sensed or not.
* The value is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 1;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, myParamArgs);
  g_touch1_value = (bool)returnLong;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_touch1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_touch1_value);
    PRINT("   %s : %d\n", g_touch1_RESOURCE_PROPERTY_NAME_value,  g_touch1_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/touch2" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes whether touch has been sensed or not.
* The value is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 2;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, myParamArgs);
  g_touch2_value = (bool)returnLong;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_touch2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_touch2_value);
    PRINT("   %s : %d\n", g_touch2_RESOURCE_PROPERTY_NAME_value,  g_touch2_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/touch3" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes whether touch has been sensed or not.
* The value is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 3;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, myParamArgs);
  g_touch3_value = (bool)returnLong;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_touch3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_touch3_value);
    PRINT("   %s : %d\n", g_touch3_RESOURCE_PROPERTY_NAME_value,  g_touch3_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/touch4" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes whether touch has been sensed or not.
* The value is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 4;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, myParamArgs);
  g_touch4_value = (bool)returnLong;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_touch4: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_touch4_value);
    PRINT("   %s : %d\n", g_touch4_RESOURCE_PROPERTY_NAME_value,  g_touch4_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/touch5" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes whether touch has been sensed or not.
* The value is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch5(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 5;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, myParamArgs);
  g_touch5_value = (bool)returnLong;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_touch5: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_touch5_value);
    PRINT("   %s : %d\n", g_touch5_RESOURCE_PROPERTY_NAME_value,  g_touch5_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/touch6" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes whether touch has been sensed or not.
* The value is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch6(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 6;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, myParamArgs);
  g_touch6_value = (bool)returnLong;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_touch6: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_touch6_value);
    PRINT("   %s : %d\n", g_touch6_RESOURCE_PROPERTY_NAME_value,  g_touch6_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/touch7" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes whether touch has been sensed or not.
* The value is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch7(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 7;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, myParamArgs);
  g_touch7_value = (bool)returnLong;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_touch7: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_touch7_value);
    PRINT("   %s : %d\n", g_touch7_RESOURCE_PROPERTY_NAME_value,  g_touch7_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/touch8" endpoint
* function is called to intialize the return values of the GET method
* initialisation of the returned values are done from the global property values
* This resource describes whether touch has been sensed or not.
* The value is a boolean.
* A value of 'true' means that touch has been sensed.
* A value of 'false' means that touch not been sensed.
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_touch8(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.
  myParamArgs[0] = 8;
  CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, myParamArgs);
  g_touch8_value = (bool)returnLong;

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_touch8: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_touch8_value);
    PRINT("   %s : %d\n", g_touch8_RESOURCE_PROPERTY_NAME_value,  g_touch8_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* post method for "/light1" endpoint
* the function has as input the request body, which are the input values of the POST method.
* the input values (as a set) are checked if all supplied values are correct.
* if the input values are correct, they will be assigned to the global  property values.

* @param requestRep the request representation.
*/
static void
post_light1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_light1:\n");
  oc_rep_t *rep = request->request_payload;
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_light1_RESOURCE_PROPERTY_NAME_value) == 0)
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
      if (strcmp ( oc_string(rep->name), g_light1_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_light1_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_light1_value);
    oc_rep_end_root_object();

    // TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
    // one can use the global variables as input to those calls
    // the global values have been updated already with the data from the request
    myParamArgs[0] = 1;
    myParamArgs[1] = g_light1_value ? 1 : 0;
    CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeLight", 2, myParamArgs);

    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
* post method for "/light2" endpoint
* the function has as input the request body, which are the input values of the POST method.
* the input values (as a set) are checked if all supplied values are correct.
* if the input values are correct, they will be assigned to the global  property values.

* @param requestRep the request representation.
*/
static void
post_light2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_light2:\n");
  oc_rep_t *rep = request->request_payload;
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_light2_RESOURCE_PROPERTY_NAME_value) == 0)
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
      if (strcmp ( oc_string(rep->name), g_light2_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_light2_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_light2_value);
    oc_rep_end_root_object();

    // TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
    // one can use the global variables as input to those calls
    // the global values have been updated already with the data from the request
    myParamArgs[0] = 2;
    myParamArgs[1] = g_light2_value ? 1 : 0;
    CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeLight", 2, myParamArgs);

    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
* post method for "/light3" endpoint
* the function has as input the request body, which are the input values of the POST method.
* the input values (as a set) are checked if all supplied values are correct.
* if the input values are correct, they will be assigned to the global  property values.

* @param requestRep the request representation.
*/
static void
post_light3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_light3:\n");
  oc_rep_t *rep = request->request_payload;
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_light3_RESOURCE_PROPERTY_NAME_value) == 0)
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
      if (strcmp ( oc_string(rep->name), g_light3_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_light3_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_light3_value);
    oc_rep_end_root_object();

    // TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
    // one can use the global variables as input to those calls
    // the global values have been updated already with the data from the request
    myParamArgs[0] = 3;
    myParamArgs[1] = g_light3_value ? 1 : 0;
    CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeLight", 2, myParamArgs);

    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
* post method for "/light4" endpoint
* the function has as input the request body, which are the input values of the POST method.
* the input values (as a set) are checked if all supplied values are correct.
* if the input values are correct, they will be assigned to the global  property values.

* @param requestRep the request representation.
*/
static void
post_light4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_light4:\n");
  oc_rep_t *rep = request->request_payload;
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_light4_RESOURCE_PROPERTY_NAME_value) == 0)
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
      if (strcmp ( oc_string(rep->name), g_light4_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_light4_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_light4_value);
    oc_rep_end_root_object();

    // TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
    // one can use the global variables as input to those calls
    // the global values have been updated already with the data from the request
    myParamArgs[0] = 4;
    myParamArgs[1] = g_light4_value ? 1 : 0;
    CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeLight", 2, myParamArgs);

    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
* post method for "/output1" endpoint
* the function has as input the request body, which are the input values of the POST method.
* the input values (as a set) are checked if all supplied values are correct.
* if the input values are correct, they will be assigned to the global  property values.

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
    myParamArgs[0] = 1;
    myParamArgs[1] = g_output1_value ? 1 : 0;
    CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeLight", 2, myParamArgs);

    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
* post method for "/output2" endpoint
* the function has as input the request body, which are the input values of the POST method.
* the input values (as a set) are checked if all supplied values are correct.
* if the input values are correct, they will be assigned to the global  property values.

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
    myParamArgs[0] = 2;
    myParamArgs[1] = g_output2_value ? 1 : 0;
    CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeLight", 2, myParamArgs);

    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
* post method for "/output3" endpoint
* the function has as input the request body, which are the input values of the POST method.
* the input values (as a set) are checked if all supplied values are correct.
* if the input values are correct, they will be assigned to the global  property values.

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
    myParamArgs[0] = 3;
    myParamArgs[1] = g_output3_value ? 1 : 0;
    CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeLight", 2, myParamArgs);

    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
* post method for "/output4" endpoint
* the function has as input the request body, which are the input values of the POST method.
* the input values (as a set) are checked if all supplied values are correct.
* if the input values are correct, they will be assigned to the global  property values.

* @param requestRep the request representation.
*/
static void
post_output4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_output4:\n");
  oc_rep_t *rep = request->request_payload;
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_output4_RESOURCE_PROPERTY_NAME_value) == 0)
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
      if (strcmp ( oc_string(rep->name), g_output4_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_output4_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_output4_value);
    oc_rep_end_root_object();

    // TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
    // one can use the global variables as input to those calls
    // the global values have been updated already with the data from the request
    myParamArgs[0] = 4;
    myParamArgs[1] = g_output4_value ? 1 : 0;
    CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeLight", 2, myParamArgs);

    oc_send_response(request, OC_STATUS_CHANGED);
  }
  else
  {
    // TODO: add error response, if any
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

  PRINT("register resource with path /analog1\n");
  oc_resource_t *res_analog1 = oc_new_resource(NULL, g_analog1_RESOURCE_ENDPOINT, g_analog1_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_analog1_nr_resource_types);
  for( int a = 0; a < g_analog1_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_analog1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_analog1,g_analog1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_analog1_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_analog1, convert_if_string(g_analog1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_analog1, convert_if_string(g_analog1_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_analog1_RESOURCE_INTERFACE[0]), g_analog1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_analog1, true);
  oc_resource_set_periodic_observable(res_analog1, 1);

  oc_resource_set_request_handler(res_analog1, OC_GET, get_analog1, NULL);
  oc_add_resource(res_analog1);

  PRINT("register resource with path /analog2\n");
  oc_resource_t *res_analog2 = oc_new_resource(NULL, g_analog2_RESOURCE_ENDPOINT, g_analog2_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_analog2_nr_resource_types);
  for( int a = 0; a < g_analog2_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_analog2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_analog2,g_analog2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_analog2_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_analog2, convert_if_string(g_analog2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_analog2, convert_if_string(g_analog2_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_analog2_RESOURCE_INTERFACE[0]), g_analog2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_analog2, true);
  oc_resource_set_periodic_observable(res_analog2, 1);

  oc_resource_set_request_handler(res_analog2, OC_GET, get_analog2, NULL);
  oc_add_resource(res_analog2);

  PRINT("register resource with path /analog3\n");
  oc_resource_t *res_analog3 = oc_new_resource(NULL, g_analog3_RESOURCE_ENDPOINT, g_analog3_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_analog3_nr_resource_types);
  for( int a = 0; a < g_analog3_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_analog3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_analog3,g_analog3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_analog3_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_analog3, convert_if_string(g_analog3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_analog3, convert_if_string(g_analog3_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_analog3_RESOURCE_INTERFACE[0]), g_analog3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_analog3, true);
  oc_resource_set_periodic_observable(res_analog3, 1);

  oc_resource_set_request_handler(res_analog3, OC_GET, get_analog3, NULL);
  oc_add_resource(res_analog3);

  PRINT("register resource with path /analog4\n");
  oc_resource_t *res_analog4 = oc_new_resource(NULL, g_analog4_RESOURCE_ENDPOINT, g_analog4_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_analog4_nr_resource_types);
  for( int a = 0; a < g_analog4_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_analog4_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_analog4,g_analog4_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_analog4_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_analog4, convert_if_string(g_analog4_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_analog4, convert_if_string(g_analog4_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_analog4_RESOURCE_INTERFACE[0]), g_analog4_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_analog4, true);
  oc_resource_set_periodic_observable(res_analog4, 1);

  oc_resource_set_request_handler(res_analog4, OC_GET, get_analog4, NULL);
  oc_add_resource(res_analog4);

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

  PRINT("register resource with path /input4\n");
  oc_resource_t *res_input4 = oc_new_resource(NULL, g_input4_RESOURCE_ENDPOINT, g_input4_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_input4_nr_resource_types);
  for( int a = 0; a < g_input4_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_input4_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_input4,g_input4_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_input4_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_input4, convert_if_string(g_input4_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_input4, convert_if_string(g_input4_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_input4_RESOURCE_INTERFACE[0]), g_input4_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_input4, true);
  oc_resource_set_periodic_observable(res_input4, 1);

  oc_resource_set_request_handler(res_input4, OC_GET, get_input4, NULL);
  oc_add_resource(res_input4);

  PRINT("register resource with path /light1\n");
  oc_resource_t *res_light1 = oc_new_resource(NULL, g_light1_RESOURCE_ENDPOINT, g_light1_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_light1_nr_resource_types);
  for( int a = 0; a < g_light1_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_light1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_light1,g_light1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_light1_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_light1, convert_if_string(g_light1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_light1, convert_if_string(g_light1_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_light1_RESOURCE_INTERFACE[0]), g_light1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_light1, true);
  oc_resource_set_periodic_observable(res_light1, 1);

  oc_resource_set_request_handler(res_light1, OC_GET, get_light1, NULL);

  oc_resource_set_request_handler(res_light1, OC_POST, post_light1, NULL);
  oc_add_resource(res_light1);

  PRINT("register resource with path /light2\n");
  oc_resource_t *res_light2 = oc_new_resource(NULL, g_light2_RESOURCE_ENDPOINT, g_light2_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_light2_nr_resource_types);
  for( int a = 0; a < g_light2_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_light2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_light2,g_light2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_light2_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_light2, convert_if_string(g_light2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_light2, convert_if_string(g_light2_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_light2_RESOURCE_INTERFACE[0]), g_light2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_light2, true);
  oc_resource_set_periodic_observable(res_light2, 1);

  oc_resource_set_request_handler(res_light2, OC_GET, get_light2, NULL);

  oc_resource_set_request_handler(res_light2, OC_POST, post_light2, NULL);
  oc_add_resource(res_light2);

  PRINT("register resource with path /light3\n");
  oc_resource_t *res_light3 = oc_new_resource(NULL, g_light3_RESOURCE_ENDPOINT, g_light3_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_light3_nr_resource_types);
  for( int a = 0; a < g_light3_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_light3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_light3,g_light3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_light3_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_light3, convert_if_string(g_light3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_light3, convert_if_string(g_light3_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_light3_RESOURCE_INTERFACE[0]), g_light3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_light3, true);
  oc_resource_set_periodic_observable(res_light3, 1);

  oc_resource_set_request_handler(res_light3, OC_GET, get_light3, NULL);

  oc_resource_set_request_handler(res_light3, OC_POST, post_light3, NULL);
  oc_add_resource(res_light3);

  PRINT("register resource with path /light4\n");
  oc_resource_t *res_light4 = oc_new_resource(NULL, g_light4_RESOURCE_ENDPOINT, g_light4_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_light4_nr_resource_types);
  for( int a = 0; a < g_light4_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_light4_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_light4,g_light4_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_light4_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_light4, convert_if_string(g_light4_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_light4, convert_if_string(g_light4_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_light4_RESOURCE_INTERFACE[0]), g_light4_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_light4, true);
  oc_resource_set_periodic_observable(res_light4, 1);

  oc_resource_set_request_handler(res_light4, OC_GET, get_light4, NULL);

  oc_resource_set_request_handler(res_light4, OC_POST, post_light4, NULL);
  oc_add_resource(res_light4);

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

  PRINT("register resource with path /output4\n");
  oc_resource_t *res_output4 = oc_new_resource(NULL, g_output4_RESOURCE_ENDPOINT, g_output4_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_output4_nr_resource_types);
  for( int a = 0; a < g_output4_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_output4_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_output4,g_output4_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_output4_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_output4, convert_if_string(g_output4_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_output4, convert_if_string(g_output4_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_output4_RESOURCE_INTERFACE[0]), g_output4_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_output4, true);
  oc_resource_set_periodic_observable(res_output4, 1);

  oc_resource_set_request_handler(res_output4, OC_GET, get_output4, NULL);

  oc_resource_set_request_handler(res_output4, OC_POST, post_output4, NULL);
  oc_add_resource(res_output4);

  PRINT("register resource with path /touch1\n");
  oc_resource_t *res_touch1 = oc_new_resource(NULL, g_touch1_RESOURCE_ENDPOINT, g_touch1_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_touch1_nr_resource_types);
  for( int a = 0; a < g_touch1_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_touch1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch1,g_touch1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch1_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_touch1, convert_if_string(g_touch1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch1, convert_if_string(g_touch1_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_touch1_RESOURCE_INTERFACE[0]), g_touch1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch1, true);
  oc_resource_set_periodic_observable(res_touch1, 1);

  oc_resource_set_request_handler(res_touch1, OC_GET, get_touch1, NULL);
  oc_add_resource(res_touch1);

  PRINT("register resource with path /touch2\n");
  oc_resource_t *res_touch2 = oc_new_resource(NULL, g_touch2_RESOURCE_ENDPOINT, g_touch2_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_touch2_nr_resource_types);
  for( int a = 0; a < g_touch2_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_touch2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch2,g_touch2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch2_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_touch2, convert_if_string(g_touch2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch2, convert_if_string(g_touch2_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_touch2_RESOURCE_INTERFACE[0]), g_touch2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch2, true);
  oc_resource_set_periodic_observable(res_touch2, 1);

  oc_resource_set_request_handler(res_touch2, OC_GET, get_touch2, NULL);
  oc_add_resource(res_touch2);

  PRINT("register resource with path /touch3\n");
  oc_resource_t *res_touch3 = oc_new_resource(NULL, g_touch3_RESOURCE_ENDPOINT, g_touch3_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_touch3_nr_resource_types);
  for( int a = 0; a < g_touch3_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_touch3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch3,g_touch3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch3_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_touch3, convert_if_string(g_touch3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch3, convert_if_string(g_touch3_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_touch3_RESOURCE_INTERFACE[0]), g_touch3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch3, true);
  oc_resource_set_periodic_observable(res_touch3, 1);

  oc_resource_set_request_handler(res_touch3, OC_GET, get_touch3, NULL);
  oc_add_resource(res_touch3);

  PRINT("register resource with path /touch4\n");
  oc_resource_t *res_touch4 = oc_new_resource(NULL, g_touch4_RESOURCE_ENDPOINT, g_touch4_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_touch4_nr_resource_types);
  for( int a = 0; a < g_touch4_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_touch4_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch4,g_touch4_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch4_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_touch4, convert_if_string(g_touch4_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch4, convert_if_string(g_touch4_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_touch4_RESOURCE_INTERFACE[0]), g_touch4_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch4, true);
  oc_resource_set_periodic_observable(res_touch4, 1);

  oc_resource_set_request_handler(res_touch4, OC_GET, get_touch4, NULL);
  oc_add_resource(res_touch4);

  PRINT("register resource with path /touch5\n");
  oc_resource_t *res_touch5 = oc_new_resource(NULL, g_touch5_RESOURCE_ENDPOINT, g_touch5_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_touch5_nr_resource_types);
  for( int a = 0; a < g_touch5_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_touch5_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch5,g_touch5_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch5_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_touch5, convert_if_string(g_touch5_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch5, convert_if_string(g_touch5_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_touch5_RESOURCE_INTERFACE[0]), g_touch5_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch5, true);
  oc_resource_set_periodic_observable(res_touch5, 1);

  oc_resource_set_request_handler(res_touch5, OC_GET, get_touch5, NULL);
  oc_add_resource(res_touch5);

  PRINT("register resource with path /touch6\n");
  oc_resource_t *res_touch6 = oc_new_resource(NULL, g_touch6_RESOURCE_ENDPOINT, g_touch6_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_touch6_nr_resource_types);
  for( int a = 0; a < g_touch6_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_touch6_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch6,g_touch6_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch6_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_touch6, convert_if_string(g_touch6_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch6, convert_if_string(g_touch6_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_touch6_RESOURCE_INTERFACE[0]), g_touch6_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch6, true);
  oc_resource_set_periodic_observable(res_touch6, 1);

  oc_resource_set_request_handler(res_touch6, OC_GET, get_touch6, NULL);
  oc_add_resource(res_touch6);

  PRINT("register resource with path /touch7\n");
  oc_resource_t *res_touch7 = oc_new_resource(NULL, g_touch7_RESOURCE_ENDPOINT, g_touch7_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_touch7_nr_resource_types);
  for( int a = 0; a < g_touch7_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_touch7_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch7,g_touch7_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch7_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_touch7, convert_if_string(g_touch7_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch7, convert_if_string(g_touch7_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_touch7_RESOURCE_INTERFACE[0]), g_touch7_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch7, true);
  oc_resource_set_periodic_observable(res_touch7, 1);

  oc_resource_set_request_handler(res_touch7, OC_GET, get_touch7, NULL);
  oc_add_resource(res_touch7);

  PRINT("register resource with path /touch8\n");
  oc_resource_t *res_touch8 = oc_new_resource(NULL, g_touch8_RESOURCE_ENDPOINT, g_touch8_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_touch8_nr_resource_types);
  for( int a = 0; a < g_touch8_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_touch8_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_touch8,g_touch8_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_touch8_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_touch8, convert_if_string(g_touch8_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_touch8, convert_if_string(g_touch8_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_touch8_RESOURCE_INTERFACE[0]), g_touch8_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_touch8, true);
  oc_resource_set_periodic_observable(res_touch8, 1);

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
  // initialize global variables for endpoint "/analog1"
  g_analog1_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
  g_analog1_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_analog1_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_analog1_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_analog1_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_analog1_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).

  // initialize global variables for endpoint "/analog2"
  g_analog2_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
  g_analog2_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_analog2_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_analog2_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_analog2_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_analog2_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).

  // initialize global variables for endpoint "/analog3"
  g_analog3_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
  g_analog3_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_analog3_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_analog3_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_analog3_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_analog3_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).

  // initialize global variables for endpoint "/analog4"
  g_analog4_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
  g_analog4_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_analog4_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_analog4_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_analog4_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_analog4_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).

  // initialize global variables for endpoint "/input1"
  g_input1_value = false; // current value of property "value" Status of the switch

  // initialize global variables for endpoint "/input2"
  g_input2_value = false; // current value of property "value" Status of the switch

  // initialize global variables for endpoint "/input3"
  g_input3_value = false; // current value of property "value" Status of the switch

  // initialize global variables for endpoint "/input4"
  g_input4_value = false; // current value of property "value" Status of the switch

  // initialize global variables for endpoint "/light1"
  g_light1_value = false; // current value of property "value" Status of the switch

  // initialize global variables for endpoint "/light2"
  g_light2_value = false; // current value of property "value" Status of the switch

  // initialize global variables for endpoint "/light3"
  g_light3_value = false; // current value of property "value" Status of the switch

  // initialize global variables for endpoint "/light4"
  g_light4_value = false; // current value of property "value" Status of the switch

  // initialize global variables for endpoint "/output1"
  g_output1_value = false; // current value of property "value" Status of the switch

  // initialize global variables for endpoint "/output2"
  g_output2_value = false; // current value of property "value" Status of the switch

  // initialize global variables for endpoint "/output3"
  g_output3_value = false; // current value of property "value" Status of the switch

  // initialize global variables for endpoint "/output4"
  g_output4_value = false; // current value of property "value" Status of the switch

  // initialize global variables for endpoint "/touch1"
  g_touch1_value = true; // current value of property "value" true = sensed, false = not sensed.

  // initialize global variables for endpoint "/touch2"
  g_touch2_value = true; // current value of property "value" true = sensed, false = not sensed.

  // initialize global variables for endpoint "/touch3"
  g_touch3_value = true; // current value of property "value" true = sensed, false = not sensed.

  // initialize global variables for endpoint "/touch4"
  g_touch4_value = true; // current value of property "value" true = sensed, false = not sensed.

  // initialize global variables for endpoint "/touch5"
  g_touch5_value = true; // current value of property "value" true = sensed, false = not sensed.

  // initialize global variables for endpoint "/touch6"
  g_touch6_value = true; // current value of property "value" true = sensed, false = not sensed.

  // initialize global variables for endpoint "/touch7"
  g_touch7_value = true; // current value of property "value" true = sensed, false = not sensed.

  // initialize global variables for endpoint "/touch8"
  g_touch8_value = true; // current value of property "value" true = sensed, false = not sensed.


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

  PRINT("file : /home/pi/workspace/explorerhatlite/device_output/out_codegeneration_merged.swagger.json\n");
  PRINT("title: Touch Sensor\n");

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
