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

// ScriptMeter.cpp : Implementation of CScriptMeterImpl
#include "stdafx.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

#include "COMMeters.h"
#include "ScriptMeterImpl.h"
#include "SysStatsUtils.h"
#include "IScriptConsole.h"

const _bstr_t CScriptMeterImpl::ACTIVESCRIPT_HOST_NAME = "ScriptMeter";
const _bstr_t CScriptMeterImpl::MODEL_NAME = "SysStatsModel";
const _bstr_t CScriptMeterImpl::UTILS_NAME = "WScript";
const _bstr_t CScriptMeterImpl::UPDATE_NAME = ACTIVESCRIPT_HOST_NAME + "_Update";
const _bstr_t CScriptMeterImpl::GETVALUE_NAME = ACTIVESCRIPT_HOST_NAME + "_GetValue";
const _bstr_t CScriptMeterImpl::PUTVALUE_NAME = ACTIVESCRIPT_HOST_NAME + "_PutValue";

static BSTR EMPTY_BSTR = L"";

#define CONNECTABLE

/////////////////////////////////////////////////////////////////////////////
// CScriptMeterImpl

STDMETHODIMP CScriptMeterImpl::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScriptMeterImpl
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CScriptMeterImpl::CScriptMeterImpl(): pConsole(IScriptConsole::createConsole()), pUtils(0)
{
#ifdef CONNECTABLE
	HRESULT hr = ::CoCreateInstance(__uuidof(ScriptUtils), NULL, CLSCTX_INPROC_SERVER, __uuidof(IScriptUtils), (void**)&pUtils);
#endif
}

/*
 * Destructor
 */
CScriptMeterImpl::~CScriptMeterImpl()
{
	delete pConsole;
#ifdef CONNECTABLE
	pUtils->Release();
#endif
}

// Return the filename as a relative path
STDMETHODIMP CScriptMeterImpl::get_FileName(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IClientModel *_model = 0;
	get_Model(&_model);
	IAppConfig *appConfig = 0;
	_model->get_AppConfig(&appConfig);

	appConfig->NormalizePath(fileName, pVal);

	AtlTrace("fn=%s, ret=%s\n",(char*)fileName, (char*)_bstr_t(*pVal));

	appConfig->Release();
	_model->Release();

	return S_OK;
}

// Store the filename as an absolute path and load the script
STDMETHODIMP CScriptMeterImpl::put_FileName(BSTR newVal)
{
	IClientModel *_model = 0;
	get_Model(&_model);
	IAppConfig *appConfig = 0;
	_model->get_AppConfig(&appConfig);

	BSTR tmp = 0;
	appConfig->CanonicalPath(newVal, &tmp);
	ActiveScriptImpl::put_FileName(tmp);
	::SysFreeString(tmp);

	appConfig->Release();
	_model->Release();

	return S_OK;
}

STDMETHODIMP CScriptMeterImpl::Update(/*[out, retval]*/ long *pVal)
{
	*pVal = Fire_Update();

	return S_OK;
}

STDMETHODIMP CScriptMeterImpl::put_Model(IClientModel *model)
{
	MeterImpl<IScriptMeterImpl>::put_Model(model);
	AddScriptItem(MODEL_NAME, model, SCRIPTITEM_ISVISIBLE | MY_SCRIPTITEM_WEAK);

	return S_OK;
}

STDMETHODIMP CScriptMeterImpl::Configure(IObserver * observer, LONG hWnd)
{
	return S_OK;
}

STDMETHODIMP CScriptMeterImpl::put_Value(BSTR newVal)
{
	Fire_PutValue(newVal);

	return S_OK;
}

STDMETHODIMP CScriptMeterImpl::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_ScriptMeterImpl, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

HRESULT CScriptMeterImpl::GetValue(BSTR selector, VARIANT *pRet)
{
	*pRet = Fire_GetValue(selector);

	return S_OK;
}

STDMETHODIMP CScriptMeterImpl::GetValue(VARIANT *pRet)
{
	*pRet = Fire_GetValue(EMPTY_BSTR);

	return S_OK;
}

