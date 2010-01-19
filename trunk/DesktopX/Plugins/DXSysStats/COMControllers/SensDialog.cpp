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

// Sens.cpp : Implementation of SensDialog.cpp
#include "stdafx.h"
#include "COMControllers.h"
#include "SensDialog.h"

#include "PickList.cpp"

static void ToValUI(UINT *out, char *s)
{
	sscanf(s, "%d", out);
}

static void ToStrUI(char *buf, int maxlen, UINT val)
{
	_snprintf(buf, maxlen, "%d", val);
}

template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

SensDialog::SensDialog(ISens *pController, IObserver *observer, IMeterHome *meters, WORD idd) :
	pController(pController),
	awayMessageList(Identity<UINT>, ToStrUI, ToValUI, /* fixed */ false),
	presentMessageList(Identity<UINT>, ToStrUI, ToValUI, /* fixed */ false),
	Dialog(idd)
{
}

SensDialog::~SensDialog()
{
}

LRESULT SensDialog::setAwayMessage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	UINT value;
	awayMessageList.getData(&value, 0);
	pController->put_Away(value);

	return 1;
}

LRESULT SensDialog::setPresentMessage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	UINT value;
	presentMessageList.getData(&value, 0);
	pController->put_Present(value);

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 * Should initialize all of the controls using values from the meter.
 */
LRESULT SensDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;	// Make CWindow go through all the message maps.

	IClientModel *pModel;
	pController->get_Model(&pModel);
	IAppConfig *pAppConfig;
	pModel->get_AppConfig(&pAppConfig);

	_bstr_t messageName("system\\Windows Messages.txt");
	BSTR configFile;
	pAppConfig->CanonicalPath(messageName, &configFile);

	UINT message = 0;
	pController->get_Away(&message);
	awayMessageList.init(m_hWnd, IDC_COMBO_AWAY_MESSAGE, _bstr_t(configFile), message);

	pController->get_Present(&message);
	presentMessageList.init(m_hWnd, IDC_COMBO_PRESENT_MESSAGE, _bstr_t(configFile), message);

	pAppConfig->Release();
	pModel->Release();

	return 1;
}
