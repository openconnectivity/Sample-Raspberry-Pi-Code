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
#include "/usr/include/python2.7/Python.h"
// #include <Python.h>

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
#include "observer.h"
#include "OCPlatform.h"
#include "OCApi.h"
#include "ocpayload.h"

using namespace OC;
namespace PH = std::placeholders;

/*
 tool_version          : 20171123
 input_file            : /home/pi/workspace/explorerhatpro-o/device_output/out_codegeneration_merged.swagger.json
 version of input_file : 20190215
 title of input_file   : Touch Sensor
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

class ExplorerHat
{
    public:
        /*
         * constructor
         */
        ExplorerHat();

        /*
         * destructor
         */
        virtual ~ExplorerHat();

        int CallPythonFunction(char moduleName[], char functionName[], int numArgs, int args[]);

        PyObject *pName, *pModule, *pDict, *pFunc;
        PyObject *pArgs, *pValue;

	int myParamArgs[2];
        long returnLong = 0;
        double returnDouble = 0.0;
};

/*
* Constructor code
*/
ExplorerHat::ExplorerHat()
{
    Py_Initialize();
}

/*
* Destructor code
*/
ExplorerHat::~ExplorerHat()
{
    Py_Finalize();
}

/*
* Funcion to call Pimoroni python libraries
*/
int ExplorerHat::CallPythonFunction(char moduleName[], char functionName[], int numArgs, int args[])
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

ExplorerHat *testExplorerHat;

/*
 * class definition for class that handles /analog1
 *
 * This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
*/
class Analog1Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Analog1Resource(std::string resourceUri = "/analog1");

        /*
         * destructor
         */
         virtual ~Analog1Resource(void);

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
         * Make the payload for the retrieve function (e.g. GET) /analog1
         * This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.energy.electrical"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/analog1"
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
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
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
Analog1Resource::Analog1Resource(std::string resourceUri)
{
    std::cout << "- Running: Analog1Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /analog1
    m_var_value_current = 5.0; // current value of property "current"  The electric current in Amps (A).
    m_var_value_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
    m_var_value_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
    m_var_value_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
    m_var_value_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.energy.electrical");
    m_var_value_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
    }

/*
* Destructor code
*/
Analog1Resource::~Analog1Resource() { }

OCStackResult Analog1Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Analog1Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Analog1Resource." << std::endl;
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

    std::cout << "Analog1Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /analog1
*/
OCStackResult Analog1Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /analog1
* @param pResponse  the response to use for the observe
*/
OCStackResult Analog1Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Make the payload for the retrieve function (e.g. GET) /analog1
* @param queries  the query parameters for this call
*/
OCRepresentation Analog1Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);

	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables
  testExplorerHat->myParamArgs[0] = 1;
  testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readAnalog", 1, testExplorerHat->myParamArgs);
  m_var_value_voltage = testExplorerHat->returnDouble;

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
bool Analog1Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Analog1Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Analog1Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Analog1Resource, URI is : "
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
                std::cout<<"Analog1Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Analog1Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /analog2
 *
 * This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
*/
class Analog2Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Analog2Resource(std::string resourceUri = "/analog2");

        /*
         * destructor
         */
         virtual ~Analog2Resource(void);

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
         * Make the payload for the retrieve function (e.g. GET) /analog2
         * This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.energy.electrical"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/analog2"
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
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
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
Analog2Resource::Analog2Resource(std::string resourceUri)
{
    std::cout << "- Running: Analog2Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /analog2
    m_var_value_current = 5.0; // current value of property "current"  The electric current in Amps (A).
    m_var_value_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
    m_var_value_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
    m_var_value_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
    m_var_value_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.energy.electrical");
    m_var_value_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
    }

/*
* Destructor code
*/
Analog2Resource::~Analog2Resource() { }

OCStackResult Analog2Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Analog2Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Analog2Resource." << std::endl;
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

    std::cout << "Analog2Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /analog2
*/
OCStackResult Analog2Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /analog2
* @param pResponse  the response to use for the observe
*/
OCStackResult Analog2Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Make the payload for the retrieve function (e.g. GET) /analog2
* @param queries  the query parameters for this call
*/
OCRepresentation Analog2Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);

	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables
  testExplorerHat->myParamArgs[0] = 2;
  testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readAnalog", 1, testExplorerHat->myParamArgs);
  m_var_value_voltage = testExplorerHat->returnDouble;

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
bool Analog2Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Analog2Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Analog2Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Analog2Resource, URI is : "
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
                std::cout<<"Analog2Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Analog2Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /analog3
 *
 * This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
*/
class Analog3Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Analog3Resource(std::string resourceUri = "/analog3");

        /*
         * destructor
         */
         virtual ~Analog3Resource(void);

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
         * Make the payload for the retrieve function (e.g. GET) /analog3
         * This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.energy.electrical"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/analog3"
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
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
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
Analog3Resource::Analog3Resource(std::string resourceUri)
{
    std::cout << "- Running: Analog3Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /analog3
    m_var_value_current = 5.0; // current value of property "current"  The electric current in Amps (A).
    m_var_value_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
    m_var_value_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
    m_var_value_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
    m_var_value_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.energy.electrical");
    m_var_value_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
    }

/*
* Destructor code
*/
Analog3Resource::~Analog3Resource() { }

OCStackResult Analog3Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Analog3Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Analog3Resource." << std::endl;
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

    std::cout << "Analog3Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /analog3
*/
OCStackResult Analog3Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /analog3
* @param pResponse  the response to use for the observe
*/
OCStackResult Analog3Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Make the payload for the retrieve function (e.g. GET) /analog3
* @param queries  the query parameters for this call
*/
OCRepresentation Analog3Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);

	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables
  testExplorerHat->myParamArgs[0] = 3;
  testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readAnalog", 1, testExplorerHat->myParamArgs);
  m_var_value_voltage = testExplorerHat->returnDouble;

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
bool Analog3Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Analog3Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Analog3Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Analog3Resource, URI is : "
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
                std::cout<<"Analog3Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Analog3Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /analog4
 *
 * This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
*/
class Analog4Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Analog4Resource(std::string resourceUri = "/analog4");

        /*
         * destructor
         */
         virtual ~Analog4Resource(void);

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
         * Make the payload for the retrieve function (e.g. GET) /analog4
         * This Resource describes the attributes associated with electrical energy. This Resource can be used for either rated (read-only), desired (read-write) or measured (read-only) energy. The Property "voltage" is in Volts (V), The Property "current" in Amps (A), and The Property "frequency" is in Hertz (Hz).
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.energy.electrical"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.s"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/analog4"
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
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
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
Analog4Resource::Analog4Resource(std::string resourceUri)
{
    std::cout << "- Running: Analog4Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /analog4
    m_var_value_current = 5.0; // current value of property "current"  The electric current in Amps (A).
    m_var_value_desiredcurrent = 0; // current value of property "desiredcurrent"  The desired electric current in Amps (A).
    m_var_value_desiredfrequency = 0; // current value of property "desiredfrequency"  The desired electric frequency in Hertz (Hz).
    m_var_value_desiredvoltage = 0; // current value of property "desiredvoltage"  The desired electric voltage in Volts (V).
    m_var_value_frequency = 60.0; // current value of property "frequency"  The electric frequency in Hertz (Hz).
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.s");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.energy.electrical");
    m_var_value_voltage = 120.0; // current value of property "voltage"  The electric voltage in Volts (V).
    }