STDMETHODIMP CScriptMeterImpl::GetAsLong(BSTR selector, long *pRet)
{
	VARIANT varRet;
	::VariantInit(&varRet);

	HRESULT hr = GetValue(selector, &varRet);
	if (!SUCCEEDED(hr))
		return hr;

	hr = SysStatsUtils::VariantToInteger(&varRet, pRet);
	::VariantClear(&varRet);
	return hr;
}

STDMETHODIMP CScriptMeterImpl::GetAsDouble(BSTR selector, double *pRet)
{
	VARIANT varRet;
	::VariantInit(&varRet);

	HRESULT hr = GetValue(selector, &varRet);
	if (!SUCCEEDED(hr))
		return hr;

	hr = SysStatsUtils::VariantToDouble(&varRet, pRet);
	::VariantClear(&varRet);
	return hr; 
}

STDMETHODIMP CScriptMeterImpl::GetAsString(BSTR format, BSTR selector, BSTR *pRet)
{
	VARIANT varRet;
	::VariantInit(&varRet);

	BSTR bStr = 0;

	HRESULT hr = GetValue(selector, &varRet);
	if (!SUCCEEDED(hr))
		return hr;

	long i;
	hr = SysStatsUtils::VariantToInteger(&varRet, &i);
	if (SUCCEEDED(hr))
	{
		try
		{
			wchar_t buf[MAX_PATH];
			_snwprintf(buf, MAX_PATH, format, i);
			buf[MAX_PATH-1] = 0;
			*pRet = ::SysAllocString(buf);
		}
		catch (...)
		{
		}

		goto consideredharmful;
	}

	double d;
	hr = SysStatsUtils::VariantToDouble(&varRet, &d);
	if (SUCCEEDED(hr))
	{
		try
		{
			wchar_t buf[MAX_PATH];
			_snwprintf(buf, MAX_PATH, format, d);
			buf[MAX_PATH-1] = 0;
			*pRet = ::SysAllocString(buf);
		}
		catch (...)
		{
		}

		goto consideredharmful;
	}

	hr = SysStatsUtils::VariantToBSTR(&varRet, &bStr);
	if (SUCCEEDED(hr))
	{
		try
		{
			wchar_t buf[MAX_PATH];
			_snwprintf(buf, MAX_PATH, format, bStr);
			buf[MAX_PATH-1] = 0;
			*pRet = ::SysAllocString(buf);
		}
		catch (...)
		{
		}

		goto consideredharmful;
	}

consideredharmful:
	::VariantClear(&varRet);

	return hr;
}

BOOL CScriptMeterImpl::AddDefaultScriptItem()
{
	HRESULT hr;
	hr = scriptPtr->AddTypeLib(LIBID_COMMETERSLib, 1, 0, 0);

	if (!SUCCEEDED(hr))
		return false;

	BOOL ret = AddScriptItem(ACTIVESCRIPT_HOST_NAME, (IMeter*)this);
#ifdef CONNECTABLE
	IDispatch *pDispatch;
	scriptPtr->GetScriptDispatch(NULL, &pDispatch);
	pDispatch->Release();
	pUtils->put_Script(pDispatch);
	pUtils->put_ScriptHost(this);

	AddScriptItem(UTILS_NAME, pUtils, SCRIPTITEM_ISVISIBLE);
#endif

	IClientModel *_model = 0;
	get_Model(&_model);
	if (_model)
	{
		ret = AddScriptItem(MODEL_NAME, _model, SCRIPTITEM_ISVISIBLE | MY_SCRIPTITEM_WEAK);
		model->Release();
	}
	else
		AtlTrace("Model is not set in ScriptMeterImpl\n");

	return ret;
}

STDMETHODIMP CScriptMeterImpl::ShowConsole(VARIANT_BOOL show)
{
	pConsole->Show(show);

	return S_OK;
}

STDMETHODIMP CScriptMeterImpl::Trace(BSTR msg)
{
	ActiveScriptImpl::Trace(msg);
	pConsole->AddString(msg);

	return S_OK;
}
