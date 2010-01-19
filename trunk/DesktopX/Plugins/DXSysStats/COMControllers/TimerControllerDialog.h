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

// TimerControllerDialog.h : Declaration of the TimerControllerDialog

#ifndef __TIMERCONTROLLERDIALOG_H_
#define __TIMERCONTROLLERDIALOG_H_

#include "MessageSenderDialog.h"

/////////////////////////////////////////////////////////////////////////////
// TimerControllerDialog

class TimerControllerDialog : public MessageSenderDialog
{
public:
	TimerControllerDialog(ITimerController *pController, IObserver *observer, IMeterHome *meters, WORD idd=IDD_TIMERCONTROLLER);
	virtual ~TimerControllerDialog();

protected:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT setInterval(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(TimerControllerDialog)
		CHAIN_MSG_MAP(MessageSenderDialog)
		COMMAND_HANDLER(IDC_EDIT_INTERVAL, EN_CHANGE, setInterval)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()
};
#endif // __TIMERCONTROLLERDIALOG_H_
