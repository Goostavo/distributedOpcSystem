// Prototype declarations of wrapper functions used in extending
// the "OPC Simple Client" to support OPC callback notifications
//
// Luiz T. S. Mendes - 13 Sept 2011

// Even with the constant below defined, the compiler keeps sending
// warning errors (it shouldn´t) regarding the insecure string functions,
// so that I defined it directly as a preprocessor option
// (Project->Project Properties->Configiration Properties->C/C++ ->
//  ->Preprocessor->Preprocessor definitions)
#define _CRT_SECURE_NO_WARNINGS 1


void SetAdviseSink(IUnknown* pGroupIUnknown, IAdviseSink* pSOCAdviseSink,
				   IDataObject* &pIDataObject, DWORD* ptkAsyncConnection);
void CancelAdviseSink(IDataObject *pIDataObject, DWORD tkAsyncConnection);
void SetGroupActive(IUnknown* pGroupIUnknown);
bool VarToStr (VARIANT pvar, char *buffer);
void SetDataCallback(IUnknown* pGroupIUnknown, IOPCDataCallback* pSOCDataCallback,
					 IConnectionPoint* &pIConnectionPoint, DWORD *pdwCookie);
void CancelDataCallback(IConnectionPoint *pIConnectionPoint,  DWORD dwCookie);
