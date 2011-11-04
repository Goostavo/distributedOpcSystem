#include "Opcclass.h"
#include "SimpleOPCFunctions.h"


void Opcclass::Addgroup()
{
	// Add the OPC group the OPC server and get an handle to the IOPCItemMgt
	//interface:
	printf("Adding a group in the INACTIVE state for the moment...\n");
	AddTheGroup(pIOPCServer, pIOPCItemMgt, hServerGroup);
}

void Opcclass:AddItem()
{
	// Add the OPC item. First we have to convert from wchar_t* to char*
	// in order to print the item name in the console.
    size_t m;
	wcstombs_s(&m, buf, 100, ITEM_ID, _TRUNCATE);
	printf("Adding the item %s to the group...\n", buf);
    AddTheItem(pIOPCItemMgt, hServerItem);
}
