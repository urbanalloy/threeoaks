///////////////////////////////////////////////////////////////////////////////////////////////
//
// CCOMError Class - Error Dispatch for COM components
//
// Copyright (c) 2008-2009, Three Oaks Crossing
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include <Windows.h>

class CCOMError
{
	public:
		// Throws an exception corresponding to the given  Win32 Error code.
		// GUID is the CLSID of the component	throwing error.
		static HRESULT DispatchWin32Error(DWORD error, REFCLSID clsid, LPCTSTR source,
										  DWORD helpContext, LPCTSTR helpFileName);

		// Throws the given error code and the message corresponding to the code. If the code is
		// a standard code and no message is provided, then the message is extracted from the system.
		static HRESULT DispatchError(HRESULT hError, REFCLSID clsid, LPCTSTR source, LPCTSTR description,
									 DWORD helpContext, LPCTSTR helpFileName);
};
