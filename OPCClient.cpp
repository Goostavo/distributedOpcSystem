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
    cout << "Adding a group in the INACTIVE state for the moment...\n";

    //Add the random Items that will be read
    Opc_tp->AddItem((wchar_t *)L"Random.Boolean");
    cout<< "Added item Random.Boolean to the group\n";
    Opc_tp->AddItem((wchar_t *)L"Random.Int1");
    cout<< "Added item Random.Int1 to the group\n";
    Opc_tp->AddItem((wchar_t *)L"Random.Int2");
    cout<< "Added item Random.Int2 to the group\n";
    Opc_tp->AddItem((wchar_t *)L"Random.Real4");
    cout<< "Added item Random.Real4 to the group\n";
    //Add the default Items that will be Writen
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.Int1");
    cout<< "Added item Bucket Brigade.Int1 to the group\n";
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.Int2");
    cout<< "Added item Bucket Brigade.Int2 to the group\n";
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.Real4");
    cout<< "Added item Bucket Brigade.Real4 to the group\n";
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.Real8");
    cout<< "Added item Bucket Brigade.Real8 to the group\n";
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.Int4");
    cout<< "Added item Bucket Brigade.Int4 to the group\n";
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.UInt1");
    cout<< "Added item Bucket Brigade.UInt1 to the group\n";
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.UInt2");
    cout<< "Added item Bucket Brigade.UInt2 to the group\n";
    Opc_tp->AddItem((wchar_t *)L"Bucket Brigade.UInt4");
    cout<< "Added item Bucket Brigade.UInt4 to the group\n";
    
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
    /*HANDLE hMailslotOPC = CreateMailslot(
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

    CloseHandle (hMailslotOPC);*/

    int debug_aux = 0;
    Opcclass *Opc_send = new Opcclass();			//Instantiate OPC Class
	Opc_send->AddGroup();                         //Add the main group

    //Add the default Items that will be Writen
    Opc_send->AddItem((wchar_t *)L"Bucket Brigade.Int1");
    Opc_send->AddItem((wchar_t *)L"Bucket Brigade.Int2");
    Opc_send->AddItem((wchar_t *)L"Bucket Brigade.Real4");
    Opc_send->AddItem((wchar_t *)L"Bucket Brigade.Real8");
    Opc_send->AddItem((wchar_t *)L"Bucket Brigade.Int4");
    Opc_send->AddItem((wchar_t *)L"Bucket Brigade.UInt1");
    Opc_send->AddItem((wchar_t *)L"Bucket Brigade.UInt2");
    Opc_send->AddItem((wchar_t *)L"Bucket Brigade.UInt4");

    while (1)
    {
        Opc_send->WriteItem(8, &debug_aux, VT_I2);    //Write a Item syncronously
        debug_aux++;
        Sleep(1000);
    }

    _endthreadex((DWORD) 0);
    return 0;
}