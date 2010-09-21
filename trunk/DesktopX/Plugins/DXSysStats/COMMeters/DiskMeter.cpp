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

// DiskMeter.cpp : Implementation of CDiskMeter
#include "stdafx.h"
#include <stdio.h>
#include "COMMeters.h"
#include "MeterImpl.cpp"
#include "SysStatsUtils.h"
#include "DiskMeter.h"
#include "DiskDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CDiskMeter

STDMETHODIMP CDiskMeter::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IDiskMeter
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CDiskMeter::CDiskMeter() : scale(1), selector(0), value(-1), path(""), pWeakTarget(0)
{
}

HRESULT CDiskMeter::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CDiskMeter::FinalRelease()
{
	pWeakTarget->Release();
}

STDMETHODIMP CDiskMeter::get_Scale(double *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = scale;

	return S_OK;
}

STDMETHODIMP CDiskMeter::put_Scale(double newVal)
{
	if (newVal != 0.0L)
		scale = newVal;

	return S_OK;
}

STDMETHODIMP CDiskMeter::get_Selector(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = selector;

	return S_OK;
}

STDMETHODIMP CDiskMeter::put_Selector(long newVal)
{
	selector = newVal;

	return S_OK;
}

STDMETHODIMP CDiskMeter::Update(long *dirty)
{
	double oldval = value;

	LONGLONG freeBytesToCaller = 0;
	LONGLONG totalBytes = 0;
	LONGLONG freeBytes = 0;
	BOOL res = GetDiskFreeSpaceEx (path,
            (PULARGE_INTEGER)&freeBytesToCaller,
            (PULARGE_INTEGER)&totalBytes,
            (PULARGE_INTEGER)&freeBytes);

	if (res)
	{
		switch (selector)
		{
			// %used
		case 0:
			value = ((totalBytes - freeBytes) * 100 / totalBytes) / scale;
			break;

			// %free
		case 1:
			value = freeBytes * 100 / totalBytes / scale;
			break;

			// Used
		case 2:
			value = (totalBytes - freeBytes) / scale;
			break;

			// Free
		case 3:
			value = freeBytes / scale;
			break;

			// Total
		case 4:
			value = totalBytes / scale;
			break;
		}
	}

	*dirty = (value != oldval);

	return S_OK;
}

STDMETHODIMP CDiskMeter::Configure(IObserver * observer, LONG hWnd)
{
	DiskDialog *d = new DiskDialog(this);
	d->DoModal((HWND)hWnd);
	delete d;

	return S_OK;
}

STDMETHODIMP CDiskMeter::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_DiskMeter, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CDiskMeter::GetAsLong(BSTR sel, long *pRet)
{
	*pRet = (long)value;

	return S_OK;
}

STDMETHODIMP CDiskMeter::GetAsDouble(BSTR sel, double *pRet)
{
	*pRet = value;

	return S_OK;
}

STDMETHODIMP CDiskMeter::GetAsString(BSTR format, BSTR selector, BSTR *pRet)
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

STDMETHODIMP CDiskMeter::GetValue(VARIANT *pRet)
{
	double value;
	GetAsDouble(NULL, &value);
	CComVariant(value).Detach(pRet);

	return S_OK;
}

STDMETHODIMP CDiskMeter::get_Path(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = path.copy();
		
	return S_OK;
}

STDMETHODIMP CDiskMeter::put_Path(BSTR newVal)
{
	path = newVal;

	return S_OK;
}
