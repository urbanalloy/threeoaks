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

// ScriptConsole.h: interface for the CScriptConsole class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTCONSOLE_H__42234C7E_ED75_40B2_B1EB_448AEADEA830__INCLUDED_)
#define AFX_SCRIPTCONSOLE_H__42234C7E_ED75_40B2_B1EB_448AEADEA830__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dialog.h"
#include "resource.h"
#include "atlapp.h"
#include "atlframe.h"
#include "ComUtil.h"

#define USE_RESIZE

class IScriptConsole;

class CScriptConsole :
	public Dialog
#ifdef USE_RESIZE
	, public CDialogResize<CScriptConsole>
#endif
{
public:
	CScriptConsole(IScriptConsole *pParent, WORD idd=IDD_SCRIPT_CONSOLE);
	virtual ~CScriptConsole();

	virtual void AddString(BSTR s);

protected:
	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	IScriptConsole *pParent;
	_bstr_t buf;

public:
	BEGIN_MSG_MAP(CScriptConsole)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
		CHAIN_MSG_MAP(Dialog)
#ifdef USE_RESIZE
		CHAIN_MSG_MAP(CDialogResize<CScriptConsole>)
#endif
	END_MSG_MAP()

#ifdef USE_RESIZE
    BEGIN_DLGRESIZE_MAP(CScriptConsole)
		DLGRESIZE_CONTROL(IDC_CONSOLE_TEXT, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()
#endif
};

#endif // !defined(AFX_SCRIPTCONSOLE_H__42234C7E_ED75_40B2_B1EB_448AEADEA830__INCLUDED_)
