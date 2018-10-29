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

#include "ocstack.h"
#include "OCPlatform.h"
#include "OCApi.h"
#include "ocpayload.h"

using namespace OC;
namespace PH = std::placeholders;

/*
 tool_version          : 20171123
 input_file            : /home/pi/tmp/automationphattest/device_output/out_codegeneration_merged.swagger.json
 version of input_file : v1.1.0-20170815
 title of input_file   : Energy
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
 * class definition for class that handles /ADC1
 *
 * This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
 * Retrieves the current energy.
*/
class Adc1Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Adc1Resource(std::string resourceUri = "/ADC1");

        /*
         * destructor
         */
         virtual ~Adc1Resource(void);

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
        OCStackResult sendNotification(const std::shared_ptr< OCResourceResponse > pResponse);
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /ADC1
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

        // member variables for path: "/ADC1"
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
Adc1Resource::Adc1Resource(std::string resourceUri)
{
    std::cout << "- Running: Adc1Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /ADC1
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
Adc1Resource::~Adc1Resource() { }

OCStackResult Adc1Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Adc1Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Adc1Resource." << std::endl;
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

    std::cout << "Adc1Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /ADC1
*/
OCStackResult Adc1Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /ADC1
* @param pResponse  the response to use for the observe
*/
OCStackResult Adc1Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}


/*
* Make the payload for the retrieve function (e.g. GET) /ADC1
* @param queries  the query parameters for this call
*/
OCRepresentation Adc1Resource::get(QueryParamsMap queries)
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
bool Adc1Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Adc1Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Adc1Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Adc1Resource, URI is : "
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
                std::cout<<"Adc1Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Adc1Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /ADC2
 *
 * This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
 * Retrieves the current energy.
*/
class Adc2Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Adc2Resource(std::string resourceUri = "/ADC2");

        /*
         * destructor
         */
         virtual ~Adc2Resource(void);

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
        OCStackResult sendNotification(const std::shared_ptr< OCResourceResponse > pResponse);
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /ADC2
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

        // member variables for path: "/ADC2"
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
Adc2Resource::Adc2Resource(std::string resourceUri)
{
    std::cout << "- Running: Adc2Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /ADC2
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
Adc2Resource::~Adc2Resource() { }

OCStackResult Adc2Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Adc2Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Adc2Resource." << std::endl;
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

    std::cout << "Adc2Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /ADC2
*/
OCStackResult Adc2Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /ADC2
* @param pResponse  the response to use for the observe
*/
OCStackResult Adc2Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}


/*
* Make the payload for the retrieve function (e.g. GET) /ADC2
* @param queries  the query parameters for this call
*/
OCRepresentation Adc2Resource::get(QueryParamsMap queries)
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
bool Adc2Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Adc2Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Adc2Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Adc2Resource, URI is : "
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
                std::cout<<"Adc2Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Adc2Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /ADC3
 *
 * This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
 * Retrieves the current energy.
*/
class Adc3Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Adc3Resource(std::string resourceUri = "/ADC3");

        /*
         * destructor
         */
         virtual ~Adc3Resource(void);

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
        OCStackResult sendNotification(const std::shared_ptr< OCResourceResponse > pResponse);
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /ADC3
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

        // member variables for path: "/ADC3"
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
Adc3Resource::Adc3Resource(std::string resourceUri)
{
    std::cout << "- Running: Adc3Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /ADC3
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
Adc3Resource::~Adc3Resource() { }

OCStackResult Adc3Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Adc3Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Adc3Resource." << std::endl;
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

    std::cout << "Adc3Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /ADC3
*/
OCStackResult Adc3Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /ADC3
* @param pResponse  the response to use for the observe
*/
OCStackResult Adc3Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}


/*
* Make the payload for the retrieve function (e.g. GET) /ADC3
* @param queries  the query parameters for this call
*/
OCRepresentation Adc3Resource::get(QueryParamsMap queries)
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
bool Adc3Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Adc3Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Adc3Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Adc3Resource, URI is : "
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
                std::cout<<"Adc3Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Adc3Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /ADC4
 *
 * This resource describes the attributes associated with electrical energy. This can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The voltage is in Volts (V), current in Amps (A), and frequency in Hertz (Hz).
 * Retrieves the current energy.
