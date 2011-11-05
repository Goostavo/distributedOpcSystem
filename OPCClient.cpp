#include <iostream>
#include <process.h>
#include <iostream>
#include "OPCClient.h"
#include "Opcclass.h"
#include "SocketClient.h"
#include "MailslotOperations.h"

typedef unsigned (WINAPI *CAST_FUNCTION)(LPVOID);
typedef unsigned *CAST_LPDWORD;

//Main function of the OPC_Thread
DWORD WINAPI dwOPC()
{
    HANDLE hCommunication;


    MSG msg;        //Used by the windows message buffer
    int bRet;       //Return values
    
    Opcclass *Opc_tp = new Opcclass();			//Instantiate OPC Class
	Opc_tp->AddGroup();                         //Add the main group

    //Add the random Items that will be read
    Opc_tp->AddItem((wchar_t *)L"Random.Boolean");
    Opc_tp->AddItem((wchar_t *)L"Random.Int1");
    Opc_tp->AddItem((wchar_t *)L"Random.Int2");
    Opc_tp->AddItem((wchar_t *)L"Random.Real4");
    //Add the default Items that will be Writen
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.Int1");
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.Int2");
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.Real4");
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.Real8");
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.Int4");
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.UInt1");
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.UInt2");
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.UInt4");

    //Set the callback configuration
    Opc_tp->ConfigCallback();
    //Activate the callbacks
    Opc_tp->ActivateAsyncRead();

    hCommunication = (HANDLE) _beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)dwCommunication,
        Opc_tp,
        0,
        NULL);

    //Keep processing the message loops
    do {
		bRet = GetMessage( &msg, NULL, 0, 0 );
		if (!bRet){
			printf ("Failed to get windows message! Error code = %d\n", GetLastError());
			exit(0);
		}
		TranslateMessage(&msg); // This call is not really needed ...
		DispatchMessage(&msg);  // ... but this one is!
	} while(1);

    CloseHandle (hCommunication);
    //Ending the Thread
    _endthreadex((DWORD) 0);
    return 0;
}


DWORD WINAPI dwCommunication (LPVOID opc)

{
    HANDLE hMailslotOPC = CreateMailslot(
        (LPCSTR)"\\\\.\\mailslot\\OPCTCP",
        0,
        MAILSLOT_WAIT_FOREVER,
        NULL);
    HANDLE hSyncOPC=OpenEvent(EVENT_MODIFY_STATE,FALSE,(LPCSTR)"SyncOPCTCP");
    SetEvent(hSyncOPC);
    while (true)
    {
        ReadSlot(hMailslotOPC);
    }

    CloseHandle (hMailslotOPC);
    _endthreadex((DWORD) 0);
    return 0;
}