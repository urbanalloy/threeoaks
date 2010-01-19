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

// SysStatsCOM.cpp : Implementation of DLL Exports.


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
//      Modify the custom build rule for SysStatsCOM.idl by adding the following 
//      files to the Outputs.
//          SysStatsCOM_p.c
//          dlldata.c
//      To build a separate proxy/stub DLL, 
//      run nmake -f SysStatsCOMps.mk in the project directory.

#include "stdafx.h"

#include <locale.h>
#include <CommCtrl.h>
#include "resource.h"
#include <initguid.h>
#include "SysStatsCOM.h"
#include "dlldatax.h"

#include "SysStatsCOM_i.c"
#include "CClientModel.h"
#include "CMeterHome.h"
#include "COverlayHome.h"
#include "CCompositeOverlay.h"
#include "AppConfig.h"
#include "IINIPersister.h"
#include "ControllerHome.h"
#include "Label.h"
#include "WeakReference.h"
#include "WeakTarget.h"
#include "ScriptConnectionObject.h"
#include "ScriptUtils.h"
#include "MetaData.h"
#include "COMINIFile.h"

#ifdef _MERGE_PROXYSTUB
extern "C" HINSTANCE hProxyDll;
#endif

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_ClientModel, CClientModel)
OBJECT_ENTRY(CLSID_MeterHome, CMeterHome)
OBJECT_ENTRY(CLSID_OverlayHome, COverlayHome)
OBJECT_ENTRY(CLSID_CompositeOverlay, CCompositeOverlay)
OBJECT_ENTRY(CLSID_AppConfig, CAppConfig)
OBJECT_ENTRY(CLSID_IINIPersister, CIINIPersister)
OBJECT_ENTRY(CLSID_ControllerHome, CControllerHome)
OBJECT_ENTRY(CLSID_Label, CLabel)
OBJECT_ENTRY(CLSID_WeakReference, CWeakReference)
OBJECT_ENTRY(CLSID_WeakTarget, CWeakTarget)
OBJECT_ENTRY(CLSID_ScriptConnectionObject, CScriptConnectionObject)
OBJECT_ENTRY(CLSID_ScriptUtils, CScriptUtils)
OBJECT_ENTRY(CLSID_MetaData, CMetaData)
OBJECT_ENTRY(CLSID_COMINIFile, CCOMINIFile)
END_OBJECT_MAP()

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    lpReserved;
#ifdef _MERGE_PROXYSTUB
    if (!PrxDllMain(hInstance, dwReason, lpReserved))
        return FALSE;
#endif
    if (dwReason == DLL_PROCESS_ATTACH)
    {
#ifdef notdef
		INITCOMMONCONTROLSEX controlDefs;
		controlDefs.dwSize = sizeof(INITCOMMONCONTROLSEX);
		controlDefs.dwICC = ICC_TREEVIEW_CLASSES;
		InitCommonControlsEx(&controlDefs);
#else
		InitCommonControls();
#endif
		AtlTrace("SysStatsCOM::DllMain: Attach\n");
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
        _Module.Init(ObjectMap, hInstance, &LIBID_SYSSTATSCOMLib);
		AtlTrace("SysStatsCOM::DllMain: Attach 1\n");
        DisableThreadLibraryCalls(hInstance);
		PluginManager::getManager();	// Initialize pluginmanager
    }
    else if (dwReason == DLL_PROCESS_DETACH)
	{
		AtlTrace("SysStatsCOM::DllMain: Detach\n");
		PluginManager::destroyManager();	// Destroy pluginmanager
        _Module.Term();
	}
	AtlTrace("SysStatsCOM::DllMain: out at (%ld) locks\n", 
		_Module.GetLockCount());	// a dll should unload at 0 locks, no COM objects
									// leaked
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
	AtlTrace("SysStatsCOM::DllCanUnloadNow - %d\n", _Module.GetLockCount());
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
	AtlTrace(L"SysStatsCOM::DllGetClassObject %s - %s\n", clsid, iid);
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
    // registers object, typelib and all interfaces in typelib
	AtlTrace("SysStatsCOM::DllRegisterServer\n");
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
#ifdef _MERGE_PROXYSTUB
    PrxDllUnregisterServer();
#endif
	AtlTrace("SysStatsCOM::DllUnregisterServer\n");
    return _Module.UnregisterServer(TRUE);
}
