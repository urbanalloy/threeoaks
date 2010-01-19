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

// ScriptMeter.cpp : Implementation of CASyncScriptMeter
#include "stdafx.h"
#include "MeterImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#include "SysStatsUtils.h"
#include "COMMeters.h"
#include "ASyncScriptMeter.h"
#include "ScriptDialog.h"
#include "ScriptThread.h"

/////////////////////////////////////////////////////////////////////////////
// CASyncScriptMeter

STDMETHODIMP CASyncScriptMeter::InterfaceSupportsErrorInfo(REFIID riid)
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
CASyncScriptMeter::CASyncScriptMeter() : pThread(new ScriptThread()), pWeakTarget(0)
{
}

CASyncScriptMeter::~CASyncScriptMeter()
{
	pThread->terminate();
}

HRESULT CASyncScriptMeter::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CASyncScriptMeter::FinalRelease()
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
STDMETHODIMP CASyncScriptMeter::Update(long *dirty)
{
	if (dirty == NULL)
		return E_POINTER;

	pThread->Update(dirty);

	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the meter should
 * be displayed so that the user can configure the meter to meet their
 * requirements.
 */
STDMETHODIMP CASyncScriptMeter::Configure(IObserver * observer, LONG hWnd)
{
	pThread->Configure(observer, hWnd);

	return S_OK;
}

STDMETHODIMP CASyncScriptMeter::ShowConsole(VARIANT_BOOL show)
{
	pThread->ShowConsole(show);

	return S_OK;
}

/*
 * Return a string representation of the meter’s type.
 */
STDMETHODIMP CASyncScriptMeter::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_ASyncScriptMeter, szFriendlyName, sizeof(szFriendlyName));
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
STDMETHODIMP CASyncScriptMeter::GetAsLong(BSTR sel, long *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	// This will return without blocking - i.e. the value will
	// be a cached value.
	pThread->GetAsLong(sel, pRet);

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
STDMETHODIMP CASyncScriptMeter::GetAsDouble(BSTR sel, double *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	// This will return without blocking - i.e. the value will
	// be a cached value.
	pThread->GetAsDouble(sel, pRet);

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
STDMETHODIMP CASyncScriptMeter::GetAsString(BSTR format, BSTR selector, BSTR *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	// This will return without blocking - i.e. the value will
	// be a cached value.
	pThread->GetAsString(format, selector, pRet);

	return S_OK;
}

/*
 * Return the meter's value in the most native format possible.
 * This is how scripts get a hold of the value.
 */
STDMETHODIMP CASyncScriptMeter::GetValue(VARIANT *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	// This will return without blocking - i.e. the value will
	// be a cached value.
	pThread->GetValue(pRet);

	return S_OK;
}

STDMETHODIMP CASyncScriptMeter::put_Model(IClientModel *newVal)
{
	if (model != newVal)
	{
		model = newVal;
		pThread->put_Model(newVal);
	}

	return S_OK;
}

STDMETHODIMP CASyncScriptMeter::get_ScriptType(BSTR *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	pThread->get_ScriptType(pRet);

	return S_OK;
}

STDMETHODIMP CASyncScriptMeter::put_ScriptType(BSTR newVal)
{
	pThread->put_ScriptType(newVal);

	return S_OK;
}

STDMETHODIMP CASyncScriptMeter::get_FileName(BSTR *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	pThread->get_FileName(pRet);

	return S_OK;
}

STDMETHODIMP CASyncScriptMeter::put_FileName(BSTR newVal)
{
	pThread->put_FileName(newVal);

	return S_OK;
}

STDMETHODIMP CASyncScriptMeter::put_Value(BSTR newVal)
{
	pThread->put_Value(newVal);

	return S_OK;
}
