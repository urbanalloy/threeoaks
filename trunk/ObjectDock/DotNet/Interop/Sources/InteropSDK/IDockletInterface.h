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

struct DOCKLET_STATIC_DATA;
struct __declspec(uuid("0ad45a28-f965-421d-a434-7bb746cc2a30")) IDockletInterface;

//
// Smart pointer typedef declarations
//

_COM_SMARTPTR_TYPEDEF(IDockletInterface, __uuidof(IDockletInterface));

//
// Type library items
//

struct __declspec(uuid("bc48bab4-19d4-4741-901d-aa0fd88dc14a"))
DOCKLET_STATIC_DATA
{
    long hwndDocklet;
    long hInstance;
    LPSTR rootFolder;		// ObjectDock Root Folder
    LPSTR interopFolder;	// (== ObjectDock relative path)
    LPSTR relativeFolder;	// ObjectDock relative path + plugin folder
};

struct __declspec(uuid("0ad45a28-f965-421d-a434-7bb746cc2a30"))
IDockletInterface : IDispatch
{
    //
    // Wrapper methods for error-handling
    //

    HRESULT OnGetInformation (
        BSTR * name,
        BSTR * author,
        long * version,
        BSTR * notes );
    HRESULT OnCreate (
        struct DOCKLET_STATIC_DATA data,
        _bstr_t ini,
        _bstr_t iniGroup );
    HRESULT OnSave (
        _bstr_t ini,
        _bstr_t iniGroup,
        VARIANT_BOOL isForExport );
    HRESULT OnDestroy ( );
    SAFEARRAY * OnExportFiles ( );
    VARIANT_BOOL OnLeftButtonClick (
		struct System_Drawing::Point * ptCursor,
		struct System_Drawing::Size * sizeDocklet );
    VARIANT_BOOL OnDoubleClick (
        struct System_Drawing::Point * ptCursor,
        struct System_Drawing::Size * sizeDocklet );
    VARIANT_BOOL OnLeftButtonHeld (
        struct System_Drawing::Point * ptCursor,
        struct System_Drawing::Size * sizeDocklet );
    VARIANT_BOOL OnRightButtonClick (
        struct System_Drawing::Point * ptCursor,
        struct System_Drawing::Size * sizeDocklet );
    VARIANT_BOOL OnConfigure ( );
    VARIANT_BOOL OnAcceptDropFiles ( );
    HRESULT OnDropFiles (
        long hDrop );
    HRESULT OnProcessMessage (
        long hwnd,
        unsigned long uMsg,
        long wParam,
        long lParam );

    //
    // Raw methods provided by interface
    //

      virtual HRESULT __stdcall raw_OnGetInformation (
        /*[out]*/ BSTR * name,
        /*[out]*/ BSTR * author,
        /*[out]*/ long * version,
        /*[out]*/ BSTR * notes ) = 0;
      virtual HRESULT __stdcall raw_OnCreate (
        /*[in]*/ struct DOCKLET_STATIC_DATA data,
        /*[in]*/ BSTR ini,
        /*[in]*/ BSTR iniGroup ) = 0;
      virtual HRESULT __stdcall raw_OnSave (
        /*[in]*/ BSTR ini,
        /*[in]*/ BSTR iniGroup,
        /*[in]*/ VARIANT_BOOL isForExport ) = 0;
      virtual HRESULT __stdcall raw_OnDestroy ( ) = 0;
      virtual HRESULT __stdcall raw_OnExportFiles (
        /*[out,retval]*/ SAFEARRAY * * pRetVal ) = 0;
      virtual HRESULT __stdcall raw_OnLeftButtonClick (
        /*[in,out]*/ struct System_Drawing::Point * ptCursor,
        /*[in,out]*/ struct System_Drawing::Size * sizeDocklet,
        /*[out,retval]*/ VARIANT_BOOL * pRetVal ) = 0;
      virtual HRESULT __stdcall raw_OnDoubleClick (
        /*[in,out]*/ struct System_Drawing::Point * ptCursor,
        /*[in,out]*/ struct System_Drawing::Size * sizeDocklet,
        /*[out,retval]*/ VARIANT_BOOL * pRetVal ) = 0;
      virtual HRESULT __stdcall raw_OnLeftButtonHeld (
        /*[in,out]*/ struct System_Drawing::Point * ptCursor,
        /*[in,out]*/ struct System_Drawing::Size * sizeDocklet,
        /*[out,retval]*/ VARIANT_BOOL * pRetVal ) = 0;
      virtual HRESULT __stdcall raw_OnRightButtonClick (
        /*[in,out]*/ struct System_Drawing::Point * ptCursor,
        /*[in,out]*/ struct System_Drawing::Size * sizeDocklet,
        /*[out,retval]*/ VARIANT_BOOL * pRetVal ) = 0;
      virtual HRESULT __stdcall raw_OnConfigure (
        /*[out,retval]*/ VARIANT_BOOL * pRetVal ) = 0;
      virtual HRESULT __stdcall raw_OnAcceptDropFiles (
        /*[out,retval]*/ VARIANT_BOOL * pRetVal ) = 0;
      virtual HRESULT __stdcall raw_OnDropFiles (
        /*[in]*/ long hDrop ) = 0;
      virtual HRESULT __stdcall raw_OnProcessMessage (
        /*[in]*/ long hwnd,
        /*[in]*/ unsigned long uMsg,
        /*[in]*/ long wParam,
        /*[in]*/ long lParam ) = 0;
};


//
// Named GUID constants initializations
//

extern "C" const GUID __declspec(selectany) IID_IDockletInterface =
    {0x0ad45a28,0xf965,0x421d,{0xa4,0x34,0x7b,0xb7,0x46,0xcc,0x2a,0x30}};

//
// Wrapper method implementations
//

#include "IDockletInterfaceImplementation.h"

#pragma pack(pop)
