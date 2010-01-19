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

// ScriptConsole.cpp: implementation of the CScriptConsole class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "IScriptConsole.h"
#include "ScriptConsole.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScriptConsole::CScriptConsole(IScriptConsole *pParent, WORD idd) :
	buf(""),
	pParent(pParent),
	Dialog(idd)
{
}

CScriptConsole::~CScriptConsole()
{
}

// ---------------------------------------------------------------------------
LRESULT CScriptConsole::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ((HWND)lParam == GetDlgItem(IDC_CONSOLE_TEXT))
	{
		COLORREF crBkColor = ::GetSysColor(COLOR_WINDOW);
		return (LRESULT)::CreateSolidBrush(crBkColor);
	}
	else
	{
		bHandled = false;
	}

	return 0;
}

LRESULT CScriptConsole::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
//	pParent->Show(false);
	DestroyWindow();
	buf = "";

	return 1;
}

LRESULT CScriptConsole::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
//	pParent->Show(false);
	DestroyWindow();
	buf = "";

	return 1;
}

void CScriptConsole::AddString(BSTR s)
{
	buf += s;
	::SetDlgItemTextW(m_hWnd, IDC_CONSOLE_TEXT, buf);
	::SendDlgItemMessage(m_hWnd, IDC_CONSOLE_TEXT, WM_VSCROLL, (WPARAM)SB_BOTTOM, 0);
}

LRESULT CScriptConsole::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#ifdef USE_RESIZE
    // Init the CDialogResize code
    DlgResize_Init();
#endif
	::SetDlgItemTextW(m_hWnd, IDC_CONSOLE_TEXT, buf);

	bHandled = false;
	return 1;
}
