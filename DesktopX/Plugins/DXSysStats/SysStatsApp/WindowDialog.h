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

// WindowDialog.h: interface for the WindowDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWDIALOG_H__A1272EF7_6351_4282_B53A_3791BB62F566__INCLUDED_)
#define AFX_WINDOWDIALOG_H__A1272EF7_6351_4282_B53A_3791BB62F566__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma once

#include "resource.h"
#include "Dialog.h"
#include "CSubject.h"

class SysStatsClientWindow;

class WindowDialog : public CSubject, public Dialog
{
public:
	WindowDialog(SysStatsClientWindow *window, IObserver *observer, WORD idd=IDD_DIALOG_WINDOW);
	virtual ~WindowDialog();

protected:
	// Initialise controls
	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT x(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT y(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT width(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT height(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT name(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
	SysStatsClientWindow *window;

public:
	BEGIN_MSG_MAP(WindowDialog)
		CHAIN_MSG_MAP(Dialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_XPOS, EN_CHANGE, x)
		COMMAND_HANDLER(IDC_YPOS, EN_CHANGE, y)
		COMMAND_HANDLER(IDC_WIDTH, EN_CHANGE, width)
		COMMAND_HANDLER(IDC_HEIGHT, EN_CHANGE, height)
		COMMAND_HANDLER(IDC_NAME, EN_CHANGE, name)
	END_MSG_MAP()
};

#endif // !defined(AFX_WINDOWDIALOG_H__A1272EF7_6351_4282_B53A_3791BB62F566__INCLUDED_)
