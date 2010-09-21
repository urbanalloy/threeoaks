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

// CControllerHome.cpp : Implementation of CSysStatsCOMApp and DLL registration.

#include "stdafx.h"
#include <comdef.h>
#include "SysStatsCOM.h"
#include "ControllerHome.h"
#include "CControllerFactory.h"
#include "IINIPersister.h"

/////////////////////////////////////////////////////////////////////////////
//

#define IS_MOUSE_EVENT(uMsg) ((uMsg >= WM_MOUSEFIRST) && (uMsg <= WM_MOUSELAST))
#define MK_MASK	(MK_SHIFT | MK_CONTROL | 0x80)

STDMETHODIMP CControllerHome::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IControllerHome,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

const char* CControllerHome::UNKNOWN = "unknown";
const char* CControllerHome::CONTROLLER_COUNT_PROPERTY = "ControllerCount";
const char* CControllerHome::CONTROLLER_INDEX_PROPERTY = "Controller";
const char* CControllerHome::CONTROLLER_PREFIX = "controller";
const char* CControllerHome::CONTROLLER_TYPE_PROPERTY = "Type";

static _bstr_t EMPTY_BSTR = "";

CControllerHome::CControllerHome() : model(0), pMouseOverOverlay(0)
{
}

CControllerHome::~CControllerHome()
{
//	ClearAndDestroy();
}

STDMETHODIMP CControllerHome::Add(/*[in]*/ IController *pController)
{
	if (model)
		pController->put_Model(model);

	return ArrayPtrVariant<IController, IControllerHome>::Add(pController);
}

STDMETHODIMP CControllerHome::Remove(/*[in]*/ VARIANT index)
{
	HRESULT hr = ArrayPtrVariant<IController, IControllerHome>::Remove(index);
	return hr;
}

STDMETHODIMP CControllerHome::Swap(/*[in]*/ VARIANT one, /*[in]*/ VARIANT two)
{
	HRESULT hr = ArrayPtrVariant<IController, IControllerHome>::Swap(one, two);
	return hr;
}

STDMETHODIMP CControllerHome::Item(VARIANT index, IController **pRet)
{
	BSTR bstr;
	short count = 0;

	if (SUCCEEDED(SysStatsUtils::VariantToBSTR(&index, &bstr)))
	{
		Count(&count);
		for (int i=0; i < count; i++)
		{
			IController *pController;
			ArrayPtrVariant<IController, IControllerHome>::Item(CComVariant(i), &pController);
			BSTR name;
			pController->get_Name(&name);
			if (wcscmp(name, bstr) == 0)
			{
				*pRet = pController;
				::SysFreeString(name);
				return S_OK;
			}
			::SysFreeString(name);
			pController->Release();
		}

		return S_OK;
	}
	else
		return ArrayPtrVariant<IController, IControllerHome>::Item(index, pRet);

	return E_INVALIDARG;
}

/*
 * messageId = uMsg
 * value = wparam
 * x = LOWORD(lparam)
 * y = HIWORD(lparam)
 */
