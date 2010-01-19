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
#include "COMMeters.h"
#include "resource.h"
#include "comutil.h"
#include "FixedDialog.h"

FixedDialog::FixedDialog(IObserver *observer, _bstr_t *out, WORD idd) :
	CSubject(observer),
	CommonDialog<>(idd)
{
	this->out = out;
}

FixedDialog::~FixedDialog()
{
}

LRESULT FixedDialog::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    EndDialog(wID);
	return 1;
}

LRESULT FixedDialog::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char buf[MAX_PATH];
	GetDlgItemText(IDC_VALUE, buf, sizeof(buf));
	*out = _bstr_t(buf);
	notifyObserver();
    EndDialog(wID);

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT FixedDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetDlgItemText(IDC_VALUE,(char*)(*out));

	return 1;
}
