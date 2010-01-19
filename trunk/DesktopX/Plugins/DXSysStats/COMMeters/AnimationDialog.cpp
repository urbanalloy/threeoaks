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

// Animation.cpp : Implementation of AnimationDialog.cpp
#include "stdafx.h"
#include <stdio.h>
#include "COMMeters.h"
#include <commctrl.h>
#include "Animation.h"
#include "AnimationDialog.h"

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

AnimationDialog::AnimationDialog(CAnimation *pMeter, WORD idd) :
	pMeter(pMeter),
	Dialog(idd),
	messageList(Identity<UINT>, ToStrUI, ToValUI, /* fixed */ false)
{
}

AnimationDialog::~AnimationDialog()
{
}

LRESULT AnimationDialog::setMessage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	UINT value;
	messageList.getData(&value, 0);
	pMeter->put_StateMsg(value);

	return 1;
}

LRESULT AnimationDialog::framerate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = FALSE;
	double interval = 1.0;
	char val[64];
	GetDlgItemText(wID, val, sizeof(val));
	sscanf(val, "%lg", &interval);
	pMeter->put_FrameRate(interval);

	return 1;
}

LRESULT AnimationDialog::framecount(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		pMeter->put_FrameCount(i);
	}

	return 1;
}

LRESULT AnimationDialog::lparam(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		pMeter->put_StateLParam((UINT)i);
	}

	return 1;
}

LRESULT AnimationDialog::loop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT state = SendDlgItemMessage(wID, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	pMeter->put_Loop(state == BST_CHECKED);

	return 1;
}

LRESULT AnimationDialog::autoreverse(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT state = SendDlgItemMessage(wID, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	pMeter->put_AutoReverse(state == BST_CHECKED);

	return 1;
}

LRESULT AnimationDialog::autostart(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT state = SendDlgItemMessage(wID, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	pMeter->put_AutoStart(state == BST_CHECKED);

	return 1;
}

LRESULT AnimationDialog::singlestep(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT state = SendDlgItemMessage(wID, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	pMeter->put_SingleStep(state == BST_CHECKED);

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 * Should initialize all of the controls using values from the meter.
 */
LRESULT AnimationDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;	// Make CWindow go through all the message maps.

	double d;
	SendDlgItemMessage(IDC_FRAMERATE_SPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(100, 0));
	pMeter->get_FrameRate(&d);
	char buf[40];
	_snprintf(buf, sizeof(buf), "%.10g", d);
	SetDlgItemText(IDC_FRAMERATE, buf);

	long l;
	SendDlgItemMessage(IDC_FRAMECOUNT_SPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(20000, 0));
	pMeter->get_FrameCount(&l);
	SetDlgItemInt(IDC_FRAMECOUNT, (UINT)l, TRUE);

	short flag = false;
	pMeter->get_Loop(&flag);
	SendDlgItemMessage(IDC_LOOP, BM_SETCHECK, flag ? BST_CHECKED : BST_UNCHECKED, 0);

	pMeter->get_AutoReverse(&flag);
	SendDlgItemMessage(IDC_AUTOREVERSE, BM_SETCHECK, flag ? BST_CHECKED : BST_UNCHECKED, 0);

	pMeter->get_SingleStep(&flag);
	SendDlgItemMessage(IDC_SINGLESTEP, BM_SETCHECK, flag ? BST_CHECKED : BST_UNCHECKED, 0);

	pMeter->get_AutoStart(&flag);
	SendDlgItemMessage(IDC_AUTOSTART, BM_SETCHECK, flag ? BST_CHECKED : BST_UNCHECKED, 0);

	UINT ui;
	pMeter->get_StateLParam(&ui);
	SetDlgItemInt(IDC_STATELPARAM, ui, TRUE);

	IClientModel *pModel;
	pMeter->get_Model(&pModel);
	IAppConfig *pAppConfig;
	pModel->get_AppConfig(&pAppConfig);

	_bstr_t messageName("system\\Windows Messages.txt");
	BSTR configFile = 0;
	pAppConfig->CanonicalPath(messageName, &configFile);

	UINT message = 0;
	pMeter->get_StateMsg(&message);

	messageList.init(m_hWnd, IDC_COMBO_MESSAGE, _bstr_t(configFile), message);

	pModel->Release();
	pAppConfig->Release();

	return 1;
}
