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

// SpeedFanMeter.cpp : Implementation of CSpeedFanMeter
#include "stdafx.h"
#include "MeterImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#include "SysStatsUtils.h"
#include "COMMeters.h"
#include "SpeedFanMeter.h"
#include "SpeedFanMeterDialog.h"

#include "speedfan.hpp"

// Ensure that one instance of the sf class is created in
// a timely fashion.
static sf::sf& getSpeedFan()
{
	static sf::sf sf;

	return sf;
}

/////////////////////////////////////////////////////////////////////////////
// CSpeedFanMeter

STDMETHODIMP CSpeedFanMeter::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_ISpeedFanMeter
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/*
 * Constructor
 */
CSpeedFanMeter::CSpeedFanMeter() : index(0), reading(0), scale(1.0L), pWeakTarget(0)
{
	getSpeedFan().open();
}

HRESULT CSpeedFanMeter::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CSpeedFanMeter::FinalRelease()
{
	pWeakTarget->Release();
}

STDMETHODIMP CSpeedFanMeter::get_Reading(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = reading;

	return S_OK;
}

STDMETHODIMP CSpeedFanMeter::put_Reading(long newVal)
{
	reading = newVal;

	return S_OK;
}

STDMETHODIMP CSpeedFanMeter::get_Index(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = index;

	return S_OK;
}

STDMETHODIMP CSpeedFanMeter::put_Index(long newVal)
{
	index = newVal;

	return S_OK;
}

/*
 * This method is called every 'interval' seconds by SysStats.
 * Some meters will use it to actually update their value.
 * Other meters update asynchronously. All meters should set
 * the 'dirty' output parameter to true if the value of the meter
 * has changed since the last time that Update() was called.
 */
STDMETHODIMP CSpeedFanMeter::Update(long *dirty)
{
	if (dirty == NULL)
		return E_POINTER;

	// Provide implementation.
	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the meter should
 * be displayed so that the user can configure the meter to meet their
 * requirements.
 */
STDMETHODIMP CSpeedFanMeter::Configure(IObserver * observer, LONG hWnd)
{
	SpeedFanMeterDialog *d = new SpeedFanMeterDialog(this);
	d->DoModal((HWND)hWnd);
	delete d;

	return S_OK;
}

/*
 * Return a string representation of the meter’s type.
 */
STDMETHODIMP CSpeedFanMeter::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_SpeedFanMeter, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

/*
 * Return the meter's value as a long - if that makes sense. Some
 * overlays require a long value, some don't.
 *
 * Parameter 'sel' can be used to select one value from several if the
 * meter actually stores multiple values. It is up to the meter to
 * interpret the contents of 'sel' as it sees fit.
 */
STDMETHODIMP CSpeedFanMeter::GetAsLong(BSTR sel, long *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	double val;
	GetAsDouble(sel, &val);
	*pRet = (long)val;

	return S_OK;
}

/*
 * Return the meter's value as a double - if that makes sense. Some
 * overlays require a double value, some don't.
 *
 * Parameter 'sel' can be used to select one value from several if the
 * meter actually stores multiple values. It is up to the meter to
 * interpret the contents of 'sel' as it sees fit.
 */
STDMETHODIMP CSpeedFanMeter::GetAsDouble(BSTR sel, double *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	double val = -1.0;

	getSpeedFan().open();
	if (getSpeedFan().opened())
	{
		switch (reading)
		{
		case 0:
			val = (double)getSpeedFan().temp(index) / 100.0;
			break;
		case 1:
			val = (double)getSpeedFan().fan(index);
			break;
		case 2:
			val = (double)getSpeedFan().volt(index) / 100.0;
			break;
		}

		val /= (double)scale;
	}

	*pRet = val;

	return S_OK;
}

/*
 * Return the meter's value as a string. If that makes sense. Some
 * overlays require a string value, some don't.
 *
 * Parameter 'format' should always be used to produce the resulting string.
 * It is a 'printf' style format string.
 *
 * Parameter 'sel' can be used to select one value from several if the
 * meter actually stores multiple values. It is up to the meter to
 * interpret the contents of 'sel' as it sees fit.
 */
STDMETHODIMP CSpeedFanMeter::GetAsString(BSTR format, BSTR selector, BSTR *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	try
	{
		double value;
		GetAsDouble(selector, &value);
		wchar_t buf[MAX_PATH];
		_snwprintf(buf, MAX_PATH, format, value);
		buf[MAX_PATH-1] = 0;
		*pRet = ::SysAllocString(buf);
	}
	catch (...)
	{
	}

	return S_OK;
}

/*
 * Return the meter's value in the most native format possible.
 * This is how scripts get a hold of the value.
 */
STDMETHODIMP CSpeedFanMeter::GetValue(VARIANT *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	double value;
	GetAsDouble(NULL, &value);
	CComVariant(value).Detach(pRet);

	return S_OK;
}

STDMETHODIMP CSpeedFanMeter::get_Scale(double *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = scale;

	return S_OK;
}

STDMETHODIMP CSpeedFanMeter::put_Scale(double newVal)
{
	if (newVal != 0.0L)
		scale = newVal;

	return S_OK;
}

sf::sf& get_SpeedFan()
{
	return getSpeedFan();
}

STDMETHODIMP CSpeedFanMeter::GetCount(long reading, long *count)
{
	if (count == NULL)
		return E_POINTER;

	long val = 0;

	getSpeedFan().open();
	if (getSpeedFan().opened())
	{
		switch (reading)
		{
		case 0:
			val = getSpeedFan().numTemps();
			break;
		case 1:
			val = getSpeedFan().numFans();
			break;
		case 2:
			val = getSpeedFan().numVolts();
			break;
		}
	}

	*count = val;

	return S_OK;
}
