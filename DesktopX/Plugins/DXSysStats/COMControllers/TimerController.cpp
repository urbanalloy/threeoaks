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

// TimerController.cpp : Implementation of CTimerController
#include "stdafx.h"
#include "MessageSenderImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#include "SysStatsUtils.h"
#include "COMControllers.h"
#include "TimerController.h"
#include "TimerControllerDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CTimerController

STDMETHODIMP CTimerController::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_ITimerController
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
CTimerController::CTimerController() : interval(0.0)
{
}

CTimerController::~CTimerController()
{
	Stop();
}

/*
 * This method is called when a windows message of the correct type has been posted. It should never be called.
 */
STDMETHODIMP CTimerController::HandleMessage(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled)
{
	*handled = true;
	if (model)
		model->PostMessage(sendMessage, wParam, lParam);

	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the controller should
 * be displayed so that the user can configure the controller to meet their
 * requirements.
 */
STDMETHODIMP CTimerController::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	TimerControllerDialog *d = new TimerControllerDialog(this, observer, meters);
	d->DoModal((HWND)hDlg);
	delete d;

	return S_OK;
}

/*
 * Return a string representation of the controller’s type.
 */
STDMETHODIMP CTimerController::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_TimerController, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CTimerController::Stop()
{
	ICOMTimerService *pTimerService = 0;

	if (model)
	{
		model->get_TimerService(&pTimerService);

		if (pTimerService)
		{
			AtlTrace("CTimerController: Got timer service\n");
			pTimerService->RemoveTimerClient(this);
		}
	}

	return S_OK;
}

STDMETHODIMP CTimerController::Start()
{
	ICOMTimerService *pTimerService = 0;

	if (model)
	{
		model->get_TimerService(&pTimerService);

		if (pTimerService)
		{
			AtlTrace("CTimerController: Got timer service\n");
			pTimerService->SetTimerClient(this);
		}
	}

	return S_OK;
}

STDMETHODIMP CTimerController::get_Interval(DOUBLE *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = interval;

	return S_OK;
}

STDMETHODIMP CTimerController::put_Interval(DOUBLE newVal)
{
	interval = newVal;

	Start();

	return S_OK;
}