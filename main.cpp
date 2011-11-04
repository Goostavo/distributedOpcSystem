//Trabalho Prático de Sistemas Distribuídos Para Automação 2011/02
//Gustavo Barreto Garcia		2006048133
//Rafael Gilmar Ribeiro Gurgel	2006048192
//main.c

#include <iostream>
#include <process.h>
#include "Opcclass.h"

typedef unsigned (WINAPI *CAST_FUNCTION)(LPVOID);
typedef unsigned *CAST_LPDWORD;

//Threads Handles
HANDLE hSocket;     //Socket Client Thread
HANDLE hOPC;        //OPC Client Thread

//Mailslot Handles
HANDLE hMailslot;   //handle to Mailslot Server where all log info will be deposited
//HANDLE hSocketOPC   //Mailslot to comunicate the threads hSocket and hOPC <-THREAD INTERNAL VARIABLE???

//Threads Functions
DWORD WINAPI dwSocket();  //Socket Client Function
DWORD WINAPI dwOPC();     //OPC Client Function

int main (void)
{
	std::cout << "Hello World!" << std::endl;
    DWORD dwThreadID;   //Where the Thread ID will be deposited when created a Thread


    //Creating Threads
    hSocket = (HANDLE) _beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)dwSocket,
        L"SocketClient",
        0,
        (CAST_LPDWORD)&dwThreadID
    );

    hOPC = (HANDLE) _beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)dwOPC,
        L"OPCClient",
        0,
        (CAST_LPDWORD)&dwThreadID
    );

    //Now the data capture program will start here:
    //...
    //...
    //...
    


    //Creating the Mailslot Server


    //...
    //...
    //...
    //Finish capture program code


    //Wait for the Threads
    WaitForSingleObject(hSocket, INFINITE);
    WaitForSingleObject(hOPC, INFINITE);

    //Closing all Handles
    CloseHandle(hSocket);
    CloseHandle(hOPC);
    CloseHandle(hMailslot);

    /*  COLOCAR EM SEU DEVIDO LUGAR
	Opcclass *Opc_tp = new Opcclass();			//Instancia Classe OPC
	Opc_tp->AddGroup();

    Opc_tp->AddItem((wchar_t *)L"Random.Boolean");
    Opc_tp->AddItem((wchar_t *)L"Random.Int1");
    Opc_tp->AddItem((wchar_t *)L"Random.Int2");
    Opc_tp->AddItem((wchar_t *)L"Random.Real4");
   */
	system("pause");
	return 0;
}


DWORD WINAPI dwSocket()
{
    std::cout << "Socket Client Thread Created" << std::endl;

    //Ending the Thread
    _endthreadex((DWORD) 0);
    return 0;
}

DWORD WINAPI dwOPC()
{
    std::cout << "OPC Client Thread Created" << std::endl;

    //Ending the Thread
    _endthreadex((DWORD) 0);
    return 0;
}