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

// ScriptMeter.cpp : Implementation of CScriptMeter
#include "stdafx.h"
#include "MeterImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#include "SysStatsUtils.h"
#include "COMMeters.h"
#include "ScriptMeter.h"
#include "ScriptDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CScriptMeter

STDMETHODIMP CScriptMeter::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IScriptMeter
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
CScriptMeter::CScriptMeter() : pImpl(0), pWeakTarget(0)
{
	HRESULT hr = ::CoCreateInstance(CLSID_ScriptMeterImpl, NULL, CLSCTX_INPROC_SERVER, IID_IScriptMeterImpl, (void**)&pImpl);	
}

CScriptMeter::~CScriptMeter()
{
	if (pImpl)
		pImpl->put_ScriptType(L"");

	pImpl->Release();

	pImpl = 0;
}

HRESULT CScriptMeter::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CScriptMeter::FinalRelease()
{
	pWeakTarget->Release();
}

/*
 * This method is called every 'interval' seconds by SysStats.
 * Some meters will use it to actually update their value.
 * Other meters update asynchronously. All meters should set
 * the 'dirty' output parameter to true if the value of the meter
 * has changed since the last time that Update() was called.
 */
STDMETHODIMP CScriptMeter::Update(long *dirty)
{
	return pImpl->Update(dirty);
}

/*
 * Called by SysStats when a configuration dialog for the meter should
 * be displayed so that the user can configure the meter to meet their
 * requirements.
 */
STDMETHODIMP CScriptMeter::Configure(IObserver * observer, LONG hWnd)
{
	ScriptDialog *d = new ScriptDialog(this);
	d->DoModal((HWND)hWnd);
	delete d;

	return S_OK;
}

/*
 * Return a string representation of the meter’s type.
 */
STDMETHODIMP CScriptMeter::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_ScriptMeter, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CScriptMeter::put_Model(IClientModel *model)
{
	MeterImpl<IScriptMeter>::put_Model(model);
	return pImpl->put_Model(model);
}

STDMETHODIMP CScriptMeter::get_Model(IClientModel **model)
{
	return pImpl->get_Model(model);
}

/*
 * Return the meter's value as a long - if that makes sense. Some
 * overlays require a long value, some don't.
 *
 * Parameter 'sel' can be used to select one value from several if the
 * meter actually stores multiple values. It is up to the meter to
 * interpret the contents of 'sel' as it sees fit.
 */
STDMETHODIMP CScriptMeter::GetAsLong(BSTR sel, long *pRet)
{
	return pImpl->GetAsLong(sel, pRet);
}

/*
 * Return the meter's value as a double - if that makes sense. Some
 * overlays require a double value, some don't.
 *
 * Parameter 'sel' can be used to select one value from several if the
 * meter actually stores multiple values. It is up to the meter to
 * interpret the contents of 'sel' as it sees fit.
 */
STDMETHODIMP CScriptMeter::GetAsDouble(BSTR sel, double *pRet)
{
	return pImpl->GetAsDouble(sel, pRet);
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
STDMETHODIMP CScriptMeter::GetAsString(BSTR format, BSTR selector, BSTR *pRet)
{
	return pImpl->GetAsString(format, selector, pRet);
}

/*
 * Return the meter's value in the most native format possible.
 * This is how scripts get a hold of the value.
 */
STDMETHODIMP CScriptMeter::GetValue(VARIANT *pRet)
{
	return pImpl->GetValue(pRet);
}

