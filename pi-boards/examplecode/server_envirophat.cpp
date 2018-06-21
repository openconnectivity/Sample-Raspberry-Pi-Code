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
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include <signal.h>
#include <functional>
#include <string>
#include <iostream>
#include <memory>
#include <exception>

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#ifdef MRAA
#include "mraa.hpp"
#endif

#include "ocstack.h"
#include "OCPlatform.h"
#include "OCApi.h"
#include "ocpayload.h"

using namespace OC;
namespace PH = std::placeholders;

/*
 tool_version          : 20171123
 input_file            : ../device_output/out_codegeneration_merged.swagger.json
 version of input_file : v1.1.0-20160519
 title of input_file   : Binary Switch
*/

#define INTERFACE_KEY "if"

/*
* default class, so that we have to define less variables/functions.
*/
class Resource
{
    protected:
    OCResourceHandle m_resourceHandle;
    OC::OCRepresentation m_rep;
    virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request)=0;
};


/*
 * class definition for class that handles /binaryswitch
 *
 * This resource describes a binary switch (on/off).
 * The value is a boolean.
 * A value of 'true' means that the switch is on.
 * A value of 'false' means that the switch is off.

*/
class BinaryswitchResource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        BinaryswitchResource(std::string resourceUri = "/binaryswitch");

        /*
         * destructor
         */
         virtual ~BinaryswitchResource(void);

        /*
         * Register the resource with the server
         *
         * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
         * setting resourceProperty as OC_OBSERVABLE will allow observation
         * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
         * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
         * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
         * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
         *
         * @param resourceProperty indicates the property of the resource. Defined in octypes.h.
         */
        OCStackResult registerResource(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

        /*
         * Attempt to send out notifications to observing clients
         * if no value on the device has been changed no notification
         * will be sent.
         *
         * @return OC_STACK_OK on success
         */
        OCStackResult sendNotification();
 
mraa::Gpio *gpio;
int ledPin = 7;

   private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /binaryswitch
         * This resource describes a binary switch (on/off).
         * The value is a boolean.
         * A value of 'true' means that the switch is on.
         * A value of 'false' means that the switch is off.

         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);

        /*
         * Parse the payload for the update function (e.g. POST) /binaryswitch

         * @param queries  the query parameters for this call
         * @param rep  the response to get the property values from
         * @return OCEntityHandlerResult ok or not ok indication
         */
        OCEntityHandlerResult post(OC::QueryParamsMap queries, const OC::OCRepresentation& rep);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.switch.binary"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/binaryswitch"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The interface set supported by this resource
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": Resource Type
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": Status of the switch
        std::string m_var_name_value = "value"; // the name for the attribute "value"
        
    protected:
        /*
         * Check if the interface is
         * @param  interface_name the interface name used during the request
         * @return true: updatable interface
         */
        bool in_updatable_interfaces(std::string interface_name);

        /*
         * the entity handler for this resource
         * @param request the incoming request to handle
         * @return OCEntityHandlerResult ok or not ok indication
         */
        virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
};

/*
* Constructor code
*/
BinaryswitchResource::BinaryswitchResource(std::string resourceUri)
{
    std::cout << "- Running: BinaryswitchResource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /binaryswitch
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" Status of the switch

gpio = new mraa::Gpio(ledPin);
if (!gpio)
{
    std::cout << "Error instantiating gpio" << std::endl;
}
gpio->dir(mraa::DIR_OUT);

gpio->write(1);
sleep(1);
gpio->write(0);        
}

/*
* Destructor code
*/
BinaryswitchResource::~BinaryswitchResource() { }

OCStackResult BinaryswitchResource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&BinaryswitchResource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register BinaryswitchResource." << std::endl;
        return result;
    }

    /// add the additional resource types
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0])); a++ )
    {
        result = OCPlatform::bindTypeToResource(m_resourceHandle, m_RESOURCE_TYPE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind resource type:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }
    // add the additional interfaces
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0])); a++)
    {
        result = OCPlatform::bindInterfaceToResource(m_resourceHandle, m_RESOURCE_INTERFACE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind interface:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }

    std::cout << "BinaryswitchResource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

OCStackResult BinaryswitchResource::sendNotification(void)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        auto pResponse = std::make_shared<OC::OCResourceResponse>();
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}

/*
* Make the payload for the retrieve function (e.g. GET) /binaryswitch
* @param queries  the query parameters for this call
*/
OCRepresentation BinaryswitchResource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);
	
	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables

    std::cout << "\t\t" << "property 'n' : "<< m_var_value_n << std::endl;
    std::cout << "\t\t" << "property 'value' : "<< ((m_var_value_value) ? "true" : "false") << std::endl;
    
    m_rep.setValue(m_var_name_if,  m_var_value_if ); 
    m_rep.setValue(m_var_name_n, m_var_value_n ); 
    m_rep.setValue(m_var_name_rt,  m_var_value_rt ); 
    m_rep.setValue(m_var_name_value, m_var_value_value ); 

    return m_rep;
}

/*
* Parse the payload for the update function (e.g. POST) /binaryswitch
* @param queries  the query parameters for this call
* @param rep  the response to get the property values from
* @return OCEntityHandlerResult ok or not ok indication
*/
OCEntityHandlerResult BinaryswitchResource::post(QueryParamsMap queries, const OCRepresentation& rep)
{
    OCEntityHandlerResult ehResult = OC_EH_OK;
    OC_UNUSED(queries);
    
    // TODO: missing code: add check on array contents out of range
	// such a check is resource specific
    try {
        if (rep.hasAttribute(m_var_name_if))
        {
            // value exist in payload
            
            // check if "if" is read only
            ehResult = OC_EH_ERROR;
            std::cout << "\t\t" << "property 'if' is readOnly "<< std::endl;
            
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    
    try {
        if (rep.hasAttribute(m_var_name_n))
        {
            // value exist in payload
            
            // check if "n" is read only
            ehResult = OC_EH_ERROR;
            std::cout << "\t\t" << "property 'n' is readOnly "<< std::endl;
            
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    
    // TODO: missing code: add check on array contents out of range
	// such a check is resource specific
    try {
        if (rep.hasAttribute(m_var_name_rt))
        {
            // value exist in payload
            
            // check if "rt" is read only
            ehResult = OC_EH_ERROR;
            std::cout << "\t\t" << "property 'rt' is readOnly "<< std::endl;
            
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    try {
        if (rep.hasAttribute(m_var_name_value))
        {
            // value exist in payload
            
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    if (ehResult == OC_EH_OK)
    {
        // no error: assign the variables
        // array only works for integer, boolean, numbers and strings
        // TODO: missing code, make it also work with array of objects
        try {
            if (rep.hasAttribute(m_var_name_if))
            {
                rep.getValue(m_var_name_if, m_var_value_if);
                int first = 1;
                std::cout << "\t\t" << "property 'if' UPDATED: " ;
                for(auto myvar: m_var_value_if)
                {
                    if(first)
                    {
                        std::cout << myvar;
                        first = 0;
                    }
                    else
                    {
                        std::cout << "," << myvar;
                    }
                }
                std::cout <<  std::endl;
            }
            else
            {
                std::cout << "\t\t" << "property 'if' not found in the representation" << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
        try {
            // value exist in payload
            std::string temp;
            if (rep.getValue(m_var_name_n, temp ))
            {
                m_var_value_n = temp;
                std::cout << "\t\t" << "property 'n' UPDATED: " << m_var_value_n << std::endl;
            }
            else
            {
                std::cout << "\t\t" << "property 'n' not found in the representation" << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }// array only works for integer, boolean, numbers and strings
        // TODO: missing code, make it also work with array of objects
        try {
            if (rep.hasAttribute(m_var_name_rt))
            {
                rep.getValue(m_var_name_rt, m_var_value_rt);
                int first = 1;
                std::cout << "\t\t" << "property 'rt' UPDATED: " ;
                for(auto myvar: m_var_value_rt)
                {
                    if(first)
                    {
                        std::cout << myvar;
                        first = 0;
                    }
                    else
                    {
                        std::cout << "," << myvar;
                    }
                }
                std::cout <<  std::endl;
            }
            else
            {
                std::cout << "\t\t" << "property 'rt' not found in the representation" << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
        try {
            bool temp;
            if (rep.getValue(m_var_name_value, temp ))
            {
                m_var_value_value = temp;
                std::cout << "\t\t" << "property 'value' UPDATED: " << ((m_var_value_value) ? "true" : "false") << std::endl;
            }
            else
            {
                std::cout << "\t\t" << "property 'value' not found in the representation" << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
	// TODO: ACTUATOR add here the code to talk to the HW if one implements an actuator.
	// one can use the member variables as input to those calls
	// the member values have been updated already with the request data
 gpio->write(m_var_value_value);
   }
    return ehResult;
}
/*
* Check if the interface name is an registered interface name
*/
bool BinaryswitchResource::in_updatable_interfaces(std::string interface_name)
{
    for (unsigned int i=0; i < (sizeof(m_IF_UPDATE)/sizeof(m_IF_UPDATE[0])); i++)
    {
        if (m_IF_UPDATE[i].compare(interface_name) == 0)
            return true;
    }
    return false;
}

/*
* the entity handler
*/
OCEntityHandlerResult BinaryswitchResource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for BinaryswitchResource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for BinaryswitchResource, URI is : "
                  << request->getResourceUri() << std::endl;

        // Check for query params (if any)
        QueryParamsMap queries = request->getQueryParameters();
        if (!queries.empty())
        {
            std::cout << "\nQuery processing up to entityHandler" << std::endl;
        }
        for (auto it : queries)
        {
            std::cout << "Query key: " << it.first << " value : " << it.second
                    << std::endl;
        }
        // get the value, so that we can AND it to check which flags are set
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            // request flag is set
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            if(request->getRequestType() == "GET")
            {
                std::cout<<"BinaryswitchResource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }

            else if(request->getRequestType() == "POST")
            {
                std::cout <<"BinaryswitchResource Post Request"<<std::endl;
                bool  handle_post = true;

                if (queries.size() > 0)
                {
                    for (const auto &eachQuery : queries)
                    {
                        std::string key = eachQuery.first;
                        if (key.compare(INTERFACE_KEY) == 0)
                        {
                            std::string value = eachQuery.second;
                            if (in_updatable_interfaces(value) == false)
                            {
                                std::cout << "Update request received via interface: " << value
                                            << " . This interface is not authorized to update resource!!" << std::endl;
                                pResponse->setResponseResult(OCEntityHandlerResult::OC_EH_FORBIDDEN);
                                handle_post = false;
                                ehResult = OC_EH_ERROR;
                                break;
                            }
                        }
                    }
                }
                if (handle_post)
                {
                    ehResult = post(queries, request->getResourceRepresentation());
                    if (ehResult == OC_EH_OK)
                    {
                        pResponse->setResourceRepresentation(get(queries), "");
                        if (OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                        {
                            if (OC_STACK_OK != sendNotification() )
                            {
                                std::cerr << "NOTIFY failed." << std::endl;
                            }
                        }
                    }
                    else
                    {
                         pResponse->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                    }
                }
            }
            else
            {
                std::cout << "BinaryswitchResource unsupported request type (delete,put,..)"
                    << request->getRequestType() << std::endl;
                pResponse->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(pResponse);
                ehResult = OC_EH_ERROR;
            }
        }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // observe flag is set
            ObservationInfo observationInfo = request->getObservationInfo();
            std::cout << "\t\trequestFlag : observer ";
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                std::cout << "register" << std::endl; 
            } 
            else
            {
                std::cout << "unregister" << std::endl;
            }

            if(ObserveAction::ObserveRegister == observationInfo.action)
            {
                // add observer
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if(ObserveAction::ObserveUnregister == observationInfo.action)
            {
                // delete observer
                m_interestedObservers.erase(std::remove(
                                            m_interestedObservers.begin(),
                                            m_interestedObservers.end(),
                                            observationInfo.obsId),
                                            m_interestedObservers.end());
            }
            ehResult = OC_EH_OK;
        }
    }
    return ehResult;
}


/*
 * class definition for class that handles /brightness
 *
 * This resource describes the brightness of a light or lamp.
 * brightness is an integer showing the current brightness level as a quantized representation in the range 0-100.
 * A brightness of 0 is the minimum for the resource.
 * A brightness of 100 is the maximum for the resource.
 * Retrieves the current brightness level.

*/
class BrightnessResource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        BrightnessResource(std::string resourceUri = "/brightness");

        /*
         * destructor
         */
         virtual ~BrightnessResource(void);

        /*
         * Register the resource with the server
         *
         * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
         * setting resourceProperty as OC_OBSERVABLE will allow observation
         * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
         * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
         * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
         * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
         *
         * @param resourceProperty indicates the property of the resource. Defined in octypes.h.
         */
        OCStackResult registerResource(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

        /*
         * Attempt to send out notifications to observing clients
         * if no value on the device has been changed no notification
         * will be sent.
         *
         * @return OC_STACK_OK on success
         */
        OCStackResult sendNotification();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /brightness
         * This resource describes the brightness of a light or lamp.
         * brightness is an integer showing the current brightness level as a quantized representation in the range 0-100.
         * A brightness of 0 is the minimum for the resource.
         * A brightness of 100 is the maximum for the resource.
         * Retrieves the current brightness level.

         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.light.brightness"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/brightness"
        int m_var_value_brightness; // the value for the attribute "brightness": Quantized representation in the range 0-100 of the current sensed or set value for Brightness
        std::string m_var_name_brightness = "brightness"; // the name for the attribute "brightness"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The interface set supported by this resource
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": Resource Type
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        void* m_var_value_value; // the value for the attribute "value": The value sensed or actuated by this Resource
        std::string m_var_name_value = "value"; // the name for the attribute "value"
        
    protected:
        /*
         * Check if the interface is
         * @param  interface_name the interface name used during the request
         * @return true: updatable interface
         */
        bool in_updatable_interfaces(std::string interface_name);

        /*
         * the entity handler for this resource
         * @param request the incoming request to handle
         * @return OCEntityHandlerResult ok or not ok indication
         */
        virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
};

/*
* Constructor code
*/
BrightnessResource::BrightnessResource(std::string resourceUri)
{
    std::cout << "- Running: BrightnessResource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /brightness
    m_var_value_brightness = 50; // current value of property "brightness" Quantized representation in the range 0-100 of the current sensed or set value for Brightness
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.light.brightness");
    }

/*
* Destructor code
*/
BrightnessResource::~BrightnessResource() { }

OCStackResult BrightnessResource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&BrightnessResource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register BrightnessResource." << std::endl;
        return result;
    }

    /// add the additional resource types
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0])); a++ )
    {
        result = OCPlatform::bindTypeToResource(m_resourceHandle, m_RESOURCE_TYPE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind resource type:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }
    // add the additional interfaces
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0])); a++)
    {
        result = OCPlatform::bindInterfaceToResource(m_resourceHandle, m_RESOURCE_INTERFACE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind interface:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }

    std::cout << "BrightnessResource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

OCStackResult BrightnessResource::sendNotification(void)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        auto pResponse = std::make_shared<OC::OCResourceResponse>();
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}

/*
* Make the payload for the retrieve function (e.g. GET) /brightness
* @param queries  the query parameters for this call
*/
OCRepresentation BrightnessResource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);
	
	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables

    std::cout << "\t\t" << "property 'brightness' : "<< m_var_value_brightness << std::endl;
    std::cout << "\t\t" << "property 'n' : "<< m_var_value_n << std::endl;
    
    m_rep.setValue(m_var_name_brightness, m_var_value_brightness ); 
    m_rep.setValue(m_var_name_if,  m_var_value_if ); 
    m_rep.setValue(m_var_name_n, m_var_value_n ); 
    m_rep.setValue(m_var_name_rt,  m_var_value_rt ); 

    return m_rep;
}
/*
* Check if the interface name is an registered interface name
*/
bool BrightnessResource::in_updatable_interfaces(std::string interface_name)
{
    for (unsigned int i=0; i < (sizeof(m_IF_UPDATE)/sizeof(m_IF_UPDATE[0])); i++)
    {
        if (m_IF_UPDATE[i].compare(interface_name) == 0)
            return true;
    }
    return false;
}

/*
* the entity handler
*/
OCEntityHandlerResult BrightnessResource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for BrightnessResource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for BrightnessResource, URI is : "
                  << request->getResourceUri() << std::endl;

        // Check for query params (if any)
        QueryParamsMap queries = request->getQueryParameters();
        if (!queries.empty())
        {
            std::cout << "\nQuery processing up to entityHandler" << std::endl;
        }
        for (auto it : queries)
        {
            std::cout << "Query key: " << it.first << " value : " << it.second
                    << std::endl;
        }
        // get the value, so that we can AND it to check which flags are set
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            // request flag is set
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            if(request->getRequestType() == "GET")
            {
                std::cout<<"BrightnessResource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "BrightnessResource unsupported request type (delete,put,..)"
                    << request->getRequestType() << std::endl;
                pResponse->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(pResponse);
                ehResult = OC_EH_ERROR;
            }
        }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // observe flag is set
            ObservationInfo observationInfo = request->getObservationInfo();
            std::cout << "\t\trequestFlag : observer ";
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                std::cout << "register" << std::endl; 
            } 
            else
            {
                std::cout << "unregister" << std::endl;
            }

            if(ObserveAction::ObserveRegister == observationInfo.action)
            {
                // add observer
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if(ObserveAction::ObserveUnregister == observationInfo.action)
            {
                // delete observer
                m_interestedObservers.erase(std::remove(
                                            m_interestedObservers.begin(),
                                            m_interestedObservers.end(),
                                            observationInfo.obsId),
                                            m_interestedObservers.end());
            }
            ehResult = OC_EH_OK;
        }
    }
    return ehResult;
}


