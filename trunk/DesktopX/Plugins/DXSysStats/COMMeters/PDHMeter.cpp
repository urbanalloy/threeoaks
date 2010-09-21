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

// PDHMeter.cpp : Implementation of CPDHMeter
#include "stdafx.h"
#include <stdio.h>
#include "SysStatsUtils.h"
#include "COMMeters.h"
#include "PDHMeter.h"
#include "PDHDialog.h"
#include "MeterImpl.cpp"

static BSTR EMPTY_BSTR = L"";

/////////////////////////////////////////////////////////////////////////////
// CPDHMeter

CPDHMeter::CPDHMeter() : scale(1.0L), pWeakTarget(0)
{
}

HRESULT CPDHMeter::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CPDHMeter::FinalRelease()
{
	pWeakTarget->Release();
}

PDH_STATUS CPDHMeter::SetCounters(char *szFullCounterPath[], int count)
{
	return pdh.SetCounters(szFullCounterPath, count);
}

STDMETHODIMP CPDHMeter::Update(long *dirty)
{
	double oldval = pdh.asDouble(0) / scale;
	pdh.Collect();
	double newval = pdh.asDouble(0) / scale;
	*dirty = (newval != oldval);

	return S_OK;
}

STDMETHODIMP CPDHMeter::Configure(IObserver * observer, LONG hWnd)
{
	PDHDialog *d = new PDHDialog(this);
	d->DoModal((HWND)hWnd);
	delete d;

	return S_OK;
}

STDMETHODIMP CPDHMeter::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_PDHMeter, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CPDHMeter::GetAsLong(BSTR selector, long *pRet)
{
	*pRet = pdh.asLong(0) / scale;

	return S_OK;
}

STDMETHODIMP CPDHMeter::GetAsDouble(BSTR selector, double *pRet)
{
	*pRet = pdh.asDouble(0) / scale;

	return S_OK;
}

STDMETHODIMP CPDHMeter::GetAsString(BSTR format, BSTR selector, BSTR *pRet)
{
	try
	{
		char buf[MAX_PATH];
		double val;
		GetAsDouble(EMPTY_BSTR, &val);
		_snprintf(buf, sizeof(buf), _bstr_t(format), val);
		buf[MAX_PATH-1] = 0;
		*pRet = _com_util::ConvertStringToBSTR(buf);
	}
	catch (...)
	{
	}


	return S_OK;
}

STDMETHODIMP CPDHMeter::GetValue(VARIANT *pRet)
{
	CComVariant(pdh.asDouble(0) / scale).Detach(pRet);

	return S_OK;
}

STDMETHODIMP CPDHMeter::get_Scale(double *pVal)
{
	*pVal = scale;
	return S_OK;
}

STDMETHODIMP CPDHMeter::put_Scale(double newVal)
{
	if (newVal != 0.0L)
		scale = newVal;

	return S_OK;
}

STDMETHODIMP CPDHMeter::get_Counters(LPSAFEARRAY *pVal)
{
	SAFEARRAY *pArray = ::SafeArrayCreateVector(VT_BSTR, 0, pdh.getNumCounters());

	char val[2048];
	long indices[1];
	for (int i=0; i<pdh.getNumCounters(); i++)
	{
		*val = 0;
		pdh.getCounterName(i, val);
		indices[0] = i;
		BSTR bStr = _com_util::ConvertStringToBSTR(val);
		::SafeArrayPutElement(pArray, indices, bStr);
	}

	*pVal = pArray;

	return S_OK;
}

STDMETHODIMP CPDHMeter::put_Counters(LPSAFEARRAY newVal)
{
	long lb = 0;
	long ub = 0;
	VARTYPE vt;
	::SafeArrayGetVartype(newVal, &vt);
	if (vt != VT_BSTR)
		return E_INVALIDARG;

	HRESULT hr = ::SafeArrayGetUBound(newVal, 1, &ub);
	hr = ::SafeArrayGetLBound(newVal, 1, &lb);

	char **paths = (char**)malloc((ub-lb+1) * sizeof(char*));
	long indices[1];
	for (int i=lb; i<=ub; i++)
	{
		indices[0] = i;
		BSTR bStr = 0;
		::SafeArrayGetElement(newVal, indices, &bStr);
		AtlTrace(L"Setting %d to %s", i, bStr);
		paths[i-lb] = strdup(_bstr_t(bStr));
		AtlTrace(" (%s)\n", paths[i-lb]);
	}

	pdh.SetCounters(paths, ub-lb+1);

	for (int i=0; i<=ub-lb; i++)
	{
		free(paths[i]);
	}

	if (paths)
		free(paths);

	return S_OK;
}
