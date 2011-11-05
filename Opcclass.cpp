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
// 1- Remove the pointer to OPC Server
// 2- Close COM Enviroment.
Opcclass::~Opcclass()
{
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
