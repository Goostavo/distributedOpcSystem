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

#ifndef _OPCCLASS_H
#define _OPCCLASS_H
using namespace std;

#include <vector>
#include "SimpleOPCClient/opcda.h"
#include "SimpleOPCClient/opcerror.h"
#include "SimpleOPCClient/SOCDataCallback.h"
#include "SimpleOPCFunctions.h"

class Opcclass
{
	public:
			Opcclass();
			~Opcclass();
            
            void Opcclass::AddGroup();
            void Opcclass::AddItem(wchar_t *endereco);
            void Opcclass::ConfigCallback();
            void Opcclass::ActivatePulling();
            void Opcclass::DeactivatePulling();

    private:
            void Opcclass::SetGroupAF(IUnknown* pGroupIUnknown, BOOL ActiveFlag);

    	    IOPCServer* pIOPCServer;   //pointer to IOPServer interface
            IOPCItemMgt* pIOPCItemMgt; //pointer to IOPCItemMgt interface
            IConnectionPoint* pIConnectionPoint; //pointer to IConnectionPoint Interface
            SOCDataCallback* pSOCDataCallback;

	        OPCHANDLE hServerGroup; // server handle to the group
            vector<OPCHANDLE> HServerItems_Vec;   //Store the server handles to the item
            vector<wchar_t*> HServerItems_Name;  //Store the client names to the item
            char buf[100];
            wchar_t *ITEM_ID;
            int nItens;
};

#endif

