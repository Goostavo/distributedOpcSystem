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
    DWORD dwThreadID;   //Where the Thread ID will be deposited when created a Thread
    string log;

    //MailSlot Sync event
    hSync = OpenEvent(EVENT_MODIFY_STATE, FALSE, (LPCSTR)"Synchronization");

    //Creating Threads
    //Create the Socket Thread, it creates thread to handle the TCP connection.
    hSocket = (HANDLE) _beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)dwSocket,
        L"SocketClient",
        0,
        (CAST_LPDWORD)&dwThreadID);

    //Create the OPC client thread, it handles the communication with the OPC server.
    hOPC = (HANDLE) _beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)dwOPC,
        L"OPCClient",
        0,
        (CAST_LPDWORD)&dwThreadID);
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

    //Read Messages sent by the OPC Client thread.
    while (true)
    {
       ReadSlot(hMailslot);
       Sleep(100);
    }

    //Wait for the Threads
    WaitForSingleObject(hSocket, INFINITE);
    WaitForSingleObject(hOPC, INFINITE);

    //Closing all Handles
    CloseHandle(hSocket);
    CloseHandle(hOPC);
    CloseHandle(hMailslot);

	system("pause");
	return 0;
}




