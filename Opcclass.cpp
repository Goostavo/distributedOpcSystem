#include "Opcclass.h"

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
        pIConnectionPoint->Release();
    if (pSOCDataCallback != NULL)
	    pSOCDataCallback->Release();

    // release the interface references:
	std::cout << "Removing the OPC server object..." << std::endl;
	pIOPCServer->Release();

	//close the COM library:
	std::cout << "Releasing the COM environment..." << std::endl;
	CoUninitialize();
}

//Add the default group to the server.
void Opcclass::AddGroup()
{
	// Add the OPC group the OPC server and get an handle to the IOPCItemMgt
	//interface:
	AddTheGroup(pIOPCServer, pIOPCItemMgt, hServerGroup);
}

// Add an Item to the server using the string to define it.
void Opcclass::AddItem(wchar_t *item_id)
{
	// Add the OPC item. First we have to convert from wchar_t* to char*
	// in order to print the item name in the console.

    size_t m;
    OPCHANDLE hServerItem;
	wcstombs_s(&m, buf, 100, item_id, _TRUNCATE);
    HServerItems_Name.insert(HServerItems_Name.end(),item_id);
    AddTheItem(pIOPCItemMgt, hServerItem, item_id, (OPCHANDLE)HServerItems_Name.size());    
    HServerItems_Vec.insert(HServerItems_Vec.end(),hServerItem);
}

// Configure the OpcClass to use the DataCallback interface to do asyncronous reads
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

// Set the group active flag state
// This function should not be used directly, it is wrapped by the Activate/DeactivateAsyncRead
void Opcclass::SetGroupAF(IUnknown* pGroupIUnknown, BOOL ActiveFlag)
{
	HRESULT hr;
	IOPCGroupStateMgt* pIOPCGroupStateMgt;
	DWORD RevisedUpdateRate;

	// Get a pointer to the IOPCGroupStateMgt interface:
    hr = pGroupIUnknown->QueryInterface(__uuidof(pIOPCGroupStateMgt),
		                               (void**) &pIOPCGroupStateMgt);
	if (hr != S_OK){
		printf ("Could not obtain a pointer to IOPCGroupStateMgt. Error = %x\n", hr);
		return;
	}
	// Set the state to Active. Since the other group properties are to remain
	// unchanged we pass NULL pointers to them as suggested by the OPC DA Spec.
	hr = pIOPCGroupStateMgt->SetState(
		    NULL,                // *pRequestedUpdateRate
			&RevisedUpdateRate,  // *pRevisedUpdateRate - can�t be NULL
			&ActiveFlag,		 // *pActive
			NULL,				 // *pTimeBias
			NULL,				 // *pPercentDeadband
			NULL,				 // *pLCID
			NULL);				 // *phClientGroup
                     
	if (hr != S_OK)
		printf ("Failed call to IOPCGroupMgt::SetState. Error = %x\n", hr);
	else
		// Free the pointer since we will not use it anymore.
		pIOPCGroupStateMgt->Release();

	return; 
}

// Activate the Asyncronous read callbacks
void Opcclass::ActivateAsyncRead(void)
{
    Opcclass::SetGroupAF(pIOPCItemMgt,1);
}

// Deactivate the assyncronous read calbacks
void Opcclass::DeactivateAsyncRead(void)
{
    Opcclass::SetGroupAF(pIOPCItemMgt,0);
}

///////////////////////////////////////////////////////////////////////////////
// Write the varValue value of the type var_type on the hClientItem Item 
// on the group/server defined at Opcclass initialization;

void Opcclass::WriteItem(int hClientItem, void* varValue, VARTYPE var_type)
{
	// value of the item:
	OPCITEMSTATE* pValue = NULL;
    OPCHANDLE hServerItem = HServerItems_Vec[hClientItem-1];
    VARIANT tempVariant;
    VariantInit(&tempVariant);

    //Prepare the VARIANT type
    GenerateVar(&tempVariant, var_type, varValue);
	//get a pointer to the IOPCSyncIOInterface:
	IOPCSyncIO* pIOPCSyncIO;
	pIOPCItemMgt->QueryInterface(__uuidof(pIOPCSyncIO), (void**) &pIOPCSyncIO);

	// read the item value from the device:
	HRESULT* pErrors = NULL; //to store error code(s)
	HRESULT hr = pIOPCSyncIO->Write(1, &hServerItem, &tempVariant, &pErrors);
	_ASSERT(!hr);

	//Release memeory allocated by the OPC server:
	CoTaskMemFree(pErrors);
	pErrors = NULL;

	CoTaskMemFree(pValue);
	pValue = NULL;

	// release the reference to the IOPCSyncIO interface:
	pIOPCSyncIO->Release();
}