/*
* Destructor code
*/
Analog4Resource::~Analog4Resource() { }

OCStackResult Analog4Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Analog4Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Analog4Resource." << std::endl;
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

    std::cout << "Analog4Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /analog4
*/
OCStackResult Analog4Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /analog4
* @param pResponse  the response to use for the observe
*/
OCStackResult Analog4Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Make the payload for the retrieve function (e.g. GET) /analog4
* @param queries  the query parameters for this call
*/
OCRepresentation Analog4Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);

	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables
  testExplorerHat->myParamArgs[0] = 4;
  testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readAnalog", 1, testExplorerHat->myParamArgs);
  m_var_value_voltage = testExplorerHat->returnDouble;

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
bool Analog4Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Analog4Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Analog4Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Analog4Resource, URI is : "
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
                std::cout<<"Analog4Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Analog4Resource unsupported request type (delete,put,..)"
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
 * This Resource describes a binary switch (on/off).
 * The Property "value" is a boolean.
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
         * This Resource describes a binary switch (on/off).
         * The Property "value" is a boolean.
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
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The status of the switch.
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
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" The status of the switch.
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
  testExplorerHat->myParamArgs[0] = 1;
  testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readInput", 1, testExplorerHat->myParamArgs);
  m_var_value_value = (bool)testExplorerHat->returnLong;

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
 * This Resource describes a binary switch (on/off).
 * The Property "value" is a boolean.
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
         * This Resource describes a binary switch (on/off).
         * The Property "value" is a boolean.
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
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The status of the switch.
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
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" The status of the switch.
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
  testExplorerHat->myParamArgs[0] = 2;
  testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readInput", 1, testExplorerHat->myParamArgs);
  m_var_value_value = (bool)testExplorerHat->returnLong;

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
 * This Resource describes a binary switch (on/off).
 * The Property "value" is a boolean.
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
         * This Resource describes a binary switch (on/off).
         * The Property "value" is a boolean.
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
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The status of the switch.
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
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" The status of the switch.
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
  testExplorerHat->myParamArgs[0] = 3;
  testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readInput", 1, testExplorerHat->myParamArgs);
  m_var_value_value = (bool)testExplorerHat->returnLong;

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
 * class definition for class that handles /input4
 *
 * This Resource describes a binary switch (on/off).
 * The Property "value" is a boolean.
 * A value of 'true' means that the switch is on.
 * A value of 'false' means that the switch is off.
*/
class Input4Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Input4Resource(std::string resourceUri = "/input4");

        /*
         * destructor
         */
         virtual ~Input4Resource(void);

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
         * Make the payload for the retrieve function (e.g. GET) /input4
         * This Resource describes a binary switch (on/off).
         * The Property "value" is a boolean.
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

        // member variables for path: "/input4"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The status of the switch.
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
Input4Resource::Input4Resource(std::string resourceUri)
{
    std::cout << "- Running: Input4Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /input4
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" The status of the switch.
    }

/*
* Destructor code
*/
Input4Resource::~Input4Resource() { }

OCStackResult Input4Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Input4Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Input4Resource." << std::endl;
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

    std::cout << "Input4Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /input4
*/
OCStackResult Input4Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /input4
* @param pResponse  the response to use for the observe
*/
OCStackResult Input4Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Make the payload for the retrieve function (e.g. GET) /input4
* @param queries  the query parameters for this call
*/
OCRepresentation Input4Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);

	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables
  testExplorerHat->myParamArgs[0] = 4;
  testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readInput", 1, testExplorerHat->myParamArgs);
  m_var_value_value = (bool)testExplorerHat->returnLong;

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
bool Input4Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Input4Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Input4Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Input4Resource, URI is : "
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
                std::cout<<"Input4Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Input4Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /light1
 *
 * This Resource describes a binary switch (on/off).
 * The Property "value" is a boolean.
 * A value of 'true' means that the switch is on.
 * A value of 'false' means that the switch is off.
*/
class Light1Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Light1Resource(std::string resourceUri = "/light1");

        /*
         * destructor
         */
         virtual ~Light1Resource(void);

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
         * Make the payload for the retrieve function (e.g. GET) /light1
         * This Resource describes a binary switch (on/off).
         * The Property "value" is a boolean.
         * A value of 'true' means that the switch is on.
         * A value of 'false' means that the switch is off.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);

        /*
         * Parse the payload for the update function (e.g. POST) /light1

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

        // member variables for path: "/light1"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The status of the switch.
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
Light1Resource::Light1Resource(std::string resourceUri)
{
    std::cout << "- Running: Light1Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /light1
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" The status of the switch.
    }

/*
* Destructor code
*/
Light1Resource::~Light1Resource() { }

OCStackResult Light1Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Light1Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Light1Resource." << std::endl;
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

    std::cout << "Light1Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /light1
*/
OCStackResult Light1Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /light1
* @param pResponse  the response to use for the observe
*/
OCStackResult Light1Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Make the payload for the retrieve function (e.g. GET) /light1
* @param queries  the query parameters for this call
*/
OCRepresentation Light1Resource::get(QueryParamsMap queries)
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
* Parse the payload for the update function (e.g. POST) /light1
* @param queries  the query parameters for this call
* @param rep  the response to get the property values from
* @return OCEntityHandlerResult ok or not ok indication
*/
OCEntityHandlerResult Light1Resource::post(QueryParamsMap queries, const OCRepresentation& rep)
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
      testExplorerHat->myParamArgs[0] = 1;
      testExplorerHat->myParamArgs[1] = m_var_value_value ? 1 : 0;
      testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeLight", 2, testExplorerHat->myParamArgs);
    }
    return ehResult;
}
/*
* Check if the interface name is an registered interface name
*/
bool Light1Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Light1Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Light1Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Light1Resource, URI is : "
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
                std::cout<<"Light1Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }

            else if(request->getRequestType() == "POST")
            {
                std::cout <<"Light1Resource Post Request"<<std::endl;
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
                std::cout << "Light1Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /light2
 *
 * This Resource describes a binary switch (on/off).
 * The Property "value" is a boolean.
 * A value of 'true' means that the switch is on.
 * A value of 'false' means that the switch is off.
*/
class Light2Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Light2Resource(std::string resourceUri = "/light2");

        /*
         * destructor
         */
         virtual ~Light2Resource(void);

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
         * Make the payload for the retrieve function (e.g. GET) /light2
         * This Resource describes a binary switch (on/off).
         * The Property "value" is a boolean.
         * A value of 'true' means that the switch is on.
         * A value of 'false' means that the switch is off.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);

        /*
         * Parse the payload for the update function (e.g. POST) /light2

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

        // member variables for path: "/light2"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The status of the switch.
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
Light2Resource::Light2Resource(std::string resourceUri)
{
    std::cout << "- Running: Light2Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /light2
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" The status of the switch.
    }

/*
* Destructor code
*/
Light2Resource::~Light2Resource() { }