/*
 * class definition for class that handles /colourRGB
 *
 * This resource specifies the actual colour in the RGB space represented as an array of integers.
 * Each colour value is described with a Red, Green, Blue component.
 * These colour values are encoded as an array of integer values ([R,G,B]).
 * The minimum and maximum colour value per component may be described by range (from oic.r.baseresource).
 * When range (from oic.r.baseresource) is omitted, then the range is [0,255].
 * Retrieves the current colour in RGB.
 * Value is an array of integer values in the order R,G,B.

*/
class ColourrgbResource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        ColourrgbResource(std::string resourceUri = "/colourRGB");

        /*
         * destructor
         */
         virtual ~ColourrgbResource(void);

        /*
         * Register the resource with the server
         *
         * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
         * setting resourceProperty as OC_OBSERVABLE will allow observation
         * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
         * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
         * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
         * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
         *
         * @param resourceProperty indicates the property of the resource. Defined in octypes.h.
         */
        OCStackResult registerResource(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

        /*
         * Attempt to send out notifications to observing clients
         * if no value on the device has been changed no notification
         * will be sent.
         *
         * @return OC_STACK_OK on success
         */
        OCStackResult sendNotification();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /colourRGB
         * This resource specifies the actual colour in the RGB space represented as an array of integers.
         * Each colour value is described with a Red, Green, Blue component.
         * These colour values are encoded as an array of integer values ([R,G,B]).
         * The minimum and maximum colour value per component may be described by range (from oic.r.baseresource).
         * When range (from oic.r.baseresource) is omitted, then the range is [0,255].
         * Retrieves the current colour in RGB.
         * Value is an array of integer values in the order R,G,B.

         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.colour.rgb"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/colourRGB"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The interface set supported by this resource
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<int>  m_var_value_rgbValue; // the value for the array attribute "rgbValue": RGB value; the first item is the R, second the G, third the B.
        std::string m_var_name_rgbValue = "rgbValue"; // the name for the attribute "rgbValue"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": Resource Type
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        void* m_var_value_value; // the value for the attribute "value": The value sensed or actuated by this Resource
        std::string m_var_name_value = "value"; // the name for the attribute "value"
        
    protected:
        /*
         * Check if the interface is
         * @param  interface_name the interface name used during the request
         * @return true: updatable interface
         */
        bool in_updatable_interfaces(std::string interface_name);

        /*
         * the entity handler for this resource
         * @param request the incoming request to handle
         * @return OCEntityHandlerResult ok or not ok indication
         */
        virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
};

/*
* Constructor code
*/
ColourrgbResource::ColourrgbResource(std::string resourceUri)
{
    std::cout << "- Running: ColourrgbResource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /colourRGB
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rgbValue  RGB value; the first item is the R, second the G, third the B.m_var_value_rgbValue.push_back(255);
    m_var_value_rgbValue.push_back(255);
    m_var_value_rgbValue.push_back(255);
    
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.colour.rgb");
    }

/*
* Destructor code
*/
ColourrgbResource::~ColourrgbResource() { }

OCStackResult ColourrgbResource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&ColourrgbResource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register ColourrgbResource." << std::endl;
        return result;
    }

    /// add the additional resource types
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0])); a++ )
    {
        result = OCPlatform::bindTypeToResource(m_resourceHandle, m_RESOURCE_TYPE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind resource type:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }
    // add the additional interfaces
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0])); a++)
    {
        result = OCPlatform::bindInterfaceToResource(m_resourceHandle, m_RESOURCE_INTERFACE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind interface:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }

    std::cout << "ColourrgbResource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

OCStackResult ColourrgbResource::sendNotification(void)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        auto pResponse = std::make_shared<OC::OCResourceResponse>();
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}

/*
* Make the payload for the retrieve function (e.g. GET) /colourRGB
* @param queries  the query parameters for this call
*/
OCRepresentation ColourrgbResource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);
	
	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables

    std::cout << "\t\t" << "property 'n' : "<< m_var_value_n << std::endl;
    
    m_rep.setValue(m_var_name_if,  m_var_value_if ); 
    m_rep.setValue(m_var_name_n, m_var_value_n ); 
    m_rep.setValue(m_var_name_rgbValue,  m_var_value_rgbValue ); 
    m_rep.setValue(m_var_name_rt,  m_var_value_rt ); 

    return m_rep;
}
/*
* Check if the interface name is an registered interface name
*/
bool ColourrgbResource::in_updatable_interfaces(std::string interface_name)
{
    for (unsigned int i=0; i < (sizeof(m_IF_UPDATE)/sizeof(m_IF_UPDATE[0])); i++)
    {
        if (m_IF_UPDATE[i].compare(interface_name) == 0)
            return true;
    }
    return false;
}

/*
* the entity handler
*/
OCEntityHandlerResult ColourrgbResource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for ColourrgbResource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for ColourrgbResource, URI is : "
                  << request->getResourceUri() << std::endl;

        // Check for query params (if any)
        QueryParamsMap queries = request->getQueryParameters();
        if (!queries.empty())
        {
            std::cout << "\nQuery processing up to entityHandler" << std::endl;
        }
        for (auto it : queries)
        {
            std::cout << "Query key: " << it.first << " value : " << it.second
                    << std::endl;
        }
        // get the value, so that we can AND it to check which flags are set
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            // request flag is set
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            if(request->getRequestType() == "GET")
            {
                std::cout<<"ColourrgbResource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "ColourrgbResource unsupported request type (delete,put,..)"
                    << request->getRequestType() << std::endl;
                pResponse->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(pResponse);
                ehResult = OC_EH_ERROR;
            }
        }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // observe flag is set
            ObservationInfo observationInfo = request->getObservationInfo();
            std::cout << "\t\trequestFlag : observer ";
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                std::cout << "register" << std::endl; 
            } 
            else
            {
                std::cout << "unregister" << std::endl;
            }

            if(ObserveAction::ObserveRegister == observationInfo.action)
            {
                // add observer
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if(ObserveAction::ObserveUnregister == observationInfo.action)
            {
                // delete observer
                m_interestedObservers.erase(std::remove(
                                            m_interestedObservers.begin(),
                                            m_interestedObservers.end(),
                                            observationInfo.obsId),
                                            m_interestedObservers.end());
            }
            ehResult = OC_EH_OK;
        }
    }
    return ehResult;
}


/*
 * class definition for class that handles /heading
 *
 * This resource describes the direction of the Earth's magnetic field at the observer's current point in space.
 * Typical use case includes measurement of compass readings on a personal device.
 * The value is an array containing Hx, Hy, Hz (in that order) each of which are floats.
 * Each of Hx, Hy and Hz are expressed in A/m (Amperes per metre)

*/
class HeadingResource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        HeadingResource(std::string resourceUri = "/heading");

        /*
         * destructor
         */
         virtual ~HeadingResource(void);

        /*
         * Register the resource with the server
         *
         * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
         * setting resourceProperty as OC_OBSERVABLE will allow observation
         * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
         * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
         * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
         * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
         *
         * @param resourceProperty indicates the property of the resource. Defined in octypes.h.
         */
        OCStackResult registerResource(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

        /*
         * Attempt to send out notifications to observing clients
         * if no value on the device has been changed no notification
         * will be sent.
         *
         * @return OC_STACK_OK on success
         */
        OCStackResult sendNotification();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /heading
         * This resource describes the direction of the Earth's magnetic field at the observer's current point in space.
         * Typical use case includes measurement of compass readings on a personal device.
         * The value is an array containing Hx, Hy, Hz (in that order) each of which are floats.
         * Each of Hx, Hy and Hz are expressed in A/m (Amperes per metre)

         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.sensor.magneticfielddirection"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/heading"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The interface set supported by this resource
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": Resource Type
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        std::vector<double>  m_var_value_value; // the value for the array attribute "value": Array containing Hx, Hy, Hz.
        std::string m_var_name_value = "value"; // the name for the attribute "value"
        
    protected:
        /*
         * Check if the interface is
         * @param  interface_name the interface name used during the request
         * @return true: updatable interface
         */
        bool in_updatable_interfaces(std::string interface_name);

        /*
         * the entity handler for this resource
         * @param request the incoming request to handle
         * @return OCEntityHandlerResult ok or not ok indication
         */
        virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
};

