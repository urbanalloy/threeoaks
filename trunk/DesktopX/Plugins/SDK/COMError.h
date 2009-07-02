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


///////////////////////////////////////////////
// Error codes (DomException + misc)
///////////////////////////////////////////////
#define INTERNAL_ERR				    99

#define INDEX_SIZE_ERR				    1
#define DOMSTRING_SIZE_ERR			    2
#define HIERARCHY_REQUEST_ERR		    3
#define WRONG_DOCUMENT_ERR			    4
#define INVALID_CHARACTER_ERR		    5
#define NO_DATA_ALLOWED_ERR			    6
#define NO_MODIFICATION_ALLOWED_ERR		7
#define NOT_FOUND_ERR				    8
#define NOT_SUPPORTED_ERR			    9
#define INUSE_ATTRIBUTE_ERR			    10
#define INVALID_STATE_ERR			    11
#define SYNTAX_ERR					    12
#define INVALID_MODIFICATION_ERR		13
#define NAMESPACE_ERR				    14
#define INVALID_ACCESS_ERR			    15
#define VALIDATION_ERR				    16
#define TYPE_MISMATCH_ERR			    17
///////////////////////////////////////////////

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