OCStackResult Light2Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Light2Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Light2Resource." << std::endl;
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

    std::cout << "Light2Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /light2
*/
OCStackResult Light2Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /light2
* @param pResponse  the response to use for the observe
*/
OCStackResult Light2Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Make the payload for the retrieve function (e.g. GET) /light2
* @param queries  the query parameters for this call
*/
OCRepresentation Light2Resource::get(QueryParamsMap queries)
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
* Parse the payload for the update function (e.g. POST) /light2
* @param queries  the query parameters for this call
* @param rep  the response to get the property values from
* @return OCEntityHandlerResult ok or not ok indication
*/
OCEntityHandlerResult Light2Resource::post(QueryParamsMap queries, const OCRepresentation& rep)
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
        testExplorerHat->myParamArgs[0] = 2;
        testExplorerHat->myParamArgs[1] = m_var_value_value ? 1 : 0;
        testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeLight", 2, testExplorerHat->myParamArgs);
    }
    return ehResult;
}
/*
* Check if the interface name is an registered interface name
*/
bool Light2Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Light2Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Light2Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Light2Resource, URI is : "
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
                std::cout<<"Light2Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }

            else if(request->getRequestType() == "POST")
            {
                std::cout <<"Light2Resource Post Request"<<std::endl;
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
                std::cout << "Light2Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /light3
 *
 * This Resource describes a binary switch (on/off).
 * The Property "value" is a boolean.
 * A value of 'true' means that the switch is on.
 * A value of 'false' means that the switch is off.
*/
class Light3Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Light3Resource(std::string resourceUri = "/light3");

        /*
         * destructor
         */
         virtual ~Light3Resource(void);

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
         * Make the payload for the retrieve function (e.g. GET) /light3
         * This Resource describes a binary switch (on/off).
         * The Property "value" is a boolean.
         * A value of 'true' means that the switch is on.
         * A value of 'false' means that the switch is off.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);

        /*
         * Parse the payload for the update function (e.g. POST) /light3

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

        // member variables for path: "/light3"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The status of the switch.
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
Light3Resource::Light3Resource(std::string resourceUri)
{
    std::cout << "- Running: Light3Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /light3
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" The status of the switch.
    }

/*
* Destructor code
*/
Light3Resource::~Light3Resource() { }

OCStackResult Light3Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Light3Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Light3Resource." << std::endl;
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

    std::cout << "Light3Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /light3
*/
OCStackResult Light3Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /light3
* @param pResponse  the response to use for the observe
*/
OCStackResult Light3Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Make the payload for the retrieve function (e.g. GET) /light3
* @param queries  the query parameters for this call
*/
OCRepresentation Light3Resource::get(QueryParamsMap queries)
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
* Parse the payload for the update function (e.g. POST) /light3
* @param queries  the query parameters for this call
* @param rep  the response to get the property values from
* @return OCEntityHandlerResult ok or not ok indication
*/
OCEntityHandlerResult Light3Resource::post(QueryParamsMap queries, const OCRepresentation& rep)
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
        testExplorerHat->myParamArgs[0] = 3;
        testExplorerHat->myParamArgs[1] = m_var_value_value ? 1 : 0;
        testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeLight", 2, testExplorerHat->myParamArgs);
    }
    return ehResult;
}
/*
* Check if the interface name is an registered interface name
*/
bool Light3Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Light3Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Light3Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Light3Resource, URI is : "
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
                std::cout<<"Light3Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }

            else if(request->getRequestType() == "POST")
            {
                std::cout <<"Light3Resource Post Request"<<std::endl;
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
                std::cout << "Light3Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /light4
 *
 * This Resource describes a binary switch (on/off).
 * The Property "value" is a boolean.
 * A value of 'true' means that the switch is on.
 * A value of 'false' means that the switch is off.
*/
class Light4Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Light4Resource(std::string resourceUri = "/light4");

        /*
         * destructor
         */
         virtual ~Light4Resource(void);

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
         * Make the payload for the retrieve function (e.g. GET) /light4
         * This Resource describes a binary switch (on/off).
         * The Property "value" is a boolean.
         * A value of 'true' means that the switch is on.
         * A value of 'false' means that the switch is off.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);

        /*
         * Parse the payload for the update function (e.g. POST) /light4

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

        // member variables for path: "/light4"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The status of the switch.
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
Light4Resource::Light4Resource(std::string resourceUri)
{
    std::cout << "- Running: Light4Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /light4
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" The status of the switch.
    }

/*
* Destructor code
*/
Light4Resource::~Light4Resource() { }

OCStackResult Light4Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Light4Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Light4Resource." << std::endl;
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

    std::cout << "Light4Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /light4