/*
* Constructor code
*/
HeadingResource::HeadingResource(std::string resourceUri)
{
    std::cout << "- Running: HeadingResource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /heading
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.sensor.magneticfielddirection");
    // initialize vector value  Array containing Hx, Hy, Hz.m_var_value_value.push_back(100.0);
    m_var_value_value.push_back(15.0);
    m_var_value_value.push_back(90.0);
    
    }

/*
* Destructor code
*/
HeadingResource::~HeadingResource() { }

OCStackResult HeadingResource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&HeadingResource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register HeadingResource." << std::endl;
        return result;
    }

    /// add the additional resource types
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0])); a++ )
    {
        result = OCPlatform::bindTypeToResource(m_resourceHandle, m_RESOURCE_TYPE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind resource type:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }
    // add the additional interfaces
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0])); a++)
    {
        result = OCPlatform::bindInterfaceToResource(m_resourceHandle, m_RESOURCE_INTERFACE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind interface:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }

    std::cout << "HeadingResource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

OCStackResult HeadingResource::sendNotification(void)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        auto pResponse = std::make_shared<OC::OCResourceResponse>();
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}

/*
* Make the payload for the retrieve function (e.g. GET) /heading
* @param queries  the query parameters for this call
*/
OCRepresentation HeadingResource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);
	
	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables

    std::cout << "\t\t" << "property 'n' : "<< m_var_value_n << std::endl;
    
    m_rep.setValue(m_var_name_if,  m_var_value_if ); 
    m_rep.setValue(m_var_name_n, m_var_value_n ); 
    m_rep.setValue(m_var_name_rt,  m_var_value_rt ); 
    m_rep.setValue(m_var_name_value,  m_var_value_value ); 

    return m_rep;
}
/*
* Check if the interface name is an registered interface name
*/
bool HeadingResource::in_updatable_interfaces(std::string interface_name)
{
    for (unsigned int i=0; i < (sizeof(m_IF_UPDATE)/sizeof(m_IF_UPDATE[0])); i++)
    {
        if (m_IF_UPDATE[i].compare(interface_name) == 0)
            return true;
    }
    return false;
}

/*
* the entity handler
*/
OCEntityHandlerResult HeadingResource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for HeadingResource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for HeadingResource, URI is : "
                  << request->getResourceUri() << std::endl;

        // Check for query params (if any)
        QueryParamsMap queries = request->getQueryParameters();
        if (!queries.empty())
        {
            std::cout << "\nQuery processing up to entityHandler" << std::endl;
        }
        for (auto it : queries)
        {
            std::cout << "Query key: " << it.first << " value : " << it.second
                    << std::endl;
        }
        // get the value, so that we can AND it to check which flags are set
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            // request flag is set
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            if(request->getRequestType() == "GET")
            {
                std::cout<<"HeadingResource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "HeadingResource unsupported request type (delete,put,..)"
                    << request->getRequestType() << std::endl;
                pResponse->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(pResponse);
                ehResult = OC_EH_ERROR;
            }
        }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // observe flag is set
            ObservationInfo observationInfo = request->getObservationInfo();
            std::cout << "\t\trequestFlag : observer ";
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                std::cout << "register" << std::endl; 
            } 
            else
            {
                std::cout << "unregister" << std::endl;
            }

            if(ObserveAction::ObserveRegister == observationInfo.action)
            {
                // add observer
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if(ObserveAction::ObserveUnregister == observationInfo.action)
            {
                // delete observer
                m_interestedObservers.erase(std::remove(
                                            m_interestedObservers.begin(),
                                            m_interestedObservers.end(),
                                            observationInfo.obsId),
                                            m_interestedObservers.end());
            }
            ehResult = OC_EH_OK;
        }
    }
    return ehResult;
}


/*
 * class definition for class that handles /pressure
 *
 * This resource provides a measurement of Mean Sea Level Pressure experienced at the measuring point expressed in millibars.
 * The value is float which describes the atmospheric pressure in hPa (hectoPascals).
 * Note that hPa and the also commonly used unit of millibars (mbar) are numerically equivalent.

*/
class PressureResource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        PressureResource(std::string resourceUri = "/pressure");

        /*
         * destructor
         */
         virtual ~PressureResource(void);

        /*
         * Register the resource with the server
         *
         * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
         * setting resourceProperty as OC_OBSERVABLE will allow observation
         * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
         * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
         * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
         * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
         *
         * @param resourceProperty indicates the property of the resource. Defined in octypes.h.
         */
        OCStackResult registerResource(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

        /*
         * Attempt to send out notifications to observing clients
         * if no value on the device has been changed no notification
         * will be sent.
         *
         * @return OC_STACK_OK on success
         */
        OCStackResult sendNotification();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /pressure
         * This resource provides a measurement of Mean Sea Level Pressure experienced at the measuring point expressed in millibars.
         * The value is float which describes the atmospheric pressure in hPa (hectoPascals).
         * Note that hPa and the also commonly used unit of millibars (mbar) are numerically equivalent.

         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.sensor.atmosphericpressure"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/pressure"
        double m_var_value_atmosphericPressure; // the value for the attribute "atmosphericPressure": Current atmospheric pressure in hPa.
        std::string m_var_name_atmosphericPressure = "atmosphericPressure"; // the name for the attribute "atmosphericPressure"
        std::string m_var_value_id; // the value for the attribute "id": Instance ID of this specific resource
        std::string m_var_name_id = "id"; // the name for the attribute "id"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The interface set supported by this resource
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": Resource Type
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        
    protected:
        /*
         * Check if the interface is
         * @param  interface_name the interface name used during the request
         * @return true: updatable interface
         */
        bool in_updatable_interfaces(std::string interface_name);

        /*
         * the entity handler for this resource
         * @param request the incoming request to handle
         * @return OCEntityHandlerResult ok or not ok indication
         */
        virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
};

/*
* Constructor code
*/
PressureResource::PressureResource(std::string resourceUri)
{
    std::cout << "- Running: PressureResource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /pressure
    m_var_value_atmosphericPressure = 1000.4; // current value of property "atmosphericPressure"  Current atmospheric pressure in hPa.
    m_var_value_id = "unique_example_id";  // current value of property "id" Instance ID of this specific resource
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.sensor.atmosphericpressure");
    }

/*
* Destructor code
*/
PressureResource::~PressureResource() { }

OCStackResult PressureResource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&PressureResource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register PressureResource." << std::endl;
        return result;
    }

    /// add the additional resource types
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0])); a++ )
    {
        result = OCPlatform::bindTypeToResource(m_resourceHandle, m_RESOURCE_TYPE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind resource type:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }
    // add the additional interfaces
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0])); a++)
    {
        result = OCPlatform::bindInterfaceToResource(m_resourceHandle, m_RESOURCE_INTERFACE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind interface:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }

    std::cout << "PressureResource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

OCStackResult PressureResource::sendNotification(void)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        auto pResponse = std::make_shared<OC::OCResourceResponse>();
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}

/*
* Make the payload for the retrieve function (e.g. GET) /pressure
* @param queries  the query parameters for this call
*/
OCRepresentation PressureResource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);
	
	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables

    std::cout << "\t\t" << "property 'atmosphericPressure' : "<< m_var_value_atmosphericPressure << std::endl;
    std::cout << "\t\t" << "property 'id' : "<< m_var_value_id << std::endl;
    std::cout << "\t\t" << "property 'n' : "<< m_var_value_n << std::endl;
    
    m_rep.setValue(m_var_name_atmosphericPressure, m_var_value_atmosphericPressure ); 
    m_rep.setValue(m_var_name_id, m_var_value_id ); 
    m_rep.setValue(m_var_name_if,  m_var_value_if ); 
    m_rep.setValue(m_var_name_n, m_var_value_n ); 
    m_rep.setValue(m_var_name_rt,  m_var_value_rt ); 

    return m_rep;
}
/*
* Check if the interface name is an registered interface name
*/
bool PressureResource::in_updatable_interfaces(std::string interface_name)
{
    for (unsigned int i=0; i < (sizeof(m_IF_UPDATE)/sizeof(m_IF_UPDATE[0])); i++)
    {
        if (m_IF_UPDATE[i].compare(interface_name) == 0)
            return true;
    }
    return false;
}

/*
* the entity handler
*/
OCEntityHandlerResult PressureResource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for PressureResource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for PressureResource, URI is : "
                  << request->getResourceUri() << std::endl;

        // Check for query params (if any)
        QueryParamsMap queries = request->getQueryParameters();
        if (!queries.empty())
        {
            std::cout << "\nQuery processing up to entityHandler" << std::endl;
        }
        for (auto it : queries)
        {
            std::cout << "Query key: " << it.first << " value : " << it.second
                    << std::endl;
        }
        // get the value, so that we can AND it to check which flags are set
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            // request flag is set
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            if(request->getRequestType() == "GET")
            {
                std::cout<<"PressureResource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "PressureResource unsupported request type (delete,put,..)"
                    << request->getRequestType() << std::endl;
                pResponse->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(pResponse);
                ehResult = OC_EH_ERROR;
            }
        }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // observe flag is set
            ObservationInfo observationInfo = request->getObservationInfo();
            std::cout << "\t\trequestFlag : observer ";
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                std::cout << "register" << std::endl; 
            } 
            else
            {
                std::cout << "unregister" << std::endl;
            }

            if(ObserveAction::ObserveRegister == observationInfo.action)
            {
                // add observer
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if(ObserveAction::ObserveUnregister == observationInfo.action)
            {
                // delete observer
                m_interestedObservers.erase(std::remove(
                                            m_interestedObservers.begin(),
                                            m_interestedObservers.end(),
                                            observationInfo.obsId),
                                            m_interestedObservers.end());
            }
            ehResult = OC_EH_OK;
        }
    }
    return ehResult;
}


/*
 * class definition for class that handles /temperature
 *
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

*/
class TemperatureResource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        TemperatureResource(std::string resourceUri = "/temperature");

        /*
         * destructor
         */
         virtual ~TemperatureResource(void);

        /*
         * Register the resource with the server
         *
         * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
         * setting resourceProperty as OC_OBSERVABLE will allow observation
         * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
         * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
         * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
         * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
         *
         * @param resourceProperty indicates the property of the resource. Defined in octypes.h.
         */
        OCStackResult registerResource(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

        /*
         * Attempt to send out notifications to observing clients
         * if no value on the device has been changed no notification
         * will be sent.
         *
         * @return OC_STACK_OK on success
         */
        OCStackResult sendNotification();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /temperature
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

         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.temperature"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/temperature"
        std::string m_var_value_id; // the value for the attribute "id": Instance ID of this specific resource
        std::string m_var_name_id = "id"; // the name for the attribute "id"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The interface set supported by this resource
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": Resource Type
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        double m_var_value_temperature; // the value for the attribute "temperature": Current temperature setting or measurement
        std::string m_var_name_temperature = "temperature"; // the name for the attribute "temperature"
        void* m_var_value_units; // the value for the attribute "units": Units for the temperature value
        std::string m_var_name_units = "units"; // the name for the attribute "units"
        
    protected:
        /*
         * Check if the interface is
         * @param  interface_name the interface name used during the request
         * @return true: updatable interface
         */
        bool in_updatable_interfaces(std::string interface_name);

        /*
         * the entity handler for this resource
         * @param request the incoming request to handle
         * @return OCEntityHandlerResult ok or not ok indication
         */
        virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
};

/*
* Constructor code
*/
TemperatureResource::TemperatureResource(std::string resourceUri)
{
    std::cout << "- Running: TemperatureResource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /temperature
    m_var_value_id = "unique_example_id";  // current value of property "id" Instance ID of this specific resource
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.temperature");
    m_var_value_temperature = 20.0; // current value of property "temperature"  Current temperature setting or measurement
    }

