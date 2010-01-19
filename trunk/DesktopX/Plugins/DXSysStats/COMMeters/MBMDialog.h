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

class CMBMMeter;

class MBMDialog : public Dialog
{
public:
	MBMDialog(CMBMMeter *pMeter, WORD idd=IDD_METER_MBM);
	virtual ~MBMDialog();

protected:
	LRESULT setType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT setName(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT setReading(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT scale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CMBMMeter *pMeter;

	PickList<long> typeList;
	PickList<long> nameList;
	PickList<long> readingList;
	PickList<double> scaleList;

public:
	BEGIN_MSG_MAP(MBMDialog)
		CHAIN_MSG_MAP(Dialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_COMBO_SCALE, scale)
		COMMAND_HANDLER(IDC_COMBO_TYPE, CBN_SELENDOK, setType)
		COMMAND_HANDLER(IDC_COMBO_INSTANCE, CBN_SELENDOK, setName)
		COMMAND_HANDLER(IDC_COMBO_READING, CBN_SELENDOK, setReading)
	END_MSG_MAP()

protected:
	LRESULT resetInstanceList();

	struct Offsets
	{
		int start;
		int end;	// >=start
	};
	Offsets offsets[6];
};