*/
OCStackResult Light4Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /light4
* @param pResponse  the response to use for the observe
*/
OCStackResult Light4Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Make the payload for the retrieve function (e.g. GET) /light4
* @param queries  the query parameters for this call
*/
OCRepresentation Light4Resource::get(QueryParamsMap queries)
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
* Parse the payload for the update function (e.g. POST) /light4
* @param queries  the query parameters for this call
* @param rep  the response to get the property values from
* @return OCEntityHandlerResult ok or not ok indication
*/
OCEntityHandlerResult Light4Resource::post(QueryParamsMap queries, const OCRepresentation& rep)
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
        testExplorerHat->myParamArgs[0] = 4;
        testExplorerHat->myParamArgs[1] = m_var_value_value ? 1 : 0;
        testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeLight", 2, testExplorerHat->myParamArgs);
    }
    return ehResult;
}
/*
* Check if the interface name is an registered interface name
*/
bool Light4Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Light4Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Light4Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Light4Resource, URI is : "
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
                std::cout<<"Light4Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }

            else if(request->getRequestType() == "POST")
            {
                std::cout <<"Light4Resource Post Request"<<std::endl;
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
                std::cout << "Light4Resource unsupported request type (delete,put,..)"
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
 * This Resource describes a binary switch (on/off).
 * The Property "value" is a boolean.
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
         * This Resource describes a binary switch (on/off).
         * The Property "value" is a boolean.
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
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The status of the switch.
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
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" The status of the switch.
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
        testExplorerHat->myParamArgs[0] = 1;
        testExplorerHat->myParamArgs[1] = m_var_value_value ? 1 : 0;
        testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeOutput", 2, testExplorerHat->myParamArgs);
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
 * This Resource describes a binary switch (on/off).
 * The Property "value" is a boolean.
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
         * This Resource describes a binary switch (on/off).
         * The Property "value" is a boolean.
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
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The status of the switch.
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
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" The status of the switch.
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
        testExplorerHat->myParamArgs[0] = 2;
        testExplorerHat->myParamArgs[1] = m_var_value_value ? 1 : 0;
        testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeOutput", 2, testExplorerHat->myParamArgs);
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
 * This Resource describes a binary switch (on/off).
 * The Property "value" is a boolean.
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
         * This Resource describes a binary switch (on/off).
         * The Property "value" is a boolean.
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
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The status of the switch.
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
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" The status of the switch.
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
        testExplorerHat->myParamArgs[0] = 3;
        testExplorerHat->myParamArgs[1] = m_var_value_value ? 1 : 0;
        testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeOutput", 2, testExplorerHat->myParamArgs);
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
 * class definition for class that handles /output4
 *
 * This Resource describes a binary switch (on/off).
 * The Property "value" is a boolean.
 * A value of 'true' means that the switch is on.
 * A value of 'false' means that the switch is off.
*/
class Output4Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Output4Resource(std::string resourceUri = "/output4");

        /*
         * destructor
         */
         virtual ~Output4Resource(void);

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
         * Make the payload for the retrieve function (e.g. GET) /output4
         * This Resource describes a binary switch (on/off).
         * The Property "value" is a boolean.
         * A value of 'true' means that the switch is on.
         * A value of 'false' means that the switch is off.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);

        /*
         * Parse the payload for the update function (e.g. POST) /output4

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

        // member variables for path: "/output4"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The status of the switch.
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
Output4Resource::Output4Resource(std::string resourceUri)
{
    std::cout << "- Running: Output4Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /output4
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.switch.binary");
    m_var_value_value = false; // current value of property "value" The status of the switch.
    }

/*
* Destructor code
*/
Output4Resource::~Output4Resource() { }

OCStackResult Output4Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Output4Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Output4Resource." << std::endl;
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

    std::cout << "Output4Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /output4
*/
OCStackResult Output4Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /output4
* @param pResponse  the response to use for the observe
*/
OCStackResult Output4Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Make the payload for the retrieve function (e.g. GET) /output4
* @param queries  the query parameters for this call
*/
OCRepresentation Output4Resource::get(QueryParamsMap queries)
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
* Parse the payload for the update function (e.g. POST) /output4
* @param queries  the query parameters for this call
* @param rep  the response to get the property values from
* @return OCEntityHandlerResult ok or not ok indication
*/
OCEntityHandlerResult Output4Resource::post(QueryParamsMap queries, const OCRepresentation& rep)
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
        testExplorerHat->myParamArgs[0] = 4;
        testExplorerHat->myParamArgs[1] = m_var_value_value ? 1 : 0;
        testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"writeOutput", 2, testExplorerHat->myParamArgs);
    }
    return ehResult;
}
/*
* Check if the interface name is an registered interface name
*/
bool Output4Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Output4Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Output4Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Output4Resource, URI is : "
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
                std::cout<<"Output4Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }

            else if(request->getRequestType() == "POST")
            {
                std::cout <<"Output4Resource Post Request"<<std::endl;
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
                std::cout << "Output4Resource unsupported request type (delete,put,..)"
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
 * class definition for class that handles /touch1
 *
 * This Resource describes whether a touch has been sensed or not.
 * The Property "value" is a boolean.
 * A value of 'true' means that touch has been sensed.
 * A value of 'false' means that touch not been sensed.
*/
class Touch1Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Touch1Resource(std::string resourceUri = "/touch1");

        /*
         * destructor
         */
         virtual ~Touch1Resource(void);

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

        //observer callback functions
        shared_ptr<IoTObserver> m_touch1ObserverLoop;
        void touch1ObserverLoop();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /touch1
         * This Resource describes whether a touch has been sensed or not.
         * The Property "value" is a boolean.
         * A value of 'true' means that touch has been sensed.
         * A value of 'false' means that touch not been sensed.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.sensor.touch"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/touch1"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The touch sensor, true = sensed, false = not sensed.
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
Touch1Resource::Touch1Resource(std::string resourceUri)
{
    std::cout << "- Running: Touch1Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /touch1
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.sensor.touch");
    m_var_value_value = true; // current value of property "value" The touch sensor, true = sensed, false = not sensed.

    // set up the observation touch1ObserverLoop
    IoTObserverCb touch1ObsCb = bind(&Touch1Resource::touch1ObserverLoop, this);
    m_touch1ObserverLoop = make_shared<IoTObserver>(touch1ObsCb);
}

/*
* Destructor code
*/
Touch1Resource::~Touch1Resource() { }

OCStackResult Touch1Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Touch1Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Touch1Resource." << std::endl;
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

    std::cout << "Touch1Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /touch1
*/
OCStackResult Touch1Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /touch1
* @param pResponse  the response to use for the observe
*/
OCStackResult Touch1Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Observer loop for the  observe function /touch1
*/
void Touch1Resource::touch1ObserverLoop()
{
    usleep(1500000);
    std::cout << "Touch1 Observer Callback" << endl;
    testExplorerHat->myParamArgs[0] = 1;
    testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, testExplorerHat->myParamArgs);
    m_var_value_value = (bool)testExplorerHat->returnLong;

//    OCStackResult result = sendNotification();

//    if (result == OC_STACK_NO_OBSERVERS)
//    {
//        cout << "No more observers..Stopping observer loop..." << endl;
//        m_touch1ObserverLoop->stop();
//    }

    std::cout << "\t\t" << "property 'touch1' : "<< m_var_value_value << std::endl;
    m_rep.setValue(m_var_name_value, m_var_value_value);
}

/*
* Make the payload for the retrieve function (e.g. GET) /touch1
* @param queries  the query parameters for this call
*/
OCRepresentation Touch1Resource::get(QueryParamsMap queries)
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
bool Touch1Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Touch1Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Touch1Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Touch1Resource, URI is : "
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
                std::cout<<"Touch1Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Touch1Resource unsupported request type (delete,put,..)"
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
                m_touch1ObserverLoop->start();
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
 * class definition for class that handles /touch2
 *
 * This Resource describes whether a touch has been sensed or not.
 * The Property "value" is a boolean.
 * A value of 'true' means that touch has been sensed.
 * A value of 'false' means that touch not been sensed.
