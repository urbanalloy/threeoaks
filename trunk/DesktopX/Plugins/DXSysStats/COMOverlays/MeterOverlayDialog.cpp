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


#include "stdafx.h"
#include "COMOverlays.h"

#include <stdio.h>
#include <shlwapi.h>
#include <shlobj.h>
#include "resource.h"

#include "MeterOverlay.h"
#include "MeterOverlayDialog.h"

MeterOverlayDialog::MeterOverlayDialog(IMeterOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd) :
	OverlayDialog(overlay, observer, meters, idd)
{
}

MeterOverlayDialog::~MeterOverlayDialog()
{
}

LRESULT MeterOverlayDialog::chooseColor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CHOOSECOLOR chooseColor;
	memset(&chooseColor, 0, sizeof(CHOOSECOLOR));
	chooseColor.hwndOwner = m_hWnd;
	long fColor;
	((CMeterOverlay*)overlay)->get_Color(&fColor);
	chooseColor.rgbResult = (ARGB)fColor;
	chooseColor.lpCustColors = OverlayDialog::custColors;
	chooseColor.lStructSize = sizeof(CHOOSECOLOR);
	chooseColor.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;

	ChooseColor(&chooseColor);

	((CMeterOverlay*)overlay)->put_Color((long)chooseColor.rgbResult);
	notifyObserver();

	return 1;
}

LRESULT MeterOverlayDialog::setMin(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CMeterOverlay*)overlay)->put_Min((long)i);
		notifyObserver();
	}

	return 1;
}

LRESULT MeterOverlayDialog::setMax(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CMeterOverlay*)overlay)->put_Max((long)i);
		notifyObserver();
	}

	return 1;
}

LRESULT MeterOverlayDialog::setLength(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CMeterOverlay*)overlay)->put_Length((long)i);
		notifyObserver();
	}

	return 1;
}

LRESULT MeterOverlayDialog::setThickness(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CMeterOverlay*)overlay)->put_Thickness((long)i);
		notifyObserver();
	}

	return 1;
}

LRESULT MeterOverlayDialog::alpha(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CMeterOverlay*)overlay)->put_Alpha(i);
		notifyObserver();
	}

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT MeterOverlayDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SendDlgItemMessage(IDC_ALPHASPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(255, 0));
	BYTE b;
	((CMeterOverlay*)overlay)->get_Alpha(&b);
	SetDlgItemInt(IDC_ALPHA, (UINT)b, TRUE);

	long l;
	((CMeterOverlay*)overlay)->get_Min(&l);
	SetDlgItemInt(IDC_MIN, (UINT)l, TRUE);
	((CMeterOverlay*)overlay)->get_Max(&l);
	SetDlgItemInt(IDC_MAX, (UINT)l, TRUE);
	((CMeterOverlay*)overlay)->get_Length(&l);
	SetDlgItemInt(IDC_WIDTH, (UINT)l, TRUE);
	((CMeterOverlay*)overlay)->get_Thickness(&l);
	SetDlgItemInt(IDC_HEIGHT, (UINT)l, TRUE);

	bHandled = FALSE;

	return TRUE;
}
