#include "SimpleOPCFunctions.h"

//Global Variables
// The OPC DA Spec requires that some constants be registered in order to use
// them. The one below refers to the OPC DA 1.0 IDataObject interface.
UINT OPC_DATA_TIME = RegisterClipboardFormat (_T("OPCSTMFORMATDATATIME"));

////////////////////////////////////////////////////////////////////
// Instantiate the IOPCServer interface of the OPCServer
// having the name ServerName. Return a pointer to this interface
//
IOPCServer* InstantiateServer(wchar_t ServerName[])
{
	CLSID CLSID_OPCServer;
	HRESULT hr;

	// get the CLSID from the OPC Server Name:
	hr = CLSIDFromString(ServerName, &CLSID_OPCServer);
	_ASSERT(!FAILED(hr));


	//queue of the class instances to create
	LONG cmq = 1; // nbr of class instance to create.
	MULTI_QI queue[1] =
		{{&IID_IOPCServer,
		NULL,
		0}};

	//Server info:
	//COSERVERINFO CoServerInfo =
    //{
	//	/*dwReserved1*/ 0,
	//	/*pwszName*/ REMOTE_SERVER_NAME,
	//	/*COAUTHINFO*/  NULL,
	//	/*dwReserved2*/ 0
    //}; 

	// create an instance of the IOPCServer
	hr = CoCreateInstanceEx(CLSID_OPCServer, NULL, CLSCTX_SERVER,
		/*&CoServerInfo*/NULL, cmq, queue);
	_ASSERT(!hr);

	// return a pointer to the IOPCServer interface:
	return(IOPCServer*) queue[0].pItf;
}


/////////////////////////////////////////////////////////////////////
// Add group "Group1" to the Server whose IOPCServer interface
// is pointed by pIOPCServer. 
// Returns a pointer to the IOPCItemMgt interface of the added group
// and a server opc handle to the added group.
//
void AddTheGroup(IOPCServer* pIOPCServer, IOPCItemMgt* &pIOPCItemMgt, 
				 OPCHANDLE& hServerGroup)
{
	DWORD dwUpdateRate = 0;
	OPCHANDLE hClientGroup = 0;

	// Add an OPC group and get a pointer to the IUnknown I/F:
    HRESULT hr = pIOPCServer->AddGroup(/*szName*/ L"Group1",
		/*bActive*/ FALSE,
		/*dwRequestedUpdateRate*/ 1000,
		/*hClientGroup*/ hClientGroup,
		/*pTimeBias*/ 0,
		/*pPercentDeadband*/ 0,
		/*dwLCID*/0,
		/*phServerGroup*/&hServerGroup,
		&dwUpdateRate,
		/*riid*/ IID_IOPCItemMgt,
		/*ppUnk*/ (IUnknown**) &pIOPCItemMgt);
	_ASSERT(!FAILED(hr));
}



//////////////////////////////////////////////////////////////////
// Add the Item ITEM_ID to the group whose IOPCItemMgt interface
// is pointed by pIOPCItemMgt pointer. Return a server opc handle
// to the item.
 
