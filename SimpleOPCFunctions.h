#ifndef _SIMPLEOPCFUNCTIONS_H
#define _SIMPLEOPCFUNCTIONS_H
#include <atlbase.h>    // required for using the "_T" macro
#include <iostream>
#include <ObjIdl.h>

#include "SimpleOPCClient/opcda.h"
#include "SimpleOPCClient/opcerror.h"

// Group and settings default names to work properly
#define OPC_SERVER_NAME L"Matrikon.OPC.Simulation.1"
#define VT VT_R4

IOPCServer *InstantiateServer(wchar_t ServerName[]);
void AddTheGroup(IOPCServer* pIOPCServer, IOPCItemMgt* &pIOPCItemMgt, 
				 OPCHANDLE& hServerGroup);
void AddTheItem(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE& hServerItem,wchar_t *item_id);
void ReadItem(IUnknown* pGroupIUnknown, OPCHANDLE hServerItem, VARIANT& varValue);
void RemoveItem(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE hServerItem);
void RemoveGroup(IOPCServer* pIOPCServer, OPCHANDLE hServerGroup);
void SetDataCallback(IUnknown* pGroupIUnknown, IOPCDataCallback* pSOCDataCallback,
					 IConnectionPoint* &pIConnectionPoint, DWORD *pdwCookie);
bool VarToStr (VARIANT pvar, char *buffer);

#endif
