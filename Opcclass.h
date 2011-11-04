// Simple OPC Client Class
//
// This is a modified version of the "Simple OPC Client" originally
// developed by Philippe Gras (CERN) and further enhanced by Luiz T. S.
// Mendes (UFMG).
//
// The code, previously monolitic was edited with the objective to be 
// fully encapsulated inside a class.
// The public functions are based in objective of use instead of being
// based in the main OPC COM functions.

#include <iostream>

using namespace std;

#include "SimpleOPCClient/opcda.h"
#include "SimpleOPCClient/opcerror.h"
#include "SimpleOPCClient/SimpleOPCClient_v3.h"
#include "SimpleOPCClient/SOCAdviseSink.h"
#include "SimpleOPCClient/SOCDataCallback.h"
#include "SimpleOPCClient/SOCWrapperFunctions.h"


#define OPC_SERVER_NAME L"Matrikon.OPC.Simulation.1"

#ifndef _OPCCLASS_H
#define _OPCCLASS_H

class Opcclass
{
	public:
			Opcclass();
			~Opcclass();
            
            void Opcclass::AddGroup();
            void Opcclass::AddItem(wchar_t *endereco);

    private:


    	    IOPCServer* pIOPCServer;   //pointer to IOPServer interface
            IOPCItemMgt* pIOPCItemMgt; //pointer to IOPCItemMgt interface

	        OPCHANDLE hServerGroup; // server handle to the group
	        OPCHANDLE hServerItem;  // server handle to the item

            char buf[100];
            wchar_t *ITEM_ID;
            int nItens;
};

#endif

