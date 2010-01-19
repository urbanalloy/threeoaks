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

// MessageSender.cpp : Implementation of MessageSenderDialog.cpp
#include "stdafx.h"
#include "COMControllers.h"
#include "MessageSenderDialog.h"

#include "PickList.cpp"

void ToValUI(UINT *out, char *s)
{
	sscanf(s, "%d", out);
}

void ToStrUI(char *buf, int maxlen, UINT val)
{
	_snprintf(buf, maxlen, "%d", val);
}

template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

MessageSenderDialog::MessageSenderDialog(IMessageSender *pController, IObserver *observer, IMeterHome *meters, WORD idd) :
	pController(pController),
	Dialog(idd),
	messageList(Identity<UINT>, ToStrUI, ToValUI, /* fixed */ false),
	commandList(Identity<UINT>, ToStrUI, ToValUI, /* fixed */ true)
{
}

MessageSenderDialog::~MessageSenderDialog()
{
}

LRESULT MessageSenderDialog::setWP(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = false;	// For GetDlgItemInt
	UINT ui = (UINT)GetDlgItemInt(wID, &iRet, true);
	if (iRet)
		pController->put_WParam(ui);

	return 1;
}

LRESULT MessageSenderDialog::setLP(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = false;	// For GetDlgItemInt
	UINT ui = (UINT)GetDlgItemInt(wID, &iRet, true);
	if (iRet)
		pController->put_LParam(ui);

	return 1;
}

LRESULT MessageSenderDialog::useWP(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT state = SendDlgItemMessage(wID, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	pController->put_UseWParam(state == BST_CHECKED);
	SetControlState();

	return 1;
}

LRESULT MessageSenderDialog::useLP(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT state = SendDlgItemMessage(wID, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	pController->put_UseLParam(state == BST_CHECKED);
	SetControlState();

	return 1;
}

LRESULT MessageSenderDialog::setMessage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	UINT value;
	messageList.getData(&value, 0);
	pController->put_SendMessage(value);
	SetControlState();

	return 1;
}

LRESULT MessageSenderDialog::setLPCombo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	UINT val = 0;
	commandList.getData(&val, 0 /* default value */);
	// This can only be for WM_APPCOMMAND so shift the value
	pController->put_LParam(val << 16);

	return 1;
}

void MessageSenderDialog::SetControlState()
{
	VARIANT_BOOL flag = false;

	pController->get_UseWParam(&flag);
	::EnableWindow(GetDlgItem(IDC_EDIT_WPARAM), flag);
	pController->get_UseLParam(&flag);
	::EnableWindow(GetDlgItem(IDC_EDIT_LPARAM), flag);
	::EnableWindow(GetDlgItem(IDC_COMBO_LPARAM), flag);

	UINT msg = 0;
	pController->get_SendMessage(&msg);
	if (msg == WM_APPCOMMAND)
	{
		::ShowWindow(GetDlgItem(IDC_EDIT_LPARAM), false);
		::ShowWindow(GetDlgItem(IDC_COMBO_LPARAM), true);
	}
	else
	{
		::ShowWindow(GetDlgItem(IDC_EDIT_LPARAM), true);
		::ShowWindow(GetDlgItem(IDC_COMBO_LPARAM), false);
	}
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 * Should initialize all of the controls using values from the meter.
 */
LRESULT MessageSenderDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;	// Make CWindow go through all the message maps.

	VARIANT_BOOL flag = false;

	pController->get_UseWParam(&flag);
	SendDlgItemMessage(IDC_CHECK_WPARAM, BM_SETCHECK, flag ? BST_CHECKED : BST_UNCHECKED, 0);

	pController->get_UseLParam(&flag);
	SendDlgItemMessage(IDC_CHECK_LPARAM, BM_SETCHECK, flag ? BST_CHECKED : BST_UNCHECKED, 0);

	IClientModel *pModel;
	pController->get_Model(&pModel);
	IAppConfig *pAppConfig;
	pModel->get_AppConfig(&pAppConfig);

	_bstr_t messageName("system\\Windows Messages.txt");
	BSTR configFile;
	pAppConfig->CanonicalPath(messageName, &configFile);

	UINT message = 0;
	pController->get_SendMessage(&message);

	messageList.init(m_hWnd, IDC_COMBO_MESSAGE, _bstr_t(configFile), message);

	messageName = "system\\App Commands.txt";
	pAppConfig->CanonicalPath(messageName, &configFile);
	pController->get_LParam(&message);
	commandList.init(m_hWnd, IDC_COMBO_LPARAM, _bstr_t(configFile), message >> 16);

	pAppConfig->Release();
	pModel->Release();

	UINT ui;
	pController->get_WParam(&ui);
	SetDlgItemInt(IDC_EDIT_WPARAM, ui);

	pController->get_LParam(&ui);
	SetDlgItemInt(IDC_EDIT_LPARAM, ui);

	SetControlState();

	return 1;
}
