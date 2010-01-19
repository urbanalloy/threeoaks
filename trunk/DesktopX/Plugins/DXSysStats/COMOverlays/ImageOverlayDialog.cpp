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
#include <io.h>
#include <shlwapi.h>
#include <shlobj.h>
#include "resource.h"

#include "ImageOverlay.h"
#include "ImageOverlayDialog.h"
#include "PickList.cpp"

extern void ToValL(long *out, char *s);
extern void ToStrL(char *buf, int maxlen, long val);
template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

ImageOverlayDialog::ImageOverlayDialog(CImageOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd) :
	OverlayDialog(overlay, observer, meters, idd),
	animationMeterList(Identity<long>, ToStrL, ToValL, /* fixed */ true),
	alignmentList(Identity<long>, ToStrL, ToValL, /* fixed */ true)
{
}

ImageOverlayDialog::~ImageOverlayDialog()
{
}

LRESULT ImageOverlayDialog::keepOpen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT state = SendDlgItemMessage(IDC_CHECK_KEEPOPEN, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	((CImageOverlay*)overlay)->put_KeepImageOpen(state == BST_CHECKED);
	notifyObserver();

	return 1;
}

LRESULT ImageOverlayDialog::path(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[MAX_PATH];
	*text = 0;
	GetDlgItemText(wID, text, sizeof(text));
	((CImageOverlay*)overlay)->put_Path(_bstr_t(text));
	notifyObserver();

	return 1;
}

LRESULT ImageOverlayDialog::chooseImage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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
		((CImageOverlay*)overlay)->put_Path(_bstr_t(of.lpstrFile));
		notifyObserver();
	}

	return 1;
}

LRESULT ImageOverlayDialog::alpha(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CImageOverlay*)overlay)->put_Alpha(i);
		notifyObserver();
	}

	return 1;
}

LRESULT ImageOverlayDialog::limitWidth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CImageOverlay*)overlay)->put_LimitWidth(i);
		notifyObserver();
	}

	return 1;
}

LRESULT ImageOverlayDialog::limitHeight(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CImageOverlay*)overlay)->put_LimitHeight(i);
		notifyObserver();
	}

	return 1;
}

LRESULT ImageOverlayDialog::alignment(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val = 0;
	alignmentList.getData(&val, 0 /* default value */);
	((CImageOverlay*)overlay)->put_Alignment((short)val);
	notifyObserver();

	return 1;
}

LRESULT ImageOverlayDialog::setAnimationMeter(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val = -1;
	animationMeterList.getData(&val, -1 /* default value */);
	((CImageOverlay*)overlay)->put_AnimationMeterIndex(val);
	notifyObserver();

	return 1;
}

LRESULT ImageOverlayDialog::OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT res = SendMessage((HWND)lParam, (UINT)TBM_GETPOS, 0, 0);
	if (GetDlgItem(IDC_SLIDER_HUE) == (HWND)lParam)
	{
		SetDlgItemInt(IDC_STATIC_HUE, (UINT)res, TRUE);
		((CImageOverlay*)overlay)->put_HueShift(res);
	}
	else if (GetDlgItem(IDC_SLIDER_SATURATION) == (HWND)lParam)
	{
		char buf[40];
		_snprintf(buf, sizeof(buf), "%.10g", res / 100.0);
		SetDlgItemText(IDC_STATIC_SATURATION, buf);
		((CImageOverlay*)overlay)->put_Saturation(res / 100.0);
	}

	notifyObserver();

	return 1;
}

static const char *alignmentLabels[] =
{
	"TopLeft",
    "TopCenter",
    "TopRight",
    "Left",
    "Center",
    "Right",
	"BotLeft",
    "BotCenter",
    "BotRight",
	NULL
};

static long alignmentValues[] =
{
	0, 1, 2, 4, 5, 6, 8, 9, 10
};

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT ImageOverlayDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	VARIANT_BOOL keepOpen = true;
	((CImageOverlay*)overlay)->get_KeepImageOpen(&keepOpen);
	SendDlgItemMessage(IDC_CHECK_KEEPOPEN, BM_SETCHECK, keepOpen ? BST_CHECKED : BST_UNCHECKED, 0);

	SendDlgItemMessage(IDC_ALPHASPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(255, 0));
	BYTE b;
	((CImageOverlay*)overlay)->get_Alpha(&b);
	SetDlgItemInt(IDC_ALPHA, (UINT)b, TRUE);

	BSTR f;
	((CImageOverlay*)overlay)->get_Path(&f);
	SetDlgItemText(IDC_FORMAT, _bstr_t(f, false));

	long l = -1;
	((CImageOverlay*)overlay)->get_LimitWidth(&l);
	SetDlgItemInt(IDC_LIMITWIDTH, (UINT)l, TRUE);

	l = -1;
	((CImageOverlay*)overlay)->get_LimitHeight(&l);
	SetDlgItemInt(IDC_LIMITHEIGHT, (UINT)l, TRUE);

	short sh = 0;
	((CImageOverlay*)overlay)->get_Alignment(&sh);
	alignmentList.init(m_hWnd, IDC_COMBO_ALIGNMENT, alignmentLabels, alignmentValues, (long)sh);

	l = -1;
	((CImageOverlay*)overlay)->get_AnimationMeterIndex(&l);

	animationMeterList.init(m_hWnd, IDC_COMBO_ANIMATIONMETER, meters, l);

	l = 0;
	((CImageOverlay*)overlay)->get_HueShift(&l);
	SendDlgItemMessage(IDC_SLIDER_HUE, TBM_SETRANGE, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) MAKELONG(-180, +180));  // min. & max. positions
	
	SendDlgItemMessage(IDC_SLIDER_HUE, TBM_SETPOS, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) l);
	
	SetDlgItemInt(IDC_STATIC_HUE, (UINT)l, TRUE);
	
	double d;
	((CImageOverlay*)overlay)->get_Saturation(&d);
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
