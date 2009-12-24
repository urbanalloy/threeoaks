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

#pragma once

#include "stdafx.h"
#include <Windows.h>


///////////////////////////////////////////////
// DomException Error codes
///////////////////////////////////////////////
#define INDEX_SIZE_ERR				1
#define DOMSTRING_SIZE_ERR			2
#define HIERARCHY_REQUEST_ERR		3
#define WRONG_DOCUMENT_ERR			4
#define INVALID_CHARACTER_ERR		5
#define NO_DATA_ALLOWED_ERR			6
#define NO_MODIFICATION_ALLOWED_ERR 7
#define NOT_FOUND_ERR				8
#define NOT_SUPPORTED_ERR			9
#define INUSE_ATTRIBUTE_ERR			10
#define INVALID_STATE_ERR			11
#define SYNTAX_ERR					12
#define INVALID_MODIFICATION_ERR    13
#define NAMESPACE_ERR				14
#define INVALID_ACCESS_ERR			15
#define VALIDATION_ERR				16
#define TYPE_MISMATCH_ERR			17
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
