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

// MailMeter.cpp : Implementation of MailMeterDialog.cpp
#include "stdafx.h"
#include "COMMeters.h"
#include "MailMeter.h"
#include "MailMeterDialog.h"

MailMeterDialog::MailMeterDialog(CMailMeter *pMeter, WORD idd) :
	pMeter(pMeter),
	Dialog(idd)
{
}

MailMeterDialog::~MailMeterDialog()
{
}

LRESULT MailMeterDialog::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    EndDialog(wID);
	return 1;
}

LRESULT MailMeterDialog::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char buf[MAX_PATH];

	GetDlgItemText(IDC_PROFILENAME, buf, sizeof(buf));
	pMeter->put_Username(_bstr_t(buf));

	GetDlgItemText(IDC_PASSWORD, buf, sizeof(buf));
	pMeter->put_Password(_bstr_t(buf));

	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(IDC_DAYS, &iRet, TRUE);

	if (iRet)
	{
		pMeter->put_ValidDays(i);
	}

    EndDialog(wID);
	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 * Should initialize all of the controls using values from the meter.
 */
LRESULT MailMeterDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BSTR owned = 0;
	pMeter->get_Username(&owned);
	SetDlgItemText(IDC_PROFILENAME, _bstr_t(owned, false));

	pMeter->get_Password(&owned);
	SetDlgItemText(IDC_PASSWORD, _bstr_t(owned, false));

	long l;
	pMeter->get_ValidDays(&l);
	SetDlgItemInt(IDC_DAYS, (UINT)l, TRUE);

	bHandled = FALSE;	// Make CWindow go through all the message maps.

	return 1;
}