/*
* Destructor code
*/
TemperatureResource::~TemperatureResource() { }

OCStackResult TemperatureResource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&TemperatureResource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register TemperatureResource." << std::endl;
        return result;
    }

    /// add the additional resource types
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0])); a++ )
    {
        result = OCPlatform::bindTypeToResource(m_resourceHandle, m_RESOURCE_TYPE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind resource type:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }
    // add the additional interfaces
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0])); a++)
    {
        result = OCPlatform::bindInterfaceToResource(m_resourceHandle, m_RESOURCE_INTERFACE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind interface:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }

    std::cout << "TemperatureResource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

OCStackResult TemperatureResource::sendNotification(void)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        auto pResponse = std::make_shared<OC::OCResourceResponse>();
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}

/*
* Make the payload for the retrieve function (e.g. GET) /temperature
* @param queries  the query parameters for this call
*/
OCRepresentation TemperatureResource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);
	
	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables

    std::cout << "\t\t" << "property 'id' : "<< m_var_value_id << std::endl;
    std::cout << "\t\t" << "property 'n' : "<< m_var_value_n << std::endl;
    std::cout << "\t\t" << "property 'temperature' : "<< m_var_value_temperature << std::endl;
    
    m_rep.setValue(m_var_name_id, m_var_value_id ); 
    m_rep.setValue(m_var_name_if,  m_var_value_if ); 
    m_rep.setValue(m_var_name_n, m_var_value_n ); 
    m_rep.setValue(m_var_name_rt,  m_var_value_rt ); 
    m_rep.setValue(m_var_name_temperature, m_var_value_temperature ); 

    return m_rep;
}
/*
* Check if the interface name is an registered interface name
*/
bool TemperatureResource::in_updatable_interfaces(std::string interface_name)
{
    for (unsigned int i=0; i < (sizeof(m_IF_UPDATE)/sizeof(m_IF_UPDATE[0])); i++)
    {
        if (m_IF_UPDATE[i].compare(interface_name) == 0)
            return true;
    }
    return false;
}

/*
* the entity handler
*/
OCEntityHandlerResult TemperatureResource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for TemperatureResource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for TemperatureResource, URI is : "
                  << request->getResourceUri() << std::endl;

        // Check for query params (if any)
        QueryParamsMap queries = request->getQueryParameters();
        if (!queries.empty())
        {
            std::cout << "\nQuery processing up to entityHandler" << std::endl;
        }
        for (auto it : queries)
        {
            std::cout << "Query key: " << it.first << " value : " << it.second
                    << std::endl;
        }
        // get the value, so that we can AND it to check which flags are set
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            // request flag is set
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            if(request->getRequestType() == "GET")
            {
                std::cout<<"TemperatureResource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "TemperatureResource unsupported request type (delete,put,..)"
                    << request->getRequestType() << std::endl;
                pResponse->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(pResponse);
                ehResult = OC_EH_ERROR;
            }
        }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // observe flag is set
            ObservationInfo observationInfo = request->getObservationInfo();
            std::cout << "\t\trequestFlag : observer ";
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                std::cout << "register" << std::endl; 
            } 
            else
            {
                std::cout << "unregister" << std::endl;
            }

            if(ObserveAction::ObserveRegister == observationInfo.action)
            {
                // add observer
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if(ObserveAction::ObserveUnregister == observationInfo.action)
            {
                // delete observer
                m_interestedObservers.erase(std::remove(
                                            m_interestedObservers.begin(),
                                            m_interestedObservers.end(),
                                            observationInfo.obsId),
                                            m_interestedObservers.end());
            }
            ehResult = OC_EH_OK;
        }
    }
    return ehResult;
}


/*
 * class definition for class that handles /voltage0
 *
 * This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
 * Retrieves the current energy.

*/
class Voltage0Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Voltage0Resource(std::string resourceUri = "/voltage0");

        /*
         * destructor
         */
         virtual ~Voltage0Resource(void);

        /*
         * Register the resource with the server
         *
         * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
         * setting resourceProperty as OC_OBSERVABLE will allow observation
         * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
         * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
         * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
         * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
         *
         * @param resourceProperty indicates the property of the resource. Defined in octypes.h.
         */
        OCStackResult registerResource(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

        /*
         * Attempt to send out notifications to observing clients
         * if no value on the device has been changed no notification
         * will be sent.
         *
         * @return OC_STACK_OK on success
         */
        OCStackResult sendNotification();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /voltage0
         * This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
         * Retrieves the current energy.

         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.energy.electrical"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/voltage0"
        double m_var_value_current; // the value for the attribute "current": The electric current in Amps (A).
        std::string m_var_name_current = "current"; // the name for the attribute "current"
        double m_var_value_desiredcurrent; // the value for the attribute "desiredcurrent": The desired electric current in Amps (A).
        std::string m_var_name_desiredcurrent = "desiredcurrent"; // the name for the attribute "desiredcurrent"
        double m_var_value_desiredfrequency; // the value for the attribute "desiredfrequency": The desired electric frequency in Hertz (Hz).
        std::string m_var_name_desiredfrequency = "desiredfrequency"; // the name for the attribute "desiredfrequency"
        double m_var_value_desiredvoltage; // the value for the attribute "desiredvoltage": The desired electric voltage in Volts (V).
        std::string m_var_name_desiredvoltage = "desiredvoltage"; // the name for the attribute "desiredvoltage"
        double m_var_value_frequency; // the value for the attribute "frequency": The electric frequency in Hertz (Hz).
        std::string m_var_name_frequency = "frequency"; // the name for the attribute "frequency"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The interface set supported by this resource
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": Resource Type
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        double m_var_value_voltage; // the value for the attribute "voltage": The electric voltage in Volts (V).
        std::string m_var_name_voltage = "voltage"; // the name for the attribute "voltage"
        
    protected:
        /*
         * Check if the interface is
         * @param  interface_name the interface name used during the request
         * @return true: updatable interface
         */
        bool in_updatable_interfaces(std::string interface_name);

        /*
         * the entity handler for this resource
         * @param request the incoming request to handle
         * @return OCEntityHandlerResult ok or not ok indication
         */
        virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
};

/*
* Constructor code
*/
Voltage0Resource::Voltage0Resource(std::string resourceUri)
{
    std::cout << "- Running: Voltage0Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /voltage0
    m_var_value_current = 5.0; // current value of property "current"  The electric current in Amps (A).
    m_var_value_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
    m_var_value_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
    m_var_value_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
    m_var_value_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.energy.electrical");
    m_var_value_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
    }

/*
* Destructor code
*/
Voltage0Resource::~Voltage0Resource() { }

OCStackResult Voltage0Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Voltage0Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Voltage0Resource." << std::endl;
        return result;
    }

    /// add the additional resource types
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0])); a++ )
    {
        result = OCPlatform::bindTypeToResource(m_resourceHandle, m_RESOURCE_TYPE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind resource type:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }
    // add the additional interfaces
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0])); a++)
    {
        result = OCPlatform::bindInterfaceToResource(m_resourceHandle, m_RESOURCE_INTERFACE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind interface:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }

    std::cout << "Voltage0Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

OCStackResult Voltage0Resource::sendNotification(void)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        auto pResponse = std::make_shared<OC::OCResourceResponse>();
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}

/*
* Make the payload for the retrieve function (e.g. GET) /voltage0
* @param queries  the query parameters for this call
*/
OCRepresentation Voltage0Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);
	
	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables

    std::cout << "\t\t" << "property 'current' : "<< m_var_value_current << std::endl;
    std::cout << "\t\t" << "property 'desiredcurrent' : "<< m_var_value_desiredcurrent << std::endl;
    std::cout << "\t\t" << "property 'desiredfrequency' : "<< m_var_value_desiredfrequency << std::endl;
    std::cout << "\t\t" << "property 'desiredvoltage' : "<< m_var_value_desiredvoltage << std::endl;
    std::cout << "\t\t" << "property 'frequency' : "<< m_var_value_frequency << std::endl;
    std::cout << "\t\t" << "property 'n' : "<< m_var_value_n << std::endl;
    std::cout << "\t\t" << "property 'voltage' : "<< m_var_value_voltage << std::endl;
    
    m_rep.setValue(m_var_name_current, m_var_value_current ); 
    m_rep.setValue(m_var_name_desiredcurrent, m_var_value_desiredcurrent ); 
    m_rep.setValue(m_var_name_desiredfrequency, m_var_value_desiredfrequency ); 
    m_rep.setValue(m_var_name_desiredvoltage, m_var_value_desiredvoltage ); 
    m_rep.setValue(m_var_name_frequency, m_var_value_frequency ); 
    m_rep.setValue(m_var_name_if,  m_var_value_if ); 
    m_rep.setValue(m_var_name_n, m_var_value_n ); 
    m_rep.setValue(m_var_name_rt,  m_var_value_rt ); 
    m_rep.setValue(m_var_name_voltage, m_var_value_voltage ); 

    return m_rep;
}
/*
* Check if the interface name is an registered interface name
*/
bool Voltage0Resource::in_updatable_interfaces(std::string interface_name)
{
    for (unsigned int i=0; i < (sizeof(m_IF_UPDATE)/sizeof(m_IF_UPDATE[0])); i++)
    {
        if (m_IF_UPDATE[i].compare(interface_name) == 0)
            return true;
    }
    return false;
}

/*
* the entity handler
*/
OCEntityHandlerResult Voltage0Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Voltage0Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Voltage0Resource, URI is : "
                  << request->getResourceUri() << std::endl;

        // Check for query params (if any)
        QueryParamsMap queries = request->getQueryParameters();
        if (!queries.empty())
        {
            std::cout << "\nQuery processing up to entityHandler" << std::endl;
        }
        for (auto it : queries)
        {
            std::cout << "Query key: " << it.first << " value : " << it.second
                    << std::endl;
        }
        // get the value, so that we can AND it to check which flags are set
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            // request flag is set
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            if(request->getRequestType() == "GET")
            {
                std::cout<<"Voltage0Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Voltage0Resource unsupported request type (delete,put,..)"
                    << request->getRequestType() << std::endl;
                pResponse->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(pResponse);
                ehResult = OC_EH_ERROR;
            }
        }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // observe flag is set
            ObservationInfo observationInfo = request->getObservationInfo();
            std::cout << "\t\trequestFlag : observer ";
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                std::cout << "register" << std::endl; 
            } 
            else
            {
                std::cout << "unregister" << std::endl;
            }

            if(ObserveAction::ObserveRegister == observationInfo.action)
            {
                // add observer
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if(ObserveAction::ObserveUnregister == observationInfo.action)
            {
                // delete observer
                m_interestedObservers.erase(std::remove(
                                            m_interestedObservers.begin(),
                                            m_interestedObservers.end(),
                                            observationInfo.obsId),
                                            m_interestedObservers.end());
            }
            ehResult = OC_EH_OK;
        }
    }
    return ehResult;
}


/*
 * class definition for class that handles /voltage1
 *
 * This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
 * Retrieves the current energy.

*/
class Voltage1Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Voltage1Resource(std::string resourceUri = "/voltage1");

        /*
         * destructor
         */
         virtual ~Voltage1Resource(void);

        /*
         * Register the resource with the server
         *
         * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
         * setting resourceProperty as OC_OBSERVABLE will allow observation
         * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
         * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
         * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
         * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
         *
         * @param resourceProperty indicates the property of the resource. Defined in octypes.h.
         */
        OCStackResult registerResource(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

        /*
         * Attempt to send out notifications to observing clients
         * if no value on the device has been changed no notification
         * will be sent.
         *
         * @return OC_STACK_OK on success
         */
        OCStackResult sendNotification();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /voltage1
         * This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
         * Retrieves the current energy.

         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.energy.electrical"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/voltage1"
        double m_var_value_current; // the value for the attribute "current": The electric current in Amps (A).
        std::string m_var_name_current = "current"; // the name for the attribute "current"
        double m_var_value_desiredcurrent; // the value for the attribute "desiredcurrent": The desired electric current in Amps (A).
        std::string m_var_name_desiredcurrent = "desiredcurrent"; // the name for the attribute "desiredcurrent"
        double m_var_value_desiredfrequency; // the value for the attribute "desiredfrequency": The desired electric frequency in Hertz (Hz).
        std::string m_var_name_desiredfrequency = "desiredfrequency"; // the name for the attribute "desiredfrequency"
        double m_var_value_desiredvoltage; // the value for the attribute "desiredvoltage": The desired electric voltage in Volts (V).
        std::string m_var_name_desiredvoltage = "desiredvoltage"; // the name for the attribute "desiredvoltage"
        double m_var_value_frequency; // the value for the attribute "frequency": The electric frequency in Hertz (Hz).
        std::string m_var_name_frequency = "frequency"; // the name for the attribute "frequency"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The interface set supported by this resource
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": Resource Type
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        double m_var_value_voltage; // the value for the attribute "voltage": The electric voltage in Volts (V).
        std::string m_var_name_voltage = "voltage"; // the name for the attribute "voltage"
        
    protected:
        /*
         * Check if the interface is
         * @param  interface_name the interface name used during the request
         * @return true: updatable interface
         */
        bool in_updatable_interfaces(std::string interface_name);

        /*
         * the entity handler for this resource
         * @param request the incoming request to handle
         * @return OCEntityHandlerResult ok or not ok indication
         */
        virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
};

