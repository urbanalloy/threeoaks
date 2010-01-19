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

// ScriptControllerDialog.h : Declaration of the ScriptControllerDialog

#ifndef __SCRIPTCONTROLLERDIALOG_H_
#define __SCRIPTCONTROLLERDIALOG_H_

#include "CSubject.h"
#include "Dialog.h"

/////////////////////////////////////////////////////////////////////////////
// ScriptControllerDialog

class CScriptController;
struct IObserver;
struct IMeterHome;

class ScriptControllerDialog : public Dialog, public CSubject
{
public:
	ScriptControllerDialog(CScriptController *pController, IObserver *observer, IMeterHome *meters, WORD idd=IDD_SCRIPTCONTROLLER);
	virtual ~ScriptControllerDialog();

protected:
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT chooseScript(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT showConsole(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CScriptController *pController;
	IMeterHome *meters;

public:
	BEGIN_MSG_MAP(ScriptControllerDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnOK)
		COMMAND_HANDLER(IDCANCEL2, BN_CLICKED, OnCancel)
		COMMAND_HANDLER(IDC_BUTTON_SCRIPT_NAME, BN_CLICKED, chooseScript)
		COMMAND_HANDLER(IDC_CONSOLE, BN_CLICKED, showConsole)
		CHAIN_MSG_MAP(Dialog)
	END_MSG_MAP()
};
#endif // __SCRIPTCONTROLLERDIALOG_H_
