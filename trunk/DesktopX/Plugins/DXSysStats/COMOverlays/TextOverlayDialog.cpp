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

#include "TextOverlay.h"
#include "TextOverlayDialog.h"
#include "PickList.cpp"

extern void ToValL(long *out, char *s);
extern void ToStrL(char *buf, int maxlen, long val);

template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

TextOverlayDialog::TextOverlayDialog(CTextOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd) :
	OverlayDialog(overlay, observer, meters, idd),
	antiAliasList(Identity<long>, ToStrL, ToValL, /* fixed */ true)
{
}

TextOverlayDialog::~TextOverlayDialog()
{
}

LRESULT TextOverlayDialog::vAlign(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[64];
	*text = 0;
	GetDlgItemText(wID, text, sizeof(text));
	if (strcmp(text, "Top") == 0)
		((CTextOverlay*)overlay)->put_VAlign((long)StringAlignmentNear);
	else if (strcmp(text, "Middle") == 0)
		((CTextOverlay*)overlay)->put_VAlign((long)StringAlignmentCenter);
	else if (strcmp(text, "Bottom") == 0)
		((CTextOverlay*)overlay)->put_VAlign((long)StringAlignmentFar);

	notifyObserver();

	return 1;
}

LRESULT TextOverlayDialog::hAlign(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[64];
	*text = 0;
	GetDlgItemText(wID, text, sizeof(text));
	if (strcmp(text, "Left") == 0)
		((CTextOverlay*)overlay)->put_HAlign((long)StringAlignmentNear);
	else if (strcmp(text, "Center") == 0)
		((CTextOverlay*)overlay)->put_HAlign((long)StringAlignmentCenter);
	else if (strcmp(text, "Right") == 0)
		((CTextOverlay*)overlay)->put_HAlign((long)StringAlignmentFar);

	notifyObserver();

	return 1;
}

LRESULT TextOverlayDialog::format(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[64];
	*text = 0;
	GetDlgItemText(wID, text, sizeof(text));	
	((CTextOverlay*)overlay)->put_Format(_bstr_t(text));
	notifyObserver();

	return 1;
}

LRESULT TextOverlayDialog::chooseFont(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	((CTextOverlay*)overlay)->getChooseFont()->hwndOwner = m_hWnd;
	ChooseFont(((CTextOverlay*)overlay)->getChooseFont());
	notifyObserver();

	return 1;
}

LRESULT TextOverlayDialog::chooseColor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CHOOSECOLOR chooseColor;
	memset(&chooseColor, 0, sizeof(CHOOSECOLOR));
	chooseColor.hwndOwner = m_hWnd;
	long fColor;
	((CTextOverlay*)overlay)->get_FontColor(&fColor);
	chooseColor.rgbResult = (ARGB)fColor;
	chooseColor.lpCustColors = OverlayDialog::custColors;
	chooseColor.lStructSize = sizeof(CHOOSECOLOR);
	chooseColor.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;

	ChooseColor(&chooseColor);

	((CTextOverlay*)overlay)->put_FontColor((long)chooseColor.rgbResult);
	notifyObserver();

	return 1;
}

LRESULT TextOverlayDialog::chooseBlurColor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CHOOSECOLOR chooseColor;
	memset(&chooseColor, 0, sizeof(CHOOSECOLOR));
	chooseColor.hwndOwner = m_hWnd;
	long fColor;
	((CTextOverlay*)overlay)->get_BlurColor(&fColor);
	chooseColor.rgbResult = (ARGB)fColor;
	chooseColor.lpCustColors = OverlayDialog::custColors;
	chooseColor.lStructSize = sizeof(CHOOSECOLOR);
	chooseColor.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;

	ChooseColor(&chooseColor);

	((CTextOverlay*)overlay)->put_BlurColor((long)chooseColor.rgbResult);
	notifyObserver();

	return 1;
}

LRESULT TextOverlayDialog::blur(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CTextOverlay*)overlay)->put_Radius(i);
		notifyObserver();
	}

	return 1;
}

LRESULT TextOverlayDialog::width(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CTextOverlay*)overlay)->put_Width(i);
		notifyObserver();
	}

	return 1;
}

LRESULT TextOverlayDialog::height(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CTextOverlay*)overlay)->put_Height(i);
		notifyObserver();
	}

	return 1;
}

LRESULT TextOverlayDialog::alpha(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CTextOverlay*)overlay)->put_Alpha((BYTE)i);
		notifyObserver();
	}

	return 1;
}

