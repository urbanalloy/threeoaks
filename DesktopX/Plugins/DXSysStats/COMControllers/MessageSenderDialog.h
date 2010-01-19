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

// MessageSenderDialog.h : Declaration of the MessageSenderDialog

#ifndef __MESSAGESENDERDIALOG_H_
#define __MESSAGESENDERDIALOG_H_

#include "Resource.h"
#include "Dialog.h"
#include "PickList.h"

/////////////////////////////////////////////////////////////////////////////
// MessageSenderDialog

class MessageSenderDialog : public Dialog
{
public:
	MessageSenderDialog(IMessageSender *pController, IObserver *observer, IMeterHome *meters, WORD idd=IDD_MESSAGESENDER);
	virtual ~MessageSenderDialog();

protected:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT setMessage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT setLPCombo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT setWP(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT setLP(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT useWP(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT useLP(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	virtual void SetControlState();

	IMessageSender *pController;

	PickList<UINT> messageList;
	PickList<UINT> commandList;

public:
	BEGIN_MSG_MAP(MessageSenderDialog)
		CHAIN_MSG_MAP(Dialog)
		COMMAND_ID_HANDLER(IDC_COMBO_MESSAGE, setMessage)
		COMMAND_HANDLER(IDC_EDIT_WPARAM, EN_CHANGE, setWP)
		COMMAND_HANDLER(IDC_EDIT_LPARAM, EN_CHANGE, setLP)
		COMMAND_ID_HANDLER(IDC_COMBO_LPARAM, setLPCombo)
		COMMAND_HANDLER(IDC_CHECK_WPARAM, BN_CLICKED, useWP)
		COMMAND_HANDLER(IDC_CHECK_LPARAM, BN_CLICKED, useLP)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()
};
#endif // __MESSAGESENDERDIALOG_H_