STDMETHODIMP CControllerHome::HandleMessage(short messageID, UINT wParam, UINT lParam, short x, short y, VARIANT_BOOL *handled)
{
	short count = 0;
	Count(&count);
	VARIANT_BOOL d = false;
	IController *pItem;
	*handled = false;

	AtlTrace("CControllerHome::Msg=0x%0.4x\n", messageID);

	if (messageID == WM_NULL) {
		*handled = true;
		return S_OK;
	}

	for (int i=0; i<count && !*handled; i++)
	{
		Item(CComVariant(i), &pItem);
		if (!pItem)
			continue;

		short handlesType;
		pItem->get_MessageType(&handlesType);
		d = false;
		if (messageID == handlesType)
		{
			UINT key = 0;
			pItem->get_Key(&key);

			if (IS_MOUSE_EVENT(messageID))
			{
				// Make ALT key apear in wParam too.
				if (GetKeyState(VK_MENU) < 0)
					wParam = wParam | 0x80;

				// We're only interested in *some* bits in wParam.
				if ((wParam & MK_MASK) != (key & MK_MASK))
					goto safebreakloop;	// System key state doesn't match what we want.

				// If its a scroll-wheel event, put the scroll distance into lparam!
				if (messageID == WM_MOUSEWHEEL)
				{
					lParam = GET_WHEEL_DELTA_WPARAM(wParam);
				}
			}
			else if (messageID == WM_CHAR)
			{
				if (wParam != key)
					goto safebreakloop;
			}
			else if (messageID == WM_APPCOMMAND)
			{
				if (key && (GET_APPCOMMAND_LPARAM(lParam) != key))
					goto safebreakloop;
			}
			else if (messageID >= WM_USER)
			{
				if (key != -1 && wParam != key)
					goto safebreakloop;
			}

			// This controller handles this sort of message
			IOverlay *pOverlay = 0;

			// Doesn't AddRef() the returned overlay
			pItem->get_Overlay(&pOverlay);

			if (pOverlay)
			{
				// The mouse should be over a particular overlay for the controller to respond.
				ISimpleOverlay2 *pOverlay2 = 0;
				pOverlay->QueryInterface(__uuidof(ISimpleOverlay2), (void**)&pOverlay2);
				if (pOverlay2)
				{
					if (messageID == WM_MOUSELEAVE)
					{
						// If mouse leave was targeted at this overlay, pass message to the controller.
						if (wParam == 0 || pOverlay2 == (ISimpleOverlay2*)wParam)
							pItem->HandleMessage(0, lParam, x, y, &d);
					}
					else
					{
						pOverlay2->HitTest(x, y, &d);
						if (d)
						{
							// Handle Mouse Move/Leave
							if (messageID == WM_MOUSEMOVE && pMouseOverOverlay != pOverlay2)
							{
								if (pMouseOverOverlay != NULL)
									model->PostMessage(WM_MOUSELEAVE, (WPARAM)pMouseOverOverlay, 0);

								pMouseOverOverlay = pOverlay2;
							}
							pItem->HandleMessage(wParam, lParam, x, y, &d);
						}
						else
						{
							if (messageID == WM_MOUSEMOVE && pMouseOverOverlay == pOverlay2)
							{
								// Handle Mouse Over/Leave
								model->PostMessage(WM_MOUSELEAVE, (WPARAM)pMouseOverOverlay, 0);
								pMouseOverOverlay = NULL;
							}
						}
					}

					pOverlay2->Release();
				}
			}
			else
			{
				// Hack for dragging
				if (messageID == WM_LBUTTONDOWN)
				{
					HWND hwnd;
					model->get_hWnd((LONG*)&hwnd);
					POINT p = { x, y };

					if (::DragDetect(hwnd, p))
					{
						goto safebreakloop;
					}
				}

				pItem->HandleMessage(wParam, lParam, x, y, &d);
			}

			*handled = *handled || d;
		}

safebreakloop:
		pItem->Release();
	}

	return S_OK;
}

STDMETHODIMP CControllerHome::get_Model(IClientModel **pVal)
{
	*pVal = model;

	model->AddRef();

	return S_OK;
}

STDMETHODIMP CControllerHome::put_Model(IClientModel *newVal)
{
	if (model == newVal)
		return S_OK;

#ifdef CIRCULAR_REFERENCES_OK
	if (model)
		model->Release();

	newVal->AddRef();
#endif
	model = newVal;

	short count = 0;
	Count(&count);

	for (int i=0; i<count; i++)
	{
		IController *pItem;
		Item(CComVariant(i), &pItem);
		pItem->put_Model(newVal);
		pItem->Release();
	}

	return S_OK;
}

STDMETHODIMP CControllerHome::get_Controllers(LPSAFEARRAY *pVal)
{
	short count;
	Count(&count);
	SAFEARRAY *pArray = ::SafeArrayCreateVector(VT_DISPATCH, 0, count);

	IController *pController;
	long indices[1];
	for (int i=0; i<count; i++)
	{
		indices[0] = i;
		Item(CComVariant(i), &pController);
		::SafeArrayPutElement(pArray, indices, pController);
		pController->Release();
	}

	*pVal = pArray;

	return S_OK;
}

STDMETHODIMP CControllerHome::put_Controllers(LPSAFEARRAY newVal)
{
	ClearAndDestroy();
	long lb = 0;
	long ub = 0;
	if( !(newVal->fFeatures & FADF_DISPATCH) )
		return E_INVALIDARG;

	HRESULT hr = ::SafeArrayGetUBound(newVal, 1, &ub);
	hr = ::SafeArrayGetLBound(newVal, 1, &lb);

	IDispatch *pDisp;
	IController *pController;
	long indices[1];
	for (int i=lb; i<=ub; i++)
	{
		indices[0] = i;
		pDisp = 0;
		::SafeArrayGetElement(newVal, indices, &pDisp);
		if (pDisp)
		{
			if (SUCCEEDED(pDisp->QueryInterface(IID_IController, (void**)&pController)))
			{
				AtlTrace("Added controller to ControllerHome\n");
				Add(pController);
				pController->Release();
			}
			pDisp->Release();
		}
	}

	return S_OK;
}
