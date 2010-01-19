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

// MailMeter.cpp : Implementation of CMailMeter
#include "stdafx.h"
#include <stdio.h>
#pragma comment(lib, "SysStatsCommon.lib")
#include "SysStatsUtils.h"
#include "MeterImpl.cpp"
#include "COMMeters.h"
#include "MailMeter.h"
#include "MailMeterDialog.h"
#include "MAPIMailCheck.h"

/////////////////////////////////////////////////////////////////////////////
// CMailMeter

STDMETHODIMP CMailMeter::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IMailMeter
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
CMailMeter::CMailMeter() :
	username(""),
	password(""),
	value(0),
	validDays(0),
	pMailChecker(NULL),
	pWeakTarget(NULL)
{
	interval = 60;
	::InitializeCriticalSection(&checkerLock);
}

CMailMeter::~CMailMeter()
{
	destroyMailChecker();
}

HRESULT CMailMeter::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	HRESULT res = ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);

	::CLSIDFromProgID(L"SysStatsCOM.MetaData", &clsid);
	 res = ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pMetaData);

	 char buf[MAX_PATH];
	 _snprintf(buf, sizeof(buf), "#%d", IDR_XML_MAILMETER);

	IMetaData *pMD = 0;
	pMetaData->QueryInterface(_uuidof(IMetaData), (void**)&pMD);
	if (pMD)
	{
		pMD->PutResourceInfo((long)_Module.GetModuleInstance(), _bstr_t(buf));
		pMD->Release();
	}

	return res;
}

void CMailMeter::FinalRelease()
{
	pWeakTarget->Release();
	pMetaData->Release();
}

IMailCheck* CMailMeter::getMailChecker()
{
	::EnterCriticalSection(&checkerLock);
	if (pMailChecker == NULL)
	{
		pMailChecker = new MAPIMailCheck();
		pMailChecker->Initialize(username, password, validDays);
	}
	::LeaveCriticalSection(&checkerLock);

	return pMailChecker;
}

void CMailMeter::destroyMailChecker()
{
	::EnterCriticalSection(&checkerLock);
	if (pMailChecker != NULL)
	{
		pMailChecker->Destroy();
		pMailChecker = NULL;
	}
	::LeaveCriticalSection(&checkerLock);

}

/*
 * This method is called every 'interval' seconds by SysStats.
 * Some meters will use it to actually update their value.
 * Other meters update asynchronously. All meters should set
 * the 'dirty' output parameter to true if the value of the meter
 * has changed since the last time that Update() was called.
 */
STDMETHODIMP CMailMeter::Update(long *dirty)
{
	if (dirty == NULL)
		return E_POINTER;

	long newValue = value;
	getMailChecker()->NewMessageCount(&newValue);
	*dirty = newValue != value;
	value = newValue;

	// Provide implementation.
	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the meter should
 * be displayed so that the user can configure the meter to meet their
 * requirements.
 */
STDMETHODIMP CMailMeter::Configure(IObserver * observer, LONG hWnd)
{
	MailMeterDialog *d = new MailMeterDialog(this);
	d->DoModal((HWND)hWnd);
	delete d;

	return S_OK;
}

/*
 * Return a string representation of the meter’s type.
 */
STDMETHODIMP CMailMeter::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_MailMeter, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

/*
 * Return the meter's value as a long - if that makes sense. Some
 * overlays require a long value, some don't.
 *
 * Parameter 'sel' can be:
 *		0 - Get the number of unread mails
 *		1 - Get 0 if there are no mails, 1 otherwise.
 */
STDMETHODIMP CMailMeter::GetAsLong(BSTR sel, long *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	long selector =	::wcstol(sel, NULL, 10);

	if (selector == 0)
		*pRet = value;
	else
		*pRet = value > 0 ? 1 : 0;

	return S_OK;
}

/*
 * Return the meter's value as a double - if that makes sense. Some
 * overlays require a double value, some don't.
 *
 * See GetAsLong for description of 'sel'.
 */
STDMETHODIMP CMailMeter::GetAsDouble(BSTR sel, double *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	long l = 0;
	GetAsLong(sel, &l);
	*pRet = (double)l;

	return S_OK;
}

/*
 * Return the meter's value as a string. If that makes sense. Some
 * overlays require a string value, some don't.
 *
 * Parameter 'format' should expect a long integer.
 *
 * See GetAsLong() for a description of sel.
 */
STDMETHODIMP CMailMeter::GetAsString(BSTR format, BSTR selector, BSTR *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	try
	{
		char buf[MAX_PATH];
		long val;
		GetAsLong(selector, &val);
		_snprintf(buf, sizeof(buf), _bstr_t(format), val);
		buf[MAX_PATH-1] = 0;
		*pRet = _com_util::ConvertStringToBSTR(buf);
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
STDMETHODIMP CMailMeter::GetValue(VARIANT *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	VARIANT ret;
	::VariantInit(&ret);
	CComVariant varResult;
	varResult = value;
	varResult.Detach(&ret);
	*pRet = ret;

	return S_OK;
}

STDMETHODIMP CMailMeter::get_Username(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = username.copy();

	return S_OK;
}

STDMETHODIMP CMailMeter::put_Username(BSTR newVal)
{
	username = newVal;
	destroyMailChecker();

	return S_OK;
}

STDMETHODIMP CMailMeter::get_Password(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = password.copy();

	return S_OK;
}

STDMETHODIMP CMailMeter::put_Password(BSTR newVal)
{
	password = newVal;
	destroyMailChecker();

	return S_OK;
}

STDMETHODIMP CMailMeter::get_ValidDays(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = validDays;

	return S_OK;
}

STDMETHODIMP CMailMeter::put_ValidDays(long newVal)
{
	validDays = newVal;

	return S_OK;
}
