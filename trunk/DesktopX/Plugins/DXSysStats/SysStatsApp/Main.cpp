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

#include "stdafx.h"

#include <CommCtrl.h>

#include <direct.h>
#include <stdlib.h>
#include <io.h>

#include "SysStatsUtils.h"
#include "AppResources.h"
#include "SysStatsClientWindow.h"
#include "SysStatsApp.h"
#include "WindowTimerService.h"

#include <Gdiplus.h>
using namespace Gdiplus;

typedef HRESULT (*DLLFunc)();

// This is supposed to happen automatically if we use an ATL project.
CComModule _Module;

int __stdcall WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
#ifdef MEM_CHECKS
#ifdef _DEBUG
		// Check for memory leaks in debug version
		long lBreakPos = 0; // change this to hunt down a numbered leak
		if( lBreakPos )		// in a debug build
			_CrtSetBreakAlloc(lBreakPos);
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
#endif
#ifdef notdef
	INITCOMMONCONTROLSEX controlDefs;
	controlDefs.dwSize = sizeof(INITCOMMONCONTROLSEX);
	controlDefs.dwICC = ICC_TREEVIEW_CLASSES;
	InitCommonControlsEx(&controlDefs);
#else
	InitCommonControls();
#endif

	_Module.Init(NULL, hInstance);
	::OleInitialize(NULL);
//	CoInitialize(NULL);

	// First off, register any COM objects in SysStatsCOM.dll.
	// Has to be called after CoInitialize
	HMODULE hDLL = GetModuleHandle("SysStatsCOM.dll");
	if (hDLL)
	{
		DLLFunc pFunc = (DLLFunc)GetProcAddress(hDLL, "DllRegisterServer");
		if (pFunc)
		{
			// call the function
			AtlTrace("Registering SysStatsCOM server\n");
			HRESULT hr = pFunc();
			if (!SUCCEEDED(hr))
			{
				AtlTrace("RegisterServer failed=%x\n", hr);
			}
		}
	}
	else
	{
		AtlTrace("Could not retrieve handle to SysStatsCOM.dll\n");
	}

	// Initialize GDI+.
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	MSG msg;
//	SysStatsUtils::SetLFHeaps();

	{
		// All this stuff has to be in a block to allow the destructors to
		// run before we CoUninitialize etc.
		SysStatsApp app(lpCmdLine, nCmdShow);
		app.SetShellIcon(IDI_ICON1);
		app.SetShellVisible();
		app.load();

		BOOL bRet;

		while( (bRet = ::GetMessage( &msg, NULL, 0, 0 )) != 0)
		{
			AtlTrace("Msg=0x%0.4x\n", msg.message);
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		app.save();
		app.SetShellVisible(false);
	}

	GdiplusShutdown(gdiplusToken);
	::OleUninitialize();
//	CoUninitialize();
	_Module.Term();

    return msg.wParam;
}
