// Add Ws2_32.lib; in Projects->Properties->Configuration Properties->Linker->Input->Additional Dependencies

#include <winsock2.h>
#include <iostream>
#include <process.h>
#include "SocketClient.h"
#include "MailslotOperations.h"

#define SIZE  55



DWORD WINAPI dwSocket()
{
    HANDLE hSync=OpenEvent(EVENT_MODIFY_STATE,FALSE,(LPCSTR)"Synchronization");
    HANDLE hSyncOPC=CreateEvent(NULL,TRUE,FALSE,(LPCSTR)"SyncOPCTCP");
    HANDLE hMailslotLog;
    HANDLE hMailslotOPC;
    char msg[SIZE+1];

    //Wait the Mailslot Server
    WaitForSingleObject(hSync,INFINITE);
    
    hMailslotLog = CreateFile (
        (LPCSTR)"\\\\.\\mailslot\\mylogs",
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    
    WaitForSingleObject(hSyncOPC,INFINITE);
    hMailslotOPC = CreateFile (
        (LPCSTR)"\\\\.\\mailslot\\OPCTCP",
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
       
    while(true)
    {
        
        if(tcpfunction((char *)&msg))
        {
            WriteSlot(hMailslotOPC, (LPTSTR)&msg);
        }

        else {
            std::cout << msg << std::endl;
            WriteSlot(hMailslotOPC, (LPTSTR)"999999,999999,9999.9,9999.9,999999,999999,999999,999999");
            WriteSlot(hMailslotLog,(LPTSTR)&msg);
        }
    }

    CloseHandle(hSync);
    CloseHandle(hSyncOPC);
    CloseHandle(hMailslotLog);
    CloseHandle(hMailslotOPC);

    _endthreadex((DWORD) 0);
    return 0;
}


//Function that connects to the TCP Server and the pointer receives
//the message sent. If there's some connection problem it will return
//false and the pointer receives the error message.
BOOL tcpfunction(char *message)
{
    WSADATA wsaData;
    SOCKET s;
    SOCKADDR_IN ServerAddr;

    char *ipaddr, buf[SIZE];
    int port, status;

    ipaddr = "192.168.0.101";
    port = 6000;

    //Winsock 2.2 Startup
    status = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (status != 0) {
        strncpy_s(message, SIZE+1, "Winsock 2 Startup Error.", status);
	    WSACleanup();
        return(false);
    }

    //Creating Socket
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET){
        status=WSAGetLastError();
	    if (status == WSAENETDOWN)
            strncpy_s(message, SIZE+1, "Network or server sockets inaccessible.", status);
	    else
            strncpy_s(message, SIZE+1, "Network Error.", status);
	    WSACleanup();
        return(false);
    }

    //SOCKADDR_IN Inicialization
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(port);    
    ServerAddr.sin_addr.s_addr = inet_addr(ipaddr);

    //Server Conection
    status = connect(s, (SOCKADDR *) &ServerAddr, sizeof(ServerAddr));
    if (status == SOCKET_ERROR){
	    strncpy_s(message, SIZE+1, "Error: Connection not established.", status);
        closesocket(s);
	    WSACleanup();
        return(false);
    }
    //Receiving the message...
    status = recv(s, buf, SIZE, 0);
    if (status > 0){
        strncpy_s(message, SIZE+1, buf, status);
        closesocket(s);
        WSACleanup();    
	    return(true);
  	}
    else if (status !=0) {
        strncpy_s(message, SIZE+1, "Receiving Error.", status);
        closesocket(s);
        WSACleanup();
        return(false);
    }
return(false);      
}
