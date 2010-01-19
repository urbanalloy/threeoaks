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

// OverlayImpl.cpp: implementation of the OverlayImpl class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <comdef.h>
#include "OverlayImpl.h"
#include "SysStatsUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template <class B>
OverlayImpl<B>::OverlayImpl() :
	name(""),
	meter(0),
	model(0),
	mode(0),
	visible(true)
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakReference", &clsid);
	::CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, /* IID_IWeakReference */ __uuidof(IWeakReference), (void**)&meter);
}

template <class B>
OverlayImpl<B>::~OverlayImpl()
{
	AtlTrace("Overlay %s destroyed\n", (char*)_bstr_t(name));

	if (meter)
		meter->Release();
}

template <class B>
STDMETHODIMP OverlayImpl<B>::Configure(/*[in]*/ IObserver *observer, /*[in]*/ IMeterHome *meters, /*[in]*/ long hWnd)
{
	// Its OK to do nothing

	return S_OK;
}

template <class B>
STDMETHODIMP OverlayImpl<B>::clone(IOverlay **pRet)
{
	OverlayImpl<B> *pBase = (OverlayImpl<B>*) createInstance();
	pBase->AddRef();
	*pRet = pBase;

	pBase->put_Name(name);
	if (model)
		pBase->put_Model(model);

	IUnknown *iUnknown = 0;
	meter->DeReference(&iUnknown);
	pBase->put_meter((IMeter*)iUnknown);

	pBase->put_Mode(mode);
	pBase->put_Visible(visible);

	return S_OK;
}

template <class B>
STDMETHODIMP OverlayImpl<B>::get_Mode(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = mode;

	return S_OK;
}

template <class B>
STDMETHODIMP OverlayImpl<B>::put_Mode(long newVal)
{
	mode = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP OverlayImpl<B>::get_Name(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = name.copy();

	return S_OK;
}

template <class B>
STDMETHODIMP OverlayImpl<B>::put_Name(BSTR newVal)
{
	name = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP OverlayImpl<B>::get_meter(IMeter **pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IUnknown *iUnknown = 0;
	meter->DeReference(&iUnknown);
	*pVal = (IMeter*)iUnknown;

	return S_OK;
}

template <class B>
STDMETHODIMP OverlayImpl<B>::put_meter(IMeter *newVal)
{
	meter->AssignReference(newVal);

	return S_OK;
}

template <class B>
STDMETHODIMP OverlayImpl<B>::get_Model(IClientModel **pVal)
{
	if (pVal == NULL)
		return E_POINTER;
	
	if (model)
		model->AddRef();

	*pVal = model;

	return S_OK;
}

template <class B>
STDMETHODIMP OverlayImpl<B>::put_Model(IClientModel *newVal)
{
	if (model == newVal)
		return S_OK;

	model = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP OverlayImpl<B>::get_MeterIndex(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IMeterHome *pMeters;
	model->get_Meters(&pMeters);
	short index = -1;
	IUnknown *iUnknown = 0;
	meter->DeReference(&iUnknown);
	pMeters->GetIndex((IMeter*)iUnknown, &index);
	*pVal = index;
	pMeters->Release();
		
	return S_OK;
}

template <class B>
STDMETHODIMP OverlayImpl<B>::put_MeterIndex(long newVal)
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
STDMETHODIMP OverlayImpl<B>::get_Visible(VARIANT_BOOL *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = visible;

	return S_OK;
}

template <class B>
STDMETHODIMP OverlayImpl<B>::put_Visible(VARIANT_BOOL newVal)
{
	visible = newVal;

	return S_OK;
}