LRESULT TextOverlayDialog::wrap(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT state = SendDlgItemMessage(wID, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	((CTextOverlay*)overlay)->put_Wrap(state == BST_CHECKED);
		notifyObserver();

	return 1;
}

LRESULT TextOverlayDialog::vertical(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT state = SendDlgItemMessage(wID, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	((CTextOverlay*)overlay)->put_Vertical(state == BST_CHECKED);
	notifyObserver();

	return 1;
}

LRESULT TextOverlayDialog::alternate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT state = SendDlgItemMessage(wID, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	((CTextOverlay*)overlay)->put_AlternateRender(state == BST_CHECKED);

	::EnableWindow(GetDlgItem(IDC_COMBO_ANTIALIAS), state == BST_CHECKED);
	::EnableWindow(GetDlgItem(IDC_CONTRAST), state == BST_CHECKED);

	notifyObserver();

	return 1;
}

LRESULT TextOverlayDialog::antialias(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val = 0;
	antiAliasList.getData(&val, 0 /* default value */);
	((CTextOverlay*)overlay)->put_TextRenderingHint((short)val);
	notifyObserver();

	return 1;
}

LRESULT TextOverlayDialog::contrast(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CTextOverlay*)overlay)->put_TextContrast(i);
		notifyObserver();
	}

	return 1;
}

static const char *antiAliasLabels[] =
{
	"Default",
    "No AntiAlias Grid Fit",
    "No AntiAlias",
    "AntiAlias Grid Fit",
    "AntiAlias",
    "ClearType",
	NULL
};

static long antiAliasValues[] =
{
	0, 1, 2, 3, 4, 5
};

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT TextOverlayDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SendDlgItemMessage(IDC_BLURSPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(10, 0));
	int i;
	((CTextOverlay*)overlay)->get_Radius(&i);
	SetDlgItemInt(IDC_BLUR, (UINT)i, TRUE);

	long l;
	((CTextOverlay*)overlay)->get_Width(&l);
	SetDlgItemInt(IDC_WIDTH, (UINT)l, TRUE);
	SendDlgItemMessage(IDC_WIDTHSPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(1000, -1));

	((CTextOverlay*)overlay)->get_Height(&l);
	SetDlgItemInt(IDC_HEIGHT, (UINT)l, TRUE);

	SendDlgItemMessage(IDC_HEIGHTSPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(1000, -1));
	BYTE b;
	((CTextOverlay*)overlay)->get_Alpha(&b);
	SetDlgItemInt(IDC_ALPHA, (UINT)b, TRUE);

	SendDlgItemMessage(IDC_HALIGN,CB_ADDSTRING,0,(LPARAM)"Left");
	SendDlgItemMessage(IDC_HALIGN,CB_ADDSTRING,0,(LPARAM)"Center");
	SendDlgItemMessage(IDC_HALIGN,CB_ADDSTRING,0,(LPARAM)"Right");

	((CTextOverlay*)overlay)->get_HAlign(&l);
	SendDlgItemMessage(IDC_HALIGN,CB_SETCURSEL,(WPARAM)l,0);

	SendDlgItemMessage(IDC_VALIGN,CB_ADDSTRING,0,(LPARAM)"Top");
	SendDlgItemMessage(IDC_VALIGN,CB_ADDSTRING,0,(LPARAM)"Middle");
	SendDlgItemMessage(IDC_VALIGN,CB_ADDSTRING,0,(LPARAM)"Bottom");
	((CTextOverlay*)overlay)->get_VAlign(&l);
	SendDlgItemMessage(IDC_VALIGN,CB_SETCURSEL,(WPARAM)l,0);

	BSTR f;
	((CTextOverlay*)overlay)->get_Format(&f);
	SetDlgItemText(IDC_FORMAT, _bstr_t(f, false));

	short flag = false;
	((CTextOverlay*)overlay)->get_Wrap(&flag);
	SendDlgItemMessage(IDC_RADIO_WRAP, BM_SETCHECK, flag ? BST_CHECKED : BST_UNCHECKED, 0);

	((CTextOverlay*)overlay)->get_Vertical(&flag);
	SendDlgItemMessage(IDC_RADIO_VERTICAL, BM_SETCHECK, flag ? BST_CHECKED : BST_UNCHECKED, 0);

	((CTextOverlay*)overlay)->get_AlternateRender(&flag);
	SendDlgItemMessage(IDC_RADIO_ALTERNATE, BM_SETCHECK, flag ? BST_CHECKED : BST_UNCHECKED, 0);

	::EnableWindow(GetDlgItem(IDC_COMBO_ANTIALIAS), flag);
	::EnableWindow(GetDlgItem(IDC_CONTRAST), flag);

	UINT ui;
	((CTextOverlay*)overlay)->get_TextContrast(&ui);
	SetDlgItemInt(IDC_CONTRAST, ui, TRUE);
	SendDlgItemMessage(IDC_CONTRASTSPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(10, 0));

	short aa;
	((CTextOverlay*)overlay)->get_TextRenderingHint(&aa);
	antiAliasList.init(m_hWnd, IDC_COMBO_ANTIALIAS, antiAliasLabels, antiAliasValues, (long)aa);

	bHandled = FALSE;

	return TRUE;
}