*/
class Touch2Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Touch2Resource(std::string resourceUri = "/touch2");

        /*
         * destructor
         */
         virtual ~Touch2Resource(void);

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

        //observer callback functions
        shared_ptr<IoTObserver> m_touch2ObserverLoop;
        void touch2ObserverLoop();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /touch2
         * This Resource describes whether a touch has been sensed or not.
         * The Property "value" is a boolean.
         * A value of 'true' means that touch has been sensed.
         * A value of 'false' means that touch not been sensed.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.sensor.touch"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/touch2"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The touch sensor, true = sensed, false = not sensed.
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
Touch2Resource::Touch2Resource(std::string resourceUri)
{
    std::cout << "- Running: Touch2Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /touch2
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.sensor.touch");
    m_var_value_value = true; // current value of property "value" The touch sensor, true = sensed, false = not sensed.

    // set up the observation touch2ObserverLoop
    IoTObserverCb touch2ObsCb = bind(&Touch2Resource::touch2ObserverLoop, this);
    m_touch2ObserverLoop = make_shared<IoTObserver>(touch2ObsCb);
}

/*
* Destructor code
*/
Touch2Resource::~Touch2Resource() { }

OCStackResult Touch2Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Touch2Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Touch2Resource." << std::endl;
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

    std::cout << "Touch2Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /touch2
*/
OCStackResult Touch2Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /touch2
* @param pResponse  the response to use for the observe
*/
OCStackResult Touch2Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Observer loop for the  observe function /touch2
*/
void Touch2Resource::touch2ObserverLoop()
{
    usleep(1500000);
    std::cout << "Touch2 Observer Callback" << endl;
    testExplorerHat->myParamArgs[0] = 2;
    testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, testExplorerHat->myParamArgs);
    m_var_value_value = (bool)testExplorerHat->returnLong;

//    OCStackResult result = sendNotification();

//    if (result == OC_STACK_NO_OBSERVERS)
//    {
//        cout << "No more observers..Stopping observer loop..." << endl;
//        m_touch2ObserverLoop->stop();
//    }

    std::cout << "\t\t" << "property 'touch2' : "<< m_var_value_value << std::endl;
    m_rep.setValue(m_var_name_value, m_var_value_value);
}

/*
* Make the payload for the retrieve function (e.g. GET) /touch2
* @param queries  the query parameters for this call
*/
OCRepresentation Touch2Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);

	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables
  testExplorerHat->myParamArgs[0] = 2;
  testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, testExplorerHat->myParamArgs);
  m_var_value_value = (bool)testExplorerHat->returnLong;

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
bool Touch2Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Touch2Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Touch2Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Touch2Resource, URI is : "
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
                std::cout<<"Touch2Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Touch2Resource unsupported request type (delete,put,..)"
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
                m_touch2ObserverLoop->start();
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
 * class definition for class that handles /touch3
 *
 * This Resource describes whether a touch has been sensed or not.
 * The Property "value" is a boolean.
 * A value of 'true' means that touch has been sensed.
 * A value of 'false' means that touch not been sensed.
*/
class Touch3Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Touch3Resource(std::string resourceUri = "/touch3");

        /*
         * destructor
         */
         virtual ~Touch3Resource(void);

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

        //observer callback functions
        shared_ptr<IoTObserver> m_touch3ObserverLoop;
        void touch3ObserverLoop();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /touch3
         * This Resource describes whether a touch has been sensed or not.
         * The Property "value" is a boolean.
         * A value of 'true' means that touch has been sensed.
         * A value of 'false' means that touch not been sensed.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.sensor.touch"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/touch3"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The touch sensor, true = sensed, false = not sensed.
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
Touch3Resource::Touch3Resource(std::string resourceUri)
{
    std::cout << "- Running: Touch3Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /touch3
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.sensor.touch");
    m_var_value_value = true; // current value of property "value" The touch sensor, true = sensed, false = not sensed.

    // set up the observation touch3ObserverLoop
    IoTObserverCb touch3ObsCb = bind(&Touch3Resource::touch3ObserverLoop, this);
    m_touch3ObserverLoop = make_shared<IoTObserver>(touch3ObsCb);
}

/*
* Destructor code
*/
Touch3Resource::~Touch3Resource() { }

OCStackResult Touch3Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Touch3Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Touch3Resource." << std::endl;
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

    std::cout << "Touch3Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /touch3
*/
OCStackResult Touch3Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /touch3
* @param pResponse  the response to use for the observe
*/
OCStackResult Touch3Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Observer loop for the  observe function /touch3
*/
void Touch3Resource::touch3ObserverLoop()
{
    usleep(1500000);
    std::cout << "Touch1 Observer Callback" << endl;
    testExplorerHat->myParamArgs[0] = 3;
    testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, testExplorerHat->myParamArgs);
    m_var_value_value = (bool)testExplorerHat->returnLong;

//    OCStackResult result = sendNotification();

//    if (result == OC_STACK_NO_OBSERVERS)
//    {
//        cout << "No more observers..Stopping observer loop..." << endl;
//        m_touch3ObserverLoop->stop();
//    }

    std::cout << "\t\t" << "property 'touch3' : "<< m_var_value_value << std::endl;
    m_rep.setValue(m_var_name_value, m_var_value_value);
}

/*
* Make the payload for the retrieve function (e.g. GET) /touch3
* @param queries  the query parameters for this call
*/
OCRepresentation Touch3Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);

	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables
  testExplorerHat->myParamArgs[0] = 3;
  testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, testExplorerHat->myParamArgs);
  m_var_value_value = (bool)testExplorerHat->returnLong;

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
bool Touch3Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Touch3Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Touch3Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Touch3Resource, URI is : "
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
                std::cout<<"Touch3Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Touch3Resource unsupported request type (delete,put,..)"
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
                m_touch3ObserverLoop->start();
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
 * class definition for class that handles /touch4
 *
 * This Resource describes whether a touch has been sensed or not.
 * The Property "value" is a boolean.
 * A value of 'true' means that touch has been sensed.
 * A value of 'false' means that touch not been sensed.
*/
class Touch4Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Touch4Resource(std::string resourceUri = "/touch4");

        /*
         * destructor
         */
         virtual ~Touch4Resource(void);

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

        //observer callback functions
        shared_ptr<IoTObserver> m_touch4ObserverLoop;
        void touch4ObserverLoop();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /touch4
         * This Resource describes whether a touch has been sensed or not.
         * The Property "value" is a boolean.
         * A value of 'true' means that touch has been sensed.
         * A value of 'false' means that touch not been sensed.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.sensor.touch"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/touch4"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The touch sensor, true = sensed, false = not sensed.
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
Touch4Resource::Touch4Resource(std::string resourceUri)
{
    std::cout << "- Running: Touch4Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /touch4
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.sensor.touch");
    m_var_value_value = true; // current value of property "value" The touch sensor, true = sensed, false = not sensed.

    // set up the observation touch4ObserverLoop
    IoTObserverCb touch4ObsCb = bind(&Touch4Resource::touch4ObserverLoop, this);
    m_touch4ObserverLoop = make_shared<IoTObserver>(touch4ObsCb);
}

