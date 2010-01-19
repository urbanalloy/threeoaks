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

// PlayerDialog.h : Declaration of the PlayerDialog

#ifndef __PLAYERDIALOG_H_
#define __PLAYERDIALOG_H_

#include "Dialog.h"

/////////////////////////////////////////////////////////////////////////////
// PlayerDialog

class CPlayer;

class PlayerDialog : public Dialog
{
public:
	PlayerDialog(CPlayer *pMeter, WORD idd=IDD_PLAYER);
	virtual ~PlayerDialog();

protected:
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT choosePath(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT autostart(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT loop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CPlayer *pMeter;

public:
	BEGIN_MSG_MAP(PlayerDialog)
		CHAIN_MSG_MAP(Dialog)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnOK)
		COMMAND_HANDLER(IDC_PLAYER_LOOP, BN_CLICKED, loop)
		COMMAND_HANDLER(IDC_PLAYER_AUTOSTART, BN_CLICKED, autostart)
		COMMAND_HANDLER(IDC_BUTTON_PLAYER_PATH, BN_CLICKED, choosePath)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()
};
#endif // __PLAYERDIALOG_H_
