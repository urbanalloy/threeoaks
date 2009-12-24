///////////////////////////////////////////////////////////////////////////////////////////////
//
// CCOMError Class - Error Dispatch for COM components
//
// Copyright (c) 2008-2010, Julien Templier
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//  3. The name of the author may not be used to endorse or promote products derived from this
//     software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
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

	// Finally release the interfaces
	pCreateErrorInfo->Release();
	pErrorInfo->Release();

	// Failed to set the error info!
	if(FAILED(hSuccess))
		return hSuccess;

	// And, Return the error code that was asked
	// to be dispatched
	HRESULT domError = MAKE_HRESULT(1, FACILITY_ITF, hError);
	return domError;
}
