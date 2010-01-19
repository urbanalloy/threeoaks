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

// MeterImpl.cpp: implementation of the MeterImpl class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <comdef.h>
#include "MeterImpl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template <class B>
MeterImpl<B>::MeterImpl() : name(""), model(0)
{

}

template <class B>
MeterImpl<B>::~MeterImpl()
{
	AtlTrace("Meter %s destroyed\n", (char*)_bstr_t(name));
#ifdef CIRCULAR_REFERENCES_OK
	if (model)
		model->Release();
#endif
}

template <class B>
STDMETHODIMP MeterImpl<B>::Update(long *dirty)
{
	*dirty = false;

	// Default behavior of 'do nothing' is OK.
	return S_OK;
}

template <class B>
STDMETHODIMP MeterImpl<B>::get_Name(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = name.copy();
		
	return S_OK;
}

template <class B>
STDMETHODIMP MeterImpl<B>::put_Name(BSTR newVal)
{
	name = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP MeterImpl<B>::Configure(/*[in]*/IObserver *observer, /*[in]*/HWND hWnd)
{
	// Default behavior of do nothing is OK.
	return S_OK;
}

template <class B>
STDMETHODIMP MeterImpl<B>::TimerFired()
{
	long dirty = false;

	Update(&dirty);
	if (dirty && model)
		model->put_dirty(dirty);

	return S_OK;
}

template <class B>
STDMETHODIMP MeterImpl<B>::get_Model(IClientModel **pVal)
{
	if (pVal == NULL)
		return E_POINTER;
	
	if (model)
		model->AddRef();

	*pVal = model;

	return S_OK;
}

template <class B>
STDMETHODIMP MeterImpl<B>::put_Model(IClientModel *newVal)
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
