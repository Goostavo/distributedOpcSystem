// Mailslot Operations
//
// This functions are a modified version of the "Reading from a Mailslot" 
// and "Writing from a Mailslot" that was originally available at Microsoft
// Developer Network Platforms (MSND) where are examples of Using Mailslots
//
// Some functions that wasn't necessary at this project were removed and 
// another adaptations were done. 
//
// The based Mailslots tutorial can be found in
//      http://msdn.microsoft.com/en-us/library/windows/desktop/aa365794.aspx
//
//
// Gustavo Barreto Garcia
// Rafael Gilmar Ribeiro Gurgel

#include <windows.h>
#include "Opcclass.h"

BOOL WriteSlot(HANDLE hSlot, LPTSTR lpszMessage);
BOOL ReadSlot(HANDLE hSlot, char *message);
void parseAndSend(char* information, Opcclass *OpcSd);
