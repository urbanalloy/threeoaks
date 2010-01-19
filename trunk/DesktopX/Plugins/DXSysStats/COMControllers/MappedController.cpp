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

// MappedController.cpp : Implementation of CMappedController
#include "stdafx.h"
#include "MessageSenderImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#include "SysStatsUtils.h"
#include "COMControllers.h"
#include "MappedController.h"
#include "MappedControllerDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMappedController

STDMETHODIMP CMappedController::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IMappedController
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/* IDC_APPSTARTING
 * Constructor
 */
CMappedController::CMappedController() : subclassed(false)
{
}

CMappedController::~CMappedController()
{
	if (subclassed)
	{
		model->RemoveWindowSubclass(this);
		::ReleaseCapture();
	}
}

void CMappedController::SetModal()
{
	HWND hwnd;
	model->SetWindowSubclass(this);
	model->get_hWnd((LONG*)&hwnd);
	::SetCapture(hwnd);
	subclassed = true;
}

/*
 * Handle Subclass Messages
 */
STDMETHODIMP CMappedController::HandleSubclassMessage(UINT msg, UINT wparam, UINT lparam, VARIANT_BOOL *bHandled)
{
	switch(msg) 
	{
		case WM_MOUSEMOVE:
			PostMessage(wparam, lparam, LOWORD(lparam), HIWORD(lparam), bHandled);
			break;

		case WM_CHAR:
			if (wParam != 0x1B)
				break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_CANCELMODE:
			ReleaseCapture();
			// Fall thru

		case WM_CAPTURECHANGED:
			// Release message pump
			model->RemoveWindowSubclass(this);
			*bHandled = TRUE;
			break;
	} 

	return 1;
}

/*
 * This method is called when a windows message of the correct type has been posted.
 */
STDMETHODIMP CMappedController::HandleMessage(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled)
{
	*handled = !passThrough;

	HWND hwnd;
	model->get_hWnd((LONG*)&hwnd);
	POINT p = { x, y };

	if (::DragDetect(hwnd, p))
	{
		SetModal();
		return S_OK;
	}
	else
		return PostMessage(wParam,
			lParam,
			LOWORD(lParam),
			HIWORD(lParam),
			handled);
}

STDMETHODIMP CMappedController::PostMessage(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled)
{
	IOverlay *pOverlay = 0;
	get_Overlay(&pOverlay);

	if (pOverlay)
	{
		LPOLESTR name = L"GetPixelValue";   
		DISPID id;
		HRESULT hr = pOverlay->GetIDsOfNames(IID_NULL, 
                               &name, 
                               1, 
                               LOCALE_USER_DEFAULT, 
                               &id);

		if (SUCCEEDED(hr))
		{
			AtlTrace("MappedController: Found GetPixelValue method\n");
			CComVariant varResult;
			CComVariant* pvars = new CComVariant[2];

			VariantClear(&varResult);
			pvars[0] = y;
			pvars[1] = x;
			DISPPARAMS disp = { pvars, NULL, 2, 0 };
			hr = pOverlay->Invoke(id, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, &varResult, NULL, NULL);
			delete[] pvars;

			if (SUCCEEDED(hr))
			{
				DWORD value = varResult.ulVal;
				AtlTrace("MappedController: Got pixel value: 0x%8.8x\n", value);
				if (value & 0xff000000)
				{
					model->PostMessage(sendMessage, (WPARAM)this->wParam, (LPARAM)(value & 0xff));
					*handled = true;
				}
			}
		}
	}

	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the controller should
 * be displayed so that the user can configure the controller to meet their
 * requirements.
 */
STDMETHODIMP CMappedController::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	MappedControllerDialog *d = new MappedControllerDialog(this, observer, meters);
	d->DoModal((HWND)hDlg);
	delete d;

	return S_OK;
}

/*
 * Return a string representation of the controller’s type.
 */
STDMETHODIMP CMappedController::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_MappedController, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}