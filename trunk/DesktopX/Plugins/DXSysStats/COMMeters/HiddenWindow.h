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

// HiddenWindow.h: interface for the HiddenWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HIDDENWINDOW_H__C9548EEA_06A4_4DD8_87A0_1DEB24055704__INCLUDED_)
#define AFX_HIDDENWINDOW_H__C9548EEA_06A4_4DD8_87A0_1DEB24055704__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlwin.h>

class ScriptThread;
struct IScriptMeter;

class HiddenWindow : public CWindowImpl<HiddenWindow, CWindow, CWinTraits<0, 0> >
{
public:
	HiddenWindow();
	virtual ~HiddenWindow();

	// All params are ignored, event = long *dirty
	LRESULT Update(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// wParam = BSTR format, lParam = BSTR selector, event = BSTR out
	LRESULT GetAsString(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// wParam = BSTR selector, lParam = ignored, event = double out
	LRESULT GetAsDouble(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// wParam = BSTR selector, lParam = ignored, event = long out
	LRESULT GetAsLong(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// wParam = BSTR selector, lParam = ignored, event = double out
	LRESULT GetValue(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// wParam = BSTR selector, lParam = ignored, event = nothing out
	LRESULT PutFilename(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// wParam = BSTR value, lParam = ignored, event = nothing out
	LRESULT PutValue(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// wParam = IClientModel* model, lParam = ignored, event = nothing out
	LRESULT PutModel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void setScript(IScriptMeter *newVal);
	void setThread(ScriptThread *thread);

BEGIN_MSG_MAP(HiddenWindow)
	MESSAGE_HANDLER(METER_UPDATE, Update)
	MESSAGE_HANDLER(METER_GETASSTRING, GetAsString)
	MESSAGE_HANDLER(METER_GETASLONG, GetAsLong)
	MESSAGE_HANDLER(METER_GETASDOUBLE, GetAsDouble)
	MESSAGE_HANDLER(METER_GETVALUE, GetValue)
	MESSAGE_HANDLER(METER_PUTFILENAME, PutFilename)
	MESSAGE_HANDLER(METER_PUTVALUE, PutValue)
	MESSAGE_HANDLER(METER_PUTMODEL, PutModel)
END_MSG_MAP()

// public constant strings
	static const unsigned long METER_UPDATE;
	static const unsigned long METER_GETASSTRING;
	static const unsigned long METER_GETASDOUBLE;
	static const unsigned long METER_GETASLONG;
	static const unsigned long METER_GETVALUE;
	static const unsigned long METER_PUTFILENAME;
	static const unsigned long METER_PUTVALUE;
	static const unsigned long METER_PUTMODEL;
	static const unsigned long WINDOW_QUIT;

private:
	IScriptMeter *pScript;
	ScriptThread *pThread;
};

#endif // !defined(AFX_HIDDENWINDOW_H__C9548EEA_06A4_4DD8_87A0_1DEB24055704__INCLUDED_)
