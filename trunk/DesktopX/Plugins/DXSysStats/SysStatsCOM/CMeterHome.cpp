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

// CMeterHome.cpp : Implementation of CSysStatsCOMApp and DLL registration.

#include "stdafx.h"
#include <comdef.h>
#include "SysStatsCOM.h"
#include "CMeterHome.h"
#include "CMeterFactory.h"
#include "IINIPersister.h"

/////////////////////////////////////////////////////////////////////////////
//

STDMETHODIMP CMeterHome::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IMeterHome,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

const char* CMeterHome::UNKNOWN = "unknown";
const char* CMeterHome::METER_COUNT_PROPERTY = "MeterCount";
const char* CMeterHome::METER_INDEX_PROPERTY = "Meter";
const char* CMeterHome::METER_PREFIX = "meter";
const char* CMeterHome::METER_TYPE_PROPERTY = "Type";

static _bstr_t EMPTY_BSTR = "";

CMeterHome::CMeterHome() : timerService(0), model(0)
{
}

CMeterHome::~CMeterHome()
{
//	ClearAndDestroy();

#ifdef CIRCULAR_REFERENCES_OK
	if (model)
		model->Release();
#endif

	if (timerService)
		timerService->Release();
}

STDMETHODIMP CMeterHome::Add(/*[in]*/ IMeter *pMeter)
{
	if (timerService)
		timerService->SetTimerClient(pMeter);

	if (model)
		pMeter->put_Model(model);

	return ArrayPtrVariant<IMeter, IMeterHome>::Add(pMeter);
}

STDMETHODIMP CMeterHome::Remove(/*[in]*/ VARIANT index)
{
	IMeter *pMeter;
	Item(index, &pMeter);

	if (timerService)
		timerService->RemoveTimerClient(pMeter);

	HRESULT hr = ArrayPtrVariant<IMeter, IMeterHome>::Remove(index);
	pMeter->Release();
	return hr;
}

STDMETHODIMP CMeterHome::Item(VARIANT index, IMeter **pRet)
{
	BSTR bstr;
	short count = 0;

	if (SUCCEEDED(SysStatsUtils::VariantToBSTR(&index, &bstr)))
	{
		Count(&count);
		for (int i=0; i < count; i++)
		{
			IMeter *pMeter;
			ArrayPtrVariant<IMeter, IMeterHome>::Item(CComVariant(i), &pMeter);
			BSTR name;
			pMeter->get_Name(&name);
			if (wcscmp(name, bstr) == 0)
			{
				*pRet = pMeter;
				::SysFreeString(name);
				return S_OK;
			}
			::SysFreeString(name);
			pMeter->Release();
		}

		return S_OK;
	}
	else
		return ArrayPtrVariant<IMeter, IMeterHome>::Item(index, pRet);

	return E_INVALIDARG;
}

STDMETHODIMP CMeterHome::put_TimerService(ICOMTimerService *timerService)
{
	if (this->timerService == timerService)
		return S_OK;

	short count = 0;
	Count(&count);

	for (int i=0; i<count; i++)
	{
		IMeter *pItem;
		Item(CComVariant(i), &pItem);

		if (timerService)
			timerService->SetTimerClient(pItem);

		if (this->timerService)
			this->timerService->RemoveTimerClient(pItem);

		pItem->Release();
	}

	if (this->timerService)
		this->timerService->Release();

	this->timerService = timerService;
	if (timerService)
		timerService->AddRef();

	return S_OK;
}

STDMETHODIMP CMeterHome::get_TimerService(ICOMTimerService **pRet)
{
	if (timerService)
		timerService->AddRef();

	*pRet = timerService;

	return S_OK;
}


STDMETHODIMP CMeterHome::Update(long *dirty)
{
	short count = 0;
	Count(&count);
	long d = false;
	IMeter *pItem;

	for (int i=0; i<count; i++)
	{
		Item(CComVariant(i), &pItem);
		pItem->Update(&d);
		*dirty = *dirty || d;
		pItem->Release();
	}

	return S_OK;
}

STDMETHODIMP CMeterHome::get_Model(IClientModel **pVal)
{
	*pVal = model;

	model->AddRef();

	return S_OK;
}

STDMETHODIMP CMeterHome::put_Model(IClientModel *newVal)
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
		IMeter *pItem;
		Item(CComVariant(i), &pItem);
		pItem->put_Model(newVal);
		pItem->Release();
	}

	return S_OK;
}

STDMETHODIMP CMeterHome::get_Meters(LPSAFEARRAY *pVal)
{
	short count;
	Count(&count);
	SAFEARRAY *pArray = ::SafeArrayCreateVector(VT_DISPATCH, 0, count);

	IMeter *pMeter;
	long indices[1];
	for (int i=0; i<count; i++)
	{
		indices[0] = i;
		Item(CComVariant(i), &pMeter);
		::SafeArrayPutElement(pArray, indices, pMeter);
		pMeter->Release();
	}

	*pVal = pArray;

	return S_OK;
}

STDMETHODIMP CMeterHome::put_Meters(LPSAFEARRAY newVal)
{
	ClearAndDestroy();
	long lb = 0;
	long ub = 0;
	if( !(newVal->fFeatures & FADF_DISPATCH) )
		return E_INVALIDARG;

	HRESULT hr = ::SafeArrayGetUBound(newVal, 1, &ub);
	hr = ::SafeArrayGetLBound(newVal, 1, &lb);

	IDispatch *pDisp;
	IMeter *pMeter;
	long indices[1];
	for (int i=lb; i<=ub; i++)
	{
		indices[0] = i;
		pDisp = 0;
		::SafeArrayGetElement(newVal, indices, &pDisp);
		if (pDisp)
		{
			if (SUCCEEDED(pDisp->QueryInterface(IID_IMeter, (void**)&pMeter)))
			{
				AtlTrace("Added meter to MeterHome\n");
				Add(pMeter);
				pMeter->Release();
			}

			pDisp->Release();
		}
	}

	return S_OK;
}