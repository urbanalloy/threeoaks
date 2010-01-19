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

// DropTargetController.cpp : Implementation of DropTargetControllerDialog.cpp
#include "stdafx.h"
#include "COMControllers.h"
#include "DropTargetController.h"
#include "DropTargetControllerDialog.h"

#include "PickList.cpp"

extern void ToValL(long *out, char *s);
extern void ToStrL(char *buf, int maxlen, long val);

template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

DropTargetControllerDialog::DropTargetControllerDialog(CDropTargetController *pController, IObserver *observer, IMeterHome *meters, WORD idd) :
	MessageSenderDialog(pController, observer, meters, idd),
	meters(meters),
	meterList(Identity<long>, ToStrL, ToValL, /* fixed */ true)
{
}

DropTargetControllerDialog::~DropTargetControllerDialog()
{
}

LRESULT DropTargetControllerDialog::setMeter(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val = -1;
	meterList.getData(&val, -1 /* default value */);
	((IDropTargetController*)pController)->put_MeterIndex(val);

	return 1;
}

LRESULT DropTargetControllerDialog::setEffect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DWORD effect = DROPEFFECT_NONE;

	LRESULT state = SendDlgItemMessage(IDC_CHECK_COPY, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if (state) effect |= DROPEFFECT_COPY;

	 state = SendDlgItemMessage(IDC_CHECK_MOVE, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if (state) effect |= DROPEFFECT_MOVE;

	 state = SendDlgItemMessage(IDC_CHECK_LINK, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if (state) effect |= DROPEFFECT_LINK;

	((IDropTargetController*)pController)->put_Effect(effect);

	SetControlState();

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 * Should initialize all of the controls using values from the meter.
 */
LRESULT DropTargetControllerDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BSTR s;

	short count;
	meters->Count(&count);
	_bstr_t *orderedMeterNames = new _bstr_t [count+2];
	long *typeData = new long[count+1];

	orderedMeterNames[0] = "(none)";
	typeData[0] = -1;
	long current = -1;
	((IDropTargetController*)pController)->get_MeterIndex(&current);

	for (int i=0; i<count; i++)
	{
		IMeter *meter;
		meters->Item(CComVariant(i), &meter);
		meter->get_Name(&s);
		orderedMeterNames[i+1] = _bstr_t(s, false);
		typeData[i+1] = i;
		meter->Release();
	}

	meterList.init(m_hWnd, IDC_COMBO_METER, orderedMeterNames, typeData, current);
	delete[] orderedMeterNames;
	delete[] typeData;

	DWORD effect = 0;
	((IDropTargetController*)pController)->get_Effect(&effect);
	SendDlgItemMessage(IDC_CHECK_COPY, BM_SETCHECK, (effect & DROPEFFECT_COPY) ? BST_CHECKED : BST_UNCHECKED, 0);
	SendDlgItemMessage(IDC_CHECK_MOVE, BM_SETCHECK, (effect & DROPEFFECT_MOVE) ? BST_CHECKED : BST_UNCHECKED, 0);
	SendDlgItemMessage(IDC_CHECK_LINK, BM_SETCHECK, (effect & DROPEFFECT_LINK) ? BST_CHECKED : BST_UNCHECKED, 0);

	bHandled = FALSE;	// Make CWindow go through all the message maps.

	return 1;
}
