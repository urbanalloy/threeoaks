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

// Label.cpp : Implementation of CLabel
#include "stdafx.h"
#include "SysStatsCOM.h"
#include "Label.h"

/////////////////////////////////////////////////////////////////////////////
// CLabel

STDMETHODIMP CLabel::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ILabel
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CLabel::CLabel() :
	meter(0),
	model(0),
	format("SysStats"),
	selector("")
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakReference", &clsid);
	::CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, /* IID_IWeakReference */ __uuidof(IWeakReference), (void**)&meter);
}

CLabel::~CLabel()
{
	if (meter)
		meter->Release();
}

STDMETHODIMP CLabel::get_meter(IMeter **pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IUnknown *iUnknown = 0;
	meter->DeReference(&iUnknown);
	*pVal = (IMeter*)iUnknown;

	return S_OK;
}

STDMETHODIMP CLabel::put_meter(IMeter *newVal)
{
	meter->AssignReference(newVal);

	return S_OK;
}

STDMETHODIMP CLabel::get_MeterIndex(long *pVal)
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

STDMETHODIMP CLabel::put_MeterIndex(long newVal)
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

STDMETHODIMP CLabel::get_Model(IClientModel **pVal)
{
	if (pVal == NULL)
		return E_POINTER;
	
	if (model)
		model->AddRef();

	*pVal = model;

	return S_OK;
}

STDMETHODIMP CLabel::put_Model(IClientModel *newVal)
{
	if (model == newVal)
		return S_OK;

	model = newVal;

	return S_OK;
}

STDMETHODIMP CLabel::get_Format(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = format.copy();

	return S_OK;
}

STDMETHODIMP CLabel::put_Format(BSTR newVal)
{
	format = newVal;

	return S_OK;
}

STDMETHODIMP CLabel::get_selector(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = selector.copy();

	return S_OK;
}

STDMETHODIMP CLabel::put_selector(BSTR newVal)
{
	selector = newVal;

	return S_OK;
}

STDMETHODIMP CLabel::GetLabel(BSTR *retVal)
{
	IMeter *pMeter = NULL;
	get_meter(&pMeter);

	if (pMeter)
		return pMeter->GetAsString(format, selector, retVal);
	else
		return get_Format(retVal);
}