/*
* Constructor code
*/
Voltage1Resource::Voltage1Resource(std::string resourceUri)
{
    std::cout << "- Running: Voltage1Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /voltage1
    m_var_value_current = 5.0; // current value of property "current"  The electric current in Amps (A).
    m_var_value_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
    m_var_value_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
    m_var_value_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
    m_var_value_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.energy.electrical");
    m_var_value_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
    }

/*
* Destructor code
*/
Voltage1Resource::~Voltage1Resource() { }

OCStackResult Voltage1Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Voltage1Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Voltage1Resource." << std::endl;
        return result;
    }

    /// add the additional resource types
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0])); a++ )
    {
        result = OCPlatform::bindTypeToResource(m_resourceHandle, m_RESOURCE_TYPE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind resource type:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }
    // add the additional interfaces
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0])); a++)
    {
        result = OCPlatform::bindInterfaceToResource(m_resourceHandle, m_RESOURCE_INTERFACE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind interface:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }

    std::cout << "Voltage1Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

OCStackResult Voltage1Resource::sendNotification(void)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        auto pResponse = std::make_shared<OC::OCResourceResponse>();
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}

/*
* Make the payload for the retrieve function (e.g. GET) /voltage1
* @param queries  the query parameters for this call
*/
OCRepresentation Voltage1Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);
	
	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables

    std::cout << "\t\t" << "property 'current' : "<< m_var_value_current << std::endl;
    std::cout << "\t\t" << "property 'desiredcurrent' : "<< m_var_value_desiredcurrent << std::endl;
    std::cout << "\t\t" << "property 'desiredfrequency' : "<< m_var_value_desiredfrequency << std::endl;
    std::cout << "\t\t" << "property 'desiredvoltage' : "<< m_var_value_desiredvoltage << std::endl;
    std::cout << "\t\t" << "property 'frequency' : "<< m_var_value_frequency << std::endl;
    std::cout << "\t\t" << "property 'n' : "<< m_var_value_n << std::endl;
    std::cout << "\t\t" << "property 'voltage' : "<< m_var_value_voltage << std::endl;
    
    m_rep.setValue(m_var_name_current, m_var_value_current ); 
    m_rep.setValue(m_var_name_desiredcurrent, m_var_value_desiredcurrent ); 
    m_rep.setValue(m_var_name_desiredfrequency, m_var_value_desiredfrequency ); 
    m_rep.setValue(m_var_name_desiredvoltage, m_var_value_desiredvoltage ); 
    m_rep.setValue(m_var_name_frequency, m_var_value_frequency ); 
    m_rep.setValue(m_var_name_if,  m_var_value_if ); 
    m_rep.setValue(m_var_name_n, m_var_value_n ); 
    m_rep.setValue(m_var_name_rt,  m_var_value_rt ); 
    m_rep.setValue(m_var_name_voltage, m_var_value_voltage ); 

    return m_rep;
}
/*
* Check if the interface name is an registered interface name
*/
bool Voltage1Resource::in_updatable_interfaces(std::string interface_name)
{
    for (unsigned int i=0; i < (sizeof(m_IF_UPDATE)/sizeof(m_IF_UPDATE[0])); i++)
    {
        if (m_IF_UPDATE[i].compare(interface_name) == 0)
            return true;
    }
    return false;
}

/*
* the entity handler
*/
OCEntityHandlerResult Voltage1Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Voltage1Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Voltage1Resource, URI is : "
                  << request->getResourceUri() << std::endl;

        // Check for query params (if any)
        QueryParamsMap queries = request->getQueryParameters();
        if (!queries.empty())
        {
            std::cout << "\nQuery processing up to entityHandler" << std::endl;
        }
        for (auto it : queries)
        {
            std::cout << "Query key: " << it.first << " value : " << it.second
                    << std::endl;
        }
        // get the value, so that we can AND it to check which flags are set
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            // request flag is set
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            if(request->getRequestType() == "GET")
            {
                std::cout<<"Voltage1Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Voltage1Resource unsupported request type (delete,put,..)"
                    << request->getRequestType() << std::endl;
                pResponse->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(pResponse);
                ehResult = OC_EH_ERROR;
            }
        }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // observe flag is set
            ObservationInfo observationInfo = request->getObservationInfo();
            std::cout << "\t\trequestFlag : observer ";
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                std::cout << "register" << std::endl; 
            } 
            else
            {
                std::cout << "unregister" << std::endl;
            }

            if(ObserveAction::ObserveRegister == observationInfo.action)
            {
                // add observer
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if(ObserveAction::ObserveUnregister == observationInfo.action)
            {
                // delete observer
                m_interestedObservers.erase(std::remove(
                                            m_interestedObservers.begin(),
                                            m_interestedObservers.end(),
                                            observationInfo.obsId),
                                            m_interestedObservers.end());
            }
            ehResult = OC_EH_OK;
        }
    }
    return ehResult;
}


/*
 * class definition for class that handles /voltage2
 *
 * This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
 * Retrieves the current energy.

*/
class Voltage2Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Voltage2Resource(std::string resourceUri = "/voltage2");

        /*
         * destructor
         */
         virtual ~Voltage2Resource(void);

        /*
         * Register the resource with the server
         *
         * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
         * setting resourceProperty as OC_OBSERVABLE will allow observation
         * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
         * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
         * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
         * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
         *
         * @param resourceProperty indicates the property of the resource. Defined in octypes.h.
         */
        OCStackResult registerResource(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

        /*
         * Attempt to send out notifications to observing clients
         * if no value on the device has been changed no notification
         * will be sent.
         *
         * @return OC_STACK_OK on success
         */
        OCStackResult sendNotification();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /voltage2
         * This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
         * Retrieves the current energy.

         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.energy.electrical"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/voltage2"
        double m_var_value_current; // the value for the attribute "current": The electric current in Amps (A).
        std::string m_var_name_current = "current"; // the name for the attribute "current"
        double m_var_value_desiredcurrent; // the value for the attribute "desiredcurrent": The desired electric current in Amps (A).
        std::string m_var_name_desiredcurrent = "desiredcurrent"; // the name for the attribute "desiredcurrent"
        double m_var_value_desiredfrequency; // the value for the attribute "desiredfrequency": The desired electric frequency in Hertz (Hz).
        std::string m_var_name_desiredfrequency = "desiredfrequency"; // the name for the attribute "desiredfrequency"
        double m_var_value_desiredvoltage; // the value for the attribute "desiredvoltage": The desired electric voltage in Volts (V).
        std::string m_var_name_desiredvoltage = "desiredvoltage"; // the name for the attribute "desiredvoltage"
        double m_var_value_frequency; // the value for the attribute "frequency": The electric frequency in Hertz (Hz).
        std::string m_var_name_frequency = "frequency"; // the name for the attribute "frequency"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The interface set supported by this resource
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": Resource Type
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        double m_var_value_voltage; // the value for the attribute "voltage": The electric voltage in Volts (V).
        std::string m_var_name_voltage = "voltage"; // the name for the attribute "voltage"
        
    protected:
        /*
         * Check if the interface is
         * @param  interface_name the interface name used during the request
         * @return true: updatable interface
         */
        bool in_updatable_interfaces(std::string interface_name);

        /*
         * the entity handler for this resource
         * @param request the incoming request to handle
         * @return OCEntityHandlerResult ok or not ok indication
         */
        virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
};

/*
* Constructor code
*/
Voltage2Resource::Voltage2Resource(std::string resourceUri)
{
    std::cout << "- Running: Voltage2Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /voltage2
    m_var_value_current = 5.0; // current value of property "current"  The electric current in Amps (A).
    m_var_value_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
    m_var_value_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
    m_var_value_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
    m_var_value_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.energy.electrical");
    m_var_value_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
    }

/*
* Destructor code
*/
Voltage2Resource::~Voltage2Resource() { }

OCStackResult Voltage2Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Voltage2Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Voltage2Resource." << std::endl;
        return result;
    }

    /// add the additional resource types
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0])); a++ )
    {
        result = OCPlatform::bindTypeToResource(m_resourceHandle, m_RESOURCE_TYPE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind resource type:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }
    // add the additional interfaces
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0])); a++)
    {
        result = OCPlatform::bindInterfaceToResource(m_resourceHandle, m_RESOURCE_INTERFACE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind interface:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }

    std::cout << "Voltage2Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

OCStackResult Voltage2Resource::sendNotification(void)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        auto pResponse = std::make_shared<OC::OCResourceResponse>();
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}

/*
* Make the payload for the retrieve function (e.g. GET) /voltage2
* @param queries  the query parameters for this call
*/
OCRepresentation Voltage2Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);
	
	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables

    std::cout << "\t\t" << "property 'current' : "<< m_var_value_current << std::endl;
    std::cout << "\t\t" << "property 'desiredcurrent' : "<< m_var_value_desiredcurrent << std::endl;
    std::cout << "\t\t" << "property 'desiredfrequency' : "<< m_var_value_desiredfrequency << std::endl;
    std::cout << "\t\t" << "property 'desiredvoltage' : "<< m_var_value_desiredvoltage << std::endl;
    std::cout << "\t\t" << "property 'frequency' : "<< m_var_value_frequency << std::endl;
    std::cout << "\t\t" << "property 'n' : "<< m_var_value_n << std::endl;
    std::cout << "\t\t" << "property 'voltage' : "<< m_var_value_voltage << std::endl;
    
    m_rep.setValue(m_var_name_current, m_var_value_current ); 
    m_rep.setValue(m_var_name_desiredcurrent, m_var_value_desiredcurrent ); 
    m_rep.setValue(m_var_name_desiredfrequency, m_var_value_desiredfrequency ); 
    m_rep.setValue(m_var_name_desiredvoltage, m_var_value_desiredvoltage ); 
    m_rep.setValue(m_var_name_frequency, m_var_value_frequency ); 
    m_rep.setValue(m_var_name_if,  m_var_value_if ); 
    m_rep.setValue(m_var_name_n, m_var_value_n ); 
    m_rep.setValue(m_var_name_rt,  m_var_value_rt ); 
    m_rep.setValue(m_var_name_voltage, m_var_value_voltage ); 

    return m_rep;
}
/*
* Check if the interface name is an registered interface name
*/
bool Voltage2Resource::in_updatable_interfaces(std::string interface_name)
{
    for (unsigned int i=0; i < (sizeof(m_IF_UPDATE)/sizeof(m_IF_UPDATE[0])); i++)
    {
        if (m_IF_UPDATE[i].compare(interface_name) == 0)
            return true;
    }
    return false;
}

/*
* the entity handler
*/
OCEntityHandlerResult Voltage2Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Voltage2Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Voltage2Resource, URI is : "
                  << request->getResourceUri() << std::endl;

        // Check for query params (if any)
        QueryParamsMap queries = request->getQueryParameters();
        if (!queries.empty())
        {
            std::cout << "\nQuery processing up to entityHandler" << std::endl;
        }
        for (auto it : queries)
        {
            std::cout << "Query key: " << it.first << " value : " << it.second
                    << std::endl;
        }
        // get the value, so that we can AND it to check which flags are set
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            // request flag is set
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            if(request->getRequestType() == "GET")
            {
                std::cout<<"Voltage2Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Voltage2Resource unsupported request type (delete,put,..)"
                    << request->getRequestType() << std::endl;
                pResponse->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(pResponse);
                ehResult = OC_EH_ERROR;
            }
        }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // observe flag is set
            ObservationInfo observationInfo = request->getObservationInfo();
            std::cout << "\t\trequestFlag : observer ";
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                std::cout << "register" << std::endl; 
            } 
            else
            {
                std::cout << "unregister" << std::endl;
            }

            if(ObserveAction::ObserveRegister == observationInfo.action)
            {
                // add observer
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if(ObserveAction::ObserveUnregister == observationInfo.action)
            {
                // delete observer
                m_interestedObservers.erase(std::remove(
                                            m_interestedObservers.begin(),
                                            m_interestedObservers.end(),
                                            observationInfo.obsId),
                                            m_interestedObservers.end());
            }
            ehResult = OC_EH_OK;
        }
    }
    return ehResult;
}