void AddTheItem(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE& hServerItem,wchar_t *item_id, OPCHANDLE hClientItem)
{
	HRESULT hr;

	// Array of items to add:
	OPCITEMDEF ItemArray[1] =
	{{
	/*szAccessPath*/ L"",
	/*szItemID*/ item_id,
	/*bActive*/ TRUE,
	/*hClient*/ hClientItem,
	/*dwBlobSize*/ 0,
	/*pBlob*/ NULL,
	/*vtRequestedDataType*/ VT,
	/*wReserved*/0
	}};

	//Add Result:
	OPCITEMRESULT* pAddResult=NULL;
	HRESULT* pErrors = NULL;

	// Add an Item to the previous Group:
	hr = pIOPCItemMgt->AddItems(1, ItemArray, &pAddResult, &pErrors);
	if (hr != S_OK){
		printf("Failed call to AddItems function. Error code = %x\n", hr);
		exit(0);
	}

	// Server handle for the added item:
	hServerItem = pAddResult[0].hServer;

	// release memory allocated by the server:
	CoTaskMemFree(pAddResult->pBlob);

	CoTaskMemFree(pAddResult);
	pAddResult = NULL;

	CoTaskMemFree(pErrors);
	pErrors = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Read from device the value of the item having the "hServerItem" server 
// handle and belonging to the group whose one interface is pointed by
// pGroupIUnknown. The value is put in varValue. 
//
void ReadItem(IUnknown* pGroupIUnknown, OPCHANDLE hServerItem, VARIANT& varValue)
{
	// value of the item:
	OPCITEMSTATE* pValue = NULL;

	//get a pointer to the IOPCSyncIOInterface:
	IOPCSyncIO* pIOPCSyncIO;
	pGroupIUnknown->QueryInterface(__uuidof(pIOPCSyncIO), (void**) &pIOPCSyncIO);

	// read the item value from the device:
	HRESULT* pErrors = NULL; //to store error code(s)
	HRESULT hr = pIOPCSyncIO->Read(OPC_DS_DEVICE, 1, &hServerItem, &pValue, &pErrors);
	_ASSERT(!hr);
	_ASSERT(pValue!=NULL);

	varValue = pValue[0].vDataValue;

	//Release memeory allocated by the OPC server:
	CoTaskMemFree(pErrors);
	pErrors = NULL;

	CoTaskMemFree(pValue);
	pValue = NULL;

	// release the reference to the IOPCSyncIO interface:
	pIOPCSyncIO->Release();
}

///////////////////////////////////////////////////////////////////////////
// Remove the item whose server handle is hServerItem from the group
// whose IOPCItemMgt interface is pointed by pIOPCItemMgt
//
void RemoveItem(IOPCItemMgt* pIOPCItemMgt, OPCHANDLE hServerItem)
{
	// server handle of items to remove:
	OPCHANDLE hServerArray[1];
	hServerArray[0] = hServerItem;
	
	//Remove the item:
	HRESULT* pErrors; // to store error code(s)
	HRESULT hr = pIOPCItemMgt->RemoveItems(1, hServerArray, &pErrors);
	_ASSERT(!hr);

	//release memory allocated by the server:
	CoTaskMemFree(pErrors);
	pErrors = NULL;
}

////////////////////////////////////////////////////////////////////////
// Remove the Group whose server handle is hServerGroup from the server
// whose IOPCServer interface is pointed by pIOPCServer
//
void RemoveGroup (IOPCServer* pIOPCServer, OPCHANDLE hServerGroup)
{
	// Remove the group:
	HRESULT hr = pIOPCServer->RemoveGroup(hServerGroup, FALSE);
	if (hr != S_OK){
		if (hr == OPC_S_INUSE)
			printf ("Failed to remove OPC group: object still has references to it.\n");
		else printf ("Failed to remove OPC group. Error code = %x\n", hr);
		exit(0);
	}
}
///////////////////////////////////////////////////////////////////////////////
// Set up an asynchronous connection with the server by means of the OPC DA
// 2.0 IConnectionPointContainer
//
void SetDataCallback(
		IUnknown* pGroupIUnknown,
		IOPCDataCallback* pSOCDataCallback,
		IConnectionPoint* &pIConnectionPoint,
		DWORD *pdwCookie)
{
	HRESULT hr;

	IConnectionPointContainer* pIConnPtCont = NULL; //pointer to IConnectionPointContainer
	                                                //interface
        
	//Get a pointer to the IConnectionPointContainer interface:
    hr = pGroupIUnknown->QueryInterface(__uuidof(pIConnPtCont), (void**) &pIConnPtCont);
	if (hr != S_OK){
		printf ("Could not obtain a pointer to IConnectionPointContainer. Error = %x\n",
			    hr);
		return;
	}
	
	// Call the IConnectionPointContainer::FindConnectionPoint method on the
	// group object to obtain a Connection Point
	hr = pIConnPtCont->FindConnectionPoint(IID_IOPCDataCallback, &pIConnectionPoint);
	if (hr != S_OK) {
		printf ("Failed call to FindConnectionPoint. Error = %x\n", hr);
		//*ptkAsyncConnection = 0;
		return;
	}

	// Now set up the Connection Point.
	// TO BE DONE: in Kepware´s code the IOPCDataCallback object is instantiated
	// here, as a consequence of the FindConnectionPoint success. It makes sense,
	// for if not we would have instantiated it unnecessarly.
	hr = pIConnectionPoint->Advise(pSOCDataCallback, pdwCookie);
	if (hr != S_OK) {
		printf ("Failed call to IConnectionPoint::Advise. Error = %x\n", hr);
		*pdwCookie = 0;
	}

	// From this point on we do not need anymore the pointer to the
	// IConnectionPointContainer interface, so release it
	pIConnPtCont->Release();

	return;
}

/////////////////////////////////////////////////////////////////////////
// Function to mimic the Delphi VARTOSTR procedure in which a VARIANT
// is converted to a string. Only a few VARIANT types are supported in
// this version.
//
// Luiz T. S. Mendes - 07/09/2011
bool VarToStr (VARIANT pvar, char *buffer)
{
	bool vReturn = true;
	switch (pvar.vt & ~VT_ARRAY)
	{
		case VT_BOOL:
		case VT_I1:
			sprintf(buffer, "%d",    pvar.iVal);	break;
		case VT_I2:
			sprintf(buffer, "%d",    pvar.intVal);	break;
		case VT_I4:
			sprintf(buffer, "%ld",   pvar.intVal);	break;
		case VT_UI1:
			sprintf(buffer, "%u",    pvar.uiVal);	break;
		case VT_UI2:
			sprintf(buffer, "%u",    pvar.ulVal);	break;
		case VT_UI4:
			sprintf(buffer, "%lu",   pvar.ulVal);	break;
		case VT_R4:
			sprintf(buffer, "%6.2f", pvar.fltVal);	break;
		case VT_R8:
			sprintf(buffer, "%lu",   pvar.dblVal);	break;
		case VT_BSTR:
			sprintf(buffer, "%s",    pvar.bstrVal);	break;
		default:
			sprintf(buffer, "%s", NULL);
			vReturn = false;
			break;
	}
	return(vReturn);
}
