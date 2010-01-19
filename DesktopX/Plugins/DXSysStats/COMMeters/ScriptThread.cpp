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
#include "COMMeters.h"
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
	scriptStream(0),
	longValue(0),
	doubleValue(0.0),
	stringValue("whatever")
{
	::VariantInit(&variantValue);

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

STDMETHODIMP ScriptThread::Update(long *dirty)
{
	*dirty = false;

	PostMessage(HiddenWindow::METER_UPDATE,
				(WPARAM)0,
				(LPARAM)0);

	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the meter should
 * be displayed so that the user can configure the meter to meet their
 * requirements.
 */
STDMETHODIMP ScriptThread::Configure(IObserver * observer, LONG hWnd)
{
	getScript()->Configure(NULL, hWnd);

	return S_OK;
}

STDMETHODIMP ScriptThread::ShowConsole(VARIANT_BOOL show)
{
	getScript()->ShowConsole(show);

	return S_OK;
}

/*
 * Return the meter's value as a long - if that makes sense. Some
 * overlays require a long value, some don't.
 *
 * Parameter 'sel' can be used to select one value from several if the
 * meter actually stores multiple values. It is up to the meter to
 * interpret the contents of 'sel' as it sees fit.
 */
STDMETHODIMP ScriptThread::GetAsLong(BSTR sel, long *pRet)
{
	PostMessage(HiddenWindow::METER_GETASLONG,
				(WPARAM)::SysAllocString(sel),
				(LPARAM)0);

	SysStatsUtils::Lock lock(valueLock);
	*pRet = longValue;

	return S_OK;
}

/*
 * Return the meter's value as a double - if that makes sense. Some
 * overlays require a double value, some don't.
 *
 * Parameter 'sel' can be used to select one value from several if the
 * meter actually stores multiple values. It is up to the meter to
 * interpret the contents of 'sel' as it sees fit.
 */
STDMETHODIMP ScriptThread::GetAsDouble(BSTR sel, double *pRet)
{
	PostMessage(HiddenWindow::METER_GETASDOUBLE,
				(WPARAM)::SysAllocString(sel),
				(LPARAM)0);

	SysStatsUtils::Lock lock(valueLock);
	*pRet = doubleValue;

	return S_OK;
}

/*
 * Return the meter's value as a string. If that makes sense. Some
 * overlays require a string value, some don't.
 *
 * Parameter 'format' should always be used to produce the resulting string.
 * It is a 'printf' style format string.
 *
 * Parameter 'sel' can be used to select one value from several if the
 * meter actually stores multiple values. It is up to the meter to
 * interpret the contents of 'sel' as it sees fit.
 */
STDMETHODIMP ScriptThread::GetAsString(BSTR format, BSTR selector, BSTR *pRet)
{
	PostMessage(HiddenWindow::METER_GETASSTRING,
				(WPARAM)::SysAllocString(format),
				(LPARAM)::SysAllocString(selector));

	SysStatsUtils::Lock lock(valueLock);
	*pRet = stringValue.copy();

	return S_OK;
}

/*
 * Return the meter's value in the most native format possible.
 * This is how scripts get a hold of the value.
 */
STDMETHODIMP ScriptThread::GetValue(VARIANT *pRet)
{
	PostMessage(HiddenWindow::METER_GETVALUE,
				(WPARAM)0,
				(LPARAM)0);

	SysStatsUtils::Lock lock(valueLock);
	::VariantCopy(pRet, &variantValue);

	return S_OK;
}

STDMETHODIMP ScriptThread::put_Model(IClientModel *newVal)
{
#ifdef notdef
	getScript()->put_Model(newVal);
#else
	PostMessage(HiddenWindow::METER_PUTMODEL,
				(WPARAM)newVal,
				(LPARAM)0);
#endif

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

STDMETHODIMP ScriptThread::put_Value(BSTR newVal)
{
	PostMessage(HiddenWindow::METER_PUTVALUE,
				(WPARAM)::SysAllocString(newVal),
				(LPARAM)0);

	return S_OK;
}

// Callback functions (from HiddenWindow)
BOOL ScriptThread::put_LongValue(long value)
{
	SysStatsUtils::Lock lock(valueLock);
	BOOL ret = longValue != value;
	longValue = value;
	return ret;
}

BOOL ScriptThread::put_DoubleValue(double value)
{
	SysStatsUtils::Lock lock(valueLock);
	BOOL ret = doubleValue != value;
	doubleValue = value;
	return ret;
}

BOOL ScriptThread::put_StringValue(BSTR value)
{
	SysStatsUtils::Lock lock(valueLock);
	_bstr_t _t_value(value);
	BOOL ret = stringValue != _t_value;
	stringValue = value;
	return ret;
}

BOOL ScriptThread::put_Value(VARIANT value)
{
	SysStatsUtils::Lock lock(valueLock);
	::VariantClear(&variantValue);
	::VariantCopy(&variantValue, &value);
	return true;
}

/*
 * Wait for the stream to be initialized before allowing any calls on pScript.
 */
IScriptMeter *ScriptThread::getScript()
{
	if (WaitForSingleObject(hScriptEvent, INFINITE) == WAIT_OBJECT_0)
	{
		if (!pScript)
		{
			HRESULT hr = CoGetInterfaceAndReleaseStream(scriptStream, IID_IScriptMeter, (void**)&pScript);
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

	IScriptMeter *localScript;
	HRESULT hr = CoCreateInstance(CLSID_ScriptMeter, NULL, CLSCTX_INPROC_SERVER, IID_IScriptMeter, (void**)&localScript);
	if (SUCCEEDED(hr))
	{
		// Marshal the pointer for other apartments too.
		HRESULT hr = CoMarshalInterThreadInterfaceInStream(IID_IScriptMeter, localScript, &scriptStream);

		window->setScript(localScript);
		window->setThread(this);

		SetEvent(hScriptEvent);	// Wake up the (possibly) waiting apartment.

		MSG msg;
		BOOL bRet;
		AtlTrace("Entering message loop\n");

		while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
		{
			AtlTrace("Got ScriptThread message\n");
			if (msg.message == HiddenWindow::WINDOW_QUIT)
			{
				AtlTrace("It was a quit message\n");
				PostQuitMessage(0);	// No. Really
			}
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}

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

