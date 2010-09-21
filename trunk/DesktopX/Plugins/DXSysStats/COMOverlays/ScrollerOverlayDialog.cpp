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

// ScrollerOverlay.cpp : Implementation of ScrollerOverlayDialog.cpp
#include "stdafx.h"
#include "commctrl.h"
#include "COMOverlays.h"
#include "ScrollerOverlay.h"
#include "ScrollerOverlayDialog.h"
#include "PickList.cpp"

extern void ToValL(long *out, char *s);
extern void ToStrL(char *buf, int maxlen, long val);
template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

ScrollerOverlayDialog::ScrollerOverlayDialog(CScrollerOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd) :
	OverlayDialog(overlay, observer, meters, idd),
	meter2List(Identity<long>, ToStrL, ToValL, /* fixed */ true)
{
}

ScrollerOverlayDialog::~ScrollerOverlayDialog()
{
}

LRESULT ScrollerOverlayDialog::alpha(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CScrollerOverlay*)overlay)->put_Alpha((BYTE)i);
		notifyObserver();
	}

	return 1;
}

LRESULT ScrollerOverlayDialog::dx(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CScrollerOverlay*)overlay)->put_dx(i);
		notifyObserver();
	}

	return 1;
}

LRESULT ScrollerOverlayDialog::dy(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CScrollerOverlay*)overlay)->put_dy(i);
		notifyObserver();
	}

	return 1;
}

LRESULT ScrollerOverlayDialog::setMeter2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val = -1;
	meter2List.getData(&val, -1 /* default value */);
	((CScrollerOverlay*)overlay)->put_SmoothScrollMeterIndex(val);
	notifyObserver();

	return 1;
}

LRESULT ScrollerOverlayDialog::loopLast(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT state = SendDlgItemMessage(wID, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	((CScrollerOverlay*)overlay)->put_DuplicateLastFrame(state == BST_CHECKED);
	notifyObserver();

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 * Should initialize all of the controls using values from the meter.
 */
LRESULT ScrollerOverlayDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BYTE b;

	SendDlgItemMessage(IDC_ALPHASPIN_START,UDM_SETRANGE,0,(LPARAM)MAKELONG(255, 0));
	((CScrollerOverlay*)overlay)->get_Alpha(&b);
	SetDlgItemInt(IDC_ALPHA, (UINT)b, TRUE);

	long l;
	SendDlgItemMessage(IDC_DXSPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(1000, -1000));
	((CScrollerOverlay*)overlay)->get_dx(&l);
	SetDlgItemInt(IDC_DX, (UINT)l, TRUE);

	SendDlgItemMessage(IDC_DYSPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(1000, -1000));
	((CScrollerOverlay*)overlay)->get_dy(&l);
	SetDlgItemInt(IDC_DY, (UINT)l, TRUE);

	long current = -1;
	((CScrollerOverlay*)overlay)->get_SmoothScrollMeterIndex(&current);

	meter2List.init(m_hWnd, IDC_COMBO_METER2, meters, current);

	VARIANT_BOOL flag = false;
	((CScrollerOverlay*)overlay)->get_DuplicateLastFrame(&flag);
	SendDlgItemMessage(IDC_LOOPLAST, BM_SETCHECK, flag ? BST_CHECKED : BST_UNCHECKED, 0);

	bHandled = FALSE; // Make CWindow go through all the message maps.

	return 1;
}
