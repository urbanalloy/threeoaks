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

#include <stdio.h>
#include <shlwapi.h>
#include <shlobj.h>

#include "AppResources.h"

#include "SysStatsClientWindow.h"
#include "WindowDialog.h"

WindowDialog::WindowDialog(SysStatsClientWindow *window, IObserver *observer, WORD idd) :
	CSubject(observer), Dialog(idd)
{
	this->window = window;
}

WindowDialog::~WindowDialog()
{
}

LRESULT WindowDialog::x(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = FALSE;	// For GetDlgItemInt
	int i = 0;			// Ditto

	i = GetDlgItemInt(wID, &iRet, TRUE);
	if (iRet)
	{
		RECT rect;
		window->GetWindowRect(&rect);
		rect.right += i - rect.left;
		rect.left=i;
		window->MoveWindow(&rect);

		notifyObserver();
	}

	return 1;
}

LRESULT WindowDialog::y(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = 0;			// Ditto

	i = GetDlgItemInt(wID, &iRet, TRUE);
	if (iRet)
	{
		RECT rect;
		window->GetWindowRect(&rect);
		rect.bottom += i - rect.top;
		rect.top=i;
		window->MoveWindow(&rect);

		notifyObserver();
	}

	return 1;
}

LRESULT WindowDialog::width(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = 0;			// Ditto

	i = GetDlgItemInt(wID, &iRet, TRUE);
	if (iRet)
	{
		RECT rect;
		window->GetWindowRect(&rect);
		rect.right=rect.left+i;
		window->MoveWindow(&rect);

		notifyObserver();
	}

	return 1;
}

LRESULT WindowDialog::height(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = 0;			// Ditto

	i = GetDlgItemInt(wID, &iRet, TRUE);
	if (iRet)
	{
		RECT rect;
		window->GetWindowRect(&rect);
		rect.bottom=rect.top+i;
		window->MoveWindow(&rect);

		notifyObserver();
	}

	return 1;
}

LRESULT WindowDialog::name(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[64];

	*text = 0;
	GetDlgItemText(wID, text, sizeof(text));
	window->setName(text);
	notifyObserver();

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT WindowDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rect;
	window->GetWindowRect(&rect);

	SetDlgItemInt(IDC_XPOS, (UINT)rect.left, TRUE);
	SetDlgItemInt(IDC_YPOS, (UINT)rect.top, TRUE);
	SetDlgItemInt(IDC_WIDTH, (UINT)rect.right-rect.left, TRUE);
	SetDlgItemInt(IDC_HEIGHT, (UINT)rect.bottom-rect.top, TRUE);
	SetDlgItemText(IDC_NAME, window->getName());

	bHandled = FALSE;

	return TRUE;
}
