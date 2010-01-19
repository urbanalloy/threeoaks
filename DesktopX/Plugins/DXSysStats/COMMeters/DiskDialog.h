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

class CDiskMeter;

class DiskDialog : public Dialog
{
public:
	DiskDialog(CDiskMeter *pMeter, WORD idd=IDD_METER_DISK);
	virtual ~DiskDialog();

protected:
	LRESULT scale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT metric(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT path(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT chooseDir(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CDiskMeter *pMeter;

	PickList<long> selList;
	PickList<double> scaleList;

public:
	BEGIN_MSG_MAP(DiskDialog)
		CHAIN_MSG_MAP(Dialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_COMBO_SCALE, scale)
		COMMAND_HANDLER(IDC_COMBO_SELECTOR, CBN_SELENDOK, metric)
		COMMAND_HANDLER(IDC_DIRECTORY, EN_CHANGE, path)
		COMMAND_ID_HANDLER(IDC_BROWSE_DIR, chooseDir)
	END_MSG_MAP()
};