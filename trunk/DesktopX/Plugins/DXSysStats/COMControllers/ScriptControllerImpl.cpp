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

// ScriptControllerImpl.cpp : Implementation of CScriptControllerImpl
#include "stdafx.h"
#include "ControllerImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#include "SysStatsUtils.h"
#include "COMControllers.h"
#include "ScriptControllerImpl.h"
#include "IScriptConsole.h"

const _bstr_t CScriptControllerImpl::UTILS_NAME = "WScript";
const _bstr_t CScriptControllerImpl::ACTIVESCRIPT_HOST_NAME = "ScriptController";
const _bstr_t CScriptControllerImpl::MODEL_NAME = "SysStatsModel";
const _bstr_t CScriptControllerImpl::HANDLE_NAME = ACTIVESCRIPT_HOST_NAME + "_HandleMessage";

static BSTR EMPTY_BSTR = _bstr_t("");

#define CONNECTABLE

/////////////////////////////////////////////////////////////////////////////
// CScriptControllerImpl

STDMETHODIMP CScriptControllerImpl::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IScriptControllerImpl
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
CScriptControllerImpl::CScriptControllerImpl() : pConsole(IScriptConsole::createConsole())
{
#ifdef CONNECTABLE
	HRESULT hr = ::CoCreateInstance(__uuidof(ScriptUtils), NULL, CLSCTX_INPROC_SERVER, __uuidof(IScriptUtils), (void**)&pUtils);
#endif
}

/*
 * Destructor
 */
CScriptControllerImpl::~CScriptControllerImpl()
{
	delete pConsole;
#ifdef CONNECTABLE
	pUtils->Release();
#endif
}

// Return the filename as a relative path
STDMETHODIMP CScriptControllerImpl::get_FileName(BSTR *pVal)
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
STDMETHODIMP CScriptControllerImpl::put_FileName(BSTR newVal)
{
	IClientModel *_model = 0;
	get_Model(&_model);
	IAppConfig *appConfig = 0;
	_model->get_AppConfig(&appConfig);

	BSTR tmp;
	appConfig->CanonicalPath(newVal, &tmp);
	ActiveScriptImpl::put_FileName(tmp);
	::SysFreeString(tmp);

	appConfig->Release();
	_model->Release();

	return S_OK;
}

/*
 * This method is called when a windows message of the correct type has been posted.
 */
STDMETHODIMP CScriptControllerImpl::HandleMessage(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled)
{
	*handled = Fire_HandleMessage(wParam, lParam, x, y);

	return S_OK;
}

STDMETHODIMP CScriptControllerImpl::put_Model(IClientModel *model)
{
	ControllerImpl<IScriptControllerImpl>::put_Model(model);
	AddScriptItem(MODEL_NAME, model);

	return S_OK;
}

STDMETHODIMP CScriptControllerImpl::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	return S_OK;
}

STDMETHODIMP CScriptControllerImpl::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_ScriptControllerImpl, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

BOOL CScriptControllerImpl::AddDefaultScriptItem()
{
	HRESULT hr;
	hr = scriptPtr->AddTypeLib(LIBID_COMCONTROLLERSLib, 1, 0, 0);

	if (!SUCCEEDED(hr))
		return false;

	BOOL ret = AddScriptItem(ACTIVESCRIPT_HOST_NAME, (IController*)this);
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
		_model->Release();
	}
	else
		AtlTrace("Model is not set in ScriptControllerImpl\n");

	return ret;
}

STDMETHODIMP CScriptControllerImpl::ShowConsole(VARIANT_BOOL show)
{
	pConsole->Show(show);

	return S_OK;
}

STDMETHODIMP CScriptControllerImpl::Trace(BSTR msg)
{
	ActiveScriptImpl::Trace(msg);
	pConsole->AddString(msg);

	return S_OK;
}