*/
class Adc4Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Adc4Resource(std::string resourceUri = "/ADC4");

        /*
         * destructor
         */
         virtual ~Adc4Resource(void);

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
        OCStackResult sendNotification(const std::shared_ptr< OCResourceResponse > pResponse);
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /ADC4
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

        // member variables for path: "/ADC4"
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
Adc4Resource::Adc4Resource(std::string resourceUri)
{
    std::cout << "- Running: Adc4Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /ADC4
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
Adc4Resource::~Adc4Resource() { }

OCStackResult Adc4Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Adc4Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Adc4Resource." << std::endl;
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

    std::cout << "Adc4Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /ADC4
*/
OCStackResult Adc4Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /ADC4
* @param pResponse  the response to use for the observe
*/
OCStackResult Adc4Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}


/*
* Make the payload for the retrieve function (e.g. GET) /ADC4
* @param queries  the query parameters for this call
*/
OCRepresentation Adc4Resource::get(QueryParamsMap queries)
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
bool Adc4Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Adc4Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Adc4Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Adc4Resource, URI is : "
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
                std::cout<<"Adc4Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Adc4Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /input1
 *
 * This resource describes a binary switch (on/off).
 * The value is a boolean.
 * A value of 'true' means that the switch is on.
 * A value of 'false' means that the switch is off.
*/
class Input1Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Input1Resource(std::string resourceUri = "/input1");

        /*
         * destructor
         */
         virtual ~Input1Resource(void);

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
        OCStackResult sendNotification(const std::shared_ptr< OCResourceResponse > pResponse);
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /input1
         * This resource describes a binary switch (on/off).
         * The value is a boolean.
         * A value of 'true' means that the switch is on.
         * A value of 'false' means that the switch is off.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.switch.binary"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/input1"
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
Input1Resource::Input1Resource(std::string resourceUri)
{
    std::cout << "- Running: Input1Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /input1
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" Status of the switch
    }

/*
* Destructor code
*/
Input1Resource::~Input1Resource() { }

OCStackResult Input1Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Input1Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Input1Resource." << std::endl;
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

    std::cout << "Input1Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /input1
*/
OCStackResult Input1Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /input1
* @param pResponse  the response to use for the observe
*/
OCStackResult Input1Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}


/*
* Make the payload for the retrieve function (e.g. GET) /input1
* @param queries  the query parameters for this call
*/
OCRepresentation Input1Resource::get(QueryParamsMap queries)
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
* Check if the interface name is an registered interface name
*/
bool Input1Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Input1Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Input1Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Input1Resource, URI is : "
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
                std::cout<<"Input1Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Input1Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /input2
 *
 * This resource describes a binary switch (on/off).
 * The value is a boolean.
 * A value of 'true' means that the switch is on.
 * A value of 'false' means that the switch is off.
*/
class Input2Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Input2Resource(std::string resourceUri = "/input2");

        /*
         * destructor
         */
         virtual ~Input2Resource(void);

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
        OCStackResult sendNotification(const std::shared_ptr< OCResourceResponse > pResponse);
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /input2
         * This resource describes a binary switch (on/off).
         * The value is a boolean.
         * A value of 'true' means that the switch is on.
         * A value of 'false' means that the switch is off.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.switch.binary"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/input2"
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
Input2Resource::Input2Resource(std::string resourceUri)
{
    std::cout << "- Running: Input2Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /input2
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" Status of the switch
    }

/*
* Destructor code
*/
Input2Resource::~Input2Resource() { }

OCStackResult Input2Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Input2Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Input2Resource." << std::endl;
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

    std::cout << "Input2Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /input2
*/
OCStackResult Input2Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /input2
* @param pResponse  the response to use for the observe
*/
OCStackResult Input2Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}


