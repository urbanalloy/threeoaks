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
#include "DockletLetter.h"

#include "SizeDialog.h"

SizeDialog::SizeDialog(IClientModel *model, IObserver *observer, WORD idd) :
	CSubject(observer), Dialog(idd), model(model)
{
}

SizeDialog::~SizeDialog()
{
}

LRESULT SizeDialog::setWidth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL ok;
	int i = GetDlgItemInt(IDC_WIDTH, &ok, true);
	if (ok)
	{
		model->put_Width(i);
		notifyObserver();
	}

	return 1;
}

LRESULT SizeDialog::setHeight(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL ok;
	int i = GetDlgItemInt(IDC_HEIGHT, &ok, true);
	if (ok)
	{
		model->put_Height(i);
		notifyObserver();
	}

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT SizeDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	long width, height;
	model->get_Width(&width);
	model->get_Height(&height);
	SetDlgItemInt(IDC_WIDTH, width, true);
	SetDlgItemInt(IDC_HEIGHT, height, true);

	bHandled = false;

	return 1;
}
