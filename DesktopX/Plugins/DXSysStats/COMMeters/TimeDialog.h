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
#include "PickList.h"

class CTimeMeter;

class TimeDialog : public Dialog
{
public:
	TimeDialog(CTimeMeter *pMeter, WORD idd=IDD_METER_TIME);
	virtual ~TimeDialog();

protected:
	LRESULT tzDesc(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT tz(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT forceDST(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CTimeMeter *pMeter;
	PickList<_bstr_t> tzList;
	_bstr_t xml;

public:
	BEGIN_MSG_MAP(TimeDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_EDIT_TZDESC, EN_CHANGE, tzDesc)
//		COMMAND_HANDLER(IDC_CHECK_FORCEDST, BN_CLICKED, forceDST)
		COMMAND_HANDLER(IDC_COMBO_TZ, CBN_SELENDOK, tz)
		CHAIN_MSG_MAP(Dialog)
	END_MSG_MAP()
};