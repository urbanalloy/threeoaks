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

// COMMeters.cpp : Implementation of DLL Exports.


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
//      Modify the custom build rule for COMMeters.idl by adding the following 
//      files to the Outputs.
//          COMMeters_p.c
//          dlldata.c
//      To build a separate proxy/stub DLL, 
//      run nmake -f COMMetersps.mk in the project directory.

#include "stdafx.h"

#include <locale.h>
#include "resource.h"
#include <initguid.h>
#include "COMMeters.h"
#include "dlldatax.h"

#include "COMMeters_i.c"
#include "PDHMeter.h"
#include "ScriptMeter.h"
#include "MemoryMeter.h"
#include "DiskMeter.h"
#include "TimeMeter.h"
#include "MBMMeter.h"
#include "XMLMeter.h"
#include "FixedMeter.h"
#include "ScriptMeterImpl.h"
#include "ASyncScriptMeter.h"
#include "Animation.h"
#include "MailMeter.h"
#include "SpeedFanMeter.h"
#include "Player.h"

#ifdef _MERGE_PROXYSTUB
extern "C" HINSTANCE hProxyDll;
#endif

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_PDHMeter, CPDHMeter)
OBJECT_ENTRY(CLSID_ScriptMeter, CScriptMeter)
OBJECT_ENTRY(CLSID_MemoryMeter, CMemoryMeter)
OBJECT_ENTRY(CLSID_DiskMeter, CDiskMeter)
OBJECT_ENTRY(CLSID_TimeMeter, CTimeMeter)
OBJECT_ENTRY(CLSID_MBMMeter, CMBMMeter)
OBJECT_ENTRY(CLSID_XMLMeter, CXMLMeter)
OBJECT_ENTRY(CLSID_FixedMeter, CFixedMeter)
OBJECT_ENTRY(CLSID_ScriptMeterImpl, CScriptMeterImpl)
OBJECT_ENTRY(CLSID_ASyncScriptMeter, CASyncScriptMeter)
OBJECT_ENTRY(CLSID_Animation, CAnimation)
OBJECT_ENTRY(CLSID_MailMeter, CMailMeter)
OBJECT_ENTRY(CLSID_SpeedFanMeter, CSpeedFanMeter)
OBJECT_ENTRY(CLSID_Player, CPlayer)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	AtlTrace("COMMeters::DllMain - in\n");

    lpReserved;
#ifdef _MERGE_PROXYSTUB
    if (!PrxDllMain(hInstance, dwReason, lpReserved))
        return FALSE;
#endif
    if (dwReason == DLL_PROCESS_ATTACH)
    {
		AtlTrace("COMMeters::DllMain: Attach\n");
#define MEM_CHECKS

#ifdef MEM_CHECKS
#ifdef _DEBUG
		// Check for memory leaks in debug version
		long lBreakPos = 0; // change this to hunt down a numbered leak 
		if( lBreakPos )		// in a debug build
			_CrtSetBreakAlloc(lBreakPos);
		_CrtSetDbgFlag ( _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
#endif

		::setlocale(LC_ALL, "");
        _Module.Init(ObjectMap, hInstance, &LIBID_COMMETERSLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
	{
		AtlTrace("COMMeters::DllMain: Detach\n");
        _Module.Term();
	}

	AtlTrace("COMMeters::DllMain - out\n");

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
	AtlTrace("COMMeters::DllCanUnloadNow - %d\n", _Module.GetLockCount());
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
	AtlTrace(L"COMMeters::DllGetClassObject %s - %s\n", clsid, iid);
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
	AtlTrace("COMMeters::DllRegisterServer\n");
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
#ifdef _MERGE_PROXYSTUB
    PrxDllUnregisterServer();
#endif
	AtlTrace("COMMeters::DllUnregisterServer\n");
    return _Module.UnregisterServer(TRUE);
}


