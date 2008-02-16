///////////////////////////////////////////////////////////////////////////////////////////////
//
// DotNetDocklets : bringing .NET to ObjectDock
//
// Copyright (c) 2004-2008, Julien Templier
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
#pragma pack(push, 8)

#include <comdef.h>

//
// Forward references and typedefs
//

struct __declspec(uuid("92504771-bbc2-403c-9331-59a6a0ae7fc3")) IRegisterInterface;

//
// Smart pointer typedef declarations
//

_COM_SMARTPTR_TYPEDEF(IRegisterInterface, __uuidof(IRegisterInterface));

//
// Type library items
//


struct __declspec(uuid("92504771-bbc2-403c-9331-59a6a0ae7fc3"))
IRegisterInterface : IDispatch
{
    //
    // Wrapper methods for error-handling
    //

    VARIANT_BOOL RegisterDll ( _bstr_t path );

    //
    // Raw methods provided by interface
    //

      virtual HRESULT __stdcall raw_RegisterDll (
        /*[in]*/ BSTR path,
		/*[out,retval]*/ VARIANT_BOOL * pRetVal) = 0;
};


//
// Named GUID constants initializations
//

// Interface IRegisterInterface
extern "C" const GUID __declspec(selectany) IID_IRegisterInterface =
    {0x92504771,0xbbc2,0x403c,{0x93,0x31,0x59,0xa6,0xa0,0xae,0x7f,0xc3}};

// Class Register
extern "C" const GUID __declspec(selectany) CLSID_Register =
	{0x89cf607d,0xf586,0x416f,{0x8c,0x93,0xbc,0x7d,0xe9,0x5f,0xa3,0x6f}};

//
// Wrapper method implementations
//

#include "IRegisterInterfaceImplementation.h"

#pragma pack(pop)
