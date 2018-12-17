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
 input_file            : /home/pi/workspace/explorer-hat-pro-example/device_output/out_codegeneration_merged.swagger.json
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

// global property variables for path: /light
static char g_light_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_light_value = false; // current value of property "value" Status of the switch
// global property variables for path: /switch
static char g_switch_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
bool g_switch_value = false; // current value of property "value" Status of the switch// registration data variables for the resources

// global resource variables for path: /light
static char g_light_RESOURCE_ENDPOINT[] = "/light";  // used path for this resource
static char g_light_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_light_nr_resource_types = 1;
static char g_light_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
int g_light_nr_resource_interfaces = 2;

// global resource variables for path: /switch
static char g_switch_RESOURCE_ENDPOINT[] = "/switch";  // used path for this resource
static char g_switch_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_switch_nr_resource_types = 1;
static char g_switch_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
int g_switch_nr_resource_interfaces = 2;
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
  ret |= oc_add_device("/oic/d", "oic.d.switchdevice", "Binary Switch",
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
* get method for "/light" endpoint
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
get_light(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_light: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_light_value);
    PRINT("   %s : %d\n", g_light_RESOURCE_PROPERTY_NAME_value,  g_light_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* get method for "/switch" endpoint
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
get_switch(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used

  // TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
  // the call to the HW needs to fill in the global variable before it returns to this function here.
  // alternative is to have a callback from the hardware that sets the global variables.

  // The implementation always return everything that belongs to the resource.
  // this implementation is not optimal, but is functionally correct and will pass CTT1.2.2

  PRINT("get_switch: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_switch_value);
    PRINT("   %s : %d\n", g_switch_RESOURCE_PROPERTY_NAME_value,  g_switch_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}

/**
* post method for "/light" endpoint
* the function has as input the request body, which are the input values of the POST method.
* the input values (as a set) are checked if all supplied values are correct.
* if the input values are correct, they will be assigned to the global  property values.

* @param requestRep the request representation.
*/
static void
post_light(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_light:\n");
  oc_rep_t *rep = request->request_payload;
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_light_RESOURCE_PROPERTY_NAME_value) == 0)
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
      if (strcmp ( oc_string(rep->name), g_light_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_light_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_light_value);
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

  PRINT("register resource with path /light\n");
  oc_resource_t *res_light = oc_new_resource(NULL, g_light_RESOURCE_ENDPOINT, g_light_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_light_nr_resource_types);
  for( int a = 0; a < g_light_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_light_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_light,g_light_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_light_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_light, convert_if_string(g_light_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_light, convert_if_string(g_light_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_light_RESOURCE_INTERFACE[0]), g_light_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_light, true);
  oc_resource_set_periodic_observable(res_light, 1);

  oc_resource_set_request_handler(res_light, OC_GET, get_light, NULL);

  oc_resource_set_request_handler(res_light, OC_POST, post_light, NULL);
  oc_add_resource(res_light);

  PRINT("register resource with path /switch\n");
  oc_resource_t *res_switch = oc_new_resource(NULL, g_switch_RESOURCE_ENDPOINT, g_switch_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_switch_nr_resource_types);
  for( int a = 0; a < g_switch_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_switch_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_switch,g_switch_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_switch_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_switch, convert_if_string(g_switch_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_switch, convert_if_string(g_switch_RESOURCE_INTERFACE[0]));
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_switch_RESOURCE_INTERFACE[0]), g_switch_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_switch, true);
  oc_resource_set_periodic_observable(res_switch, 1);

  oc_resource_set_request_handler(res_switch, OC_GET, get_switch, NULL);
  oc_add_resource(res_switch);
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
  // initialize global variables for endpoint "/light"
  g_light_value = false; // current value of property "value" Status of the switch

  // initialize global variables for endpoint "/switch"
  g_switch_value = false; // current value of property "value" Status of the switch


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

  PRINT("file : /home/pi/workspace/explorer-hat-pro-example/device_output/out_codegeneration_merged.swagger.json\n");
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
