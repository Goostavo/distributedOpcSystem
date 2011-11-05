//Trabalho Prático de Sistemas Distribuídos Para Automação 2011/02
//Gustavo Barreto Garcia        2006048133
//Rafael Gilmar Ribeiro Gurgel  2006048192
//main.c

#include <iostream>
#include <process.h>
#include <string.h>
#include "Opcclass.h"
#include "MailslotOperations.h"
#include "SocketClient.h"
#include "OPCClient.h"

typedef unsigned (WINAPI *CAST_FUNCTION)(LPVOID);
typedef unsigned *CAST_LPDWORD;

//Threads Handles
HANDLE hSocket;     //Socket Client Thread
HANDLE hOPC;        //OPC Client Thread

//Mailslot Handles
HANDLE hMailslot;   //handle to Mailslot Server where all log info will be deposited

//Event Handles
HANDLE hSync;   //To notify when mailslot client will be ready

int main (void)
{
    std::cout << "Hello World!" << std::endl;
    DWORD dwThreadID;   //Where the Thread ID will be deposited when created a Thread
    string log;

    hSync =OpenEvent(EVENT_MODIFY_STATE, FALSE, (LPCSTR)"Synchronization");

    //Creating Threads
    hSocket = (HANDLE) _beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)dwSocket,
        L"SocketClient",
        0,
        (CAST_LPDWORD)&dwThreadID);

    hOPC = (HANDLE) _beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)dwOPC,
        L"OPCClient",
        0,
        (CAST_LPDWORD)&dwThreadID);
    //Now the data capture program will start here:
    //...
    //...
    //...
    //Creating the Mailslot Server
    hMailslot = CreateMailslot(
        (LPCSTR)"\\\\.\\mailslot\\mylogs",
        0,
        MAILSLOT_WAIT_FOREVER,
        NULL);
    if (hMailslot == INVALID_HANDLE_VALUE)
        std::cout << "Create Mailslot failed" << std::endl;
    else
        SetEvent(hSync);
    while (true)
    {
       ReadSlot(hMailslot);
       Sleep(100);
    }

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




