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

// DropTargetControllerDialog.h : Declaration of the DropTargetControllerDialog

#ifndef __DROPTARGETCONTROLLERDIALOG_H_
#define __DROPTARGETCONTROLLERDIALOG_H_

#include "MessageSenderDialog.h"

/////////////////////////////////////////////////////////////////////////////
// DropTargetControllerDialog

class CDropTargetController;

class DropTargetControllerDialog : public MessageSenderDialog
{
public:
	DropTargetControllerDialog(CDropTargetController *pController, IObserver *observer, IMeterHome *meters, WORD idd=IDD_DROPTARGETCONTROLLER);
	virtual ~DropTargetControllerDialog();

protected:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT setMeter(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT setEffect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	PickList<long> meterList;
	IMeterHome *meters;

public:
	BEGIN_MSG_MAP(DropTargetControllerDialog)
		CHAIN_MSG_MAP(MessageSenderDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_COMBO_METER, CBN_SELENDOK, setMeter)
		COMMAND_HANDLER(IDC_CHECK_COPY, BN_CLICKED, setEffect)
		COMMAND_HANDLER(IDC_CHECK_MOVE, BN_CLICKED, setEffect)
		COMMAND_HANDLER(IDC_CHECK_LINK, BN_CLICKED, setEffect)
	END_MSG_MAP()
};
#endif // __DROPTARGETCONTROLLERDIALOG_H_
