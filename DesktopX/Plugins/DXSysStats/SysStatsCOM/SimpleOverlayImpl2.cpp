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

// SimpleOverlayImpl2.cpp: implementation of the SimpleOverlayImpl2 class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <comdef.h>
#include "SimpleOverlayImpl.cpp"
#include "SimpleOverlayImpl2.h"
#include "SysStatsUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template <class B>
SimpleOverlayImpl2<B>::SimpleOverlayImpl2() :
	parent(NULL),
	effectOverlay(NULL),
	effectOverlayEffect(0),
	cachedPath(NULL)
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakReference", &clsid);
	::CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, /* IID_IWeakReference */ __uuidof(IWeakReference), (void**)&effectOverlay);
}

template <class B>
SimpleOverlayImpl2<B>::~SimpleOverlayImpl2()
{
	if (effectOverlay)
		effectOverlay->Release();

	if (cachedPath)
		::SafeArrayDestroy(cachedPath);
}

template <class B>
STDMETHODIMP SimpleOverlayImpl2<B>::clone(IOverlay **pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	// Clone all the superclass's state
	SimpleOverlayImpl<B>::clone(pRet);

	SimpleOverlayImpl2<B> *o = (SimpleOverlayImpl2<B>*) *pRet;

	o->put_Parent(parent);

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl2<B>::get_Parent(ISimpleOverlay2 **pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = parent;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl2<B>::put_Parent(ISimpleOverlay2 *newVal)
{
	parent = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl2<B>::get_EffectOverlayEffect(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = effectOverlayEffect;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl2<B>::put_EffectOverlayEffect(long newVal)
{
	effectOverlayEffect = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl2<B>::get_EffectOverlay(IOverlay **pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	// Resolve any cached path
	ResolveEffectPath();

	IUnknown *iUnknown = 0;
	effectOverlay->DeReference(&iUnknown);
	*pVal = (IOverlay*)iUnknown;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl2<B>::put_EffectOverlay(IOverlay *pVal)
{
	// Trash any cached path
	if (cachedPath)
	{
		::SafeArrayDestroy(cachedPath);
		cachedPath = NULL;
	}

	effectOverlay->AssignReference(pVal);

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl2<B>::get_EffectOverlayPath(LPSAFEARRAY *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	// Always calculate the path - might have to force a resolution of
	// a cached path fist.
	IOverlay *iOverlay = 0;
	get_EffectOverlay(&iOverlay);

	IOverlayHome *pOverlays;
	model->get_Overlays(&pOverlays);

	LPSAFEARRAY path = 0;
	HRESULT ret = pOverlays->GetPath(iOverlay, &path);
	if (!SUCCEEDED(ret))
	{
		path = ::SafeArrayCreateVector(VT_VARIANT, 0, 0);
	}
	*pVal = path;
	pOverlays->Release();
		
	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl2<B>::put_EffectOverlayPath(LPSAFEARRAY newVal)
{
	// Just cache the path
	if (cachedPath)
	{
		::SafeArrayDestroy(cachedPath);
	}

	::SafeArrayCopy(newVal, &cachedPath);

	return S_OK;
}

template <class B>
void SimpleOverlayImpl2<B>::ResolveEffectPath()
{
	if (cachedPath)
	{
		// Try and find the overlay
		IOverlayHome *pOverlays;
		model->get_Overlays(&pOverlays);

		IOverlay *pOverlay = 0;
		pOverlays->Find(cachedPath, &pOverlay);
		put_EffectOverlay(pOverlay);
		if (pOverlay)
			pOverlay->Release();

		pOverlays->Release();

		::SafeArrayDestroy(cachedPath);
		cachedPath = NULL;
	}
}

template <class B>
STDMETHODIMP SimpleOverlayImpl2<B>::CumulativeTransform(/*[out, retval]*/ float *elements)
{
	if (elements == NULL)
		return E_POINTER;

	Matrix m;
	if (parent)
	{
		float el[6];
		parent->CumulativeTransform(el);
		Matrix cm(el[0], el[1], el[2], el[3], el[4], el[5]);
		m.Multiply(&cm);
	}

	m.Multiply(&getTransform());
	m.GetElements(elements);
	return S_OK;
}
