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

// ScriptThread.h: interface for the ScriptThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTTHREAD_H__B5A545EB_03BA_47E6_9FAC_7CC89FD704DC__INCLUDED_)
#define AFX_SCRIPTTHREAD_H__B5A545EB_03BA_47E6_9FAC_7CC89FD704DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols
#include <comutil.h>
#include "SysStatsUtils.h"

struct IScriptMeter;
struct IObserver;
struct IClientModel;

/*
 * Used by ASyncScriptMeter
 */
class ScriptThread  
{
public:
	ScriptThread();
	virtual ~ScriptThread();

	void terminate();

// IASyncScriptMeter
public:
	STDMETHOD(get_FileName)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_FileName)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ScriptType)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ScriptType)(/*[in]*/ BSTR newVal);
	STDMETHOD(put_Value)(/*[in]*/ BSTR newVal);
	STDMETHOD(ShowConsole)(/*[in]*/ VARIANT_BOOL show);

// IMeter
public:
	STDMETHOD(Update)(/*[out, retval]*/ long *dirty);
	STDMETHOD(Configure)(IObserver * observer, LONG hWnd);
	STDMETHOD(GetAsString)(/*[in]*/ BSTR format, /*[in]*/ BSTR selector, /*[out, retval]*/ BSTR *pRet);
	STDMETHOD(GetAsDouble)(/*[in]*/ BSTR selector, /*[out, retval]*/ double *pRet);
	STDMETHOD(GetAsLong)(/*[in]*/ BSTR selector, /*[out,retval]*/ long *pRet);
	STDMETHOD(GetValue)(/*[out,retval]*/ VARIANT *pRet);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel* newVal);

	BOOL put_LongValue(long value);
	BOOL put_DoubleValue(double value);
	BOOL put_StringValue(BSTR value);
	BOOL put_Value(VARIANT value);
	BOOL put_SValue(BSTR value);

private:
	void PostMessage(UINT Msg, WPARAM wParam, LPARAM lParam);
	DWORD  WINAPI scriptThread(/*LPVOID lpInput*/);
	IScriptMeter* getScript();

	IScriptMeter	*pScript;
	HWND			hWnd;
	IStream			*scriptStream;
	HANDLE			hScriptEvent;
	HANDLE			hScriptThread;
	DWORD			threadID;

	// Cached values
	SysStatsUtils::CriticalSection valueLock;

	long			longValue;
	double			doubleValue;
	_bstr_t			stringValue;
	VARIANT			variantValue;

	static DWORD WINAPI _scriptThread(LPVOID in);
};

#endif // !defined(AFX_SCRIPTTHREAD_H__B5A545EB_03BA_47E6_9FAC_7CC89FD704DC__INCLUDED_)
