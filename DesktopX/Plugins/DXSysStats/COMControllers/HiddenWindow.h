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
struct IScriptController;

class HiddenWindow : public CWindowImpl<HiddenWindow, CWindow, CWinTraits<0, 0> >
{
public:
	HiddenWindow();
	virtual ~HiddenWindow();

	// wParam = MessageParams, lParam = ignored, event = nothing out
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// wParam = BSTR selector, lParam = ignored, event = nothing out
	LRESULT PutFilename(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// wParam = IClientModel* model, lParam = ignored, event = nothing out
	LRESULT PutModel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void setScript(IScriptController *newVal);

BEGIN_MSG_MAP(HiddenWindow)
	MESSAGE_HANDLER(CONTROLLER_HANDLEMESSAGE, HandleMessage)
	MESSAGE_HANDLER(CONTROLLER_PUTFILENAME, PutFilename)
	MESSAGE_HANDLER(CONTROLLER_PUTMODEL, PutModel)
END_MSG_MAP()

// public constant strings
	static const unsigned long CONTROLLER_HANDLEMESSAGE;
	static const unsigned long CONTROLLER_PUTFILENAME;
	static const unsigned long CONTROLLER_PUTMODEL;
	static const unsigned long WINDOW_QUIT;

private:
	IScriptController *pScript;
};

#endif // !defined(AFX_HIDDENWINDOW_H__C9548EEA_06A4_4DD8_87A0_1DEB24055704__INCLUDED_)
