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

// SpeedFanMeterDialog.h : Declaration of the SpeedFanMeterDialog

#ifndef __SPEEDFANMETERDIALOG_H_
#define __SPEEDFANMETERDIALOG_H_

#include "Dialog.h"
#include "PickList.h"

/////////////////////////////////////////////////////////////////////////////
// SpeedFanMeterDialog

class CSpeedFanMeter;

class SpeedFanMeterDialog : public Dialog
{
public:
	SpeedFanMeterDialog(CSpeedFanMeter *pMeter, WORD idd=IDD_SPEEDFANMETER);
	virtual ~SpeedFanMeterDialog();

protected:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT setIndex(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT setReading(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT scale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT resetIndexList();

	CSpeedFanMeter *pMeter;

	PickList<long> indexList;
	PickList<long> readingList;
	PickList<double> scaleList;
public:
	BEGIN_MSG_MAP(SpeedFanMeterDialog)
		CHAIN_MSG_MAP(Dialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_COMBO_SCALE, scale)
		COMMAND_HANDLER(IDC_COMBO_INDEX, CBN_SELENDOK, setIndex)
		COMMAND_HANDLER(IDC_COMBO_READING, CBN_SELENDOK, setReading)
	END_MSG_MAP()
};
#endif // __SPEEDFANMETERDIALOG_H_
