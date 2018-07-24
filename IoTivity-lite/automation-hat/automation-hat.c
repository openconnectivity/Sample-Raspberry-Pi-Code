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
/*
 tool_version          : 20171123
 input_file            : ../device_output/out_codegeneration_merged.swagger.json
 version of input_file : v1.1.0-20170815
 title of input_file   : Energy
*/
#include "/usr/include/python2.7/Python.h"

#include "oc_api.h"
#include "port/oc_clock.h"
#include <signal.h>

#ifdef __linux__
#include <pthread.h>
static pthread_mutex_t mutex;
static pthread_cond_t cv;
static struct timespec ts;
#endif

#ifdef WIN32
#include <windows.h>
static CONDITION_VARIABLE cv;   // event loop variable
static CRITICAL_SECTION cs;     // event loop variable
#endif

#define MAX_STRING 25

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

volatile int quit = 0;          // stop variable
oc_string_t name;               // name of the device


// membervariables for path: /ADC1
static char g_ADC1_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
//double g_ADC1_voltage; // the value for the attribute

double g_ADC1_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_ADC1_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
//double g_ADC1_desiredfrequency; // the value for the attribute

double g_ADC1_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
static char g_ADC1_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
//double g_ADC1_desiredvoltage; // the value for the attribute

double g_ADC1_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_ADC1_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
//double g_ADC1_current; // the value for the attribute

double g_ADC1_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_ADC1_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
//double g_ADC1_desiredcurrent; // the value for the attribute

double g_ADC1_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_ADC1_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
//double g_ADC1_frequency; // the value for the attribute

double g_ADC1_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
// membervariables for path: /ADC2
static char g_ADC2_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
//double g_ADC2_voltage; // the value for the attribute

double g_ADC2_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_ADC2_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
//double g_ADC2_desiredfrequency; // the value for the attribute

double g_ADC2_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
static char g_ADC2_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
//double g_ADC2_desiredvoltage; // the value for the attribute

double g_ADC2_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_ADC2_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
//double g_ADC2_current; // the value for the attribute

double g_ADC2_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_ADC2_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
//double g_ADC2_desiredcurrent; // the value for the attribute

double g_ADC2_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_ADC2_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
//double g_ADC2_frequency; // the value for the attribute

double g_ADC2_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
// membervariables for path: /ADC3
static char g_ADC3_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
//double g_ADC3_voltage; // the value for the attribute

double g_ADC3_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_ADC3_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
//double g_ADC3_desiredfrequency; // the value for the attribute

double g_ADC3_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
static char g_ADC3_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
//double g_ADC3_desiredvoltage; // the value for the attribute

double g_ADC3_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_ADC3_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
//double g_ADC3_current; // the value for the attribute

double g_ADC3_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_ADC3_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
//double g_ADC3_desiredcurrent; // the value for the attribute

double g_ADC3_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_ADC3_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
//double g_ADC3_frequency; // the value for the attribute

double g_ADC3_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
// membervariables for path: /ADC4
static char g_ADC4_RESOURCE_PROPERTY_NAME_voltage[] = "voltage"; // the name for the attribute
//double g_ADC4_voltage; // the value for the attribute

double g_ADC4_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
static char g_ADC4_RESOURCE_PROPERTY_NAME_desiredfrequency[] = "desiredfrequency"; // the name for the attribute
//double g_ADC4_desiredfrequency; // the value for the attribute

double g_ADC4_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
static char g_ADC4_RESOURCE_PROPERTY_NAME_desiredvoltage[] = "desiredvoltage"; // the name for the attribute
//double g_ADC4_desiredvoltage; // the value for the attribute

double g_ADC4_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
static char g_ADC4_RESOURCE_PROPERTY_NAME_current[] = "current"; // the name for the attribute
//double g_ADC4_current; // the value for the attribute

double g_ADC4_current = 5.0; // current value of property "current"  The electric current in Amps (A).
static char g_ADC4_RESOURCE_PROPERTY_NAME_desiredcurrent[] = "desiredcurrent"; // the name for the attribute
//double g_ADC4_desiredcurrent; // the value for the attribute

