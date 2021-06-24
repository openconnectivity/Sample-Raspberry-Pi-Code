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
 tool_version          : 20200103
 input_file            : /home/pi/workspace/roambee/device_output/out_codegeneration_merged.swagger.json
 version of input_file : 20190222
 title of input_file   : Roambee
*/

#include "oc_api.h"
#include "port/oc_clock.h"
#include <signal.h>

#ifdef __linux__
/* linux specific code */
#include <pthread.h>
#ifndef NO_MAIN
static pthread_mutex_t mutex;
static pthread_cond_t cv;
static struct timespec ts;
#endif /* NO_MAIN */
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


/* global property variables for path: "/geolocation" */
static char g_geolocation_RESOURCE_PROPERTY_NAME_alt[] = "alt"; /* the name for the attribute */
double g_geolocation_alt = 12.07; /* current value of property "alt"  The current distance (metres) above or below 'local' sea-level. */
static char g_geolocation_RESOURCE_PROPERTY_NAME_latitude[] = "latitude"; /* the name for the attribute */
double g_geolocation_latitude = 55.070859; /* current value of property "latitude"  The Device's Current Latitude coordinate (degrees). */
static char g_geolocation_RESOURCE_PROPERTY_NAME_longitude[] = "longitude"; /* the name for the attribute */
double g_geolocation_longitude = -3.60512; /* current value of property "longitude"  The Device's Current Longitude coordinate (degrees). */
/* global property variables for path: "/light" */
static char g_light_RESOURCE_PROPERTY_NAME_value[] = "value"; /* the name for the attribute */
bool g_light_value = false; /* current value of property "value" The status of the switch. */
/* global property variables for path: "/temperature" */
static char g_temperature_RESOURCE_PROPERTY_NAME_temperature[] = "temperature"; /* the name for the attribute */
double g_temperature_temperature = 20.0; /* current value of property "temperature"  The current temperature setting or measurement. */
static char g_temperature_RESOURCE_PROPERTY_NAME_units[] = "units"; /* the name for the attribute */
char g_temperature_units[ MAX_PAYLOAD_STRING ] = "C"; /* current value of property "units" The unit for the conveyed temperature value, Note that when doing an UPDATE, the unit on the device does NOT change, it only indicates the unit of the conveyed value during the UPDATE operation. *//* registration data variables for the resources */

/* global resource variables for path: /geolocation */
static char g_geolocation_RESOURCE_ENDPOINT[] = "/geolocation"; /* used path for this resource */
static char g_geolocation_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.sensor.geolocation"}; /* rt value (as an array) */
int g_geolocation_nr_resource_types = 1;
static char g_geolocation_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.s","oic.if.baseline"}; /* interface if (as an array) */
int g_geolocation_nr_resource_interfaces = 2;

/* global resource variables for path: /light */
static char g_light_RESOURCE_ENDPOINT[] = "/light"; /* used path for this resource */
static char g_light_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.switch.binary"}; /* rt value (as an array) */
int g_light_nr_resource_types = 1;
static char g_light_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.baseline","oic.if.a"}; /* interface if (as an array) */
int g_light_nr_resource_interfaces = 2;

