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

// ASyncScriptController.cpp : Implementation of CASyncScriptController
#include "stdafx.h"
#include "ControllerImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#include "SysStatsUtils.h"
#include "COMControllers.h"
#include "ASyncScriptController.h"
#include "ScriptThread.h"

/////////////////////////////////////////////////////////////////////////////
// CASyncScriptController

STDMETHODIMP CASyncScriptController::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IASyncScriptController
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/*
 * Constructor
 */
CASyncScriptController::CASyncScriptController() : pThread(new ScriptThread())
{
}

CASyncScriptController::~CASyncScriptController()
{
	AtlTrace("Destroying ASyncScriptController\n");
	pThread->terminate();
}

STDMETHODIMP CASyncScriptController::put_Model(IClientModel *newVal)
{
	if (model != newVal)
	{
		model = newVal;
		pThread->put_Model(newVal);
	}

	return S_OK;
}

/*
 * This method is called when a windows message of the correct type has been posted.
 */
STDMETHODIMP CASyncScriptController::HandleMessage(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled)
{
	*handled = !passThrough;

	pThread->HandleMessage(wParam, lParam, x, y);

	// Provide implementation.
	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the controller should
 * be displayed so that the user can configure the controller to meet their
 * requirements.
 */
STDMETHODIMP CASyncScriptController::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	pThread->Configure(observer, meters, hDlg);

	return S_OK;
}

/*
 * Return a string representation of the controller’s type.
 */
STDMETHODIMP CASyncScriptController::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_ASyncScriptController, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CASyncScriptController::ShowConsole(VARIANT_BOOL show)
{
	pThread->ShowConsole(show);

	return S_OK;
}

STDMETHODIMP CASyncScriptController::get_ScriptType(BSTR *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	pThread->get_ScriptType(pRet);

	return S_OK;
}

STDMETHODIMP CASyncScriptController::put_ScriptType(BSTR newVal)
{
	pThread->put_ScriptType(newVal);

	return S_OK;
}

STDMETHODIMP CASyncScriptController::get_FileName(BSTR *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	pThread->get_FileName(pRet);

	return S_OK;
}

STDMETHODIMP CASyncScriptController::put_FileName(BSTR newVal)
{
	pThread->put_FileName(newVal);

	return S_OK;
}