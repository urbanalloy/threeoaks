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

// FixedMeter.cpp : Implementation of CFixedMeter
#include "stdafx.h"
#include <stdio.h>
#include "COMMeters.h"
#include "MeterImpl.cpp"
#include "SysStatsUtils.h"
#include "FixedMeter.h"
#include "FixedDialog.h"

static _bstr_t percentS("%s");
static _bstr_t emptyStr("");

/////////////////////////////////////////////////////////////////////////////
// CFixedMeter

STDMETHODIMP CFixedMeter::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IFixedMeter
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CFixedMeter::CFixedMeter() : value(""), pWeakTarget(0)
{
	interval = 0;
}

HRESULT CFixedMeter::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CFixedMeter::FinalRelease()
{
	pWeakTarget->Release();
}

STDMETHODIMP CFixedMeter::get_Value(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = value.copy();

	return S_OK;
}

STDMETHODIMP CFixedMeter::put_Value(BSTR newVal)
{
	value = newVal;
	model->put_dirty(true);

	return S_OK;
}

STDMETHODIMP CFixedMeter::Update(long *dirty)
{
	*dirty = false;

	return S_OK;
}

STDMETHODIMP CFixedMeter::Configure(IObserver * observer, LONG hWnd)
{
	FixedDialog *d = new FixedDialog(observer, &value);
	d->DoModal((HWND)hWnd);
	model->put_dirty(true);
	delete d;

	return S_OK;
}

STDMETHODIMP CFixedMeter::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_FixedMeter, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CFixedMeter::GetAsLong(BSTR sel, long *pRet)
{
	BSTR pVal = 0;
	GetAsString(percentS, sel, &pVal);
	sscanf(_bstr_t(pVal, false), "%ld", pRet);

	return S_OK;
}

STDMETHODIMP CFixedMeter::GetAsDouble(BSTR sel, double *pRet)
{
	BSTR pVal = 0;
	GetAsString(percentS, sel, &pVal);
	sscanf(_bstr_t(pVal, false), "%lg", pRet);

	return S_OK;
}

STDMETHODIMP CFixedMeter::GetAsString(BSTR format, BSTR selector, BSTR *pRet)
{
	try
	{
		wchar_t buf[MAX_PATH];
		_snwprintf(buf, MAX_PATH, format, (wchar_t*)value);
		buf[MAX_PATH-1] = 0;
		*pRet = ::SysAllocString(buf);
	}
	catch (...)
	{
	}


	return S_OK;
}

STDMETHODIMP CFixedMeter::GetValue(VARIANT *pRet)
{
	BSTR s = L"";
	GetAsString(percentS, emptyStr, &s);
	CComVariant(s).Detach(pRet);
	::SysFreeString(s);

	return S_OK;
}

