#include <iostream>
#include <process.h>
#include "SocketClient.h"
#include "MailslotOperations.h"

DWORD WINAPI dwSocket()
{
    std::cout << "Socket Client Thread Created" << std::endl;

    _endthreadex((DWORD) 0);
    return 0;
}