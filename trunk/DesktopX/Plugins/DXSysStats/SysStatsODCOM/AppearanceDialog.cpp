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
#include "PickList.cpp"
#include "AppearanceDialog.h"

#import <msxml4.dll> implementation_only

void ToValL(long *out, char *s)
{
	sscanf(s, "%d", out);
}

void ToStrL(char *buf, int maxlen, long val)
{
	_snprintf(buf, maxlen, "%d", val);
}

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

AppearanceDialog::AppearanceDialog(ILabel *app, IObserver *observer, IMeterHome *meters, WORD idd) :
	CSubject(observer),
	Dialog(idd),
	app(app),
	meters(meters),
	meterList(Identity<long>, ToStrL, ToValL, /* fixed */ true),
	selectorList(Identity<_bstr_t>, ToStrBSTR, ToValBSTR, /* not fixed */ false)
{
}

AppearanceDialog::~AppearanceDialog()
{
}

LRESULT AppearanceDialog::format(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char buf[64];
	*buf = 0;
	GetDlgItemText(wID, buf, sizeof(buf));
	app->put_format(_bstr_t(buf));
	notifyObserver();

	return S_OK;
}

LRESULT AppearanceDialog::setMeter(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val = -1;
	meterList.getData(&val, -1 /* default value */);
	app->put_MeterIndex(val);
	app->put_selector(L"");
	resetSelectorCombo(val);
	notifyObserver();

	return 1;
}

LRESULT AppearanceDialog::selector(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	_bstr_t text = L"";
	selectorList.getValue(&text, text);
	app->put_selector(text);
	notifyObserver();

	return 1;
}

void AppearanceDialog::resetSelectorCombo(long meterIndex)
{
	xml = L"";
	BSTR xPath = L"";

	if (meterIndex >= 0)
	{
		IMeter *pMeter;
		meters->Item(CComVariant(meterIndex), &pMeter);
		IMetaData *pMetaData = 0;
		pMeter->QueryInterface(_uuidof(IMetaData), (void**)&pMetaData);
		if (pMetaData)
		{
			BSTR xmlString;
			if (SUCCEEDED(pMetaData->GetMetaData(&xmlString)))
			{
				xPath = L"//valueset[@id=//method[@name=\"GetValue\"]/arg[@name=\"selector\"]/valuesetref/@idref]";
				xml = xmlString;
			}
			pMetaData->Release();
		}
	}

	BSTR s;

	app->get_selector(&s);
	_bstr_t _s(s);
	selectorList.init(m_hWnd, IDC_COMBO_SELECTOR, xml, xPath, _s);
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT AppearanceDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
	BSTR s;
	app->get_format(&s);
	SetDlgItemText(IDC_FORMAT, _bstr_t(s, false));

	long current = -1;
	app->get_MeterIndex(&current);

	meterList.init(m_hWnd, IDC_COMBO_METER, meters, current);

	resetSelectorCombo(current);

	bHandled = false;

	return S_OK;
}
