#include <iostream>
#include <process.h>
#include <winsock2.h>
#include "SocketClient.h"
#include "MailslotOperations.h"

#define SIZE  55
#define ERROR "999999,999999,9999.9,9999.9,999999,999999,999999,999999"
DWORD WINAPI dwSocket()
{
    Sleep(1000);

    HANDLE hMailslotSocket= CreateFile (
        (LPCSTR)"\\\\.\\mailslot\\OPCTCP",
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    
    WSADATA wsaData;
    SOCKET s;
    SOCKADDR_IN ServerAddr;

    char *ipaddr, buf[SIZE], message[SIZE+1];
    int port, status;

    ipaddr = "192.168.0.101";
    port = 6001;

    while (true)
    {

        //Winsock 2.2 Startup
        status = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (status != 0) {
            //"Winsock 2 Startup Error."
            strncpy_s(message, SIZE, ERROR, status);
	        WSACleanup();
        }

        //Creating Socket
        s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET){
            status=WSAGetLastError();
	        if (status == WSAENETDOWN)
                //"Network or server sockets inaccessible."
                strncpy_s(message, SIZE+1, ERROR, status);
	        else
                //"Network Error."
                strncpy_s(message, SIZE+1, ERROR, status);
	        WSACleanup();
        }

        //SOCKADDR_IN Inicialization
        ServerAddr.sin_family = AF_INET;
        ServerAddr.sin_port = htons(port);    
        ServerAddr.sin_addr.s_addr = inet_addr(ipaddr);

        //Server Conection
        status = connect(s, (SOCKADDR *) &ServerAddr, sizeof(ServerAddr));
        if (status == SOCKET_ERROR){
            //"Error: Connection not established."
	        strncpy_s(message, SIZE+1, ERROR, status);
            closesocket(s);
	        WSACleanup();
        }
        
        //Receiving the message...
        status = recv(s, buf, SIZE, 0);
        if (status > 0){
            strncpy_s(message, SIZE+1, buf, status);
            closesocket(s);
            WSACleanup();    
  	    }
        else if (status !=0) {
            //"Receiving Error."
            strncpy_s(message, SIZE+1, ERROR, status);
            closesocket(s);
            WSACleanup();
        }
        WriteSlot(hMailslotSocket, (LPTSTR) message);
        Sleep(1000);
    }
    _endthreadex((DWORD) 0);
    return 0;
}