/*
* Destructor code
*/
Touch4Resource::~Touch4Resource() { }

OCStackResult Touch4Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Touch4Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Touch4Resource." << std::endl;
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

    std::cout << "Touch4Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /touch4
*/
OCStackResult Touch4Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /touch4
* @param pResponse  the response to use for the observe
*/
OCStackResult Touch4Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Observer loop for the  observe function /touch4
*/
void Touch4Resource::touch4ObserverLoop()
{
    usleep(1500000);
    std::cout << "Touch4 Observer Callback" << endl;
    testExplorerHat->myParamArgs[0] = 4;
    testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, testExplorerHat->myParamArgs);
    m_var_value_value = (bool)testExplorerHat->returnLong;

//    OCStackResult result = sendNotification();

//    if (result == OC_STACK_NO_OBSERVERS)
//    {
//        cout << "No more observers..Stopping observer loop..." << endl;
//        m_touch4ObserverLoop->stop();
//    }

    std::cout << "\t\t" << "property 'touch4' : "<< m_var_value_value << std::endl;
    m_rep.setValue(m_var_name_value, m_var_value_value);
}

/*
* Make the payload for the retrieve function (e.g. GET) /touch4
* @param queries  the query parameters for this call
*/
OCRepresentation Touch4Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);

	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables
  testExplorerHat->myParamArgs[0] = 4;
  testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, testExplorerHat->myParamArgs);
  m_var_value_value = (bool)testExplorerHat->returnLong;

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
bool Touch4Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Touch4Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Touch4Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Touch4Resource, URI is : "
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
                std::cout<<"Touch4Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Touch4Resource unsupported request type (delete,put,..)"
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
                m_touch4ObserverLoop->start();
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
 * class definition for class that handles /touch5
 *
 * This Resource describes whether a touch has been sensed or not.
 * The Property "value" is a boolean.
 * A value of 'true' means that touch has been sensed.
 * A value of 'false' means that touch not been sensed.
*/
class Touch5Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Touch5Resource(std::string resourceUri = "/touch5");

        /*
         * destructor
         */
         virtual ~Touch5Resource(void);

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

        //observer callback functions
        shared_ptr<IoTObserver> m_touch5ObserverLoop;
        void touch5ObserverLoop();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /touch5
         * This Resource describes whether a touch has been sensed or not.
         * The Property "value" is a boolean.
         * A value of 'true' means that touch has been sensed.
         * A value of 'false' means that touch not been sensed.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.sensor.touch"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/touch5"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The touch sensor, true = sensed, false = not sensed.
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
Touch5Resource::Touch5Resource(std::string resourceUri)
{
    std::cout << "- Running: Touch5Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /touch5
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.sensor.touch");
    m_var_value_value = true; // current value of property "value" The touch sensor, true = sensed, false = not sensed.

    // set up the observation touch5ObserverLoop
    IoTObserverCb touch5ObsCb = bind(&Touch5Resource::touch5ObserverLoop, this);
    m_touch5ObserverLoop = make_shared<IoTObserver>(touch5ObsCb);
}

/*
* Destructor code
*/
Touch5Resource::~Touch5Resource() { }

OCStackResult Touch5Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Touch5Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Touch5Resource." << std::endl;
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

    std::cout << "Touch5Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /touch5
*/
OCStackResult Touch5Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /touch5
* @param pResponse  the response to use for the observe
*/
OCStackResult Touch5Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Observer loop for the  observe function /touch1
*/
void Touch5Resource::touch5ObserverLoop()
{
    usleep(1500000);
    std::cout << "Touch5 Observer Callback" << endl;
    testExplorerHat->myParamArgs[0] = 5;
    testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, testExplorerHat->myParamArgs);
    m_var_value_value = (bool)testExplorerHat->returnLong;

//    OCStackResult result = sendNotification();

//    if (result == OC_STACK_NO_OBSERVERS)
//    {
//        cout << "No more observers..Stopping observer loop..." << endl;
//        m_touch5ObserverLoop->stop();
//    }

    std::cout << "\t\t" << "property 'touch5' : "<< m_var_value_value << std::endl;
    m_rep.setValue(m_var_name_value, m_var_value_value);
}

/*
* Make the payload for the retrieve function (e.g. GET) /touch5
* @param queries  the query parameters for this call
*/
OCRepresentation Touch5Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);

	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables
  testExplorerHat->myParamArgs[0] = 5;
  testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, testExplorerHat->myParamArgs);
  m_var_value_value = (bool)testExplorerHat->returnLong;

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
bool Touch5Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Touch5Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Touch5Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Touch5Resource, URI is : "
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
                std::cout<<"Touch5Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Touch5Resource unsupported request type (delete,put,..)"
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
                m_touch5ObserverLoop->start();
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
 * class definition for class that handles /touch6
 *
 * This Resource describes whether a touch has been sensed or not.
 * The Property "value" is a boolean.
 * A value of 'true' means that touch has been sensed.
 * A value of 'false' means that touch not been sensed.
*/
class Touch6Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Touch6Resource(std::string resourceUri = "/touch6");

        /*
         * destructor
         */
         virtual ~Touch6Resource(void);

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

        //observer callback functions
        shared_ptr<IoTObserver> m_touch6ObserverLoop;
        void touch6ObserverLoop();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /touch6
         * This Resource describes whether a touch has been sensed or not.
         * The Property "value" is a boolean.
         * A value of 'true' means that touch has been sensed.
         * A value of 'false' means that touch not been sensed.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.sensor.touch"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/touch6"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The touch sensor, true = sensed, false = not sensed.
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
Touch6Resource::Touch6Resource(std::string resourceUri)
{
    std::cout << "- Running: Touch6Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /touch6
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.sensor.touch");
    m_var_value_value = true; // current value of property "value" The touch sensor, true = sensed, false = not sensed.

    // set up the observation touch6ObserverLoop
    IoTObserverCb touch6ObsCb = bind(&Touch6Resource::touch6ObserverLoop, this);
    m_touch6ObserverLoop = make_shared<IoTObserver>(touch6ObsCb);
}

/*
* Destructor code
*/
Touch6Resource::~Touch6Resource() { }

OCStackResult Touch6Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Touch6Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Touch6Resource." << std::endl;
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

    std::cout << "Touch6Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /touch6
