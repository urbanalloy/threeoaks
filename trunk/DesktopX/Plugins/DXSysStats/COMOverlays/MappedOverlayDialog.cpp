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

// MappedOverlay.cpp : Implementation of MappedOverlayDialog.cpp
#include "stdafx.h"
#include "COMOverlays.h"

#include <stdio.h>
#include <io.h>
#include <shlwapi.h>
#include <shlobj.h>
#include "resource.h"

#include "MappedOverlay.h"
#include "MappedOverlayDialog.h"

MappedOverlayDialog::MappedOverlayDialog(CMappedOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd) :
	OverlayDialog(overlay, observer, meters, idd)
{
}

MappedOverlayDialog::~MappedOverlayDialog()
{
}

LRESULT MappedOverlayDialog::imagePath(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[MAX_PATH];
	*text = 0;
	GetDlgItemText(wID, text, sizeof(text));
	CMappedOverlay *pOverlay = (CMappedOverlay*)overlay;
	((CMappedOverlay*)overlay)->put_ImagePath(_bstr_t(text));
	notifyObserver();

	return 1;
}

LRESULT MappedOverlayDialog::chooseImage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	OPENFILENAME of;
    memset(&of,0,sizeof(OPENFILENAME));   
    of.lStructSize=sizeof(OPENFILENAME);
	char _path[MAX_PATH];
	*_path = 0;
	of.lpstrFile = _path;
	of.nMaxFile = sizeof(_path);
	of.lpstrTitle = "Choose image...";
	of.lpstrFilter = "Bitmap\0*.bmp\0GIF\0*.gif\0JPEG\0*.jpg\0PNG\0*.png\0Icon\0*.ico\0All\0*.*\0\0\0";
	of.lpstrDefExt = "png";
	of.nFilterIndex = 4;
	of.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	of.hwndOwner = m_hWnd;

	if (GetOpenFileName(&of))
	{
		SetDlgItemText(IDC_FORMAT, of.lpstrFile);
		((CMappedOverlay*)overlay)->put_ImagePath(_bstr_t(of.lpstrFile));
		notifyObserver();
	}

	return 1;
}

LRESULT MappedOverlayDialog::mapPath(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[MAX_PATH];
	*text = 0;
	GetDlgItemText(wID, text, sizeof(text));
	CMappedOverlay *pOverlay = (CMappedOverlay*)overlay;
	((CMappedOverlay*)overlay)->put_MapPath(_bstr_t(text));
	notifyObserver();

	return 1;
}

LRESULT MappedOverlayDialog::chooseMap(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	OPENFILENAME of;
    memset(&of,0,sizeof(OPENFILENAME));   
    of.lStructSize=sizeof(OPENFILENAME);
	char _path[MAX_PATH];
	*_path = 0;
	of.lpstrFile = _path;
	of.nMaxFile = sizeof(_path);
	of.lpstrTitle = "Choose map...";
	of.lpstrFilter = "Bitmap\0*.bmp\0GIF\0*.gif\0JPEG\0*.jpg\0PNG\0*.png\0Icon\0*.ico\0All\0*.*\0\0\0";
	of.lpstrDefExt = "png";
	of.nFilterIndex = 4;
	of.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	of.hwndOwner = m_hWnd;

	if (GetOpenFileName(&of))
	{
		SetDlgItemText(IDC_FORMAT_MAP, of.lpstrFile);
		((CMappedOverlay*)overlay)->put_MapPath(_bstr_t(of.lpstrFile));
		notifyObserver();
	}

	return 1;
}

LRESULT MappedOverlayDialog::alpha(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CMappedOverlay*)overlay)->put_Alpha(i);
		notifyObserver();
	}

	return 1;
}

LRESULT MappedOverlayDialog::setMin(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CMappedOverlay*)overlay)->put_Min((long)i);
		notifyObserver();
	}

	return 1;
}

LRESULT MappedOverlayDialog::setMax(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CMappedOverlay*)overlay)->put_Max((long)i);
		notifyObserver();
	}

	return 1;
}

LRESULT MappedOverlayDialog::OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT res = SendMessage((HWND)lParam, (UINT)TBM_GETPOS, 0, 0);
	if (GetDlgItem(IDC_SLIDER_HUE) == (HWND)lParam)
	{
		SetDlgItemInt(IDC_STATIC_HUE, (UINT)res, TRUE);
		((CMappedOverlay*)overlay)->put_HueShift(res);
	}
	else if (GetDlgItem(IDC_SLIDER_SATURATION) == (HWND)lParam)
	{
		char buf[40];
		_snprintf(buf, sizeof(buf), "%.10g", res / 100.0);
		SetDlgItemText(IDC_STATIC_SATURATION, buf);
		((CMappedOverlay*)overlay)->put_Saturation(res / 100.0);
	}

	notifyObserver();

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT MappedOverlayDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SendDlgItemMessage(IDC_ALPHASPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(255, 0));

	BYTE b;

	((CMappedOverlay*)overlay)->get_Alpha(&b);
	SetDlgItemInt(IDC_ALPHA, (UINT)b, TRUE);

	BSTR f;

	((CMappedOverlay*)overlay)->get_ImagePath(&f);
	SetDlgItemText(IDC_FORMAT, _bstr_t(f, false));

	((CMappedOverlay*)overlay)->get_MapPath(&f);
	SetDlgItemText(IDC_FORMAT_MAP, _bstr_t(f, false));

	long l;

	((CMappedOverlay*)overlay)->get_Min(&l);
	SetDlgItemInt(IDC_MIN, (UINT)l, TRUE);

	((CMappedOverlay*)overlay)->get_Max(&l);
	SetDlgItemInt(IDC_MAX, (UINT)l, TRUE);

	((CMappedOverlay*)overlay)->get_HueShift(&l);
	SendDlgItemMessage(IDC_SLIDER_HUE, TBM_SETRANGE, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) MAKELONG(-180, +180));  // min. & max. positions
	
	SendDlgItemMessage(IDC_SLIDER_HUE, TBM_SETPOS, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) l);
	
	SetDlgItemInt(IDC_STATIC_HUE, (UINT)l, TRUE);
	
	double d;
	((CMappedOverlay*)overlay)->get_Saturation(&d);
	SendDlgItemMessage(IDC_SLIDER_SATURATION, TBM_SETRANGE, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) MAKELONG(0, 300));  // min. & max. positions
	
	SendDlgItemMessage(IDC_SLIDER_SATURATION, TBM_SETPOS, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) (d * 100));
	
	char buf[40];
	_snprintf(buf, sizeof(buf), "%.10g", d);
	SetDlgItemText(IDC_STATIC_SATURATION, buf);
	
	bHandled = FALSE;

	return TRUE;
}
