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

// OverlayHome.cpp : Implementation of CSysStatsCOMApp and DLL registration.

#include "stdafx.h"
#include "SysStatsCOM.h"
#include "COverlayHome.h"

/////////////////////////////////////////////////////////////////////////////
//

COverlayHome::~COverlayHome()
{
}

STDMETHODIMP COverlayHome::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IOverlayHome,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP COverlayHome::get_Type(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_CompositeOverlay, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP COverlayHome::Configure(/*[in]*/ IObserver *observer, /*[in]*/ IMeterHome *meters, /*[in]*/ long hWnd)
{
	CompositeOverlayDialog *pDialog = new CompositeOverlayDialog(this, observer);
	pDialog->DoModal((HWND)hWnd);
	delete pDialog;

	return S_OK;
}

// From OverlayImpl<> - used by 'clone'.
IOverlay *COverlayHome::createInstance()
{
	CComObject<COverlayHome> *pRet;
	CComObject<COverlayHome>::CreateInstance(&pRet);

	return pRet;
}

STDMETHODIMP COverlayHome::Render(long _hdc)
{
	CompositeOverlayImpl<IOverlayHome>::Render(_hdc);
	
	IOverlay *effectOverlay;
	get_EffectOverlay(&effectOverlay);
	long effect;
	get_EffectOverlayEffect(&effect);

	if (effectOverlay)
	{
		long width = 128;
		long height = 128;
		model->get_Width(&width);
		model->get_Height(&height);

		HDCImage hdcMask((HDC)_hdc, width, height, PixelFormat32bppPARGB);
		effectOverlay->Render(hdcMask);
		switch(effect)
		{
		case 0:
			CHDC(_hdc).MaskOut(hdcMask);
			break;

		case 1:
			CHDC(_hdc).MaskIn(hdcMask);
			break;
		}
	}

	return S_OK;
}

/*
 * Return overlay indexed by passed array of indices (if any). Reference count
 * of returned overlay is *not* incremented.
 */
STDMETHODIMP COverlayHome::Find(LPSAFEARRAY path, IOverlay **pRet)
{
	HRESULT retVal = E_INVALIDARG;

	IOverlay *pOverlay = 0;

	long ub = 0;
	long lb = 0;

	HRESULT hr = ::SafeArrayGetUBound(path, 1, &ub);
	hr = ::SafeArrayGetLBound(path, 1, &lb);
	if (SUCCEEDED(hr) && lb <= ub)
	{
		hr = QueryInterface(__uuidof(IOverlay), (void**)&pOverlay);

		ICompositeOverlay *pCompositeOverlay = 0;

		long indices[1];

		for (int i=lb; i<=ub && SUCCEEDED(hr) && pOverlay; i++)
		{
			hr = pOverlay->QueryInterface(__uuidof(ICompositeOverlay), (void**)&pCompositeOverlay);
			pOverlay->Release();
			if (SUCCEEDED(hr))
			{
				// pOverlay was composite, index into it.
				indices[0] = i;
				VARIANT vTmp;

				::SafeArrayGetElement(path, indices, &vTmp);
				hr = pCompositeOverlay->Item(vTmp, &pOverlay);
				::VariantClear(&vTmp);
				pCompositeOverlay->Release();
			}
		}

		if(SUCCEEDED(hr))
		{
			*pRet = pOverlay;

			retVal = S_OK;
		}
	}

	return retVal;
}

HRESULT COverlayHome::DepthFirstSearch(ICompositeOverlay *coll, IOverlay *target, LPSAFEARRAY *pRet, int depth)
{
	short max;
	long count[1];
	count[0] = depth-1;
	coll->Count(&max);
	for (int i=0; i<max; i++)
	{
		IOverlay *pOverlay;
		coll->Item(CComVariant(i), &pOverlay);
		if (target == pOverlay)
		{
			// Allocate array and fill in this index
			*pRet = ::SafeArrayCreateVector(VT_VARIANT, 0, count[0]+1);
			VARIANT *pVar = new VARIANT;
			::VariantInit(pVar);
			pVar->vt = VT_INT;
			pVar->intVal = i;
			::SafeArrayPutElement(*pRet, count, pVar);
			delete pVar;

			pOverlay->Release();
			return S_OK;
		}
		else
		{
			ICompositeOverlay *pCO = NULL;
			HRESULT hr = pOverlay->QueryInterface(IID_ICompositeOverlay, (void **)&pCO);
			if (SUCCEEDED(hr))
			{
				pCO->Release();
				HRESULT ret = DepthFirstSearch((ICompositeOverlay*)pOverlay, target, pRet, depth+1);
				if (SUCCEEDED(ret))
				{
					//Fill in this index
					VARIANT *pVar = new VARIANT;
					::VariantInit(pVar);
					pVar->vt = VT_INT;
					pVar->intVal = i;
					::SafeArrayPutElement(*pRet, count, pVar);
					delete pVar;
					pOverlay->Release();
					return ret;
				}
			}
		}

		pOverlay->Release();
	}

	return E_INVALIDARG;
}

STDMETHODIMP COverlayHome::GetPath(IOverlay *pOverlay, LPSAFEARRAY *pRet)
{
	// Make a depth-first search
	return DepthFirstSearch(this, pOverlay, pRet, 1);
}
