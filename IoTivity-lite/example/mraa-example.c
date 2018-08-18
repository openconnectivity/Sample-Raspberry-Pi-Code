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
 version of input_file : v1.1.0-20160519
 title of input_file   : Binary Switch
*/

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

volatile int quit = 0;          // stop variable
oc_string_t name;               // name of the device


// membervariables for path: /binaryswitch
static char g_binaryswitch_RESOURCE_PROPERTY_NAME_value[] = "value"; // the name for the attribute
//bool g_binaryswitch_value; // the value for the attribute 

bool g_binaryswitch_value = false; // current value of property "value" Status of the switch// variables for the resources

static char g_binaryswitch_RESOURCE_ENDPOINT[] = "/binaryswitch";  // used path for this resource
static char g_binaryswitch_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; // rt value (as an array)
int g_binaryswitch_nr_resource_types = 1;
static char g_binaryswitch_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.a","oic.if.baseline"}; // interface if (as an array) 
int g_binaryswitch_nr_resource_interfaces = 2;
/**
*  function to set up the device.
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
*  get method for /binaryswitch to assign the returned values to the member values
* @param request the request representation.
* @param interfaces the interfaces.
* @param user_data the user data.
* TODO: this function is also referenced when only get is implemented, something to fix..
*/
static void
get_binaryswitch(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  // not used
  // the current implementation always return everything that belongs to the resource.
  // this kind of implementation is not optimal, but is correct and will pass CTT1.2.2
  PRINT("get_binaryswitch: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_binaryswitch_value); 
    PRINT("   %s : %d\n", g_binaryswitch_RESOURCE_PROPERTY_NAME_value,  g_binaryswitch_value );
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
}
 
/**
*  post method for /binaryswitch to assign the returned values to the member values
* @param requestRep the request representation.
*/
static void
post_binaryswitch(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("post_binaryswitch:\n");
  oc_rep_t *rep = request->request_payload;
  while (rep != NULL) {
    PRINT("key: (check) %s ", oc_string(rep->name));if (strcmp ( oc_string(rep->name), g_binaryswitch_RESOURCE_PROPERTY_NAME_value) == 0)
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
      if (strcmp ( oc_string(rep->name), g_binaryswitch_RESOURCE_PROPERTY_NAME_value)== 0)
      {
        // assign value
        g_binaryswitch_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    
    // set the response
    oc_rep_start_root_object();
    //oc_process_baseline_interface(request->resource);
    oc_rep_set_boolean(root, value, g_binaryswitch_value); 
    oc_rep_end_root_object();
    
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

  PRINT("register resource with path /binaryswitch\n");
  oc_resource_t *res_binaryswitch = oc_new_resource(NULL, g_binaryswitch_RESOURCE_ENDPOINT, g_binaryswitch_nr_resource_types, 0);
  PRINT("     number of resource types: %d\n", g_binaryswitch_nr_resource_types);
  for( int a = 0; a < g_binaryswitch_nr_resource_types; a++ )
  {
    PRINT("     resource type: %s\n", g_binaryswitch_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_binaryswitch,g_binaryswitch_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_binaryswitch_nr_resource_interfaces; a++ )
  {
    oc_resource_bind_resource_interface(res_binaryswitch, convert_if_string(g_binaryswitch_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_binaryswitch, convert_if_string(g_binaryswitch_RESOURCE_INTERFACE[0]));  
  PRINT("     default interface: %d (%s)\n", convert_if_string(g_binaryswitch_RESOURCE_INTERFACE[0]), g_binaryswitch_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_binaryswitch, true);
  oc_resource_set_periodic_observable(res_binaryswitch, 1);
   
  oc_resource_set_request_handler(res_binaryswitch, OC_GET, get_binaryswitch, NULL);
   
  oc_resource_set_request_handler(res_binaryswitch, OC_POST, post_binaryswitch, NULL);
  oc_add_resource(res_binaryswitch);
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
  // initialize member variables /binaryswitch
  g_binaryswitch_value = false; // current value of property "value" Status of the switch
   
  
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
  PRINT("title: Binary Switch\n");
  

#ifdef OC_SECURITY
  PRINT("intialize secure resources\n");
  oc_storage_config("./simpleserver_creds/");
#endif /* OC_SECURITY */

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

  oc_main_shutdown();
  return 0;
}