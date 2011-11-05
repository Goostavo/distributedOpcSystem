#include <iostream>
#include <process.h>
#include <winsock2.h>
#include "SocketClient.h"
#include "MailslotOperations.h"

#define SIZE  55

DWORD WINAPI dwSocket()
{
    WSADATA wsaData;
    SOCKET s;
    SOCKADDR_IN ServerAddr;

    char *ipaddr, buf[SIZE], message[SIZE+1];
    int port, status;

    ipaddr = "192.168.0.101";
    port = 6000;

    while (true)
    {

        //Winsock 2.2 Startup
        status = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (status != 0) {
            strncpy_s(message, SIZE, "Winsock 2 Startup Error.", status);
	        WSACleanup();
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
        }
        
        //Receiving the message...
        status = recv(s, buf, SIZE, 0);
        if (status > 0){
            strncpy_s(message, SIZE+1, buf, status);
            closesocket(s);
            WSACleanup();    
  	    }
        else if (status !=0) {
            strncpy_s(message, SIZE+1, "Receiving Error.", status);
            closesocket(s);
            WSACleanup();
        }
        std::cout << message << std::endl;
        Sleep(1000);
    }
    _endthreadex((DWORD) 0);
    return 0;
}
