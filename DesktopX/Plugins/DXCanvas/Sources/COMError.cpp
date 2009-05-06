///////////////////////////////////////////////////////////////////////////////////////////////
//
// CCOMError Class - Error Dispatch for COM components
//
// Copyright (c) 2008, Three Oaks Crossing
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "COMError.h"

#include <crtdbg.h>
#include <atlbase.h>

// Throws an exception corresponding to the given Win32 Error code.
// GUID is the CLSID of the component throwing error.
HRESULT CCOMError::DispatchWin32Error(DWORD error, REFCLSID clsid, LPCTSTR source,
									  DWORD helpContext, LPCTSTR helpFileName)
{
	// Dispatch the requested error message
	return DispatchError(HRESULT_FROM_WIN32(error), clsid, source, NULL, helpContext, helpFileName);
}

// Throws the given error code and the message corresponding to the code. If the code is
// a standard code and no message is provided, then the message is extracted from the system.
HRESULT CCOMError::DispatchError(HRESULT hError, REFCLSID clsid, LPCTSTR source, LPCTSTR description,
								 DWORD helpContext, LPCTSTR helpFileName)
{
	// This function uses ATL conversion macros
	// (Hence we must use this MACRO provided by ATL)
	USES_CONVERSION;

	// Convert the description to OLE string
	LPOLESTR wszError = NULL;
	if(description != NULL)
	{
		// Convert to wide char
		wszError = T2OLE(description);
	}
	else
	{
		// If the code is a Win32 error code
		if(HRESULT_FACILITY(hError) == FACILITY_WIN32)
		{
			// Get the error from the system
			LPTSTR szError = NULL;
			if(!::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 
								NULL,
								HRESULT_CODE(hError),
								MAKELANGID(LANG_USER_DEFAULT, SUBLANG_DEFAULT),
								(LPTSTR)&szError,
								0,
								NULL))
				return HRESULT_FROM_WIN32(GetLastError());

			// Convert the Error multibyte string to OLE string
			if(szError != NULL)
			{
				// Convert to wide char
				wszError = T2OLE(szError);
				// Free the multibyte string
				LocalFree(szError);
			}
		}
	}

	// Convert the source string to OLE string
	LPOLESTR wszSource = NULL;
	if(source != NULL)
		wszSource = T2OLE(source);

	// Convert the help filename to OLE string
	LPOLESTR wszHelpFile = NULL;
	if(helpFileName != NULL)
		wszHelpFile = T2OLE(helpFileName);

	// Get the ICreateErrorInfo Interface
	ICreateErrorInfo *pCreateErrorInfo = NULL;
	HRESULT hSuccess = CreateErrorInfo(&pCreateErrorInfo);
	ATLASSERT(SUCCEEDED(hSuccess));

	// Fill the error information into it
	pCreateErrorInfo->SetGUID(clsid);
	if(wszError != NULL)
		pCreateErrorInfo->SetDescription(wszError);
	if(wszSource != NULL)
		pCreateErrorInfo->SetSource(wszSource);
	if(wszHelpFile != NULL)
		pCreateErrorInfo->SetHelpFile(wszHelpFile);
	pCreateErrorInfo->SetHelpContext(helpContext);

	// Get the IErrorInfo interface
	IErrorInfo *pErrorInfo = NULL;
	hSuccess = pCreateErrorInfo->QueryInterface(IID_IErrorInfo, (LPVOID *)&pErrorInfo);
	if(FAILED(hSuccess))
	{
		pCreateErrorInfo->Release();
		return hSuccess;
	}

	// Set this error information in the current thread
	hSuccess = SetErrorInfo(0, pErrorInfo);
	if(FAILED(hSuccess))
	{
		pCreateErrorInfo->Release();
		pErrorInfo->Release();
		return hSuccess;
	}

	// Finally release the interfaces
	pCreateErrorInfo->Release();
	pErrorInfo->Release();

	// And, Return the error code that was asked
	// to be dispatched
	return hError;
}
