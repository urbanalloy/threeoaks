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

// ScriptThread.cpp: implementation of the ScriptThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "COMControllers.h"
#include "HiddenWindow.h"
#include "ScriptThread.h"
#pragma comment(lib, "SysStatsCommon.lib")

DWORD WINAPI ScriptThread::_scriptThread(LPVOID in)
{
	ScriptThread *ut = (ScriptThread*)in;
	return ut->scriptThread();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ScriptThread::ScriptThread() :
	pScript(0),
	scriptStream(0)
{
	hScriptEvent = CreateEvent( 
        NULL,         // no security attributes
        TRUE,        // Manual reset event
        FALSE,        // initial state is non-signaled - i.e. waiters will block
        NULL		 // object name
     );

	hScriptThread = CreateThread(NULL, 0, _scriptThread, (VOID *) this, 0, &threadID);
	getScript();	// Wait for the message loop to start up.
}

ScriptThread::~ScriptThread()
{
	AtlTrace("Destroying script thread");
	if (pScript)
	{
		pScript->Release();
		pScript = NULL;
	}
}

/*
 * We need to post messages to a window so that Modal dialogs and shit will work.
 */
void ScriptThread::PostMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	::PostMessage(hWnd, Msg, wParam, lParam);
	::SwitchToThread();
}

/*
 * Thread will terminate eventually
 */
void ScriptThread::terminate()
{
	PostMessage(HiddenWindow::WINDOW_QUIT, 0, 0);
}

STDMETHODIMP ScriptThread::HandleMessage(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y)
{
	MessageParams *wp = new MessageParams(wParam, lParam, x, y);

	PostMessage(HiddenWindow::CONTROLLER_HANDLEMESSAGE,
				(WPARAM)wp,
				(LPARAM)0);

	return S_OK;
}

STDMETHODIMP ScriptThread::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	getScript()->Configure(NULL, NULL, (long)hWnd);

	return S_OK;
}

STDMETHODIMP ScriptThread::ShowConsole(VARIANT_BOOL show)
{
	getScript()->ShowConsole(show);

	return S_OK;
}

STDMETHODIMP ScriptThread::put_Model(IClientModel *newVal)
{
	PostMessage(HiddenWindow::CONTROLLER_PUTMODEL,
				(WPARAM)newVal,
				(LPARAM)0);

	return S_OK;
}

STDMETHODIMP ScriptThread::get_ScriptType(BSTR *pRet)
{
	getScript()->get_ScriptType(pRet);

	return S_OK;
}

STDMETHODIMP ScriptThread::put_ScriptType(BSTR newVal)
{
	getScript()->put_ScriptType(newVal);

	return S_OK;
}

STDMETHODIMP ScriptThread::get_FileName(BSTR *pRet)
{
	getScript()->get_FileName(pRet);

	return S_OK;
}

STDMETHODIMP ScriptThread::put_FileName(BSTR newVal)
{
	getScript()->put_FileName(newVal);

	return S_OK;
}

/*
 * Wait for the stream to be initialized before allowing any calls on pScript.
 */
IScriptController *ScriptThread::getScript()
{
	if (WaitForSingleObject(hScriptEvent, INFINITE) == WAIT_OBJECT_0)
	{
		if (!pScript)
		{
			HRESULT hr = CoGetInterfaceAndReleaseStream(scriptStream, IID_IScriptController, (void**)&pScript);
			pScript->AddRef();
		}

		return pScript;
	}

	return NULL;
}

/*
 * Use the windows message queue as a thread-safe way to serialize calls.
 */
DWORD WINAPI ScriptThread::scriptThread(/*LPVOID lpInput*/)
{
	AtlTrace("Creating script thread\n");

	// We want a single threaded apartment.
	CoInitialize(NULL);

	// We need a hidden window so we can post messages with a window ID
	// otherwise we can't do modal dialogs.
	HiddenWindow *window = new HiddenWindow();
	RECT whatever = { 0, 0, 0, 0 };
	hWnd = window->Create(NULL, whatever, "HiddenWindow", 0);
	AtlTrace("Window created\n");

	IScriptController *localScript;
	HRESULT hr = CoCreateInstance(CLSID_ScriptController, NULL, CLSCTX_INPROC_SERVER, IID_IScriptController, (void**)&localScript);
	if (SUCCEEDED(hr))
	{
		// Marshal the pointer for other apartments too.
		HRESULT hr = CoMarshalInterThreadInterfaceInStream(IID_IScriptController, localScript, &scriptStream);

		window->setScript(localScript);

		SetEvent(hScriptEvent);	// Wake up the (possibly) waiting apartment.

		MSG msg;
		BOOL bRet;
		AtlTrace("Entering message loop\n");

		while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
		{
//_ATL_DEBUG_INTERFACES
			AtlTrace("Got ScriptThread message: 0x%0.4x\n", msg.message);
			if (msg.message == HiddenWindow::WINDOW_QUIT)
			{
				AtlTrace("It was a quit message\n");
				PostQuitMessage(0);	// No. Really
			}
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}

		AtlTrace("Releasing localScript\n");
		localScript->Release();
	}

	::DestroyWindow(window->m_hWnd);
	delete window;

	AtlTrace("Exiting Script thread\n");
	//Exit the thread
	CoUninitialize();
	CloseHandle(hScriptEvent);
	CloseHandle(hScriptThread);
	hScriptThread = NULL;

	delete this;	// Only way to guarantee that the object hangs around until
					// the thread exits without waiting on the thread.

	return 0;
}