*/
OCStackResult Touch6Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /touch6
* @param pResponse  the response to use for the observe
*/
OCStackResult Touch6Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Observer loop for the  observe function /touch6
*/
void Touch6Resource::touch6ObserverLoop()
{
    usleep(1500000);
    std::cout << "Touch6 Observer Callback" << endl;
    testExplorerHat->myParamArgs[0] = 6;
    testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, testExplorerHat->myParamArgs);
    m_var_value_value = (bool)testExplorerHat->returnLong;

//    OCStackResult result = sendNotification();

//    if (result == OC_STACK_NO_OBSERVERS)
//    {
//        cout << "No more observers..Stopping observer loop..." << endl;
//        m_touch6ObserverLoop->stop();
//    }

    std::cout << "\t\t" << "property 'touch6' : "<< m_var_value_value << std::endl;
    m_rep.setValue(m_var_name_value, m_var_value_value);
}

/*
* Make the payload for the retrieve function (e.g. GET) /touch6
* @param queries  the query parameters for this call
*/
OCRepresentation Touch6Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);

	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables
  testExplorerHat->myParamArgs[0] = 6;
  testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, testExplorerHat->myParamArgs);
  m_var_value_value = (bool)testExplorerHat->returnLong;

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
bool Touch6Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Touch6Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Touch6Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Touch6Resource, URI is : "
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
                std::cout<<"Touch6Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Touch6Resource unsupported request type (delete,put,..)"
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
                m_touch6ObserverLoop->start();
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
 * class definition for class that handles /touch7
 *
 * This Resource describes whether a touch has been sensed or not.
 * The Property "value" is a boolean.
 * A value of 'true' means that touch has been sensed.
 * A value of 'false' means that touch not been sensed.
*/
class Touch7Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Touch7Resource(std::string resourceUri = "/touch7");

        /*
         * destructor
         */
         virtual ~Touch7Resource(void);

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

        //observer callback functions
        shared_ptr<IoTObserver> m_touch7ObserverLoop;
        void touch7ObserverLoop();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /touch7
         * This Resource describes whether a touch has been sensed or not.
         * The Property "value" is a boolean.
         * A value of 'true' means that touch has been sensed.
         * A value of 'false' means that touch not been sensed.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.sensor.touch"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/touch7"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The touch sensor, true = sensed, false = not sensed.
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
Touch7Resource::Touch7Resource(std::string resourceUri)
{
    std::cout << "- Running: Touch7Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /touch7
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.sensor.touch");
    m_var_value_value = true; // current value of property "value" The touch sensor, true = sensed, false = not sensed.

    // set up the observation touch7ObserverLoop
    IoTObserverCb touch7ObsCb = bind(&Touch7Resource::touch7ObserverLoop, this);
    m_touch7ObserverLoop = make_shared<IoTObserver>(touch7ObsCb);
}

/*
* Destructor code
*/
Touch7Resource::~Touch7Resource() { }

OCStackResult Touch7Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Touch7Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Touch7Resource." << std::endl;
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

    std::cout << "Touch7Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /touch7
*/
OCStackResult Touch7Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /touch7
* @param pResponse  the response to use for the observe
*/
OCStackResult Touch7Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Observer loop for the  observe function /touch7
*/
void Touch7Resource::touch7ObserverLoop()
{
    usleep(1500000);
    std::cout << "Touch7 Observer Callback" << endl;
    testExplorerHat->myParamArgs[0] = 7;
    testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, testExplorerHat->myParamArgs);
    m_var_value_value = (bool)testExplorerHat->returnLong;

//    OCStackResult result = sendNotification();

//    if (result == OC_STACK_NO_OBSERVERS)
//    {
//        cout << "No more observers..Stopping observer loop..." << endl;
//        m_touch7ObserverLoop->stop();
//    }

    std::cout << "\t\t" << "property 'touch7' : "<< m_var_value_value << std::endl;
    m_rep.setValue(m_var_name_value, m_var_value_value);
}

/*
* Make the payload for the retrieve function (e.g. GET) /touch7
* @param queries  the query parameters for this call
*/
OCRepresentation Touch7Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);

	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables
  testExplorerHat->myParamArgs[0] = 7;
  testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, testExplorerHat->myParamArgs);
  m_var_value_value = (bool)testExplorerHat->returnLong;

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
bool Touch7Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Touch7Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Touch7Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Touch7Resource, URI is : "
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
                std::cout<<"Touch7Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Touch7Resource unsupported request type (delete,put,..)"
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
                m_touch7ObserverLoop->start();
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
 * class definition for class that handles /touch8
 *
 * This Resource describes whether a touch has been sensed or not.
 * The Property "value" is a boolean.
 * A value of 'true' means that touch has been sensed.
 * A value of 'false' means that touch not been sensed.
*/
class Touch8Resource : public Resource
{
    public:
        /*
         * constructor
         *
         * @param resourceUri the uri for this resource
         */
        Touch8Resource(std::string resourceUri = "/touch8");

        /*
         * destructor
         */
         virtual ~Touch8Resource(void);

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

        //observer callback functions
        shared_ptr<IoTObserver> m_touch8ObserverLoop;
        void touch8ObserverLoop();
    private:

        /*
         * Make the payload for the retrieve function (e.g. GET) /touch8
         * This Resource describes whether a touch has been sensed or not.
         * The Property "value" is a boolean.
         * A value of 'true' means that touch has been sensed.
         * A value of 'false' means that touch not been sensed.
         * @param queries  the query parameters for this call
         */
        OCRepresentation get(OC::QueryParamsMap queries);


        std::string m_resourceUri;
        // resource types and interfaces as array..
        std::string m_RESOURCE_TYPE[1] = {"oic.r.sensor.touch"}; // rt value (as an array)
        std::string m_RESOURCE_INTERFACE[2] = {"oic.if.baseline","oic.if.a"}; // interface if (as an array)
        std::string m_IF_UPDATE[3] = {"oic.if.a", "oic.if.rw", "oic.if.baseline"}; // updateble interfaces
        ObservationIds m_interestedObservers;