/*
* Make the payload for the retrieve function (e.g. GET) /input2
* @param queries  the query parameters for this call
*/
OCRepresentation Input2Resource::get(QueryParamsMap queries)
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
* Check if the interface name is an registered interface name
*/
bool Input2Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Input2Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Input2Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Input2Resource, URI is : "
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
                std::cout<<"Input2Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Input2Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /input3
 *
 * This resource describes a binary switch (on/off).
 * The value is a boolean.
 * A value of 'true' means that the switch is on.
 * A value of 'false' means that the switch is off.
*/
class Input3Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Input3Resource(std::string resourceUri = "/input3");

        /*
         * destructor
         */
         virtual ~Input3Resource(void);

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
        OCStackResult sendNotification(const std::shared_ptr< OCResourceResponse > pResponse);
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /input3
         * This resource describes a binary switch (on/off).
         * The value is a boolean.
         * A value of 'true' means that the switch is on.
         * A value of 'false' means that the switch is off.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.switch.binary"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/input3"
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
Input3Resource::Input3Resource(std::string resourceUri)
{
    std::cout << "- Running: Input3Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /input3
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" Status of the switch
    }

/*
* Destructor code
*/
Input3Resource::~Input3Resource() { }

OCStackResult Input3Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Input3Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Input3Resource." << std::endl;
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

    std::cout << "Input3Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /input3
*/
OCStackResult Input3Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /input3
* @param pResponse  the response to use for the observe
*/
OCStackResult Input3Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}


/*
* Make the payload for the retrieve function (e.g. GET) /input3
* @param queries  the query parameters for this call
*/
OCRepresentation Input3Resource::get(QueryParamsMap queries)
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
* Check if the interface name is an registered interface name
*/
bool Input3Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Input3Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Input3Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Input3Resource, URI is : "
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
                std::cout<<"Input3Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Input3Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /output1
 *
 * This resource describes a binary switch (on/off).
 * The value is a boolean.
 * A value of 'true' means that the switch is on.
 * A value of 'false' means that the switch is off.
*/
class Output1Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Output1Resource(std::string resourceUri = "/output1");

        /*
         * destructor
         */
         virtual ~Output1Resource(void);

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
        OCStackResult sendNotification(const std::shared_ptr< OCResourceResponse > pResponse);
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /output1
         * This resource describes a binary switch (on/off).
         * The value is a boolean.
         * A value of 'true' means that the switch is on.
         * A value of 'false' means that the switch is off.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);

        /*
         * Parse the payload for the update function (e.g. POST) /output1

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

        // member variables for path: "/output1"
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
Output1Resource::Output1Resource(std::string resourceUri)
{
    std::cout << "- Running: Output1Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /output1
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" Status of the switch
    }

/*
* Destructor code
*/
Output1Resource::~Output1Resource() { }

OCStackResult Output1Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Output1Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Output1Resource." << std::endl;
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

    std::cout << "Output1Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /output1
*/
OCStackResult Output1Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /output1
* @param pResponse  the response to use for the observe
*/
OCStackResult Output1Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}


/*
* Make the payload for the retrieve function (e.g. GET) /output1
* @param queries  the query parameters for this call
*/
OCRepresentation Output1Resource::get(QueryParamsMap queries)
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
* Parse the payload for the update function (e.g. POST) /output1
* @param queries  the query parameters for this call
* @param rep  the response to get the property values from
* @return OCEntityHandlerResult ok or not ok indication
*/
OCEntityHandlerResult Output1Resource::post(QueryParamsMap queries, const OCRepresentation& rep)
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
    }
    return ehResult;
}
/*
* Check if the interface name is an registered interface name
*/
bool Output1Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Output1Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Output1Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Output1Resource, URI is : "
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
                std::cout<<"Output1Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }

            else if(request->getRequestType() == "POST")
            {
                std::cout <<"Output1Resource Post Request"<<std::endl;
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
                            if (OC_STACK_OK != sendNotification(pResponse) )
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
                std::cout << "Output1Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /output2
 *
 * This resource describes a binary switch (on/off).
 * The value is a boolean.
 * A value of 'true' means that the switch is on.
 * A value of 'false' means that the switch is off.
*/
class Output2Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Output2Resource(std::string resourceUri = "/output2");

        /*
         * destructor
         */
         virtual ~Output2Resource(void);

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
        OCStackResult sendNotification(const std::shared_ptr< OCResourceResponse > pResponse);
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /output2
         * This resource describes a binary switch (on/off).
         * The value is a boolean.
         * A value of 'true' means that the switch is on.
         * A value of 'false' means that the switch is off.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);

        /*
         * Parse the payload for the update function (e.g. POST) /output2

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

        // member variables for path: "/output2"
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
Output2Resource::Output2Resource(std::string resourceUri)
{
    std::cout << "- Running: Output2Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /output2
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" Status of the switch
    }

/*
* Destructor code
*/
Output2Resource::~Output2Resource() { }