double g_ADC4_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
static char g_ADC4_RESOURCE_PROPERTY_NAME_frequency[] = "frequency"; // the name for the attribute
//double g_ADC4_frequency; // the value for the attribute

double g_ADC4_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
// membervariables for path: /CommsLight
static char g_CommsLight_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
//bool g_CommsLight_value; // the value for the attribute

bool g_CommsLight_value = false; // current value of property "value" Status of the switch
// membervariables for path: /PowerLight
static char g_PowerLight_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
//bool g_PowerLight_value; // the value for the attribute

bool g_PowerLight_value = false; // current value of property "value" Status of the switch
// membervariables for path: /WarnLight
static char g_WarnLight_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
//bool g_WarnLight_value; // the value for the attribute

bool g_WarnLight_value = false; // current value of property "value" Status of the switch
// membervariables for path: /input1
static char g_input1_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
//bool g_input1_value; // the value for the attribute

bool g_input1_value = false; // current value of property "value" Status of the switch
// membervariables for path: /input2
static char g_input2_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
//bool g_input2_value; // the value for the attribute

bool g_input2_value = false; // current value of property "value" Status of the switch
// membervariables for path: /input3
static char g_input3_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
//bool g_input3_value; // the value for the attribute

bool g_input3_value = false; // current value of property "value" Status of the switch
// membervariables for path: /output1
static char g_output1_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
//bool g_output1_value; // the value for the attribute

bool g_output1_value = false; // current value of property "value" Status of the switch
// membervariables for path: /output2
static char g_output2_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
//bool g_output2_value; // the value for the attribute

bool g_output2_value = false; // current value of property "value" Status of the switch
// membervariables for path: /output3
static char g_output3_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
//bool g_output3_value; // the value for the attribute

bool g_output3_value = false; // current value of property "value" Status of the switch
// membervariables for path: /relay1
static char g_relay1_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
//bool g_relay1_value; // the value for the attribute

bool g_relay1_value = false; // current value of property "value" Status of the switch
// membervariables for path: /relay2
static char g_relay2_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
//bool g_relay2_value; // the value for the attribute

bool g_relay2_value = false; // current value of property "value" Status of the switch
// membervariables for path: /relay3
static char g_relay3_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
//bool g_relay3_value; // the value for the attribute

bool g_relay3_value = false; // current value of property "value" Status of the switch// variables for the resources

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

static char g_CommsLight_RESOURCE_ENDPOINT[] = "/CommsLight";  // used path for this resource
static char g_CommsLight_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_CommsLight_nr_resource_types = 1;
static char g_CommsLight_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_CommsLight_nr_resource_interfaces = 2;

static char g_PowerLight_RESOURCE_ENDPOINT[] = "/PowerLight";  // used path for this resource
static char g_PowerLight_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_PowerLight_nr_resource_types = 1;
static char g_PowerLight_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_PowerLight_nr_resource_interfaces = 2;

static char g_WarnLight_RESOURCE_ENDPOINT[] = "/WarnLight";  // used path for this resource
static char g_WarnLight_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_WarnLight_nr_resource_types = 1;
static char g_WarnLight_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_WarnLight_nr_resource_interfaces = 2;

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

static char g_relay1_RESOURCE_ENDPOINT[] = "/relay1";  // used path for this resource
static char g_relay1_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_relay1_nr_resource_types = 1;
static char g_relay1_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_relay1_nr_resource_interfaces = 2;

static char g_relay2_RESOURCE_ENDPOINT[] = "/relay2";  // used path for this resource
static char g_relay2_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_relay2_nr_resource_types = 1;
static char g_relay2_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_relay2_nr_resource_interfaces = 2;