/* global resource variables for path: /temperature */
static char g_temperature_RESOURCE_ENDPOINT[] = "/temperature"; /* used path for this resource */
static char g_temperature_RESOURCE_TYPE[][MAX_STRING] = {"oic.r.temperature"}; /* rt value (as an array) */
int g_temperature_nr_resource_types = 1;
static char g_temperature_RESOURCE_INTERFACE[][MAX_STRING] = {"oic.if.s","oic.if.baseline"}; /* interface if (as an array) */
int g_temperature_nr_resource_interfaces = 2;
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
  ret |= oc_add_device("/oic/d", "oic.d.roambee", "Roambee", 
                       "ocf.2.0.5", /* icv value */
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
* get method for "/geolocation" resource.
* function is called to intialize the return values of the GET method.
* initialisation of the returned values are done from the global property values.
* Resource Description:
* This Resource describes the properties associated with the current geolocation coordinate.
* Geolocation is a geolocation coordinate data.
* The Property "latitude" is a device's current Latitude coordinate (degrees).
* The Property "longitude" is a device's current Longitude coordinate (degrees).
* The Property "alt" is a device's current distance (metres) above or below 'local' sea-level.
* The Property "accuracy" is the accuracy level of the latitude and longitude coordinates (metres).
* The Property "altitudeAccuracy" is the accuracy level of the altitude coordinates (metres).
* The Property "heading" is a direction of travel of device (degree).
* The Property "speed" is a device's current velocity (metres per second).
*
* @param request the request representation.
* @param interfaces the interface used for this call
* @param user_data the user data.
*/
static void
get_geolocation(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_geolocation: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_S:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (number) 'alt' */
    oc_rep_set_double(root, alt, g_geolocation_alt);
    PRINT("   %s : %f\n", g_geolocation_RESOURCE_PROPERTY_NAME_alt, g_geolocation_alt);
    /* property (number) 'latitude' */
    oc_rep_set_double(root, latitude, g_geolocation_latitude);
    PRINT("   %s : %f\n", g_geolocation_RESOURCE_PROPERTY_NAME_latitude, g_geolocation_latitude);
    /* property (number) 'longitude' */
    oc_rep_set_double(root, longitude, g_geolocation_longitude);
    PRINT("   %s : %f\n", g_geolocation_RESOURCE_PROPERTY_NAME_longitude, g_geolocation_longitude);
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
  PRINT("-- End get_geolocation\n");
}
 
/**
* get method for "/light" resource.
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
get_light(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)user_data;  /* variable not used */
  /* TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
     the call to the HW needs to fill in the global variable before it returns to this function here.
     alternative is to have a callback from the hardware that sets the global variables.
  
     The implementation always return everything that belongs to the resource.
     this implementation is not optimal, but is functionally correct and will pass CTT1.2.2 */
  bool error_state = false;
  
  
  PRINT("-- Begin get_light: interface %d\n", interfaces);
  oc_rep_start_root_object();
  switch (interfaces) {
  case OC_IF_BASELINE:
    /* fall through */
  case OC_IF_A:
  PRINT("   Adding Baseline info\n" );
    oc_process_baseline_interface(request->resource);
    
    /* property (boolean) 'value' */
    oc_rep_set_boolean(root, value, g_light_value);
    PRINT("   %s : %s\n", g_light_RESOURCE_PROPERTY_NAME_value,  btoa(g_light_value));
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
  PRINT("-- End get_light\n");
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
* post method for "/light" resource.
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
post_light(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_light:\n");
  oc_rep_t *rep = request->request_payload;
  
  /* loop over the request document for each required input field to check if all required input fields are present */
  bool var_in_request= false; 
  rep = request->request_payload;
  while (rep != NULL) {
    if (strcmp ( oc_string(rep->name), g_light_RESOURCE_PROPERTY_NAME_value) == 0) {
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
    if (strcmp ( oc_string(rep->name), g_light_RESOURCE_PROPERTY_NAME_value) == 0) {
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
      
      if (strcmp ( oc_string(rep->name), g_light_RESOURCE_PROPERTY_NAME_value)== 0) {
        /* assign "value" */
        PRINT ("  property 'value' : %s\n", btoa(rep->value.boolean));
        g_light_value = rep->value.boolean;
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_boolean(root, value, g_light_value);
    
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
    //oc_send_response(request, OC_STATUS_NOT_MODIFIED);
    oc_send_response(request, OC_STATUS_BAD_REQUEST);
  }
  PRINT("-- End post_light\n");
}
 
/**
* post method for "/temperature" resource.
* The function has as input the request body, which are the input values of the POST method.
* The input values (as a set) are checked if all supplied values are correct.
* If the input values are correct, they will be assigned to the global  property values.
* Resource Description:
* Sets the desired temperature value.
* If a "unit" is included and the server may not support the unit indicated the request will fail.
* If the units are omitted value is taken to be in C.
*
* @param request the request representation.
* @param interfaces the used interfaces during the request.
* @param user_data the supplied user data.
*/
static void
post_temperature(oc_request_t *request, oc_interface_mask_t interfaces, void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_temperature:\n");
  oc_rep_t *rep = request->request_payload;
  
  /* query name 'units' type: 'string', enum: ['C', 'F', 'K']*/
  char *_units = NULL; /* not null terminated Units */
  int _units_len = oc_get_query_value(request, "units", &_units);
  if (_units_len != -1) {
    bool query_ok = false;
  
    if ( strncmp (_units, "C", _units_len) == 0)  query_ok = true;
    if ( strncmp (_units, "F", _units_len) == 0)  query_ok = true;
    if ( strncmp (_units, "K", _units_len) == 0)  query_ok = true;
    if (query_ok == false) error_state = true;
    PRINT (" query value 'units': %.*s\n", _units_len, _units);
    /* TODO: use the query value to tailer the response*/
  }
  /* loop over the request document for each required input field to check if all required input fields are present */
  bool var_in_request= false; 
  rep = request->request_payload;
  while (rep != NULL) {
    if (strcmp ( oc_string(rep->name), g_temperature_RESOURCE_PROPERTY_NAME_temperature) == 0) {
      var_in_request = true;
    }
    rep = rep->next;
  }
  if ( var_in_request == false) 
  { 
      error_state = true;
      PRINT (" required property: 'temperature' not in request\n");
  }
  /* loop over the request document to check if all inputs are ok */
  rep = request->request_payload;
  while (rep != NULL) {
    PRINT("key: (check) %s \n", oc_string(rep->name));
    
    error_state = check_on_readonly_common_resource_properties(rep->name, error_state);
    if (strcmp ( oc_string(rep->name), g_temperature_RESOURCE_PROPERTY_NAME_temperature) == 0) {
      /* property "temperature" of type double exist in payload */ 
      if ( (rep->type != OC_REP_DOUBLE) & (rep->type != OC_REP_INT)) {
        error_state = true;
        PRINT ("   property 'temperature' is not of type double or int %d \n", rep->type);
      }
    }
    if (strcmp ( oc_string(rep->name), g_temperature_RESOURCE_PROPERTY_NAME_units) == 0) {
      /* property "units" of type string exist in payload */
      if (rep->type != OC_REP_STRING) {
        error_state = true;
        PRINT ("   property 'units' is not of type string %d \n", rep->type);
      }
      if (strlen(oc_string(rep->value.string)) >=  (MAX_PAYLOAD_STRING-1) )
      {
        error_state = true;
        PRINT ("   property 'units' is too long %d expected: MAX_PAYLOAD_STRING-1 \n", (int)strlen(oc_string(rep->value.string)));
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
      
      if (strcmp ( oc_string(rep->name), g_temperature_RESOURCE_PROPERTY_NAME_temperature) == 0) {
        /* assign "temperature" */
        PRINT ("  property 'temperature' : %f\n", rep->value.double_p);
        g_temperature_temperature = rep->value.double_p;
      }
      if (strcmp ( oc_string(rep->name), g_temperature_RESOURCE_PROPERTY_NAME_units) == 0) {
        /* assign "units" */
        PRINT ("  property 'units' : %s\n", oc_string(rep->value.string));
        strncpy(g_temperature_units, oc_string(rep->value.string), MAX_PAYLOAD_STRING-1);
      }
      rep = rep->next;
    }
    /* set the response */
    PRINT("Set response \n");
    oc_rep_start_root_object();
    /*oc_process_baseline_interface(request->resource); */
    oc_rep_set_double(root, temperature, g_temperature_temperature );
    oc_rep_set_text_string(root, units, g_temperature_units);
    
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
    //oc_send_response(request, OC_STATUS_NOT_MODIFIED);
    oc_send_response(request, OC_STATUS_BAD_REQUEST);
  }
  PRINT("-- End post_temperature\n");
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

  PRINT("Register Resource with local path \"/geolocation\"\n");
  oc_resource_t *res_geolocation = oc_new_resource(NULL, g_geolocation_RESOURCE_ENDPOINT, g_geolocation_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_geolocation_nr_resource_types);
  for( int a = 0; a < g_geolocation_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_geolocation_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_geolocation,g_geolocation_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_geolocation_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_geolocation, convert_if_string(g_geolocation_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_geolocation, convert_if_string(g_geolocation_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_geolocation_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_geolocation, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_geolocation, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_geolocation, true); */
   
  oc_resource_set_request_handler(res_geolocation, OC_GET, get_geolocation, NULL);
  oc_add_resource(res_geolocation);

  PRINT("Register Resource with local path \"/light\"\n");
  oc_resource_t *res_light = oc_new_resource(NULL, g_light_RESOURCE_ENDPOINT, g_light_nr_resource_types, 0);
  PRINT("     number of Resource Types: %d\n", g_light_nr_resource_types);
  for( int a = 0; a < g_light_nr_resource_types; a++ ) {
    PRINT("     Resource Type: \"%s\"\n", g_light_RESOURCE_TYPE[a]);
    oc_resource_bind_resource_type(res_light,g_light_RESOURCE_TYPE[a]);
  }
  for( int a = 0; a < g_light_nr_resource_interfaces; a++ ) {
    oc_resource_bind_resource_interface(res_light, convert_if_string(g_light_RESOURCE_INTERFACE[a]));
  }
  oc_resource_set_default_interface(res_light, convert_if_string(g_light_RESOURCE_INTERFACE[0]));  
  PRINT("     Default OCF Interface: \"%s\"\n", g_light_RESOURCE_INTERFACE[0]);
  oc_resource_set_discoverable(res_light, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  oc_resource_set_periodic_observable(res_light, 1);
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is called.
    this function must be called when the value changes, perferable on an interrupt when something is read from the hardware. */
  /*oc_resource_set_observable(res_light, true); */
   
  oc_resource_set_request_handler(res_light, OC_GET, get_light, NULL);
   
  oc_resource_set_request_handler(res_light, OC_POST, post_light, NULL);
  oc_add_resource(res_light);

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
   
  oc_resource_set_request_handler(res_temperature, OC_POST, post_temperature, NULL);
  oc_add_resource(res_temperature);
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
    PRINT("ERROR installing PKI certificate\n");
  } else {
    PRINT("Successfully installed PKI certificate\n");
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
#else
    PRINT("No PKI certificates installed\n");
#endif /* OC_SECURITY && OC_PKI */
}


/**
* intializes the global variables
* registers and starts the handler

*/
void
initialize_variables(void)
{
  /* initialize global variables for resource "/geolocation" */
  g_geolocation_alt = 12.07; /* current value of property "alt"  The current distance (metres) above or below 'local' sea-level. */
  g_geolocation_latitude = 55.070859; /* current value of property "latitude"  The Device's Current Latitude coordinate (degrees). */
  g_geolocation_longitude = -3.60512; /* current value of property "longitude"  The Device's Current Longitude coordinate (degrees). */
  /* initialize global variables for resource "/light" */  g_light_value = false; /* current value of property "value" The status of the switch. */
  /* initialize global variables for resource "/temperature" */
  g_temperature_temperature = 20.0; /* current value of property "temperature"  The current temperature setting or measurement. */
  strcpy(g_temperature_units, "C");  /* current value of property "units" The unit for the conveyed temperature value, Note that when doing an UPDATE, the unit on the device does NOT change, it only indicates the unit of the conveyed value during the UPDATE operation. */
  
  /* set the flag for NO oic/con resource. */
  oc_set_con_res_announced(false);

}

#ifndef NO_MAIN

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
#endif /* WIN32 */

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
#endif /* __linux__ */

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
  oc_clock_time_t next_event;

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

  PRINT("Used input file : \"/home/pi/workspace/roambee/device_output/out_codegeneration_merged.swagger.json\"\n");
  PRINT("OCF Server name : \"Roambee\"\n");

  /*intialize the variables */
  initialize_variables();
  
  
  /* initializes the handlers structure */
  static const oc_handler_t handler = {.init = app_init,
                                       .signal_event_loop = signal_event_loop,
                                       .register_resources = register_resources
#ifdef OC_CLIENT
                                       ,
                                       .requests_entry = 0 
#endif
                                       };

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
  
  
  /* start the stack */
  init = oc_main_init(&handler);

  if (init < 0) {
    PRINT("oc_main_init failed %d, exiting.\n", init);
    return init;
  }

  PRINT("OCF server \"Roambee\" running, waiting on incoming connections.\n");

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
#endif /* NO_MAIN */