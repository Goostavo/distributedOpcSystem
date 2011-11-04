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
#include <windows.h>

BOOL WriteSlot(HANDLE hSlot, LPTSTR lpszMessage);
BOOL ReadSlot(HANDLE hSlot);