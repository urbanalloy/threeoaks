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

#pragma once

#include "Dialog.h"
#include "ArrayPtr.cpp"
#include "PickList.h"

class CShortcut;
struct IObserver;
struct IMeterHome;

class ShortcutDialog : public Dialog
{
public:
	ShortcutDialog(IShortcut *pController, IObserver *observer, IMeterHome *meters, WORD idd=IDD_SHORTCUT);
	virtual ~ShortcutDialog();

protected:
	LRESULT startIn(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT args(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT selector(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT setMeter(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT setShortcut(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT chooseShortcut(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT chooseStartIn(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	IMeterHome *meters;
	IShortcut *pController;
	ArrayPtr<char> shortcuts;
	PickList<long> meterList;

public:
	BEGIN_MSG_MAP(Shortcut)
		CHAIN_MSG_MAP(Dialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_SELECTOR, EN_CHANGE, selector)
		COMMAND_HANDLER(IDC_DIRECTORY, EN_CHANGE, startIn)
		COMMAND_HANDLER(IDC_COMMANDLINE, EN_CHANGE, args)
		COMMAND_HANDLER(IDC_COMBO_METER, CBN_SELENDOK, setMeter)
		COMMAND_ID_HANDLER(IDC_COMBO_APP, setShortcut)
		COMMAND_ID_HANDLER(IDC_BROWSE, chooseShortcut)
		COMMAND_ID_HANDLER(IDC_BROWSE_DIR, chooseStartIn)
	END_MSG_MAP()
};

