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

// ScriptController.cpp : Implementation of CScriptController
#include "stdafx.h"
#include "ControllerImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#include "SysStatsUtils.h"
#include "COMControllers.h"
#include "ScriptController.h"
#include "ScriptControllerDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CScriptController

STDMETHODIMP CScriptController::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IScriptController
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
CScriptController::CScriptController() : pImpl(0)
{
	/*HRESULT hr = */::CoCreateInstance(CLSID_ScriptControllerImpl, NULL, CLSCTX_INPROC_SERVER, IID_IScriptControllerImpl, (void**)&pImpl);	
}

CScriptController::~CScriptController()
{
	AtlTrace("Destroying ScriptController\n");

	if (pImpl)
		pImpl->put_ScriptType(L"");

	pImpl->Release();

	pImpl = 0;
}

/*
 * This method is called when a windows message of the correct type has been posted.
 */
STDMETHODIMP CScriptController::HandleMessage(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled)
{
	*handled = !passThrough;
	HRESULT ret = pImpl->HandleMessage(wParam, lParam, x, y, handled);

	return ret;
}

/*
 * Called by SysStats when a configuration dialog for the controller should
 * be displayed so that the user can configure the controller to meet their
 * requirements.
 */
STDMETHODIMP CScriptController::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	ScriptControllerDialog *d = new ScriptControllerDialog(this, observer, meters);
	d->DoModal((HWND)hDlg);
	delete d;

	return S_OK;
}

/*
 * Return a string representation of the controller’s type.
 */
STDMETHODIMP CScriptController::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_ScriptController, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CScriptController::put_Model(IClientModel *model)
{
	ControllerImpl<IScriptController>::put_Model(model);
	return pImpl->put_Model(model);
}

STDMETHODIMP CScriptController::get_Model(IClientModel **model)
{
	return pImpl->get_Model(model);
}
