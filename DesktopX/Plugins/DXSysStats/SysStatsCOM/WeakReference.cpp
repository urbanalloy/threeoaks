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

// WeakReference.cpp : Implementation of CWeakReference
#include "stdafx.h"
#include "SysStatsCOM.h"
#include "WeakReference.h"
#include "WeakTarget.h"

/////////////////////////////////////////////////////////////////////////////
// CWeakReference

STDMETHODIMP CWeakReference::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IWeakReference
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CWeakReference::CWeakReference() : ref(0)
{
}

// Make sure that the weak target doesn't try to clear us when it is destroyed.
CWeakReference::~CWeakReference()
{
	IWeakTarget *pTarget = 0;

	if (ref)
	{
		if (ref->QueryInterface(IID_IWeakTarget, (void**)&pTarget) == S_OK)
		{
			// pTarget->Detach() should *not* call us back.
			pTarget->Detach(this);
			pTarget->Release();
		}
		else
			ref->Release();	// Backwards compatibility
	}

}

/*
 * Called by IWeakTarget to clear the reference when it is destroyed.
 */
STDMETHODIMP CWeakReference::ClearReference()
{
	ref = 0;

	return S_OK;
}

/*
 * Called by IWeakTarget to set the reference.
 *
 * Actually should never happen.
 */
STDMETHODIMP CWeakReference::SetReference(IUnknown *pVal)
{
	ref = pVal;

	return S_OK;
}

/*
 * Called to get the object being referred to.
 */
STDMETHODIMP CWeakReference::DeReference(IUnknown **pVal)
{
	*pVal = ref;

	return S_OK;
}

/*
 * Called to establish a weak reference to the passed object.
 * The passed object must implement IWeakTarget.
 *
 * pVal can be null to clear the existing reference.
 */
STDMETHODIMP CWeakReference::AssignReference(IUnknown *pVal)
{
	if (pVal == ref)
		return S_OK;	// No change.

	IWeakTarget *pTarget = 0;

	// Release existing reference
	if (ref)
	{
		if (ref->QueryInterface(IID_IWeakTarget, (void**)&pTarget) == S_OK)
		{
			// Detach() should *not* call us back.
			pTarget->Detach(this);
			pTarget->Release();
		}
		else
			ref->Release();	// Backwards compatibility
	}

	ref = pVal;	// We do this whether or not pVal implements IWeakTarget

	if (ref)
	{
		if (ref->QueryInterface(IID_IWeakTarget, (void**)&pTarget) == S_OK)
		{
			// Attach should *not* call us back.
			pTarget->Attach(this);
			pTarget->Release();
		}
		else
			ref->AddRef();	// Backwards compatibility
	}

	return S_OK;
}