OCStackResult Output2Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Output2Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Output2Resource." << std::endl;
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

    std::cout << "Output2Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /output2
*/
OCStackResult Output2Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /output2
* @param pResponse  the response to use for the observe
*/
OCStackResult Output2Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}


/*
* Make the payload for the retrieve function (e.g. GET) /output2
* @param queries  the query parameters for this call
*/
OCRepresentation Output2Resource::get(QueryParamsMap queries)
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
* Parse the payload for the update function (e.g. POST) /output2
* @param queries  the query parameters for this call
* @param rep  the response to get the property values from
* @return OCEntityHandlerResult ok or not ok indication
*/
OCEntityHandlerResult Output2Resource::post(QueryParamsMap queries, const OCRepresentation& rep)
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
    }
    return ehResult;
}
/*
* Check if the interface name is an registered interface name
*/
bool Output2Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Output2Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Output2Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Output2Resource, URI is : "
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
                std::cout<<"Output2Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }

            else if(request->getRequestType() == "POST")
            {
                std::cout <<"Output2Resource Post Request"<<std::endl;
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
                            if (OC_STACK_OK != sendNotification(pResponse) )
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
                std::cout << "Output2Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /output3
 *
 * This resource describes a binary switch (on/off).
 * The value is a boolean.
 * A value of 'true' means that the switch is on.
 * A value of 'false' means that the switch is off.
*/
class Output3Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Output3Resource(std::string resourceUri = "/output3");

        /*
         * destructor
         */
         virtual ~Output3Resource(void);

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
        OCStackResult sendNotification(const std::shared_ptr< OCResourceResponse > pResponse);
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /output3
         * This resource describes a binary switch (on/off).
         * The value is a boolean.
         * A value of 'true' means that the switch is on.
         * A value of 'false' means that the switch is off.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);

        /*
         * Parse the payload for the update function (e.g. POST) /output3

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

        // member variables for path: "/output3"
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
Output3Resource::Output3Resource(std::string resourceUri)
{
    std::cout << "- Running: Output3Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /output3
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" Status of the switch
    }

/*
* Destructor code
*/
Output3Resource::~Output3Resource() { }

OCStackResult Output3Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Output3Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Output3Resource." << std::endl;
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

    std::cout << "Output3Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /output3
*/
OCStackResult Output3Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /output3
* @param pResponse  the response to use for the observe
*/
OCStackResult Output3Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}


