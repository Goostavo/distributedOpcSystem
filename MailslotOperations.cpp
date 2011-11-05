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


BOOL ReadSlot(HANDLE hSlot, char* message) 
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
 
        // Write the message. 
        
        strcpy_s(message, 1024, lpszBuffer);
        //std::cout << message << std::endl;

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

//Parse the information sent by the Sockets client and write on the OPC registers.
void parseAndSend(char* information, Opcclass *OpcSd)
{
    int tempValue = 0;  int aux = 0; int index = 0;
    unsigned int tempUValue = 0;
    float tempFloat;
    double tempDouble;
    char tempBuff[6];

    ////////////////////////////////////////////////////////////////////////////////
    //Send the VT_I1 data
    for (aux=0;aux<6;aux++)
    {
        tempBuff[aux]=information[aux+index];
    }
    sscanf(tempBuff, "%d", &tempValue);
    //Set max and min values
    if (tempValue > 127)
        tempValue = 127;
    else if (tempValue < -128)
        tempValue = -128;
    OpcSd->WriteItem(1,&tempValue,VT_I1);

    ////////////////////////////////////////////////////////////////////////////////
    //Send the VT_I2 data
    index=7*1;
    for (aux=0;aux<6;aux++)
    {
        tempBuff[aux]=information[aux+index];
    }
        //Set max and min values
    if (tempValue > 32767)
        tempValue = 32767;
    else if (tempValue < -32768)
        tempValue = -32768;
    sscanf(tempBuff, "%d", &tempValue);
    OpcSd->WriteItem(2,&tempValue,VT_I2);

    ////////////////////////////////////////////////////////////////////////////////
    //Send the VT_R4 data
    index=7*2;
    for (aux=0;aux<6;aux++)
    {
        tempBuff[aux]=information[aux+index];
    }
    sscanf(tempBuff, "%f", &tempFloat);
    //Set max and min values
    OpcSd->WriteItem(3,&tempFloat,VT_R4);

    ////////////////////////////////////////////////////////////////////////////////
    //Send the VT_R8 data
    index=7*3;
    for (aux=0;aux<6;aux++)
    {
        tempBuff[aux]=information[aux+index];
    }
    sscanf(tempBuff, "%f", &tempDouble);
    //Set max and min values
    OpcSd->WriteItem(4,&tempDouble,VT_R8);

    ////////////////////////////////////////////////////////////////////////////////
    //Send the VT_I4 data
    index=7*4;
    for (aux=0;aux<6;aux++)
    {
        tempBuff[aux]=information[aux+index];
    }
    sscanf(tempBuff, "%d", &tempValue);
    //Set max and min values
    if (tempValue > 2147483647)
        tempValue = 2147483647;
    else if (tempValue < -2147483648)
        tempValue = -2147483648;
    OpcSd->WriteItem(5,&tempValue,VT_I4);

    ////////////////////////////////////////////////////////////////////////////////
    //Send the VT_UI1 data
    index=7*5;
    for (aux=0;aux<6;aux++)
    {
        tempBuff[aux]=information[aux+index];
    }
    sscanf(tempBuff, "%d", &tempUValue);
    //Set max and min values
    if (tempUValue > 256)
        tempUValue = 256;
    else if (tempUValue < 0)
        tempUValue = 0;
    OpcSd->WriteItem(6,&tempUValue,VT_UI1);

    ////////////////////////////////////////////////////////////////////////////////
    //Send the VT_UI2 data
    index=7*6;
    for (aux=0;aux<6;aux++)
    {
        tempBuff[aux]=information[aux+index];
    }
    sscanf(tempBuff, "%d", &tempUValue);
    //Set max and min values
    if (tempUValue > 65535)
        tempUValue = 65535;
    else if (tempUValue < 0)
        tempUValue = 0;
    OpcSd->WriteItem(7,&tempUValue,VT_UI2);

    ////////////////////////////////////////////////////////////////////////////////
    //Send the VT_UI4 data
    index=7*7;
    for (aux=0;aux<6;aux++)
    {
        tempBuff[aux]=information[aux+index];
    }
    sscanf(tempBuff, "%d", &tempUValue);
    //Set max and min values
    if (tempUValue > 4294967295)
        tempUValue = 4294967295;
    else if (tempUValue < 0)
        tempUValue = 0;
    OpcSd->WriteItem(8,&tempUValue,VT_UI4);
}