/*
 * class definition for class that handles /voltage3
 *
 * This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
 * Retrieves the current energy.

*/
class Voltage3Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Voltage3Resource(std::string resourceUri = "/voltage3");

        /*
         * destructor
         */
         virtual ~Voltage3Resource(void);

        /*
         * Register the resource with the server
         *
         * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
         * setting resourceProperty as OC_OBSERVABLE will allow observation
         * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
         * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
         * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
         * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
         *
         * @param resourceProperty indicates the property of the resource. Defined in octypes.h.
         */
        OCStackResult registerResource(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

        /*
         * Attempt to send out notifications to observing clients
         * if no value on the device has been changed no notification
         * will be sent.
         *
         * @return OC_STACK_OK on success
         */
        OCStackResult sendNotification();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /voltage3
         * This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
         * Retrieves the current energy.

         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.energy.electrical"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/voltage3"
        double m_var_value_current; // the value for the attribute "current": The electric current in Amps (A).
        std::string m_var_name_current = "current"; // the name for the attribute "current"
        double m_var_value_desiredcurrent; // the value for the attribute "desiredcurrent": The desired electric current in Amps (A).
        std::string m_var_name_desiredcurrent = "desiredcurrent"; // the name for the attribute "desiredcurrent"
        double m_var_value_desiredfrequency; // the value for the attribute "desiredfrequency": The desired electric frequency in Hertz (Hz).
        std::string m_var_name_desiredfrequency = "desiredfrequency"; // the name for the attribute "desiredfrequency"
        double m_var_value_desiredvoltage; // the value for the attribute "desiredvoltage": The desired electric voltage in Volts (V).
        std::string m_var_name_desiredvoltage = "desiredvoltage"; // the name for the attribute "desiredvoltage"
        double m_var_value_frequency; // the value for the attribute "frequency": The electric frequency in Hertz (Hz).
        std::string m_var_name_frequency = "frequency"; // the name for the attribute "frequency"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The interface set supported by this resource
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": Resource Type
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        double m_var_value_voltage; // the value for the attribute "voltage": The electric voltage in Volts (V).
        std::string m_var_name_voltage = "voltage"; // the name for the attribute "voltage"
        
    protected:
        /*
         * Check if the interface is
         * @param  interface_name the interface name used during the request
         * @return true: updatable interface
         */
        bool in_updatable_interfaces(std::string interface_name);

        /*
         * the entity handler for this resource
         * @param request the incoming request to handle
         * @return OCEntityHandlerResult ok or not ok indication
         */
        virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
};

/*
* Constructor code
*/
Voltage3Resource::Voltage3Resource(std::string resourceUri)
{
    std::cout << "- Running: Voltage3Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /voltage3
    m_var_value_current = 5.0; // current value of property "current"  The electric current in Amps (A).
    m_var_value_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
    m_var_value_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
    m_var_value_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
    m_var_value_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.energy.electrical");
    m_var_value_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
    }

/*
* Destructor code
*/
Voltage3Resource::~Voltage3Resource() { }

OCStackResult Voltage3Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Voltage3Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Voltage3Resource." << std::endl;
        return result;
    }

    /// add the additional resource types
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0])); a++ )
    {
        result = OCPlatform::bindTypeToResource(m_resourceHandle, m_RESOURCE_TYPE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind resource type:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }
    // add the additional interfaces
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0])); a++)
    {
        result = OCPlatform::bindInterfaceToResource(m_resourceHandle, m_RESOURCE_INTERFACE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind interface:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }

    std::cout << "Voltage3Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

OCStackResult Voltage3Resource::sendNotification(void)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        auto pResponse = std::make_shared<OC::OCResourceResponse>();
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}

/*
* Make the payload for the retrieve function (e.g. GET) /voltage3
* @param queries  the query parameters for this call
*/
OCRepresentation Voltage3Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);
	
	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables

    std::cout << "\t\t" << "property 'current' : "<< m_var_value_current << std::endl;
    std::cout << "\t\t" << "property 'desiredcurrent' : "<< m_var_value_desiredcurrent << std::endl;
    std::cout << "\t\t" << "property 'desiredfrequency' : "<< m_var_value_desiredfrequency << std::endl;
    std::cout << "\t\t" << "property 'desiredvoltage' : "<< m_var_value_desiredvoltage << std::endl;
    std::cout << "\t\t" << "property 'frequency' : "<< m_var_value_frequency << std::endl;
    std::cout << "\t\t" << "property 'n' : "<< m_var_value_n << std::endl;
    std::cout << "\t\t" << "property 'voltage' : "<< m_var_value_voltage << std::endl;
    
    m_rep.setValue(m_var_name_current, m_var_value_current ); 
    m_rep.setValue(m_var_name_desiredcurrent, m_var_value_desiredcurrent ); 
    m_rep.setValue(m_var_name_desiredfrequency, m_var_value_desiredfrequency ); 
    m_rep.setValue(m_var_name_desiredvoltage, m_var_value_desiredvoltage ); 
    m_rep.setValue(m_var_name_frequency, m_var_value_frequency ); 
    m_rep.setValue(m_var_name_if,  m_var_value_if ); 
    m_rep.setValue(m_var_name_n, m_var_value_n ); 
    m_rep.setValue(m_var_name_rt,  m_var_value_rt ); 
    m_rep.setValue(m_var_name_voltage, m_var_value_voltage ); 

    return m_rep;
}
/*
* Check if the interface name is an registered interface name
*/
bool Voltage3Resource::in_updatable_interfaces(std::string interface_name)
{
    for (unsigned int i=0; i < (sizeof(m_IF_UPDATE)/sizeof(m_IF_UPDATE[0])); i++)
    {
        if (m_IF_UPDATE[i].compare(interface_name) == 0)
            return true;
    }
    return false;
}

/*
* the entity handler
*/
OCEntityHandlerResult Voltage3Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Voltage3Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Voltage3Resource, URI is : "
                  << request->getResourceUri() << std::endl;

        // Check for query params (if any)
        QueryParamsMap queries = request->getQueryParameters();
        if (!queries.empty())
        {
            std::cout << "\nQuery processing up to entityHandler" << std::endl;
        }
        for (auto it : queries)
        {
            std::cout << "Query key: " << it.first << " value : " << it.second
                    << std::endl;
        }
        // get the value, so that we can AND it to check which flags are set
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            // request flag is set
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            if(request->getRequestType() == "GET")
            {
                std::cout<<"Voltage3Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Voltage3Resource unsupported request type (delete,put,..)"
                    << request->getRequestType() << std::endl;
                pResponse->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(pResponse);
                ehResult = OC_EH_ERROR;
            }
        }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // observe flag is set
            ObservationInfo observationInfo = request->getObservationInfo();
            std::cout << "\t\trequestFlag : observer ";
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                std::cout << "register" << std::endl; 
            } 
            else
            {
                std::cout << "unregister" << std::endl;
            }

            if(ObserveAction::ObserveRegister == observationInfo.action)
            {
                // add observer
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if(ObserveAction::ObserveUnregister == observationInfo.action)
            {
                // delete observer
                m_interestedObservers.erase(std::remove(
                                            m_interestedObservers.begin(),
                                            m_interestedObservers.end(),
                                            observationInfo.obsId),
                                            m_interestedObservers.end());
            }
            ehResult = OC_EH_OK;
        }
    }
    return ehResult;
}


/*
 * class definition for class that handles /xmotion
 *
 * This resource provides a measure of proper acceleration (g force) as opposed to co-ordinate acceleration
 * (which is dependent on the co-ordinate system and the observer).
 * The value is a float which describes the acceleration experienced by the object in "g".

*/
class XmotionResource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        XmotionResource(std::string resourceUri = "/xmotion");

        /*
         * destructor
         */
         virtual ~XmotionResource(void);

        /*
         * Register the resource with the server
         *
         * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
         * setting resourceProperty as OC_OBSERVABLE will allow observation
         * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
         * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
         * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
         * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
         *
         * @param resourceProperty indicates the property of the resource. Defined in octypes.h.
         */
        OCStackResult registerResource(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

        /*
         * Attempt to send out notifications to observing clients
         * if no value on the device has been changed no notification
         * will be sent.
         *
         * @return OC_STACK_OK on success
         */
        OCStackResult sendNotification();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /xmotion
         * This resource provides a measure of proper acceleration (g force) as opposed to co-ordinate acceleration
         * (which is dependent on the co-ordinate system and the observer).
         * The value is a float which describes the acceleration experienced by the object in "g".

         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.sensor.acceleration"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/xmotion"
        double m_var_value_acceleration; // the value for the attribute "acceleration": sensed acceleration experienced in 'g'.
        std::string m_var_name_acceleration = "acceleration"; // the name for the attribute "acceleration"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The interface set supported by this resource
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": Resource Type
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        
    protected:
        /*
         * Check if the interface is
         * @param  interface_name the interface name used during the request
         * @return true: updatable interface
         */
        bool in_updatable_interfaces(std::string interface_name);

        /*
         * the entity handler for this resource
         * @param request the incoming request to handle
         * @return OCEntityHandlerResult ok or not ok indication
         */
        virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
};

/*
* Constructor code
*/
XmotionResource::XmotionResource(std::string resourceUri)
{
    std::cout << "- Running: XmotionResource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /xmotion
    m_var_value_acceleration = 0.5; // current value of property "acceleration"  sensed acceleration experienced in 'g'.
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.sensor.acceleration");
    }

/*
* Destructor code
*/
XmotionResource::~XmotionResource() { }

OCStackResult XmotionResource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&XmotionResource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register XmotionResource." << std::endl;
        return result;
    }

    /// add the additional resource types
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0])); a++ )
    {
        result = OCPlatform::bindTypeToResource(m_resourceHandle, m_RESOURCE_TYPE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind resource type:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }
    // add the additional interfaces
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0])); a++)
    {
        result = OCPlatform::bindInterfaceToResource(m_resourceHandle, m_RESOURCE_INTERFACE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind interface:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }

    std::cout << "XmotionResource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

OCStackResult XmotionResource::sendNotification(void)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        auto pResponse = std::make_shared<OC::OCResourceResponse>();
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}

/*
* Make the payload for the retrieve function (e.g. GET) /xmotion
* @param queries  the query parameters for this call
*/
OCRepresentation XmotionResource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);
	
	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables

    std::cout << "\t\t" << "property 'acceleration' : "<< m_var_value_acceleration << std::endl;
    std::cout << "\t\t" << "property 'n' : "<< m_var_value_n << std::endl;
    
    m_rep.setValue(m_var_name_acceleration, m_var_value_acceleration ); 
    m_rep.setValue(m_var_name_if,  m_var_value_if ); 
    m_rep.setValue(m_var_name_n, m_var_value_n ); 
    m_rep.setValue(m_var_name_rt,  m_var_value_rt ); 

    return m_rep;
}
/*
* Check if the interface name is an registered interface name
*/
bool XmotionResource::in_updatable_interfaces(std::string interface_name)
{
    for (unsigned int i=0; i < (sizeof(m_IF_UPDATE)/sizeof(m_IF_UPDATE[0])); i++)
    {
        if (m_IF_UPDATE[i].compare(interface_name) == 0)
            return true;
    }
    return false;
}

/*
* the entity handler
*/
OCEntityHandlerResult XmotionResource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for XmotionResource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for XmotionResource, URI is : "
                  << request->getResourceUri() << std::endl;

        // Check for query params (if any)
        QueryParamsMap queries = request->getQueryParameters();
        if (!queries.empty())
        {
            std::cout << "\nQuery processing up to entityHandler" << std::endl;
        }
        for (auto it : queries)
        {
            std::cout << "Query key: " << it.first << " value : " << it.second
                    << std::endl;
        }
        // get the value, so that we can AND it to check which flags are set
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            // request flag is set
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            if(request->getRequestType() == "GET")
            {
                std::cout<<"XmotionResource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "XmotionResource unsupported request type (delete,put,..)"
                    << request->getRequestType() << std::endl;
                pResponse->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(pResponse);
                ehResult = OC_EH_ERROR;
            }
        }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // observe flag is set
            ObservationInfo observationInfo = request->getObservationInfo();
            std::cout << "\t\trequestFlag : observer ";
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                std::cout << "register" << std::endl; 
            } 
            else
            {
                std::cout << "unregister" << std::endl;
            }

            if(ObserveAction::ObserveRegister == observationInfo.action)
            {
                // add observer
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if(ObserveAction::ObserveUnregister == observationInfo.action)
            {
                // delete observer
                m_interestedObservers.erase(std::remove(
                                            m_interestedObservers.begin(),
                                            m_interestedObservers.end(),
                                            observationInfo.obsId),
                                            m_interestedObservers.end());
            }
            ehResult = OC_EH_OK;
        }
    }
    return ehResult;
}


