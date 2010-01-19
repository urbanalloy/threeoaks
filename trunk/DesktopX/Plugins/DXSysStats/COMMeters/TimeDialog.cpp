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

#include <stdafx.h>
#include "resource.h"

#include "TimeMeter.h"
#include "TimeDialog.h"
#include "PickList.cpp"

template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

void ToValBSTR(_bstr_t *out, char *s)
{
	*out = _bstr_t(s);
}

void ToStrBSTR(char *buf, int maxlen, _bstr_t val)
{
	::strncpy(buf, val, maxlen);
}

TimeDialog::TimeDialog(CTimeMeter *pMeter, WORD idd) :
	pMeter(pMeter),
	Dialog(idd),
	tzList(Identity<_bstr_t>, ToStrBSTR, ToValBSTR, /* fixed */ true)
{
}

TimeDialog::~TimeDialog()
{
}

LRESULT TimeDialog::tzDesc(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[MAX_PATH];
	*text = 0;
	GetDlgItemText(wID, text, sizeof(text));
	pMeter->put_TZDescription(_bstr_t(text));

	return 1;
}

LRESULT TimeDialog::tz(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	_bstr_t text = L"";
	tzList.getValue(&text, text);
	pMeter->put_TimeZone(text);

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT TimeDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#ifdef notdef
	short flag = false;
	pMeter->get_ForceDST(&flag);
	SendDlgItemMessage(IDC_CHECK_FORCEDST, BM_SETCHECK, flag ? BST_CHECKED : BST_UNCHECKED, 0);
#endif

	xml = L"";
	BSTR xPath = L"";

	IMetaData *pMetaData = 0;
	pMeter->QueryInterface(_uuidof(IMetaData), (void**)&pMetaData);
	if (pMetaData)
	{
		BSTR xmlString = 0;
		if (SUCCEEDED(pMetaData->GetMetaData(&xmlString)))
		{
			xPath = L"//valueset[@id=//property[@name=\"TimeZone\"]/valuesetref/@idref]";
			xml = xmlString;
		}
		pMetaData->Release();
	}

	BSTR s = 0;
	pMeter->get_TimeZone(&s);
	tzList.init(m_hWnd, IDC_COMBO_TZ, xml, xPath, _bstr_t(s, false));

	pMeter->get_TZDescription(&s);
	SetDlgItemText(IDC_EDIT_TZDESC, _bstr_t(s, false));

	bHandled = FALSE;	// Make CWindow go through all the message maps.

	return 1;
}
