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

// MemoryMeter.cpp : Implementation of CMemoryMeter
#include "stdafx.h"
#include <stdio.h>
#include "COMMeters.h"
#include "MeterImpl.cpp"
#include "SysStatsUtils.h"
#include "MemoryMeter.h"
#include "MemoryDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMemoryMeter

STDMETHODIMP CMemoryMeter::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IMemoryMeter
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CMemoryMeter::CMemoryMeter() : scale(1), selector(0), value(-1), pWeakTarget(0)
{
}

HRESULT CMemoryMeter::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CMemoryMeter::FinalRelease()
{
	pWeakTarget->Release();
}

STDMETHODIMP CMemoryMeter::get_Scale(double *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = scale;

	return S_OK;
}

STDMETHODIMP CMemoryMeter::put_Scale(double newVal)
{
	if (newVal != 0.0L)
		scale = newVal;

	return S_OK;
}

STDMETHODIMP CMemoryMeter::get_Selector(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = selector;

	return S_OK;
}

STDMETHODIMP CMemoryMeter::put_Selector(long newVal)
{
	selector = newVal;

	return S_OK;
}

STDMETHODIMP CMemoryMeter::Update(long *dirty)
{
	double oldval = value;

	MEMORYSTATUS stat;
	stat.dwLength = sizeof (stat);
	GlobalMemoryStatus (&stat);

	switch (selector)
	{
		// %used
	case 0:
		value = (double)stat.dwMemoryLoad / scale;
		break;

		// %free
	case 1:
		value = (100.0L - (double)stat.dwMemoryLoad) / scale;
		break;

		// Used
	case 2:
		value = (double)(stat.dwTotalPhys - stat.dwAvailPhys) / scale;
		break;

		// Free
	case 3:
		value = (double)stat.dwAvailPhys / scale;
		break;

		// Total
	case 4:
		value = (double)stat.dwTotalPhys / scale;
		break;
	}

	*dirty = (value != oldval);

	return S_OK;
}

STDMETHODIMP CMemoryMeter::Configure(IObserver * observer, LONG hWnd)
{
	MemoryDialog *d = new MemoryDialog(this);
	d->DoModal((HWND)hWnd);
	delete d;

	return S_OK;
}

STDMETHODIMP CMemoryMeter::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_MemoryMeter, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CMemoryMeter::GetAsLong(BSTR sel, long *pRet)
{
	*pRet = (long)value;

	return S_OK;
}

STDMETHODIMP CMemoryMeter::GetAsDouble(BSTR sel, double *pRet)
{
	*pRet = value;

	return S_OK;
}

STDMETHODIMP CMemoryMeter::GetAsString(BSTR format, BSTR selector, BSTR *pRet)
{
	try
	{
		char buf[MAX_PATH];
		_snprintf(buf, sizeof(buf), _bstr_t(format), value);
		buf[MAX_PATH-1] = 0;
		*pRet = _com_util::ConvertStringToBSTR(buf);
	}
	catch (...)
	{
	}


	return S_OK;
}

STDMETHODIMP CMemoryMeter::GetValue(VARIANT *pRet)
{
	CComVariant(value / scale).Detach(pRet);

	return S_OK;
}

