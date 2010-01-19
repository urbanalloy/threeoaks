/*
 * SysStats Widget Framework
 * Copyright (C) 2002-2006 Paul Andrews
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

// COMControllers.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To merge the proxy/stub code into the object DLL, add the file 
//      dlldatax.c to the project.  Make sure precompiled headers 
//      are turned off for this file, and add _MERGE_PROXYSTUB to the 
//      defines for the project.  
//
//      If you are not running WinNT4.0 or Win95 with DCOM, then you
//      need to remove the following define from dlldatax.c
//      #define _WIN32_WINNT 0x0400
//
//      Further, if you are running MIDL without /Oicf switch, you also 
//      need to remove the following define from dlldatax.c.
//      #define USE_STUBLESS_PROXY
//
//      Modify the custom build rule for COMControllers.idl by adding the following 
//      files to the Outputs.
//          COMControllers_p.c
//          dlldata.c
//      To build a separate proxy/stub DLL, 
//      run nmake -f COMControllersps.mk in the project directory.

#include "stdafx.h"

#include <locale.h>
#include "resource.h"
#include <initguid.h>
#include "COMControllers.h"
#include "dlldatax.h"

#include "COMControllers_i.c"
#include "Shortcut.h"
#include "ScriptController.h"
#include "ScriptControllerImpl.h"
#include "ValueSetter.h"
#include "MappedController.h"
#include "TimerController.h"
#include "MessageSender.h"
#include "ASyncScriptController.h"
#include "DropTargetController.h"
#include "CursorController.h"
#include "Sens.h"

#ifdef _MERGE_PROXYSTUB
extern "C" HINSTANCE hProxyDll;
#endif

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_Shortcut, CShortcut)
OBJECT_ENTRY(CLSID_ScriptController, CScriptController)
OBJECT_ENTRY(CLSID_ScriptControllerImpl, CScriptControllerImpl)
OBJECT_ENTRY(CLSID_ValueSetter, CValueSetter)
OBJECT_ENTRY(CLSID_MappedController, CMappedController)
OBJECT_ENTRY(CLSID_TimerController, CTimerController)
OBJECT_ENTRY(CLSID_MessageSender, CMessageSender)
OBJECT_ENTRY(CLSID_ASyncScriptController, CASyncScriptController)
OBJECT_ENTRY(CLSID_DropTargetController, CDropTargetController)
OBJECT_ENTRY(CLSID_CursorController, CCursorController)
OBJECT_ENTRY(CLSID_SysStatsSens, CSens)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	AtlTrace("COMControllers::DllMain - in\n");

    lpReserved;
#ifdef _MERGE_PROXYSTUB
    if (!PrxDllMain(hInstance, dwReason, lpReserved))
        return FALSE;
#endif
    if (dwReason == DLL_PROCESS_ATTACH)
    {
		AtlTrace("COMControllers::DllMain: Attach\n");
#define MEM_CHECKS

#ifdef MEM_CHECKS
#ifdef _DEBUG
		// Check for memory leaks in debug version
		long lBreakPos = 0; // change this to hunt down a numbered leak 
		if( lBreakPos )		// in a debug build
			_CrtSetBreakAlloc(lBreakPos);
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
#endif
		::setlocale(LC_ALL, "");
        _Module.Init(ObjectMap, hInstance, &LIBID_COMCONTROLLERSLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
	{
		AtlTrace("COMControllers::DllMain: Detach\n");
        _Module.Term();
	}

	AtlTrace("COMControllers::DllMain - out\n");
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
#ifdef _MERGE_PROXYSTUB
    if (PrxDllCanUnloadNow() != S_OK)
        return S_FALSE;
#endif
	AtlTrace("COMControllers::DllCanUnloadNow - %d\n", _Module.GetLockCount());
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
#ifdef _MERGE_PROXYSTUB
    if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
        return S_OK;
#endif
#ifdef _DEBUG
	OLECHAR clsid[128];
	OLECHAR iid[128];
	StringFromGUID2(rclsid, clsid, 128);
	StringFromGUID2(riid, iid, 128);
	AtlTrace(L"COMControllers::DllGetClassObject %s - %s\n", clsid, iid);
#endif
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
#ifdef _MERGE_PROXYSTUB
    HRESULT hRes = PrxDllRegisterServer();
    if (FAILED(hRes))
        return hRes;
#endif
	AtlTrace("COMControllers::DllRegisterServer\n");
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
#ifdef _MERGE_PROXYSTUB
    PrxDllUnregisterServer();
#endif
	AtlTrace("COMControllers::DllUnregisterServer\n");
    return _Module.UnregisterServer(TRUE);
}