        // member variables for path: "/touch8"
        std::vector<std::string>  m_var_value_if; // the value for the array attribute "if": The OCF Interface set supported by this Resource.
        std::string m_var_name_if = "if"; // the name for the attribute "if"
        std::string m_var_value_n; // the value for the attribute "n": Friendly name of the Resource
        std::string m_var_name_n = "n"; // the name for the attribute "n"
        std::vector<std::string>  m_var_value_rt; // the value for the array attribute "rt": The Resource Type.
        std::string m_var_name_rt = "rt"; // the name for the attribute "rt"
        bool m_var_value_value; // the value for the attribute "value": The touch sensor, true = sensed, false = not sensed.
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
Touch8Resource::Touch8Resource(std::string resourceUri)
{
    std::cout << "- Running: Touch8Resource constructor" << std::endl;

    m_resourceUri = resourceUri;
    // initialize member variables /touch8
    // initialize vector if  The OCF Interface set supported by this Resource.
    m_var_value_if.push_back("oic.if.baseline");
    m_var_value_if.push_back("oic.if.a");
    m_var_value_n = "";  // current value of property "n" Friendly name of the Resource
    // initialize vector rt  The Resource Type.
    m_var_value_rt.push_back("oic.r.sensor.touch");
    m_var_value_value = true; // current value of property "value" The touch sensor, true = sensed, false = not sensed.

    // set up the observation touch8ObserverLoop
    IoTObserverCb touch8ObsCb = bind(&Touch8Resource::touch8ObserverLoop, this);
    m_touch8ObserverLoop = make_shared<IoTObserver>(touch8ObsCb);
}

/*
* Destructor code
*/
Touch8Resource::~Touch8Resource() { }

OCStackResult Touch8Resource::registerResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    EntityHandler cb = std::bind(&Touch8Resource::entityHandler, this,PH::_1);
    result = OCPlatform::registerResource(m_resourceHandle,
                                          m_resourceUri,
                                          m_RESOURCE_TYPE[0],
                                          m_RESOURCE_INTERFACE[0],
                                          cb,
                                          resourceProperty);
    if(OC_STACK_OK != result)
    {
        std::cerr << "Failed to register Touch8Resource." << std::endl;
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

    std::cout << "Touch8Resource:" << std::endl;
    std::cout << "\t" << "# resource interfaces: "
              << sizeof(m_RESOURCE_INTERFACE)/sizeof(m_RESOURCE_INTERFACE[0]) << std::endl;
    std::cout << "\t" << "# resource types     : "
              << sizeof(m_RESOURCE_TYPE)/sizeof(m_RESOURCE_TYPE[0]) << std::endl;

    return result;
}

/*
* Make the payload for the observe function (e.g. GET) /touch8
*/
OCStackResult Touch8Resource::sendNotification(void)
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
* Make the payload for the observe function (e.g. GET) /touch8
* @param pResponse  the response to use for the observe
*/
OCStackResult Touch8Resource::sendNotification(const std::shared_ptr< OCResourceResponse > pResponse)
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
* Observer loop for the  observe function /touch8
*/
void Touch8Resource::touch8ObserverLoop()
{
    usleep(1500000);
    std::cout << "Touch8 Observer Callback" << endl;
    testExplorerHat->myParamArgs[0] = 8;
    testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, testExplorerHat->myParamArgs);
    m_var_value_value = (bool)testExplorerHat->returnLong;

//    OCStackResult result = sendNotification();

//    if (result == OC_STACK_NO_OBSERVERS)
//    {
//        cout << "No more observers..Stopping observer loop..." << endl;
//        m_touch8ObserverLoop->stop();
//    }

    std::cout << "\t\t" << "property 'touch8' : "<< m_var_value_value << std::endl;
    m_rep.setValue(m_var_name_value, m_var_value_value);
}

/*
* Make the payload for the retrieve function (e.g. GET) /touch8
* @param queries  the query parameters for this call
*/
OCRepresentation Touch8Resource::get(QueryParamsMap queries)
{
    OC_UNUSED(queries);

	// TODO: SENSOR add here the code to talk to the HW if one implements a sensor.
	// the calls needs to fill in the member variable before it is returned.
	// alternative is to have a callback from the hardware that sets the member variables
  testExplorerHat->myParamArgs[0] = 8;
  testExplorerHat->CallPythonFunction((char *)"explorer-hat-pro", (char *)"readTouch", 1, testExplorerHat->myParamArgs);
  m_var_value_value = (bool)testExplorerHat->returnLong;

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
bool Touch8Resource::in_updatable_interfaces(std::string interface_name)
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
OCEntityHandlerResult Touch8Resource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    //std::cout << "In entity handler for Touch8Resource " << std::endl;

    if(request)
    {
        std::cout << "In entity handler for Touch8Resource, URI is : "
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
                std::cout<<"Touch8Resource Get Request"<< std::endl;

                pResponse->setResourceRepresentation(get(queries), "");
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
            }
else
            {
                std::cout << "Touch8Resource unsupported request type (delete,put,..)"
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
                m_touch8ObserverLoop->start();
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
        Analog1Resource  m_analog1Instance;
        Analog2Resource  m_analog2Instance;
        Analog3Resource  m_analog3Instance;
        Analog4Resource  m_analog4Instance;
        Input1Resource  m_input1Instance;
        Input2Resource  m_input2Instance;
        Input3Resource  m_input3Instance;
        Input4Resource  m_input4Instance;
        Light1Resource  m_light1Instance;
        Light2Resource  m_light2Instance;
        Light3Resource  m_light3Instance;
        Light4Resource  m_light4Instance;
        Output1Resource  m_output1Instance;
        Output2Resource  m_output2Instance;
        Output3Resource  m_output3Instance;
        Output4Resource  m_output4Instance;
        Touch1Resource  m_touch1Instance;
        Touch2Resource  m_touch2Instance;
        Touch3Resource  m_touch3Instance;
        Touch4Resource  m_touch4Instance;
        Touch5Resource  m_touch5Instance;
        Touch6Resource  m_touch6Instance;
        Touch7Resource  m_touch7Instance;
        Touch8Resource  m_touch8Instance;};

IoTServer::IoTServer()
    :m_analog1Instance(),
     m_analog2Instance(),
     m_analog3Instance(),
     m_analog4Instance(),
     m_input1Instance(),
     m_input2Instance(),
     m_input3Instance(),
     m_input4Instance(),
     m_light1Instance(),
     m_light2Instance(),
     m_light3Instance(),
     m_light4Instance(),
     m_output1Instance(),
     m_output2Instance(),
     m_output3Instance(),
     m_output4Instance(),
     m_touch1Instance(),
     m_touch2Instance(),
     m_touch3Instance(),
     m_touch4Instance(),
     m_touch5Instance(),
     m_touch6Instance(),
     m_touch7Instance(),
     m_touch8Instance()
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
    result = m_analog1Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_analog2Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_analog3Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_analog4Instance.registerResource(resourceProperty);
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
    result = m_input4Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_light1Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_light2Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_light3Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_light4Instance.registerResource(resourceProperty);
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
    result = m_output4Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_touch1Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_touch2Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_touch3Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_touch4Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_touch5Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_touch6Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_touch7Instance.registerResource(resourceProperty);
    if(OC_STACK_OK != result)
    {
        return result;
    }
    result = m_touch8Instance.registerResource(resourceProperty);
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
    std::string  deviceName = "Touch Sensor";
    std::string  deviceType = "oic.d.cooldevice";
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
