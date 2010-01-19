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

// AnimatorOverlay.cpp : Implementation of AnimatorOverlayDialog.cpp
#include "stdafx.h"
#include "commctrl.h"
#include "COMOverlays.h"
#include "AnimatorOverlay.h"
#include "AnimatorOverlayDialog.h"
#include "PickList.cpp"

AnimatorOverlayDialog::AnimatorOverlayDialog(CAnimatorOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd) :
	OverlayDialog(overlay, observer, meters, idd)
{
}

AnimatorOverlayDialog::~AnimatorOverlayDialog()
{
}

LRESULT AnimatorOverlayDialog::alpha(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CAnimatorOverlay*)overlay)->put_Alpha(i);
		notifyObserver();
	}

	return 1;
}

LRESULT AnimatorOverlayDialog::alphaEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CAnimatorOverlay*)overlay)->put_AlphaEnd(i);
		notifyObserver();
	}

	return 1;
}

LRESULT AnimatorOverlayDialog::xEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CAnimatorOverlay*)overlay)->put_XEnd(i);
		notifyObserver();
	}

	return 1;
}

LRESULT AnimatorOverlayDialog::yEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CAnimatorOverlay*)overlay)->put_YEnd(i);
		notifyObserver();
	}

	return 1;
}

LRESULT AnimatorOverlayDialog::xRotEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CAnimatorOverlay*)overlay)->put_CenterXEnd(i);
		notifyObserver();
	}

	return 1;
}

LRESULT AnimatorOverlayDialog::yRotEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CAnimatorOverlay*)overlay)->put_CenterYEnd(i);
		notifyObserver();
	}

	return 1;
}

LRESULT AnimatorOverlayDialog::rotationEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CAnimatorOverlay*)overlay)->put_RotationEnd(i);
		notifyObserver();
	}

	return 1;
}

LRESULT AnimatorOverlayDialog::hScaleEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	double scale = 1.0L;
	char val[64];
	GetDlgItemText(wID, val, sizeof(val));
	sscanf(val, "%lg", &scale);
	((CAnimatorOverlay*)overlay)->put_HScaleEnd(scale);
	notifyObserver();

	return 1;
}

LRESULT AnimatorOverlayDialog::vScaleEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	double scale = 1.0L;
	char val[64];
	GetDlgItemText(wID, val, sizeof(val));
	sscanf(val, "%lg", &scale);
	((CAnimatorOverlay*)overlay)->put_VScaleEnd(scale);
	notifyObserver();

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT AnimatorOverlayDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BYTE b;

	SendDlgItemMessage(IDC_ALPHASPIN_START,UDM_SETRANGE,0,(LPARAM)MAKELONG(255, 0));
	((CAnimatorOverlay*)overlay)->get_Alpha(&b);
	SetDlgItemInt(IDC_ALPHA_START, (UINT)b, TRUE);

	SendDlgItemMessage(IDC_ALPHASPIN_END,UDM_SETRANGE,0,(LPARAM)MAKELONG(255, 0));
	((CAnimatorOverlay*)overlay)->get_AlphaEnd(&b);
	SetDlgItemInt(IDC_ALPHA_END, (UINT)b, TRUE);

	long l;

	SendDlgItemMessage(IDC_XSPIN_END,UDM_SETRANGE,0,(LPARAM)MAKELONG(300, -100));
	((CAnimatorOverlay*)overlay)->get_XEnd(&l);
	SetDlgItemInt(IDC_XPOS_END, (UINT)l, TRUE);

	SendDlgItemMessage(IDC_YSPIN_END,UDM_SETRANGE,0,(LPARAM)MAKELONG(-100, 300));
	((CAnimatorOverlay*)overlay)->get_YEnd(&l);
	SetDlgItemInt(IDC_YPOS_END, (UINT)l, TRUE);

	SendDlgItemMessage(IDC_CXSPIN_END,UDM_SETRANGE,0,(LPARAM)MAKELONG(300, -100));
	((CAnimatorOverlay*)overlay)->get_CenterXEnd(&l);
	SetDlgItemInt(IDC_CXPOS_END, (UINT)l, TRUE);

	SendDlgItemMessage(IDC_CYSPIN_END,UDM_SETRANGE,0,(LPARAM)MAKELONG(-100, 300));
	((CAnimatorOverlay*)overlay)->get_CenterYEnd(&l);
	SetDlgItemInt(IDC_CYPOS_END, (UINT)l, TRUE);

	SendDlgItemMessage(IDC_ROTSPIN_END,UDM_SETRANGE,0,(LPARAM)MAKELONG(359, 0));
	((CAnimatorOverlay*)overlay)->get_RotationEnd(&l);
	SetDlgItemInt(IDC_ROT_END, (UINT)l, TRUE);

	double scale;
	char val[64];

	((CAnimatorOverlay*)overlay)->get_HScaleEnd(&scale);
	sprintf(val, "%.20g", scale);
	SetDlgItemText(IDC_XSCALE_END, val);

	((CAnimatorOverlay*)overlay)->get_VScaleEnd(&scale);
	sprintf(val, "%.20g", scale);
	SetDlgItemText(IDC_YSCALE_END, val);

	bHandled = FALSE;

	return TRUE;
}
