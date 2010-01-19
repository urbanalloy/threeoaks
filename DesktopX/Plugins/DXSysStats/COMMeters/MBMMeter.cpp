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

// MBMMeter.cpp : Implementation of CMBMMeter
#include "stdafx.h"
#include "COMMeters.h"
#include "MeterImpl.cpp"
#include "SysStatsUtils.h"
#include "MBMMeter.h"
#include "MBMDialog.h"

#include "mbm.hpp"

// Ensure that one instance of the mbm class is created in
// a timely fashion.
static mbm::mbm& getMBM()
{
	static mbm::mbm mbm;

	return mbm;
}

/////////////////////////////////////////////////////////////////////////////
// CMBMMeter

STDMETHODIMP CMBMMeter::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IMBMMeter
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CMBMMeter::CMBMMeter() : index(0), reading(0), scale(1.0L), pWeakTarget(0)
{
	getMBM().open();
}

HRESULT CMBMMeter::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CMBMMeter::FinalRelease()
{
	pWeakTarget->Release();
}

mbm::index *CMBMMeter::getTypes()
{
	getMBM().open();
	if (getMBM().opened())
		return getMBM().index();
	else
		return NULL;
}

mbm::sensor *CMBMMeter::getSensors()
{
	getMBM().open();
	if (getMBM().opened())
		return getMBM().sensor();
	else
		return NULL;
}

STDMETHODIMP CMBMMeter::get_Reading(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = reading;

	return S_OK;
}

STDMETHODIMP CMBMMeter::put_Reading(long newVal)
{
	reading = newVal;

	return S_OK;
}

STDMETHODIMP CMBMMeter::get_Index(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = index;

	return S_OK;
}

STDMETHODIMP CMBMMeter::put_Index(long newVal)
{
	index = newVal;

	return S_OK;
}

STDMETHODIMP CMBMMeter::Update(long *dirty)
{
	// MBM updates all on its ownybones.
	*dirty = true;

	return S_OK;
}

STDMETHODIMP CMBMMeter::Configure(IObserver * observer, LONG hWnd)
{
	MBMDialog *d = new MBMDialog(this);
	d->DoModal((HWND)hWnd);
	delete d;

	return S_OK;
}

STDMETHODIMP CMBMMeter::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_MBMMeter, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CMBMMeter::GetAsLong(BSTR sel, long *pRet)
{
	double val;
	GetAsDouble(sel, &val);
	*pRet = (long)val;

	return S_OK;
}

STDMETHODIMP CMBMMeter::GetAsDouble(BSTR sel, double *pRet)
{
	double val = 0;

	getMBM().open();
	if (getMBM().opened() && index >= 0 && index < 100)
	{
		// Is this executing a copy-constructor?
		mbm::sensor sense = getMBM().sensor()[index];
		switch (reading)
		{
		default:
		case 0:
			val = (double)sense.current();
			break;

		case 1:
			val = (double)sense.low();
			break;

		case 2:
			val = (double)sense.high();
			break;
		}

		val /= (double)scale;
	}

	*pRet = val;

	return S_OK;
}

STDMETHODIMP CMBMMeter::GetAsString(BSTR format, BSTR selector, BSTR *pRet)
{
	try
	{
		double value;
		GetAsDouble(selector, &value);
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

STDMETHODIMP CMBMMeter::GetValue(VARIANT *pRet)
{
	double value;
	GetAsDouble(NULL, &value);
	CComVariant(value).Detach(pRet);

	return S_OK;
}


STDMETHODIMP CMBMMeter::get_Scale(double *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = scale;

	return S_OK;
}

STDMETHODIMP CMBMMeter::put_Scale(double newVal)
{
	if (newVal != 0.0L)
		scale = newVal;

	return S_OK;
}
