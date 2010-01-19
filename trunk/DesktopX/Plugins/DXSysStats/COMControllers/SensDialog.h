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

// SensDialog.h : Declaration of the SensDialog

#ifndef __SENSDIALOG_H_
#define __SENSDIALOG_H_

#include "Resource.h"
#include "Dialog.h"
#include "PickList.h"

/////////////////////////////////////////////////////////////////////////////
// SensDialog

class SensDialog : public Dialog
{
public:
	SensDialog(ISens *pController, IObserver *observer, IMeterHome *meters, WORD idd=IDD_SENS);
	virtual ~SensDialog();

protected:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT setAwayMessage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT setPresentMessage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	PickList<UINT> awayMessageList;
	PickList<UINT> presentMessageList;

	ISens *pController;

public:
	BEGIN_MSG_MAP(SensDialog)
		CHAIN_MSG_MAP(Dialog)
		COMMAND_ID_HANDLER(IDC_COMBO_AWAY_MESSAGE, setAwayMessage)
		COMMAND_ID_HANDLER(IDC_COMBO_PRESENT_MESSAGE, setPresentMessage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()
};
#endif // __SENSDIALOG_H_