/*
 * class definition for class that handles /ymotion
 *
 * This resource provides a measure of proper acceleration (g force) as opposed to co-ordinate acceleration
 * (which is dependent on the co-ordinate system and the observer).
 * The value is a float which describes the acceleration experienced by the object in "g".

*/
class YmotionResource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        YmotionResource(std::string resourceUri = "/ymotion");

        /*
         * destructor
         */
         virtual ~YmotionResource(void);

        /*
         * Register the resource with the server
         *
         * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
         * setting resourceProperty as OC_OBSERVABLE will allow observation
         * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
         * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
         * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
         * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
         *
         * @param resourceProperty indicates the property of the resource. Defined in octypes.h.
         */
        OCStackResult registerResource(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

        /*
         * Attempt to send out notifications to observing clients
         * if no value on the device has been changed no notification
         * will be sent.
         *
         * @return OC_STACK_OK on success
         */
        OCStackResult sendNotification();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /ymotion
         * This resource provides a measure of proper acceleration (g force) as opposed to co-ordinate acceleration
         * (which is dependent on the co-ordinate system and the observer).
         * The value is a float which describes the acceleration experienced by the object in "g".

         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.sensor.acceleration"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/ymotion"
        double m_var_value_acceleration; // the value for the attribute "acceleration": sensed acceleration experienced in 'g'.
        std::string m_var_name_acceleration = "acceleration"; // the name for the attribute "acceleration"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The interface set supported by this resource
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": Resource Type
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        
    protected:
        /*
         * Check if the interface is
         * @param  interface_name the interface name used during the request
         * @return true: updatable interface
         */
        bool in_updatable_interfaces(std::string interface_name);

        /*
         * the entity handler for this resource
         * @param request the incoming request to handle
         * @return OCEntityHandlerResult ok or not ok indication
         */
        virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
};

/*
* Constructor code
*/
YmotionResource::YmotionResource(std::string resourceUri)
{
    std::cout << "- Running: YmotionResource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /ymotion
    m_var_value_acceleration = 0.5; // current value of property "acceleration"  sensed acceleration experienced in 'g'.
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.sensor.acceleration");
    }

/*
* Destructor code
*/
YmotionResource::~YmotionResource() { }

OCStackResult YmotionResource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&YmotionResource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register YmotionResource." << std::endl;
        return result;
    }

    /// add the additional resource types
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0])); a++ )
    {
        result = OCPlatform::bindTypeToResource(m_resourceHandle, m_RESOURCE_TYPE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind resource type:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }
    // add the additional interfaces
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0])); a++)
    {
        result = OCPlatform::bindInterfaceToResource(m_resourceHandle, m_RESOURCE_INTERFACE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind interface:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }

    std::cout << "YmotionResource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

OCStackResult YmotionResource::sendNotification(void)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        auto pResponse = std::make_shared<OC::OCResourceResponse>();
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}

/*
* Make the payload for the retrieve function (e.g. GET) /ymotion
* @param queries  the query parameters for this call
*/
OCRepresentation YmotionResource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);
	
	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables

    std::cout << "\t\t" << "property 'acceleration' : "<< m_var_value_acceleration << std::endl;
    std::cout << "\t\t" << "property 'n' : "<< m_var_value_n << std::endl;
    
    m_rep.setValue(m_var_name_acceleration, m_var_value_acceleration ); 
    m_rep.setValue(m_var_name_if,  m_var_value_if ); 
    m_rep.setValue(m_var_name_n, m_var_value_n ); 
    m_rep.setValue(m_var_name_rt,  m_var_value_rt ); 

    return m_rep;
}
/*
* Check if the interface name is an registered interface name
*/
bool YmotionResource::in_updatable_interfaces(std::string interface_name)
{
    for (unsigned int i=0; i < (sizeof(m_IF_UPDATE)/sizeof(m_IF_UPDATE[0])); i++)
    {
        if (m_IF_UPDATE[i].compare(interface_name) == 0)
            return true;
    }
    return false;
}

/*
* the entity handler
*/
OCEntityHandlerResult YmotionResource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for YmotionResource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for YmotionResource, URI is : "
                  << request->getResourceUri() << std::endl;

        // Check for query params (if any)
        QueryParamsMap queries = request->getQueryParameters();
        if (!queries.empty())
        {
            std::cout << "\nQuery processing up to entityHandler" << std::endl;
        }
        for (auto it : queries)
        {
            std::cout << "Query key: " << it.first << " value : " << it.second
                    << std::endl;
        }
        // get the value, so that we can AND it to check which flags are set
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            // request flag is set
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            if(request->getRequestType() == "GET")
            {
                std::cout<<"YmotionResource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "YmotionResource unsupported request type (delete,put,..)"
                    << request->getRequestType() << std::endl;
                pResponse->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(pResponse);
                ehResult = OC_EH_ERROR;
            }
        }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // observe flag is set
            ObservationInfo observationInfo = request->getObservationInfo();
            std::cout << "\t\trequestFlag : observer ";
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                std::cout << "register" << std::endl; 
            } 
            else
            {
                std::cout << "unregister" << std::endl;
            }

            if(ObserveAction::ObserveRegister == observationInfo.action)
            {
                // add observer
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if(ObserveAction::ObserveUnregister == observationInfo.action)
            {
                // delete observer
                m_interestedObservers.erase(std::remove(
                                            m_interestedObservers.begin(),
                                            m_interestedObservers.end(),
                                            observationInfo.obsId),
                                            m_interestedObservers.end());
            }
            ehResult = OC_EH_OK;
        }
    }
    return ehResult;
}


/*
 * class definition for class that handles /zmotion
 *
 * This resource provides a measure of proper acceleration (g force) as opposed to co-ordinate acceleration
 * (which is dependent on the co-ordinate system and the observer).
 * The value is a float which describes the acceleration experienced by the object in "g".

*/
class ZmotionResource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        ZmotionResource(std::string resourceUri = "/zmotion");

        /*
         * destructor
         */
         virtual ~ZmotionResource(void);

        /*
         * Register the resource with the server
         *
         * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
         * setting resourceProperty as OC_OBSERVABLE will allow observation
         * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
         * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
         * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
         * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
         *
         * @param resourceProperty indicates the property of the resource. Defined in octypes.h.
         */
        OCStackResult registerResource(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

        /*
         * Attempt to send out notifications to observing clients
         * if no value on the device has been changed no notification
         * will be sent.
         *
         * @return OC_STACK_OK on success
         */
        OCStackResult sendNotification();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /zmotion
         * This resource provides a measure of proper acceleration (g force) as opposed to co-ordinate acceleration
         * (which is dependent on the co-ordinate system and the observer).
         * The value is a float which describes the acceleration experienced by the object in "g".

         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.sensor.acceleration"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/zmotion"
        double m_var_value_acceleration; // the value for the attribute "acceleration": sensed acceleration experienced in 'g'.
        std::string m_var_name_acceleration = "acceleration"; // the name for the attribute "acceleration"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The interface set supported by this resource
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": Resource Type
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        
    protected:
        /*
         * Check if the interface is
         * @param  interface_name the interface name used during the request
         * @return true: updatable interface
         */
        bool in_updatable_interfaces(std::string interface_name);

        /*
         * the entity handler for this resource
         * @param request the incoming request to handle
         * @return OCEntityHandlerResult ok or not ok indication
         */
        virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
};

/*
* Constructor code
*/
ZmotionResource::ZmotionResource(std::string resourceUri)
{
    std::cout << "- Running: ZmotionResource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /zmotion
    m_var_value_acceleration = 0.5; // current value of property "acceleration"  sensed acceleration experienced in 'g'.
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.sensor.acceleration");
    }

/*
* Destructor code
*/
ZmotionResource::~ZmotionResource() { }

OCStackResult ZmotionResource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&ZmotionResource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register ZmotionResource." << std::endl;
        return result;
    }

    /// add the additional resource types
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0])); a++ )
    {
        result = OCPlatform::bindTypeToResource(m_resourceHandle, m_RESOURCE_TYPE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind resource type:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }
    // add the additional interfaces
    for( unsigned int a = 1; a < (sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0])); a++)
    {
        result = OCPlatform::bindInterfaceToResource(m_resourceHandle, m_RESOURCE_INTERFACE[a].c_str());
        if(OC_STACK_OK != result)
        {
            std::cerr << "Could not bind interface:" << m_RESOURCE_INTERFACE[a] << std::endl;
            return result;
        }
    }

    std::cout << "ZmotionResource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

OCStackResult ZmotionResource::sendNotification(void)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        auto pResponse = std::make_shared<OC::OCResourceResponse>();
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}

/*
* Make the payload for the retrieve function (e.g. GET) /zmotion
* @param queries  the query parameters for this call
*/
OCRepresentation ZmotionResource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);
	
	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables

    std::cout << "\t\t" << "property 'acceleration' : "<< m_var_value_acceleration << std::endl;
    std::cout << "\t\t" << "property 'n' : "<< m_var_value_n << std::endl;
    
    m_rep.setValue(m_var_name_acceleration, m_var_value_acceleration ); 
    m_rep.setValue(m_var_name_if,  m_var_value_if ); 
    m_rep.setValue(m_var_name_n, m_var_value_n ); 
    m_rep.setValue(m_var_name_rt,  m_var_value_rt ); 

    return m_rep;
}
/*
* Check if the interface name is an registered interface name
*/
bool ZmotionResource::in_updatable_interfaces(std::string interface_name)
{
    for (unsigned int i=0; i < (sizeof(m_IF_UPDATE)/sizeof(m_IF_UPDATE[0])); i++)
    {
        if (m_IF_UPDATE[i].compare(interface_name) == 0)
            return true;
    }
    return false;
}

/*
* the entity handler
*/
OCEntityHandlerResult ZmotionResource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for ZmotionResource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for ZmotionResource, URI is : "
                  << request->getResourceUri() << std::endl;

        // Check for query params (if any)
        QueryParamsMap queries = request->getQueryParameters();
        if (!queries.empty())
        {
            std::cout << "\nQuery processing up to entityHandler" << std::endl;
        }
        for (auto it : queries)
        {
            std::cout << "Query key: " << it.first << " value : " << it.second
                    << std::endl;
        }
        // get the value, so that we can AND it to check which flags are set
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            // request flag is set
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            if(request->getRequestType() == "GET")
            {
                std::cout<<"ZmotionResource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "ZmotionResource unsupported request type (delete,put,..)"
                    << request->getRequestType() << std::endl;
                pResponse->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(pResponse);
                ehResult = OC_EH_ERROR;
            }
        }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // observe flag is set
            ObservationInfo observationInfo = request->getObservationInfo();
            std::cout << "\t\trequestFlag : observer ";
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                std::cout << "register" << std::endl; 
            } 
            else
            {
                std::cout << "unregister" << std::endl;
            }

            if(ObserveAction::ObserveRegister == observationInfo.action)
            {
                // add observer
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if(ObserveAction::ObserveUnregister == observationInfo.action)
            {
                // delete observer
                m_interestedObservers.erase(std::remove(
                                            m_interestedObservers.begin(),
                                            m_interestedObservers.end(),
                                            observationInfo.obsId),
                                            m_interestedObservers.end());
            }
            ehResult = OC_EH_OK;
        }
    }
    return ehResult;
}



class IoTServer
{
    public:
        /**
         *  constructor
         *  creates all resources from the resource classes.
         */
        IoTServer();

        /**
         *  destructor
         */
        ~IoTServer();

