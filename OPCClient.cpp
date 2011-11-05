#include <iostream>
#include <process.h>
#include <iostream>
#include "OPCClient.h"
#include "Opcclass.h"
#include "SocketClient.h"
#include "MailslotOperations.h"

DWORD WINAPI dwOPC()
{
    MSG msg;
    int bRet;
    
    Opcclass *Opc_tp = new Opcclass();			//Instancia Classe OPC
	Opc_tp->AddGroup();

    Opc_tp->AddItem((wchar_t *)L"Random.Boolean");
    Opc_tp->AddItem((wchar_t *)L"Random.Int1");
    Opc_tp->AddItem((wchar_t *)L"Random.Int2");
    Opc_tp->AddItem((wchar_t *)L"Random.Real4");

    Opc_tp->ConfigCallback();
    Opc_tp->ActivatePulling();

    do {
		bRet = GetMessage( &msg, NULL, 0, 0 );
		if (!bRet){
			printf ("Failed to get windows message! Error code = %d\n", GetLastError());
			exit(0);
		}
		TranslateMessage(&msg); // This call is not really needed ...
		DispatchMessage(&msg);  // ... but this one is!
	} while(1);
    //Ending the Thread
    _endthreadex((DWORD) 0);
    return 0;
}
