//
// C++ class to implement the OPC DA 1.0 IAdviseSink interface.
//
// Note that only the IadviseSink::OnDataChangeMethod() is
// required in the OPC DA specs.
//
// This code is largely based on the KEPWARE´s sample client code.
//
// Luiz T. S. Mendes - DELT/UFMG - 06 Sept 2011
//

#include <stdio.h>
#include <assert.h>
#include "opcda.h"
#include "SOCAdviseSink.h"
#include "SOCWrapperFunctions.h"

extern UINT OPC_DATA_TIME;

// Constructor: initializes the reference count to 0.
SOCAdviseSink::SOCAdviseSink () : m_cRef (0)
	{
	}

// IUnknown methods
HRESULT STDMETHODCALLTYPE SOCAdviseSink::QueryInterface (REFIID riid, LPVOID *ppv)
{
	if (ppv == NULL)
		return E_POINTER;       // invalid pointer
	*ppv = NULL;
	if (riid == IID_IUnknown)
		*ppv = (IUnknown*) this;
	else if (riid == IID_IAdviseSink)
		*ppv = (IAdviseSink*) this;
	else
		return (E_NOINTERFACE); //unsupported interface

	// Success
	AddRef ();                  
	return (S_OK);
}

ULONG STDMETHODCALLTYPE SOCAdviseSink::AddRef (void)
{
    return InterlockedIncrement((volatile LONG*)&m_cRef);
}

ULONG STDMETHODCALLTYPE SOCAdviseSink::Release (void)
{
	if (InterlockedDecrement((volatile LONG*)&m_cRef) == 0){
		delete this;
		return 0;
	}
	else
	    return m_cRef;
}

