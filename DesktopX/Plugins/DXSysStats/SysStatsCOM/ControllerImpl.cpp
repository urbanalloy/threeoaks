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

// ControllerImpl.cpp: implementation of the ControllerImpl class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <comdef.h>
#include "ControllerImpl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template <class B>
ControllerImpl<B>::ControllerImpl() :
	name(""),
	model(0),
	meter(0),
	overlay(0),
	messageType(0),
	passThrough(false),
	key(0)
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakReference", &clsid);
	::CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWeakReference), (void**)&meter);
	::CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWeakReference), (void**)&overlay);
}

template <class B>
ControllerImpl<B>::~ControllerImpl()
{
	AtlTrace("Controller %s destroyed\n", (char*)_bstr_t(name));

	if (overlay)
		overlay->Release();

	if (meter)
		meter->Release();
}

template <class B>
STDMETHODIMP ControllerImpl<B>::HandleSubclassMessage(/*[in]*/ UINT msg, /*[in]*/ UINT wParam, /*[in]*/ UINT lParam, VARIANT_BOOL *handled)
{
	*handled = false;

	// Default behavior of 'do nothing' is OK.
	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::HandleMessage(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, VARIANT_BOOL *handled)
{
	*handled = false;

	// Default behavior of 'do nothing' is OK.
	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::get_Name(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = name.copy();
		
	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::put_Name(BSTR newVal)
{
	name = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::Configure(/*[in]*/ IObserver * observer, /*[in]*/ IMeterHome * meters, /*[in]*/ LONG hDlg)
{
	// Default behavior of do nothing is OK.
	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::get_Model(IClientModel **pVal)
{
	if (pVal == NULL)
		return E_POINTER;
	
	if (model)
		model->AddRef();

	*pVal = model;

	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::put_Model(IClientModel *newVal)
{
	if (model == newVal)
		return S_OK;

#ifdef CIRCULAR_REFERENCES_OK
	if (model)
		model->Release();

	newVal->AddRef();
#endif
	model = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::get_meter(IMeter **pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IUnknown *iUnknown = 0;
	meter->DeReference(&iUnknown);
	*pVal = (IMeter*)iUnknown;

	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::put_meter(IMeter *newVal)
{
	meter->AssignReference(newVal);

	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::get_MeterIndex(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IMeterHome *pMeters;
	model->get_Meters(&pMeters);
	short index = -1;
	IUnknown *iUnknown = 0;
	meter->DeReference(&iUnknown);
#ifdef notdef
	if (iUnknown)
	{
		IMeter *iMeter = 0;
		iUnknown->QueryInterface(__uuidof(IMeter), (void**)&iMeter);
		pMeters->GetIndex(iMeter, &index);
	}
#else
	pMeters->GetIndex((IMeter*)iUnknown, &index);
#endif
	*pVal = index;
	pMeters->Release();
		
	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::put_MeterIndex(long newVal)
{
	IMeterHome *pMeters;
	model->get_Meters(&pMeters);
	IMeter *pMeter;
	pMeters->Item(CComVariant(newVal), &pMeter);

	put_meter(pMeter);
	if (pMeter)
		pMeter->Release();

	pMeters->Release();

	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::get_MessageType(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = messageType;
		
	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::put_MessageType(short newVal)
{
	messageType = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::get_Key(UINT *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = key;
		
	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::put_Key(UINT newVal)
{
	key = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::get_OverlayPath(LPSAFEARRAY *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IOverlayHome *pOverlays;
	model->get_Overlays(&pOverlays);
	IUnknown *iUnknown = 0;
	overlay->DeReference(&iUnknown);
	LPSAFEARRAY path = 0;
	HRESULT ret = pOverlays->GetPath((IOverlay*)iUnknown, &path);
	if (!SUCCEEDED(ret))
	{
		path = ::SafeArrayCreateVector(VT_VARIANT, 0, 0);
	}
	*pVal = path;
	pOverlays->Release();
		
	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::put_OverlayPath(LPSAFEARRAY newVal)
{
	// Try and find the overlay
	IOverlayHome *pOverlays;
	model->get_Overlays(&pOverlays);

	IOverlay *pOverlay = 0;
	pOverlays->Find(newVal, &pOverlay);
	put_Overlay(pOverlay);
	if (pOverlay)
		pOverlay->Release();

	pOverlays->Release();

	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::get_Overlay(IOverlay **pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IUnknown *iUnknown = 0;
	overlay->DeReference(&iUnknown);
	*pVal = (IOverlay*)iUnknown;

	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::put_Overlay(IOverlay *pVal)
{
	overlay->AssignReference(pVal);

	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::get_PassThrough(VARIANT_BOOL *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = passThrough;

	return S_OK;
}

template <class B>
STDMETHODIMP ControllerImpl<B>::put_PassThrough(VARIANT_BOOL pVal)
{
	passThrough = pVal;

	return S_OK;
}