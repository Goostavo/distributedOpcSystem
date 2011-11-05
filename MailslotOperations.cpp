// Mailslot Operations
//
// Thess functions are a modified version of the "Reading from a Mailslot" 
// and "Writing from a Mailslot", originally available at Microsoft
// Developer Network Platforms (MSDN).
//
// Some functions that wasn't necessary for this project were removed and 
// some adaptations were done. 
//
// The Mailslots tutorial used can be found in
//      http://msdn.microsoft.com/en-us/library/windows/desktop/aa365794.aspx
//
//s
// Gustavo Barreto Garcia
// Rafael Gilmar Ribeiro Gurgel

#include <iostream>
#include <tchar.h>
#include "MailslotOperations.h"

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
      return FALSE; 
   } 
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
         std::cout << "GetMailslotInfo failed." << std::endl; 
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
             std::cout << "ReadFile failed." << std::endl; 
            GlobalFree((HGLOBAL) lpszBuffer); 
            return FALSE; 
        } 
 
        // Display the message. 
       //_tprintf(TEXT("%s"), lpszBuffer); 
       std::cout << lpszBuffer << std::endl;

        GlobalFree((HGLOBAL) lpszBuffer); 
 
        fResult = GetMailslotInfo(hSlot,  // mailslot handle 
            (LPDWORD) NULL,               // no maximum message size 
            &cbMessage,                   // size of next message 
            &cMessage,                    // number of messages 
            (LPDWORD) NULL);              // no read time-out 
 
        if (!fResult) 
        { 
             std::cout << "GetMailslotInfo failed." << std::endl;
            return FALSE; 
        } 
    } 
    CloseHandle(hEvent);
    return TRUE; 
}