#include <iostream>
#include <process.h>
#include "OPCClient.h"

DWORD WINAPI dwOPC()
{
    std::cout << "OPC Client Thread Created" << std::endl;

    //Ending the Thread
    _endthreadex((DWORD) 0);
    return 0;
}