// OnDataChange method. Note that the OPC DA spec defines three types of
// notifications that should be treated by this method, namely "Data with
// Time Stamp", "Data without Timestamp" and "Async Write Complete". In this
// code sample, only the "Data with Time Stamp" is implemented, so the
// call to the IDataObject interface should specify only this case.
//
// The code for this method is a stripped-down version from the KEPWARE´s
// original C++ sample client code.
//
void STDMETHODCALLTYPE SOCAdviseSink::OnDataChange(
	   FORMATETC *pFormatEtc,
	   STGMEDIUM *pMedium)
{
	HRESULT hr;
	IStream *pStream = NULL;
	LARGE_INTEGER dlibMove;
	OPCGROUPHEADER groupheader;
	OPCITEMHEADER1 itemheader;
	VARIANT vtVal;
	FILETIME lft;
	SYSTEMTIME st;
    char szLocalDate[255], szLocalTime[255];
	bool status;
	char buffer[100];
	WORD quality;

	// Check for valid pointers.  Return if invalid:
	if ((pFormatEtc == NULL) || (pMedium == NULL)) {
		printf ("IAdviseSink::OnDataChange: Invalid pointers.\n");
		return;
	}
	
	// The OPC DA spec dictates that the storage medium must be a handle to global
	// memory. Check this up and return if not:
	if ((pFormatEtc->tymed != TYMED_HGLOBAL) && (pMedium->tymed != TYMED_HGLOBAL)){
		printf ("IAdviseSink::OnDataChange: Incompatible Storage Medium\n");
		return;
	}

	// Although this is not strictly necessary, KEPWARE´s sample client
	// treats the storage medium as a stream object. So, it first
	// obtains an IStream instance from the global memory handle.
	//
	// We recall that OPC data are passed as a structure that begins with an OPC
	// group header followed by an array of OPC item headers (one for each OPC item
	// passed), and then the OPC items of VARIANT data type. So, to properly parse
	// the OPC data passed, one first must point to the OPC group header and extract
	// the "dwItemCount" item count from it, and then walk the structure along the
	// each OPC item header. Each of these, in turn, contains a "dwValueOffset" field
	// which indicates the actual location of the OPC item value;
	//
	hr = CreateStreamOnHGlobal (pMedium->hGlobal,	// global memory handle
								TRUE,				// delete memory when stream is release
								&pStream);			// put the stream here

	// Make sure we succeeded.  Release stream and return if not:
	if (hr != S_OK){
		printf("IAdviseSink::OnDataChange: Failed to create stream object. Error = %x\n", hr);
		pStream->Release ();
		return;
	}

	// Seek to the beginning of the stream.
	dlibMove.LowPart = dlibMove.HighPart = 0;
	hr = pStream->Seek (dlibMove,			// relative offset set to
					    STREAM_SEEK_SET,	// beginning of the stream
						NULL);				// don't care about the new seek pointer

	// Make sure we succeeded.  Release stream and return if not:
	if (hr != S_OK){
		printf("IAdviseSink::OnDataChange: Seek failed on object stream. Error = %x\n", hr);
		pStream->Release ();
		return;
	}

	// Check the event that generated the callback. In this example, only data passed
	// with time stamp is supported.
	if (pFormatEtc->cfFormat != OPC_DATA_TIME){
		printf("IAdviseSink::OnDataChange: Unsupported event type %x\n",pFormatEtc->cfFormat);
		return;
	}

	// Read the OPC group header:
	hr = pStream->Read (&groupheader,				// where to put the stream data
						sizeof (OPCGROUPHEADER),	// how much to read
						NULL);						// don't care about how many bytes are read

	// Release stream and return if read failed:
	if (hr != S_OK){
		printf("IAdviseSink::OnDataChange: Failed to read stream object. Error = %x\n", hr);
		pStream->Release ();
		return;
	}

	// Initialize a variant to place a data value in:
	VariantInit (&vtVal);

	// Loop over OPC item headers in the stream:
	for (DWORD dwItem = 0; dwItem < groupheader.dwItemCount; dwItem++)
	{
		// Compute offset of next item:
		dlibMove.HighPart = 0;
		dlibMove.LowPart = sizeof (OPCGROUPHEADER) + (dwItem * sizeof (OPCITEMHEADER1));
		// Seek to next item:
		hr = pStream->Seek (dlibMove, STREAM_SEEK_SET, NULL);
		if (hr != S_OK){
			printf("IAdviseSink::OnDataChange: Seek next item failed. Error = %x\n", hr);
		pStream->Release ();
		return;
		}

		// Read the item header:
		hr = pStream->Read (&itemheader, sizeof (OPCITEMHEADER1), NULL);
		if (hr != S_OK){
			printf("IAdviseSink::OnDataChange: Read Item Header failed. Error = %x\n", hr);
			pStream->Release ();
			return;
		}

		// Seek to beginning of the OPC utem value:
		dlibMove.LowPart = itemheader.dwValueOffset;	
		hr = pStream->Seek (dlibMove, STREAM_SEEK_SET, NULL);
		if (hr != S_OK){
			printf("IAdviseSink::OnDataChange: Failed to seek data. Error = %x\n", hr);
			pStream->Release ();
			return;
		}

		// Read the variant data:
		hr = pStream->Read (&vtVal, sizeof (VARIANT), NULL);
		if (hr != S_OK){
			printf("IAdviseSink::OnDataChange: Read variant failed. Error = %x\n", hr);
			pStream->Release ();
			return;
		}

		// A little more work for strings and arrays:
		switch (vtVal.vt)
		{
			case VT_BSTR:
				// For VT_BSTR items, the data is passed as the VARIANT value
				// (whose value "bstrval" is just a pointer to the string)
				// immediately followed by a DWORD byte count, and then the
				// MULL-terminated string itself. So, one must first allocate memory
				// for copying the string, then store its new pointer in the
				// VARIANT "bstrval" field, and the copy the string to its new position.
				// Note that the OPC Spec states that "any pointers in the SAFEARRAY and
				// VARIANT unions need to be recreated by the receiver when the data
				// is unmarshalled and stored locally", and this is why the initial
				// "bstrval" passed to the client is overwritten with a new valid pointer.
				{
				DWORD dwLength = 0;
				// Read string length:
				hr = pStream->Read (&dwLength, sizeof (DWORD), NULL);
				if (hr != S_OK){
					printf("IAdviseSink::OnDataChange: Read string size. Error = %x\n", hr);
					pStream->Release ();
					return;
				}
				if (dwLength == 0)
					vtVal.bstrVal = NULL;
				else{ 
					// Allocate memory for string.  We will free this once we copy
					// the contents of the string into the item object:
					vtVal.bstrVal = SysAllocStringLen (NULL, dwLength);
					if (vtVal.bstrVal != NULL){
						// Read string data:
						hr = pStream->Read (vtVal.bstrVal, (dwLength + 1) * sizeof (OLECHAR), NULL);
						if (hr != S_OK){
							printf("IAdviseSink::OnDataChange: VT_BSTR: Failed to create stream object. Error = %x\n", hr);
							SysFreeString (vtVal.bstrVal);						
							pStream->Release ();
							return;
						}
					}
				}
				}
				break;
			case VT_UI1	| VT_ARRAY:
			case VT_I1	| VT_ARRAY:
			case VT_UI2	| VT_ARRAY:
			case VT_I2	| VT_ARRAY:
			case VT_UI4	| VT_ARRAY:
			case VT_I4	| VT_ARRAY:
			case VT_R4	| VT_ARRAY:
			case VT_R8	| VT_ARRAY:
				// For VT_ARRAY items, the data is passed as the VARIANT value
				// followed by a SAFEARRAY structure, and then the data items
				// themselves. As in the VT_BSTR case, one must create a new
				// array descriptor (by means of the SafeArrayCreate function),
				// copy its pointer to the VARIANT "parray" field, and finally
				// copy the original array data into this new array descriptor.
				{
				// Read array parameters:
				SAFEARRAY sa;
				DWORD cdwElements;
				pStream->Read (&sa, sizeof (SAFEARRAY), NULL);
				// 2-D array:
				if (sa.cDims != 1){
					// Read array parameters for second dimension:
					// (Note that ASSERT is a MFC function that here is replaced
					// by the standard CRT function ´assert'
					//ASSERT (sa.cDims == 2);
					assert (sa.cDims == 2);
					SAFEARRAYBOUND sab [2];
					sab [0] = sa.rgsabound [0];
					pStream->Read (&sab [1], sizeof (SAFEARRAYBOUND), NULL);
					// Compute array size:
					cdwElements = sa.rgsabound [0].cElements * sab [1].cElements;
					// Allocate array (specify basic, i.e. non-array, data type):
					vtVal.parray = SafeArrayCreate (vtVal.vt & ~VT_ARRAY, sa.cDims, sab);
				}
				// 1-D array
				else {
					// Compute array size:
					cdwElements = sa.rgsabound [0].cElements;
					// Allocate array (specify basic, i.e. non-array, data type):
					vtVal.parray = SafeArrayCreate (vtVal.vt & ~VT_ARRAY, sa.cDims, sa.rgsabound);
				}
				// Read array data:
				pStream->Read (vtVal.parray->pvData, sa.cbElements * cdwElements, NULL);
				}
				break;
			case VT_BSTR | VT_BYREF:
				//ASSERT (FALSE);
				assert (false);
				break;
		}

		// Print the item value, quality and time stamp. In this version, only
		// a few OPC data types are supported.
		status = VarToStr(vtVal, buffer);
		if (status){
			printf("Data Advise: Value = %s", buffer);
			if (vtVal.vt == VT_BSTR)
				SysFreeString (vtVal.bstrVal);	
			quality = itemheader.wQuality & OPC_QUALITY_MASK;
			if (quality == OPC_QUALITY_GOOD)
				printf(" Quality: good");
			else
			    printf(" Quality: not good");
			// Code below extracted from the Microsoft KB:
			//     http://support.microsoft.com/kb/188768
			// Note that in order for it to work, the Visual Studio C++ must
			// be configured so that the "character set" property is "not set"
			// (Project->Project Properties->Configuration Properties->General).
			// Otherwise, if defined e.g. as "use Unicode" (as it seems to be
			// the default when a new project is created), there will be
			// compilation errors.
			FileTimeToLocalFileTime(&itemheader.ftTimeStampItem,&lft);
			FileTimeToSystemTime(&lft, &st);
			GetDateFormat(LOCALE_SYSTEM_DEFAULT, DATE_SHORTDATE, &st, NULL, szLocalDate, 255);
			GetTimeFormat(LOCALE_SYSTEM_DEFAULT, 0, &st, NULL, szLocalTime, 255);
			printf(" Time: %s %s\n", szLocalDate, szLocalTime);
		}
		else printf ("Data Advise: Unsupported item type\n");
		
		// Clear variant for reuse:
		VariantClear (&vtVal);
	}
	// Release our stream on exit:
	pStream->Release();
}