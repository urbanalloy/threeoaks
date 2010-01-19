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
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>

#include "resource.h"
#include "CRenameDialog.h"

CRenameDialog::CRenameDialog(char *out, WORD idd) :
	CommonDialog<>(idd)
{
	this->out = out;
}

CRenameDialog::~CRenameDialog()
{
}

LRESULT CRenameDialog::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    EndDialog(wID);
	return 1;
}

LRESULT CRenameDialog::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	::GetDlgItemText(m_hWnd, IDC_VALUE, out, 64);
    EndDialog(wID);

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT CRenameDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::SetDlgItemText(m_hWnd,IDC_VALUE,out);

	return 1;
}
