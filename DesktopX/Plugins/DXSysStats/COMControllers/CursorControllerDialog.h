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

// CursorDialog.h : Declaration of the CursorDialog

#ifndef __CURSORCONTROLLERDIALOG_H_
#define __CURSORCONTROLLERDIALOG_H_

#include "Dialog.h"
#include "Resource.h"
#include "PickList.h"

/////////////////////////////////////////////////////////////////////////////
// CursorDialog

class CursorControllerDialog : public Dialog
{
public:
	CursorControllerDialog(ICursorController *pController, IObserver *observer, IMeterHome *meters, WORD idd=IDD_CURSOR);
	virtual ~CursorControllerDialog();

protected:
	LRESULT setCursor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	ICursorController *pController;
	PickList<long> cursorList;

public:
	BEGIN_MSG_MAP(CursorDialog)
		CHAIN_MSG_MAP(Dialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_COMBO_CURSOR, CBN_SELENDOK, setCursor)
	END_MSG_MAP()
};
#endif // __CURSORCONTROLLERDIALOG_H_
