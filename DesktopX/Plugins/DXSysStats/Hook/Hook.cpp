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

// Hook.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <atlbase.h>
#include "Hook.h"

#pragma data_seg(".SYSCMD")
UINT monitorOff = (UINT)-1;	// Unknown
UINT UWM_SYSCOMMAND = 0;
UINT numHooked = 0;
HHOOK hook = NULL;
#pragma data_seg()
#pragma comment(linker, "/section:.SYSCMD,rws")

static HINSTANCE hInst = NULL;
static HANDLE hMutex = NULL;

// Forward declaration
static LRESULT CALLBACK msghook(UINT nCode, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain( HINSTANCE hInstance, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
    //**********************************************
    // PROCESS_ATTACH
    //**********************************************
    case DLL_PROCESS_ATTACH:
       // Save the instance handle because we need it to set the hook later
       hInst = hInstance;
       // This code initializes the hook notification message
       UWM_SYSCOMMAND = RegisterWindowMessage(UWM_SYSCOMMAND_MSG);
	   AtlTrace("regmsg=%d\n", UWM_SYSCOMMAND);
		hMutex = CreateMutex( 
			NULL,		// no security attributes
			FALSE,		// initially not owned
			"SysStatsHook"	// object name
		);

       return TRUE;

    //**********************************************
    // PROCESS_DETACH
    //**********************************************
    case DLL_PROCESS_DETACH:
		CloseHandle(hMutex);
       return TRUE;
    }
    return TRUE;
}


__declspec(dllexport) UINT getSysCommandMsg()
{
	return UWM_SYSCOMMAND;
}

/****************************************************************************
*                                 setMyHook
* Inputs:
*       HWND hWnd: Window to notify
* Result: BOOL
*       TRUE if successful
*	FALSE if error
* Effect: 
*       Sets the hook
****************************************************************************/

__declspec(dllexport) void setSysCommandHook()
{
	if (WaitForSingleObject(hMutex, INFINITE) == WAIT_OBJECT_0)
	{
		AtlTrace("numHooked(++)=%d\n", numHooked);
		if (numHooked++ == 0) {
			hook = SetWindowsHookEx(WH_GETMESSAGE,
								(HOOKPROC)msghook,
								hInst,
								0);
		}

		ReleaseMutex(hMutex);
	}
} // setMyHook

/****************************************************************************
*                                 clearMyHook
* Inputs:
*       HWND hWnd: Window hook
* Result: BOOL
*       TRUE if successful
*	FALSE if error
* Effect: 
*       Removes the hook that has been set
****************************************************************************/

__declspec(dllexport) void clearSysCommandHook()
{
	AtlTrace("Clearing SysCommand Hook\n");

	if (WaitForSingleObject(hMutex, INFINITE) == WAIT_OBJECT_0)
	{
		if (--numHooked == 0) {
			/*BOOL unhooked = */UnhookWindowsHookEx(hook);
			DWORD res;
			::SendMessageTimeout(HWND_BROADCAST, WM_NULL, 0, 0, SMTO_NORMAL, 1000, &res);
		}
		AtlTrace("numHooked(--)=%d\n", numHooked);

		ReleaseMutex(hMutex);
	}
} // clearMyHook

/****************************************************************************
*                                   msghook
* Inputs:
*       int nCode: Code value
*	WPARAM wParam:
*	LPARAM lParam:
* Result: LRESULT
*       Either 0 or the result of CallNextHookEx
* Effect: 
****************************************************************************/

static LRESULT CALLBACK msghook(UINT nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode < 0)
	{ /* pass it on */
		CallNextHookEx(hook, nCode, wParam, lParam);
		return 0;
	} /* pass it on */

	LPMSG msg = (LPMSG)lParam;

	if(msg->message == WM_SYSCOMMAND && (msg->wParam & 0xFFF0) == SC_MONITORPOWER)
	{
		AtlTrace("Broadcasting message\n");

		if (WaitForSingleObject(hMutex, INFINITE) == WAIT_OBJECT_0)
		{
			monitorOff = 1;
			::PostMessage(HWND_BROADCAST, UWM_SYSCOMMAND, msg->wParam, msg->lParam);
			ReleaseMutex(hMutex);
		}
	} else if ((msg->message == WM_KEYDOWN || msg->message == WM_MOUSEMOVE) && (monitorOff == 1)) {
		AtlTrace("Broadcasting power on message\n");
		if (WaitForSingleObject(hMutex, INFINITE) == WAIT_OBJECT_0)
		{
			monitorOff = 0;
			::PostMessage(HWND_BROADCAST, UWM_SYSCOMMAND, SC_MONITORPOWER, -1);
			ReleaseMutex(hMutex);
		}
	}

	return CallNextHookEx(hook, nCode, wParam, lParam);
} // msghook
