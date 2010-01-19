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

class CPDHMeter;

class PDHDialog : public Dialog
{
public:
	PDHDialog(CPDHMeter *pMeter, WORD idd=IDD_METER_PDH);
	virtual ~PDHDialog();

protected:
	LRESULT showPDHConfig(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT setScale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CPDHMeter *pMeter;
	PickList<double> scaleList;

public:
	BEGIN_MSG_MAP(PDHDialog)
		CHAIN_MSG_MAP(Dialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_COMBO_SCALE, setScale)
		COMMAND_ID_HANDLER(IDC_METER_CONFIGURE, showPDHConfig)
	END_MSG_MAP()
};