#include <iostream>
#include <process.h>
#include "SocketClient.h"
#include "MailslotOperations.h"

DWORD WINAPI dwSocket()
{
    std::cout << "Socket Client Thread Created" << std::endl;

    HANDLE hMailSocket;
    HANDLE hSync=OpenEvent(EVENT_MODIFY_STATE,FALSE,(LPCSTR)"Synchronization");
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



    WriteSlot(hMailSocket,TEXT("5 seconds to next message"));
    Sleep(5000);
    WriteSlot(hMailSocket,TEXT("QUIT!"));

    CloseHandle(hSync);
    CloseHandle(hMailSocket);

    //Ending the Thread
    _endthreadex((DWORD) 0);
    return 0;
}