/*
* Make the payload for the retrieve function (e.g. GET) /output3
* @param queries  the query parameters for this call
*/
OCRepresentation Output3Resource::get(QueryParamsMap queries)
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
* Parse the payload for the update function (e.g. POST) /output3
* @param queries  the query parameters for this call
* @param rep  the response to get the property values from
* @return OCEntityHandlerResult ok or not ok indication
*/
OCEntityHandlerResult Output3Resource::post(QueryParamsMap queries, const OCRepresentation& rep)
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
    }
    return ehResult;
}
/*
* Check if the interface name is an registered interface name
*/
bool Output3Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Output3Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Output3Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Output3Resource, URI is : "
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
                std::cout<<"Output3Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }

            else if(request->getRequestType() == "POST")
            {
                std::cout <<"Output3Resource Post Request"<<std::endl;
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
                            if (OC_STACK_OK != sendNotification(pResponse) )
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
                std::cout << "Output3Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /relay
 *
 * This resource describes a binary switch (on/off).
 * The value is a boolean.
 * A value of 'true' means that the switch is on.
 * A value of 'false' means that the switch is off.
*/
class RelayResource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        RelayResource(std::string resourceUri = "/relay");

        /*
         * destructor
         */
         virtual ~RelayResource(void);

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
        OCStackResult sendNotification(const std::shared_ptr< OCResourceResponse > pResponse);
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /relay
         * This resource describes a binary switch (on/off).
         * The value is a boolean.
         * A value of 'true' means that the switch is on.
         * A value of 'false' means that the switch is off.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);

        /*
         * Parse the payload for the update function (e.g. POST) /relay

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

        // member variables for path: "/relay"
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
RelayResource::RelayResource(std::string resourceUri)
{
    std::cout << "- Running: RelayResource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /relay
    // initialize vector if  The interface set supported by this resource
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the resource
    // initialize vector rt  Resource Type
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" Status of the switch
    }

/*
* Destructor code
*/
RelayResource::~RelayResource() { }

OCStackResult RelayResource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&RelayResource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register RelayResource." << std::endl;
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

    std::cout << "RelayResource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /relay
*/
OCStackResult RelayResource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /relay
* @param pResponse  the response to use for the observe
*/
OCStackResult RelayResource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
{
    OCStackResult sResult = OC_STACK_OK;
    if ( m_interestedObservers.size() > 0) {
        std::cout << "Notifying list "  << m_interestedObservers.size() << " of observers\n";
        sResult = OCPlatform::notifyListOfObservers(m_resourceHandle,
                                                    m_interestedObservers,
                                                    pResponse);
    }
    return sResult;
}


/*
* Make the payload for the retrieve function (e.g. GET) /relay
* @param queries  the query parameters for this call
*/
OCRepresentation RelayResource::get(QueryParamsMap queries)
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
* Parse the payload for the update function (e.g. POST) /relay
* @param queries  the query parameters for this call
* @param rep  the response to get the property values from
* @return OCEntityHandlerResult ok or not ok indication
*/
OCEntityHandlerResult RelayResource::post(QueryParamsMap queries, const OCRepresentation& rep)
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
    }
    return ehResult;
}
/*
* Check if the interface name is an registered interface name
*/
bool RelayResource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult RelayResource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for RelayResource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for RelayResource, URI is : "
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
                std::cout<<"RelayResource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }

            else if(request->getRequestType() == "POST")
            {
                std::cout <<"RelayResource Post Request"<<std::endl;
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
                            if (OC_STACK_OK != sendNotification(pResponse) )
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
                std::cout << "RelayResource unsupported request type (delete,put,..)"
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
        Adc1Resource  m_ADC1Instance;
        Adc2Resource  m_ADC2Instance;
        Adc3Resource  m_ADC3Instance;
        Adc4Resource  m_ADC4Instance;
        Input1Resource  m_input1Instance;
        Input2Resource  m_input2Instance;
        Input3Resource  m_input3Instance;
        Output1Resource  m_output1Instance;
        Output2Resource  m_output2Instance;
        Output3Resource  m_output3Instance;
        RelayResource  m_relayInstance;};

IoTServer::IoTServer()
    :m_ADC1Instance(),
     m_ADC2Instance(),
     m_ADC3Instance(),
     m_ADC4Instance(),
     m_input1Instance(),
     m_input2Instance(),
     m_input3Instance(),
     m_output1Instance(),
     m_output2Instance(),
     m_output3Instance(),
     m_relayInstance()
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
    result = m_ADC1Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_ADC2Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_ADC3Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_ADC4Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_input1Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_input2Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_input3Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_output1Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_output2Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_output3Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_relayInstance.registerResource(resourceProperty);
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
    std::string  deviceName = "Energy";
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