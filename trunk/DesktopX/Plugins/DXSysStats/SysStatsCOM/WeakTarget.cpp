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

// WeakTarget.cpp : Implementation of CWeakTarget
#include "stdafx.h"
#include "SysStatsCOM.h"
#include "WeakTarget.h"

/////////////////////////////////////////////////////////////////////////////
// CWeakTarget

STDMETHODIMP CWeakTarget::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IWeakTarget
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CWeakTarget::CWeakTarget()
{
}

CWeakTarget::~CWeakTarget()
{
	AtlTrace("CWeakTarget destroyed\n");
}

// Clear all the weak references that refer to us.
void CWeakTarget::FinalRelease()
{
	// Used to be in FinalRelease - but that never gets called!
	short count = referers.count();

	for (int i=0; i<count; i++)
	{
		referers[i]->ClearReference();
	}
}

// Called by WeakReference when a reference to us is being set.
STDMETHODIMP CWeakTarget::Attach(IWeakReference *pVal)
{
	referers.add(pVal);

	return S_OK;
}

// Called by WeakReference when it is being cleared.
STDMETHODIMP CWeakTarget::Detach(IWeakReference *pVal)
{
	referers.remove(referers.find(pVal));

	return S_OK;
}