static char g_relay3_RESOURCE_ENDPOINT[] = "/relay3";  // used path for this resource
static char g_relay3_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_relay3_nr_resource_types = 1;
static char g_relay3_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_relay3_nr_resource_interfaces = 2;
/**
*  function to set up the device.
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
  oc_new_string(&name, "ocf", sizeof("ocf") );
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
*  get method for /ADC1 to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_ADC1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  myParamArgs[0] = 1;
  CallPythonFunction((char *)"automation-hat", (char *)"readADC", 1, myParamArgs);
  g_ADC1_voltage = returnDouble;

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

    oc_rep_set_double(root, desiredfrequency, g_ADC1_desiredfrequency );
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_desiredfrequency, g_ADC1_desiredfrequency );

    oc_rep_set_double(root, desiredvoltage, g_ADC1_desiredvoltage );
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_desiredvoltage, g_ADC1_desiredvoltage );

    oc_rep_set_double(root, current, g_ADC1_current );
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_current, g_ADC1_current );

    oc_rep_set_double(root, desiredcurrent, g_ADC1_desiredcurrent );
    PRINT("   %s : %f\n", g_ADC1_RESOURCE_PROPERTY_NAME_desiredcurrent, g_ADC1_desiredcurrent );

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
*  get method for /ADC2 to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_ADC2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  myParamArgs[0] = 2;
  CallPythonFunction((char *)"automation-hat", (char *)"readADC", 1, myParamArgs);
  g_ADC2_voltage = returnDouble;

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

    oc_rep_set_double(root, desiredfrequency, g_ADC2_desiredfrequency );
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_desiredfrequency, g_ADC2_desiredfrequency );

    oc_rep_set_double(root, desiredvoltage, g_ADC2_desiredvoltage );
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_desiredvoltage, g_ADC2_desiredvoltage );

    oc_rep_set_double(root, current, g_ADC2_current );
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_current, g_ADC2_current );

    oc_rep_set_double(root, desiredcurrent, g_ADC2_desiredcurrent );
    PRINT("   %s : %f\n", g_ADC2_RESOURCE_PROPERTY_NAME_desiredcurrent, g_ADC2_desiredcurrent );

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
*  get method for /ADC3 to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_ADC3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  myParamArgs[0] = 3;
  CallPythonFunction((char *)"automation-hat", (char *)"readADC", 1, myParamArgs);
  g_ADC3_voltage = returnDouble;

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

    oc_rep_set_double(root, desiredfrequency, g_ADC3_desiredfrequency );
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_desiredfrequency, g_ADC3_desiredfrequency );

    oc_rep_set_double(root, desiredvoltage, g_ADC3_desiredvoltage );
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_desiredvoltage, g_ADC3_desiredvoltage );

    oc_rep_set_double(root, current, g_ADC3_current );
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_current, g_ADC3_current );

    oc_rep_set_double(root, desiredcurrent, g_ADC3_desiredcurrent );
    PRINT("   %s : %f\n", g_ADC3_RESOURCE_PROPERTY_NAME_desiredcurrent, g_ADC3_desiredcurrent );

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
*  get method for /ADC4 to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_ADC4(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  myParamArgs[0] = 4;
  CallPythonFunction((char *)"automation-hat", (char *)"readADC", 1, myParamArgs);
  g_ADC4_voltage = returnDouble;

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

    oc_rep_set_double(root, desiredfrequency, g_ADC4_desiredfrequency );
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_desiredfrequency, g_ADC4_desiredfrequency );

    oc_rep_set_double(root, desiredvoltage, g_ADC4_desiredvoltage );
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_desiredvoltage, g_ADC4_desiredvoltage );

    oc_rep_set_double(root, current, g_ADC4_current );
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_current, g_ADC4_current );

    oc_rep_set_double(root, desiredcurrent, g_ADC4_desiredcurrent );
    PRINT("   %s : %f\n", g_ADC4_RESOURCE_PROPERTY_NAME_desiredcurrent, g_ADC4_desiredcurrent );

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
*  get method for /CommsLight to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_CommsLight(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  PRINT("get_CommsLight: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_CommsLight_value);
    PRINT("   %s : %d\n", g_CommsLight_RESOURCE_PROPERTY_NAME_value,  g_CommsLight_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
*  get method for /PowerLight to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_PowerLight(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  PRINT("get_PowerLight: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_PowerLight_value);
    PRINT("   %s : %d\n", g_PowerLight_RESOURCE_PROPERTY_NAME_value,  g_PowerLight_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
*  get method for /WarnLight to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_WarnLight(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  PRINT("get_WarnLight: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_WarnLight_value);
    PRINT("   %s : %d\n", g_WarnLight_RESOURCE_PROPERTY_NAME_value,  g_WarnLight_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
*  get method for /input1 to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_input1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  myParamArgs[0] = 1;
  CallPythonFunction((char *)"automation-hat", (char *)"readInput", 1, myParamArgs);
  g_input1_value = returnLong;

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
*  get method for /input2 to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_input2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  myParamArgs[0] = 2;
  CallPythonFunction((char *)"automation-hat", (char *)"readInput", 1, myParamArgs);
  g_input2_value = returnLong;

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
*  get method for /input3 to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_input3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  myParamArgs[0] = 3;
  CallPythonFunction((char *)"automation-hat", (char *)"readInput", 1, myParamArgs);
  g_input3_value = returnLong;

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
*  get method for /output1 to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_output1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
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
*  get method for /output2 to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_output2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
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
*  get method for /output3 to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_output3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
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
*  get method for /relay1 to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_relay1(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  PRINT("get_relay1: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_relay1_value);
    PRINT("   %s : %d\n", g_relay1_RESOURCE_PROPERTY_NAME_value,  g_relay1_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
*  get method for /relay2 to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_relay2(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  PRINT("get_relay2: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_relay2_value);
    PRINT("   %s : %d\n", g_relay2_RESOURCE_PROPERTY_NAME_value,  g_relay2_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
*  get method for /relay3 to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_relay3(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  PRINT("get_relay3: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_relay3_value);
    PRINT("   %s : %d\n", g_relay3_RESOURCE_PROPERTY_NAME_value,  g_relay3_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
*  post method for /CommsLight to assign the returned values to the member values
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
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_CommsLight_RESOURCE_PROPERTY_NAME_value) == 0)
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
      if (strcmp ( oc_string(rep->name), g_CommsLight_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_CommsLight_value = rep->value.boolean;
      }
      rep = rep->next;
    }

    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_CommsLight_value);
    oc_rep_end_root_object();

    oc_send_response(request, OC_STATUS_CHANGED);
    myParamArgs[0] = g_CommsLight_value ? 1 : 0;
    CallPythonFunction((char *)"automation-hat", (char *)"writeCommsLight", 1, myParamArgs);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
*  post method for /PowerLight to assign the returned values to the member values
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
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_PowerLight_RESOURCE_PROPERTY_NAME_value) == 0)
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
      if (strcmp ( oc_string(rep->name), g_PowerLight_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_PowerLight_value = rep->value.boolean;
      }
      rep = rep->next;
    }

    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_PowerLight_value);
    oc_rep_end_root_object();

    oc_send_response(request, OC_STATUS_CHANGED);
    myParamArgs[0] = g_PowerLight_value ? 1 : 0;
    CallPythonFunction((char *)"automation-hat", (char *)"writePowerLight", 1, myParamArgs);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
*  post method for /WarnLight to assign the returned values to the member values
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
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_WarnLight_RESOURCE_PROPERTY_NAME_value) == 0)
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
      if (strcmp ( oc_string(rep->name), g_WarnLight_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_WarnLight_value = rep->value.boolean;
      }
      rep = rep->next;
    }

    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_WarnLight_value);
    oc_rep_end_root_object();

    oc_send_response(request, OC_STATUS_CHANGED);
    myParamArgs[0] = g_WarnLight_value ? 1 : 0;
    CallPythonFunction((char *)"automation-hat", (char *)"writeWarnLight", 1, myParamArgs);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
*  post method for /output1 to assign the returned values to the member values
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

    oc_send_response(request, OC_STATUS_CHANGED);
    myParamArgs[0] = 1;
    myParamArgs[1] = g_output1_value ? 1 : 0;
    CallPythonFunction((char *)"automation-hat", (char *)"writeOutput", 2, myParamArgs);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
*  post method for /output2 to assign the returned values to the member values
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

    oc_send_response(request, OC_STATUS_CHANGED);
    myParamArgs[0] = 2;
    myParamArgs[1] = g_output2_value ? 1 : 0;
    CallPythonFunction((char *)"automation-hat", (char *)"writeOutput", 2, myParamArgs);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
*  post method for /output3 to assign the returned values to the member values
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

    oc_send_response(request, OC_STATUS_CHANGED);
    myParamArgs[0] = 3;
    myParamArgs[1] = g_output3_value ? 1 : 0;
    CallPythonFunction((char *)"automation-hat", (char *)"writeOutput", 2, myParamArgs);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
*  post method for /relay1 to assign the returned values to the member values
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
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_relay1_RESOURCE_PROPERTY_NAME_value) == 0)
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
      if (strcmp ( oc_string(rep->name), g_relay1_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_relay1_value = rep->value.boolean;
      }
      rep = rep->next;
    }

    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_relay1_value);
    oc_rep_end_root_object();

    oc_send_response(request, OC_STATUS_CHANGED);
    myParamArgs[0] = 1;
    myParamArgs[1] = g_relay1_value ? 1 : 0;
    CallPythonFunction((char *)"automation-hat", (char *)"writeRelay", 2, myParamArgs);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
*  post method for /relay2 to assign the returned values to the member values
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
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_relay2_RESOURCE_PROPERTY_NAME_value) == 0)
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
      if (strcmp ( oc_string(rep->name), g_relay2_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_relay2_value = rep->value.boolean;
      }
      rep = rep->next;
    }

    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_relay2_value);
    oc_rep_end_root_object();

    oc_send_response(request, OC_STATUS_CHANGED);
    myParamArgs[0] = 2;
    myParamArgs[1] = g_relay2_value ? 1 : 0;
    CallPythonFunction((char *)"automation-hat", (char *)"writeRelay", 2, myParamArgs);
  }
  else
  {
    // TODO: add error response, if any
    oc_send_response(request, OC_STATUS_NOT_MODIFIED);
  }
}

/**
*  post method for /relay3 to assign the returned values to the member values
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
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_relay3_RESOURCE_PROPERTY_NAME_value) == 0)
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
      if (strcmp ( oc_string(rep->name), g_relay3_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_relay3_value = rep->value.boolean;
      }
      rep = rep->next;
    }

    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_relay3_value);
    oc_rep_end_root_object();

    oc_send_response(request, OC_STATUS_CHANGED);
    myParamArgs[0] = 3;
    myParamArgs[1] = g_relay3_value ? 1 : 0;
    CallPythonFunction((char *)"automation-hat", (char *)"writeRelay", 2, myParamArgs);
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

  PRINT("register resource with path /CommsLight\n");
  oc_resource_t *res_CommsLight = oc_new_resource(NULL, g_CommsLight_RESOURCE_ENDPOINT, g_CommsLight_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_CommsLight_nr_resource_types);
  for( int a = 0; a < g_CommsLight_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_CommsLight_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_CommsLight,g_CommsLight_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_CommsLight_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_CommsLight, convert_if_string(g_CommsLight_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_CommsLight, convert_if_string(g_CommsLight_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_CommsLight_RESOURCE_INTERFACE[0]), g_CommsLight_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_CommsLight, true);
  oc_resource_set_periodic_observable(res_CommsLight, 1);

  oc_resource_set_request_handler(res_CommsLight, OC_GET, get_CommsLight, NULL);

  oc_resource_set_request_handler(res_CommsLight, OC_POST, post_CommsLight, NULL);
  oc_add_resource(res_CommsLight);

  PRINT("register resource with path /PowerLight\n");
  oc_resource_t *res_PowerLight = oc_new_resource(NULL, g_PowerLight_RESOURCE_ENDPOINT, g_PowerLight_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_PowerLight_nr_resource_types);
  for( int a = 0; a < g_PowerLight_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_PowerLight_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_PowerLight,g_PowerLight_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_PowerLight_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_PowerLight, convert_if_string(g_PowerLight_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_PowerLight, convert_if_string(g_PowerLight_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_PowerLight_RESOURCE_INTERFACE[0]), g_PowerLight_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_PowerLight, true);
  oc_resource_set_periodic_observable(res_PowerLight, 1);

  oc_resource_set_request_handler(res_PowerLight, OC_GET, get_PowerLight, NULL);

  oc_resource_set_request_handler(res_PowerLight, OC_POST, post_PowerLight, NULL);
  oc_add_resource(res_PowerLight);

  PRINT("register resource with path /WarnLight\n");
  oc_resource_t *res_WarnLight = oc_new_resource(NULL, g_WarnLight_RESOURCE_ENDPOINT, g_WarnLight_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_WarnLight_nr_resource_types);
  for( int a = 0; a < g_WarnLight_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_WarnLight_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_WarnLight,g_WarnLight_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_WarnLight_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_WarnLight, convert_if_string(g_WarnLight_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_WarnLight, convert_if_string(g_WarnLight_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_WarnLight_RESOURCE_INTERFACE[0]), g_WarnLight_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_WarnLight, true);
  oc_resource_set_periodic_observable(res_WarnLight, 1);

  oc_resource_set_request_handler(res_WarnLight, OC_GET, get_WarnLight, NULL);

  oc_resource_set_request_handler(res_WarnLight, OC_POST, post_WarnLight, NULL);
  oc_add_resource(res_WarnLight);

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

  PRINT("register resource with path /relay1\n");
  oc_resource_t *res_relay1 = oc_new_resource(NULL, g_relay1_RESOURCE_ENDPOINT, g_relay1_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_relay1_nr_resource_types);
  for( int a = 0; a < g_relay1_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_relay1_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_relay1,g_relay1_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_relay1_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_relay1, convert_if_string(g_relay1_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_relay1, convert_if_string(g_relay1_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_relay1_RESOURCE_INTERFACE[0]), g_relay1_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_relay1, true);
  oc_resource_set_periodic_observable(res_relay1, 1);

  oc_resource_set_request_handler(res_relay1, OC_GET, get_relay1, NULL);

  oc_resource_set_request_handler(res_relay1, OC_POST, post_relay1, NULL);
  oc_add_resource(res_relay1);

  PRINT("register resource with path /relay2\n");
  oc_resource_t *res_relay2 = oc_new_resource(NULL, g_relay2_RESOURCE_ENDPOINT, g_relay2_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_relay2_nr_resource_types);
  for( int a = 0; a < g_relay2_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_relay2_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_relay2,g_relay2_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_relay2_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_relay2, convert_if_string(g_relay2_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_relay2, convert_if_string(g_relay2_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_relay2_RESOURCE_INTERFACE[0]), g_relay2_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_relay2, true);
  oc_resource_set_periodic_observable(res_relay2, 1);

  oc_resource_set_request_handler(res_relay2, OC_GET, get_relay2, NULL);

  oc_resource_set_request_handler(res_relay2, OC_POST, post_relay2, NULL);
  oc_add_resource(res_relay2);

  PRINT("register resource with path /relay3\n");
  oc_resource_t *res_relay3 = oc_new_resource(NULL, g_relay3_RESOURCE_ENDPOINT, g_relay3_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_relay3_nr_resource_types);
  for( int a = 0; a < g_relay3_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_relay3_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_relay3,g_relay3_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_relay3_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_relay3, convert_if_string(g_relay3_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_relay3, convert_if_string(g_relay3_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_relay3_RESOURCE_INTERFACE[0]), g_relay3_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_relay3, true);
  oc_resource_set_periodic_observable(res_relay3, 1);

  oc_resource_set_request_handler(res_relay3, OC_GET, get_relay3, NULL);

  oc_resource_set_request_handler(res_relay3, OC_POST, post_relay3, NULL);
  oc_add_resource(res_relay3);
}

#ifdef WIN32
static void
signal_event_loop(void)
{
  WakeConditionVariable(&cv);
}
#endif

#ifdef __linux__
static void
signal_event_loop(void)
{
  pthread_mutex_lock(&mutex);
  pthread_cond_signal(&cv);
  pthread_mutex_unlock(&mutex);
}
#endif


void
handle_signal(int signal)
{
  (void)signal;
  signal_event_loop();
  quit = 1;
}

int
main(void)
{
int init;

#ifdef WIN32
  InitializeCriticalSection(&cs);
  InitializeConditionVariable(&cv);
  signal(SIGINT, handle_signal);
#endif
#ifdef __linux__
  struct sigaction sa;
  sigfillset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handle_signal;
  sigaction(SIGINT, &sa, NULL);
#endif
  // initialize member variables /ADC1
  g_ADC1_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_ADC1_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
  g_ADC1_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_ADC1_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_ADC1_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_ADC1_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).

  // initialize member variables /ADC2
  g_ADC2_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_ADC2_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
  g_ADC2_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_ADC2_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_ADC2_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_ADC2_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).

  // initialize member variables /ADC3
  g_ADC3_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_ADC3_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
  g_ADC3_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_ADC3_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_ADC3_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_ADC3_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).

  // initialize member variables /ADC4
  g_ADC4_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
  g_ADC4_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
  g_ADC4_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
  g_ADC4_current = 5.0; // current value of property "current"  The electric current in Amps (A).
  g_ADC4_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
  g_ADC4_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).

  // initialize member variables /CommsLight
  g_CommsLight_value = false; // current value of property "value" Status of the switch

  // initialize member variables /PowerLight
  g_PowerLight_value = false; // current value of property "value" Status of the switch

  // initialize member variables /WarnLight
  g_WarnLight_value = false; // current value of property "value" Status of the switch

  // initialize member variables /input1
  g_input1_value = false; // current value of property "value" Status of the switch

  // initialize member variables /input2
  g_input2_value = false; // current value of property "value" Status of the switch

  // initialize member variables /input3
  g_input3_value = false; // current value of property "value" Status of the switch

  // initialize member variables /output1
  g_output1_value = false; // current value of property "value" Status of the switch

  // initialize member variables /output2
  g_output2_value = false; // current value of property "value" Status of the switch

  // initialize member variables /output3
  g_output3_value = false; // current value of property "value" Status of the switch

  // initialize member variables /relay1
  g_relay1_value = false; // current value of property "value" Status of the switch

  // initialize member variables /relay2
  g_relay2_value = false; // current value of property "value" Status of the switch

  // initialize member variables /relay3
  g_relay3_value = false; // current value of property "value" Status of the switch


  // no oic/con resource.
  oc_set_con_res_announced(false);

  static const oc_handler_t handler = {.init = app_init,
                                       .signal_event_loop = signal_event_loop,
                                       .register_resources = register_resources
#ifdef WIN32
                                       ,
                                       .requests_entry = 0
#endif
                                       };

  oc_clock_time_t next_event;


  PRINT("file : ../device_output/out_codegeneration_merged.swagger.json\n");
  PRINT("title: Energy\n");


#ifdef OC_SECURITY
  PRINT("intialize secure resources\n");
  oc_storage_config("./simpleserver_creds/");
#endif /* OC_SECURITY */

  Py_Initialize();

  // initialize the IDD from a specific file
  // oc_set_introspection_file(0, "myfile.dat");

  init = oc_main_init(&handler);
  if (init < 0)
    return init;

#ifdef WIN32
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

  Py_Finalize();

  oc_main_shutdown();
  return 0;
}
