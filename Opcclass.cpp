#include "Opcclass.h"
#include "SimpleOPCFunctions.h"

//Object Constructor
//This constructor will be responsible to run some steps that have to be
// done when we're using microsoft COM library, these steps are:
// 1- Initialisation of the COM library
// 2- Instatiation of OPC's Server Interface
Opcclass::Opcclass()
{
    //Initializing some variables
    pIOPCServer = NULL;   //pointer to IOPServer interface
    pIOPCItemMgt = NULL;  //pointer to IOPCItemMgt interface
    pIConnectionPoint = NULL; //pointer to IConnectionPoint Interface
    pSOCDataCallback = NULL; //Pointer to datacallback class
    nItens = 0;

    // Have to be done before using microsoft COM library:
    std::cout << "Initializing the COM environment..." << std::endl;
	CoInitialize(NULL);

    // Let's instantiante the IOPCServer interface and get a pointer of it:
	std::cout << "Instantiating the MATRIKON OPC Server for Simulation..." << std::endl;
	pIOPCServer = InstantiateServer(OPC_SERVER_NAME);
}

//Object Destructor
//This destructor will be responsible to run the last two steps when we're
//programming at COM enviroment.
// 1- Close Data Subscription Objects
// 2- Remove the pointer to OPC Server
// 3- Close COM Enviroment.
Opcclass::~Opcclass()
{
    // Cancel the callback and release its reference
    if (pIConnectionPoint != NULL)
        pIConnectionPoint->Release();    if (pSOCDataCallback != NULL)
	    pSOCDataCallback->Release();

    // release the interface references:
	std::cout << "Removing the OPC server object..." << std::endl;
	pIOPCServer->Release();

	//close the COM library:
	std::cout << "Releasing the COM environment..." << std::endl;
	CoUninitialize();
}

void Opcclass::AddGroup()
{
	// Add the OPC group the OPC server and get an handle to the IOPCItemMgt
	//interface:
	printf("Adding a group in the INACTIVE state for the moment...\n");
	AddTheGroup(pIOPCServer, pIOPCItemMgt, hServerGroup);
}

void Opcclass::AddItem(wchar_t *item_id)
{
	// Add the OPC item. First we have to convert from wchar_t* to char*
	// in order to print the item name in the console.

    size_t m;
	wcstombs_s(&m, buf, 100, item_id, _TRUNCATE);
	printf("Adding the item %s to the group...\n", buf);
    AddTheItem(pIOPCItemMgt, hServerItem, item_id);
}

void Opcclass::ConfigCallback(void)
{
    // Establish a callback asynchronous read by means of the IOPCDaraCallback
	// (OPC DA 2.0) method. We first instantiate a new SOCDataCallback object and
	// adjusts its reference count, and then call a wrapper function to
	// setup the callback.
	DWORD dwCookie = 0;
	pSOCDataCallback = new SOCDataCallback();
	pSOCDataCallback->AddRef();
	//printf("Setting up the IConnectionPoint callback connection...\n");
	SetDataCallback(pIOPCItemMgt, pSOCDataCallback, pIConnectionPoint, &dwCookie);
}