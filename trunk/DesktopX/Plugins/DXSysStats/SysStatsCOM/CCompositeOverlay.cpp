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

// CCompositeOverlay.cpp : Implementation of CCompositeOverlay
#include "stdafx.h"
#include "SysStatsCOM.h"
#include "CCompositeOverlay.h"

	/////////////////////////////////////////////////////////////////////////////
// CCompositeOverlay

STDMETHODIMP CCompositeOverlay::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ICompositeOverlay
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CCompositeOverlay::CCompositeOverlay() : pWeakTarget(0)
{
}

HRESULT CCompositeOverlay::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CCompositeOverlay::FinalRelease()
{
	pWeakTarget->Release();
}

STDMETHODIMP CCompositeOverlay::get_Type(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_CompositeOverlay, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CCompositeOverlay::Configure(/*[in]*/ IObserver *observer, /*[in]*/ IMeterHome *meters, /*[in]*/ long hWnd)
{
	CompositeOverlayDialog *pDialog = new CompositeOverlayDialog(this, observer);
	pDialog->DoModal((HWND)hWnd);
	delete pDialog;

	return S_OK;
}

// From OverlayImpl<> - used by 'clone'.
IOverlay *CCompositeOverlay::createInstance()
{
	return COverlayFactory::createInstance("CompositeOverlay Class", model);
}
