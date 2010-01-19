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

// Cursor.cpp : Implementation of CursorControllerDialog.cpp
#include "stdafx.h"
#include "COMControllers.h"
#include "CursorControllerDialog.h"
#include "PickList.cpp"

extern void ToValL(long *out, char *s);
extern void ToStrL(char *buf, int maxlen, long val);

template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

CursorControllerDialog::CursorControllerDialog(ICursorController *pController, IObserver *observer, IMeterHome *meters, WORD idd) :
	pController(pController),
	Dialog(idd),
	cursorList(Identity<long>, ToStrL, ToValL, /* fixed */ true)
{
}

CursorControllerDialog::~CursorControllerDialog()
{
}

LRESULT CursorControllerDialog::setCursor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val = -1;
	cursorList.getData(&val, -1 /* default value */);
	pController->put_CursorID(val);

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 * Should initialize all of the controls using values from the meter.
 */
LRESULT CursorControllerDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	long cursorID;
	pController->get_CursorID(&cursorID);

	IClientModel *pModel;
	pController->get_Model(&pModel);
	IAppConfig *pAppConfig;
	pModel->get_AppConfig(&pAppConfig);

	_bstr_t cursorIDs("system\\Cursors.txt");
	BSTR configFile;
	pAppConfig->CanonicalPath(cursorIDs, &configFile);

	cursorList.init(m_hWnd, IDC_COMBO_CURSOR, _bstr_t(configFile), cursorID);

	pAppConfig->Release();
	pModel->Release();

	bHandled = FALSE;	// Make CWindow go through all the message maps.

	return 1;
}