        /*
         * Register the resources with the server
         *
         * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
         * setting resourceProperty as OC_OBSERVABLE will allow observation
         * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
         * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
         * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
         * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
         *
         * @param resourceProperty indicates the property of the resources. Defined in octypes.h.
         */
        OCStackResult registerResources(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

    private:
        BinaryswitchResource  m_binaryswitchInstance;
        BrightnessResource  m_brightnessInstance;
        ColourrgbResource  m_colourRGBInstance;
        HeadingResource  m_headingInstance;
        PressureResource  m_pressureInstance;
        TemperatureResource  m_temperatureInstance;
        Voltage0Resource  m_voltage0Instance;
        Voltage1Resource  m_voltage1Instance;
        Voltage2Resource  m_voltage2Instance;
        Voltage3Resource  m_voltage3Instance;
        XmotionResource  m_xmotionInstance;
        YmotionResource  m_ymotionInstance;
        ZmotionResource  m_zmotionInstance;};

IoTServer::IoTServer()
    :m_binaryswitchInstance(),
     m_brightnessInstance(),
     m_colourRGBInstance(),
     m_headingInstance(),
     m_pressureInstance(),
     m_temperatureInstance(),
     m_voltage0Instance(),
     m_voltage1Instance(),
     m_voltage2Instance(),
     m_voltage3Instance(),
     m_xmotionInstance(),
     m_ymotionInstance(),
     m_zmotionInstance()
{
    std::cout << "Running IoTServer constructor" << std::endl;
}

IoTServer::~IoTServer()
{
    std::cout << "Running IoTServer destructor" << std::endl;
}

OCStackResult IoTServer::registerResources(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    result = m_binaryswitchInstance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_brightnessInstance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_colourRGBInstance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_headingInstance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_pressureInstance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_temperatureInstance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_voltage0Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_voltage1Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_voltage2Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_voltage3Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_xmotionInstance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_ymotionInstance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_zmotionInstance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }return result;
}

class Platform
{
public:
    /*
     * Platform constructor
     */
    Platform(void);

    /*
     * Platform destructor
     */
    virtual ~Platform();

    /*
     * Start the platform
     */
    OCStackResult start();

    /*
     * Register all platform info
     * This will register the platformId, manufaturerName, manufacturerUrl,
     * modelNumber, dateOfManufacture, platformVersion, operatingSystemVersion,
     * hardwareVersion, firmwareVersion, supportUrl, and systemTime
     */
    OCStackResult registerPlatformInfo(void);

    /*
     * Get OCPlatformInfo pointer
     */
    OCPlatformInfo* getPlatformInfo(void);

    /*
     * Stop the platform
     */
    OCStackResult stop(void);

    /*
     * SetDeviceInfo
     * Sets the device information ("/oic/d")
     *
     * @return OC_STACK_OK on success OC_STACK_ERROR on failure
     */
    OCStackResult setDeviceInfo(void);

    // Set of strings for each of device info fields
    std::string  deviceName = "Binary Switch";
    std::string  deviceType = "oic.d.light";
    std::string  specVersion = "ocf.1.0.0";
    std::vector<std::string> dataModelVersions;

    std::string  protocolIndependentID = "fa008167-3bbf-4c9d-8604-c9bcb96cb712";

private:
    // Set of strings for each of platform Info fields
    std::string m_platformId = "0A3E0D6F-DBF5-404E-8719-D6880042463A";
    std::string m_manufacturerName = "ocf";
    std::string m_manufacturerLink = "https://ocf.org/";
    std::string m_modelNumber = "ModelNumber";
    std::string m_dateOfManufacture = "2017-12-01";
    std::string m_platformVersion = "1.0";
    std::string m_operatingSystemVersion = "myOS";
    std::string m_hardwareVersion = "1.0";
    std::string m_firmwareVersion = "1.0";
    std::string m_supportLink = "https://ocf.org/";
    std::string m_systemTime = "2017-12-01T12:00:00.52Z";

    /*
    * duplicateString
    *
    * @param targetString  destination string, will be allocated
    * @param sourceString  source string, e.g. will be copied
    */
    void duplicateString(char ** targetString, std::string sourceString);

    /**
     *  SetPlatformInfo
     *  Sets the platform information ("oic/p")
     *
     * @param platformID the platformID
     * @param manufacturerName the manufacturerName
     * @param manufacturerUrl the manufacturerUrl
     * @param modelNumber the modelNumber
     * @param platformVersion the platformVersion
     * @param operatingSystemVersion the operatingSystemVersion
     * @param hardwareVersion the hardwareVersion
     * @param firmwareVersion the firmwareVersion
     * @param supportUrl the supportUrl
     * @param systemTime the systemTime
     * @return OC_STACK_ERROR or OC_STACK_OK
     */
    void setPlatformInfo(std::string platformID,
                         std::string manufacturerName,
                         std::string manufacturerUrl,
                         std::string modelNumber,
                         std::string dateOfManufacture,
                         std::string platformVersion,
                         std::string operatingSystemVersion,
                         std::string hardwareVersion,
                         std::string firmwareVersion,
                         std::string supportUrl,
                         std::string systemTime);

    /*
     * deletePlatformInfo
     * Deleted the allocated platform information
     */
    void deletePlatformInfo(void);

    // OCPlatformInfo Contains all the platform info
    OCPlatformInfo platformInfo;
};

/**
*  server_fopen
*  opens file
*  implements redirection to open:
* - initial security settings
* - introspection file
* @param path path+filename of the file to open
* @param mode mode of the file to open
* @return the filehandle of the opened file (or error)
*/
FILE* server_fopen(const char* path, const char* mode)
{
    FILE* fileptr = NULL;

    if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        // reading the security initial setup file
        fileptr = fopen("server_security.dat", mode);
        std::cout << "reading security file 'server_security.dat' ptr: " << fileptr << std::endl;
        return fileptr;
    }
    else if (0 == strcmp(path, OC_INTROSPECTION_FILE_NAME))
    {
        // reading the introspection file
        fileptr = fopen("server_introspection.dat", mode);
        std::cout << "reading introspection file  'server_introspection.dat' ptr: " << fileptr << std::endl;
        return fileptr;
    }
    else
    {
        std::cout << "persistent storage - server_fopen: " << path << std::endl;
        return fopen(path, mode);
    }
}

// Create persistent storage handlers
OCPersistentStorage ps{server_fopen, fread, fwrite, fclose, unlink};

/*
* Platform Constructor
*/
Platform::Platform(void)
{
    std::cout << "Running Platform constructor" << std::endl;
    dataModelVersions.push_back("ocf.res.1.3.0");
    dataModelVersions.push_back("ocf.sh.1.3.0");

    // create the platform
    PlatformConfig cfg
    {
        ServiceType::InProc,
        ModeType::Server,
        &ps
    };
    OCPlatform::Configure(cfg);
    setPlatformInfo(m_platformId, m_manufacturerName, m_manufacturerLink,
                    m_modelNumber, m_dateOfManufacture, m_platformVersion,
                    m_operatingSystemVersion, m_hardwareVersion,
                    m_firmwareVersion, m_supportLink, m_systemTime);
}

/*
* Platform Destructor
*/
Platform::~Platform(void)
{
    std::cout << "Running Platform destructor" << std::endl;
    deletePlatformInfo();
}

OCStackResult Platform::start(void)
{
    return OCPlatform::start();
}

OCStackResult Platform::registerPlatformInfo(void)
{
    OCStackResult result = OC_STACK_ERROR;
    result = OCPlatform::registerPlatformInfo(platformInfo);
    return result;
}

OCPlatformInfo* Platform::getPlatformInfo(void)
{
    return &platformInfo;
}

OCStackResult Platform::stop(void)
{
    return OCPlatform::stop();
}

void Platform::duplicateString(char ** targetString, std::string sourceString)
{
    *targetString = new char[sourceString.length() + 1];
    strncpy(*targetString, sourceString.c_str(), (sourceString.length() + 1));
}

void Platform::setPlatformInfo(std::string platformID,
                               std::string manufacturerName,
                               std::string manufacturerUrl,
                               std::string modelNumber,
                               std::string dateOfManufacture,
                               std::string platformVersion,
                               std::string operatingSystemVersion,
                               std::string hardwareVersion,
                               std::string firmwareVersion,
                               std::string supportUrl,
                               std::string systemTime)
{
    duplicateString(&platformInfo.platformID, platformID);
    duplicateString(&platformInfo.manufacturerName, manufacturerName);
    duplicateString(&platformInfo.manufacturerUrl, manufacturerUrl);
    duplicateString(&platformInfo.modelNumber, modelNumber);
    duplicateString(&platformInfo.dateOfManufacture, dateOfManufacture);
    duplicateString(&platformInfo.platformVersion, platformVersion);
    duplicateString(&platformInfo.operatingSystemVersion, operatingSystemVersion);
    duplicateString(&platformInfo.hardwareVersion, hardwareVersion);
    duplicateString(&platformInfo.firmwareVersion, firmwareVersion);
    duplicateString(&platformInfo.supportUrl, supportUrl);
    duplicateString(&platformInfo.systemTime, systemTime);
}

void Platform::deletePlatformInfo()
{
    delete[] platformInfo.platformID;
    delete[] platformInfo.manufacturerName;
    delete[] platformInfo.manufacturerUrl;
    delete[] platformInfo.modelNumber;
    delete[] platformInfo.dateOfManufacture;
    delete[] platformInfo.platformVersion;
    delete[] platformInfo.operatingSystemVersion;
    delete[] platformInfo.hardwareVersion;
    delete[] platformInfo.firmwareVersion;
    delete[] platformInfo.supportUrl;
    delete[] platformInfo.systemTime;
}

/**
*  SetDeviceInfo
*  Sets the device information ("oic/d"), from the globals

* @return OC_STACK_ERROR or OC_STACK_OK
*/
OCStackResult Platform::setDeviceInfo()
{
    OCStackResult result = OC_STACK_ERROR;

    OCResourceHandle handle = OCGetResourceHandleAtUri(OC_RSRVD_DEVICE_URI);
    if (handle == NULL)
    {
        std::cout << "Failed to find resource " << OC_RSRVD_DEVICE_URI << std::endl;
        return result;
    }
    result = OCBindResourceTypeToResource(handle, deviceType.c_str());
    if (result != OC_STACK_OK)
    {
        std::cout << "Failed to add device type" << std::endl;
        return result;
    }
    result = OCPlatform::setPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DEVICE_NAME, deviceName);
    if (result != OC_STACK_OK)
    {
        std::cout << "Failed to set device name" << std::endl;
        return result;
    }
    result = OCPlatform::setPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DATA_MODEL_VERSION,
                                          dataModelVersions);
    if (result != OC_STACK_OK)
    {
        std::cout << "Failed to set data model versions" << std::endl;
        return result;
    }
    result = OCPlatform::setPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_SPEC_VERSION, specVersion);
    if (result != OC_STACK_OK)
    {
        std::cout << "Failed to set spec version" << std::endl;
        return result;
    }
    result = OCPlatform::setPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_PROTOCOL_INDEPENDENT_ID,
                                          protocolIndependentID);
    if (result != OC_STACK_OK)
    {
        std::cout << "Failed to set piid" << std::endl;
        return result;
    }

    return OC_STACK_OK;
}

#ifdef __unix__
// global needs static, otherwise it can be compiled out and then Ctrl-C does not work
static int quit = 0;
// handler for the signal to stop the application
void handle_signal(int signal)
{
    OC_UNUSED(signal);
    quit = 1;
}
#endif

// main application
// starts the server
int main(void)
{
    Platform platform;
    if(OC_STACK_OK != platform.start())
    {
        std::cerr << "Failed to start the IoTivity platform." << std::endl;
        return 1;
    }

    std::cout << "/oic/p" << std::endl;
    // initialize "/oic/p"
    if (OC_STACK_OK != platform.registerPlatformInfo())
    {
        std::cerr << "Failed platform registration (/oic/p)." << std::endl;
    }
    // initialize "/oic/d"
    std::cout << "/oic/d" << std::endl;
    if (OC_STACK_OK != platform.setDeviceInfo())
    {
        std::cerr << "Failed device registration (/oic/d)." << std::endl;
    }

    std::cout << "device type: " <<  platform.deviceType << std::endl;
    std::cout << "platformID: " <<  platform.getPlatformInfo()->platformID << std::endl;
    std::cout << "platform independent: " <<  platform.protocolIndependentID << std::endl;

    // create the server
    IoTServer server;
    if (OC_STACK_OK != server.registerResources())
    {
        std::cerr << "Failed to register server resources." << std::endl;
        return 1;
    }

#ifdef __unix__
    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    std::cout << "Press Ctrl-C to quit...." << std::endl;
    do
    {
        usleep(2000000);
    }
    while (quit != 1);
#endif


#if defined(_WIN32)
    std::cout << "Press Ctrl-C to quit...." << std::endl;
    // we will keep the server alive for at most 30 minutes
    std::this_thread::sleep_for(std::chrono::minutes(30));
    OC_VERIFY(OCPlatform::stop() == OC_STACK_OK);
#endif

    return 0;
}