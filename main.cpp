//Trabalho Prático de Sistemas Distribuídos Para Automação 2011/02
//Gustavo Barreto Garcia		2006048133
//Rafael Gilmar Ribeiro Gurgel	2006048192
//main.c

#include <iostream>
#include <process.h>
#include <string.h>
#include <tchar.h>//ler mailslot -> pode mudar localizacao
//#include <strsafe.h>//ler mailslot -> pode mudar localizacao
#include "Opcclass.h"

typedef unsigned (WINAPI *CAST_FUNCTION)(LPVOID);
typedef unsigned *CAST_LPDWORD;


BOOL WriteSlot(HANDLE hSlot, LPTSTR lpszMessage)
{
   BOOL fResult; 
   DWORD cbWritten; 
 
   fResult = WriteFile(hSlot, 
     lpszMessage, 
     (DWORD) (lstrlen(lpszMessage)+1)*sizeof(TCHAR),  
     &cbWritten, 
     (LPOVERLAPPED) NULL); 
 
   if (!fResult) 
   { 
      printf("WriteFile failed.\n"); 
      return FALSE; 
   } 
 
   printf("Slot written to successfully.\n"); 

   return TRUE;
}


BOOL ReadSlot(HANDLE hSlot) 
{ 
    DWORD cbMessage, cMessage, cbRead; 
    BOOL fResult; 
    LPTSTR lpszBuffer; 
    TCHAR achID[80]; 
    DWORD cAllMessages; 
    HANDLE hEvent;
    OVERLAPPED ov;
 
    cbMessage = cMessage = cbRead = 0; 

    hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("ExampleSlot"));
    if( NULL == hEvent )
        return FALSE;
    ov.Offset = 0;
    ov.OffsetHigh = 0;
    ov.hEvent = hEvent;
 
    fResult = GetMailslotInfo( hSlot, // mailslot handle 
        (LPDWORD) NULL,               // no maximum message size 
        &cbMessage,                   // size of next message 
        &cMessage,                    // number of messages 
        (LPDWORD) NULL);              // no read time-out 
 
    if (!fResult) 
    { 
        printf("GetMailslotInfo failed\n"); 
        return FALSE; 
    } 
 
    if (cbMessage == MAILSLOT_NO_MESSAGE) 
    { 
        //std::cout << "Waiting for a message..." << std::endl; 
        return TRUE; 
    } 
 
    cAllMessages = cMessage; 
 
    while (cMessage != 0)  // retrieve all messages
    { 
        // Allocate memory for the message. 

        lpszBuffer = (LPTSTR) GlobalAlloc(GPTR, 
            lstrlen((LPTSTR) achID)*sizeof(TCHAR) + cbMessage); 
        if( NULL == lpszBuffer )
            return FALSE;
        lpszBuffer[0] = '\0'; 
 
        fResult = ReadFile(hSlot, 
            lpszBuffer, 
            cbMessage, 
            &cbRead, 
            &ov); 
 
        if (!fResult) 
        { 
            printf("ReadFile failed.\n"); 
            GlobalFree((HGLOBAL) lpszBuffer); 
            return FALSE; 
        } 
 
        // Display the message. 
       _tprintf(TEXT("LOG at %s\n"), lpszBuffer); 

        GlobalFree((HGLOBAL) lpszBuffer); 
 
        fResult = GetMailslotInfo(hSlot,  // mailslot handle 
            (LPDWORD) NULL,               // no maximum message size 
            &cbMessage,                   // size of next message 
            &cMessage,                    // number of messages 
            (LPDWORD) NULL);              // no read time-out 
 
        if (!fResult) 
        { 
            printf("GetMailslotInfo failed (%d)\n", GetLastError());
            return FALSE; 
        } 
    } 
    CloseHandle(hEvent);
    return TRUE; 
}




//Threads Handles
HANDLE hSocket;     //Socket Client Thread
HANDLE hOPC;        //OPC Client Thread

//Mailslot Handles
HANDLE hMailslot;   //handle to Mailslot Server where all log info will be deposited
//HANDLE hSocketOPC   //Mailslot to comunicate the threads hSocket and hOPC <-THREAD INTERNAL VARIABLE???

//Event Handles
HANDLE hSync;   //To notify when mailslot client will be ready

//Threads Functions
DWORD WINAPI dwSocket();  //Socket Client Function
DWORD WINAPI dwOPC();     //OPC Client Function

int main (void)
{
	std::cout << "Hello World!" << std::endl;
    DWORD dwThreadID;   //Where the Thread ID will be deposited when created a Thread
    DWORD dwBytesRead;  //Bytes read in the mailslot message
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
    char buff[2048];
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


DWORD WINAPI dwSocket()
{
    std::cout << "Socket Client Thread Created" << std::endl;

    HANDLE hMailSocket;

    //Wait the Mailslot Server
    WaitForSingleObject(hSync,INFINITE);
    
    hMailSocket = CreateFile (
        (LPCSTR)"\\\\.\\mailslot\\mylogs",
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);



    WriteSlot(hMailSocket,TEXT("QUALQUER COISA"));
    Sleep(6000);
    WriteSlot(hMailSocket,TEXT("QUIT!"));


    CloseHandle(hMailSocket);

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