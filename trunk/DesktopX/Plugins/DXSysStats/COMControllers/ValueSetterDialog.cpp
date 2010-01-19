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

// ValueSetter.cpp : Implementation of ValueSetterDialog.cpp
#include "stdafx.h"
#include "PickList.cpp"
#include "COMControllers.h"
#include "ValueSetter.h"
#include "ValueSetterDialog.h"
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
	strncpy(buf, val, maxlen);
}

ValueSetterDialog::ValueSetterDialog(CValueSetter *pController, IObserver *observer, IMeterHome *meters, WORD idd) :
	pController(pController),
	Dialog(idd),
	meters(meters),
	meterList(Identity<long>, ToStrL, ToValL, /* fixed */ true),
	selectorList(Identity<_bstr_t>, ToStrBSTR, ToValBSTR, /* not fixed */ false),
	valueList(Identity<_bstr_t>, ToStrBSTR, ToValBSTR, /* not fixed */ false),
	xml(L"")
{
}

ValueSetterDialog::~ValueSetterDialog()
{
}

LRESULT ValueSetterDialog::setMeter(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val = -1;
	meterList.getData(&val, -1 /* default value */);
	pController->put_MeterIndex(val);
	pController->put_Selector(L"");
	pController->put_Value(L"");
	resetCombos(val);

	return 1;
}

LRESULT ValueSetterDialog::value(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	_bstr_t text = L"";
	valueList.getValue(&text, text);
	pController->put_Value(text);

	return 1;
}

LRESULT ValueSetterDialog::selector(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	_bstr_t text = L"";
	selectorList.getValue(&text, text);
	pController->put_Selector(text);
	pController->put_Value(L"");
	resetValueCombo(text);

	return 1;
}

void ValueSetterDialog::resetValueCombo(_bstr_t selectorValue)
{
	_bstr_t xPath = L"";
	_bstr_t selectorItemID = L"";
	LRESULT haveID = selectorList.getItemID(&selectorItemID, selectorValue);
	if (haveID)
	{
		xPath = L"//valueset[@idref=\"" + selectorItemID + "\"]";
	}

	BSTR s;

	pController->get_Value(&s);
	valueList.init(m_hWnd, IDC_COMBO_VALUE, xml, xPath, _bstr_t(s, false));
}

void ValueSetterDialog::resetCombos(long meterIndex)
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
				xPath = L"//valueset[@id=//method[@name=\"PutValue\"]/arg[@name=\"selector\"]/valuesetref/@idref]";
				xml = xmlString;
			}
			pMetaData->Release();
		}
	}

	BSTR s;

	pController->get_Selector(&s);
	_bstr_t _s(s);
	selectorList.init(m_hWnd, IDC_COMBO_SELECTOR, xml, xPath, _s);

	resetValueCombo(_s);
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 * Should initialize all of the controls using values from the meter.
 */
LRESULT ValueSetterDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	long current = -1;
	pController->get_MeterIndex(&current);

	meterList.init(m_hWnd, IDC_COMBO_METER, meters, current);

	resetCombos(current);

	bHandled = FALSE;	// Make CWindow go through all the message maps.

	return 1;
}
