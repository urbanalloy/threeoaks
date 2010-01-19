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

// SimpleOverlayImpl.cpp: implementation of the SimpleOverlayImpl class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "HDCImage.h"
#include "SimpleOverlayImpl2.cpp"
#include "CompositeOverlayDialog.h"
#include "CompositeOverlayImpl.h"
#include "COverlayFactory.h"
#include "IINIPersister.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template <class B>
CompositeOverlayImpl<B>::CompositeOverlayImpl()
{
}

template <class B>
CompositeOverlayImpl<B>::~CompositeOverlayImpl()
{
}

template <class B>
STDMETHODIMP CompositeOverlayImpl<B>::clone(IOverlay **pRet)
{
	SimpleOverlayImpl2<B>::clone(pRet);

	B **pB = (B**)pRet;

	short count = 0;
	Count(&count);
	for (int i=0; i<count; i++)
	{
		IOverlay *pItem;
		Item(CComVariant(i), &pItem);
		IOverlay *pClone;
		pItem->clone(&pClone);
		(*pB)->Add(pClone);
		pItem->Release();
	}

	return S_OK;
}

template <class B>
STDMETHODIMP CompositeOverlayImpl<B>::HitTest(/*[in]*/ short X, /*[in]*/ short Y, /*[out, retval]*/ VARIANT_BOOL *retVal)
{
	if (retVal == NULL)
		return E_POINTER;

	*retVal = false;

	short count = 0;
	Count(&count);

	for (int i=count-1; i>=0 && !(*retVal); i--)
	{
		IOverlay *pItem;
		Item(CComVariant(i), &pItem);
		VARIANT_BOOL vis;
		pItem->get_Visible(&vis);
		if (vis)
		{
			ISimpleOverlay2 *pISO2 = NULL;
			HRESULT hr = pItem->QueryInterface(__uuidof(ISimpleOverlay2), (void **)&pISO2);
			if (SUCCEEDED(hr))
			{
				pISO2->HitTest(X, Y, retVal);
				pISO2->Release();
			}
		}
		pItem->Release();
	}

	return S_OK;
}

template <class B>
STDMETHODIMP CompositeOverlayImpl<B>::ModernRender(long _hdc)
{
	short count = 0;
	Count(&count);

	for (int i=count-1; i>=0; i--)
	{
		IOverlay *pItem;
		Item(CComVariant(i), &pItem);
		VARIANT_BOOL vis;
		pItem->get_Visible(&vis);
		if (vis)
		{
			ISimpleOverlay2 *pISO2 = NULL;
			HRESULT hr = pItem->QueryInterface(__uuidof(ISimpleOverlay2), (void **)&pISO2);
			if (SUCCEEDED(hr))
			{
				IOverlay *effectOverlay;
				pISO2->get_EffectOverlay(&effectOverlay);
				long effect;
				pISO2->get_EffectOverlayEffect(&effect);

				if (!effectOverlay)
				{
					pItem->Render(_hdc);
				}
				else
				{
					// prevent recursion
					pISO2->put_EffectOverlay(NULL);

					// Create a private HDC to draw into so that we can mask it.
					long width = 128;
					long height = 128;
					model->get_Width(&width);
					model->get_Height(&height);
					HDCImage hdc((HDC)_hdc, width, height);

					// Render the item
					pItem->Render(hdc);

					// Now mask it
					HDCImage hdcMask((HDC)hdc, width, height);
					effectOverlay->Render(hdcMask);
					switch(effect)
					{
					case 0:
						hdc.MaskOut(hdcMask);
						break;

					case 1:
						hdc.MaskIn(hdcMask);
						break;
					}

					// Copy private HDC into the passed HDC
					Graphics g((HDC)_hdc);
					g.DrawImage(
					   &hdc.GetImage(), 
					   Rect(0, 0, width, height),	// Destination rectangle
					   0,						// Source rectangle X 
					   0,						// Source rectangle Y
					   width,		// Source rectangle width
					   height,		// Source rectangle height
					   UnitPixel);

					// Reset recursion avoider
					pISO2->put_EffectOverlay(effectOverlay);
				}

				pISO2->Release();
			}
			else
				pItem->Render(_hdc);
		}
		pItem->Release();
	}

	return S_OK;
}

template <class B>
STDMETHODIMP CompositeOverlayImpl<B>::Render(long _hdc)
{
	short count = 0;
	Count(&count);

	REAL el[6];
	CumulativeTransform(el);
	Matrix matrix(el[0], el[1], el[2], el[3], el[4], el[5]);

	// Firstly, if us or our parents don't transform the coordinate space, life is simple.
	if (matrix.IsIdentity())
	{
		ModernRender(_hdc);
	}
	else
	{
		// Check if all children are of type ISimpleOverlay2
		bool iso2 = true;
		for (int i=count-1; i>=0; i--)
		{
			IOverlay *pItem;
			Item(CComVariant(i), &pItem);
			VARIANT_BOOL vis;
			pItem->get_Visible(&vis);
			if (vis)
			{
				ISimpleOverlay2 *pISO2 = NULL;
				HRESULT hr = pItem->QueryInterface(__uuidof(ISimpleOverlay2), (void **)&pISO2);
				if (!SUCCEEDED(hr))
					iso2 = false;
				else
					pISO2->Release();
			}
			pItem->Release();
		}

		// If all children are of type ISimpleOverlay2, life is also simple.
		if (iso2)
		{
			ModernRender(_hdc);
		}
		else
		{
			// Otherwise, draw into a pre-transformed hdc.
			long width = 128;
			long height = 128;
			model->get_Width(&width);
			model->get_Height(&height);

			HDCImage hdc((HDC)_hdc, width, height);

			for (int i=count-1; i>=0; i--)
			{
				IOverlay *pItem;
				Item(CComVariant(i), &pItem);
				VARIANT_BOOL vis;
				pItem->get_Visible(&vis);
				if (vis)
					pItem->Render(hdc);
				pItem->Release();
			}

			Graphics g((HDC)_hdc);
			g.SetInterpolationMode(InterpolationModeHighQuality);
			g.SetSmoothingMode(SmoothingModeAntiAlias);
			g.MultiplyTransform(&matrix);
			g.DrawImage(&hdc.GetImage(), 0, 0);
		}
	}

	return S_OK;
}

template <class B>
STDMETHODIMP CompositeOverlayImpl<B>::Add(/*[in]*/ IOverlay *pOverlay)
{
	if (model)
		pOverlay->put_Model(model);

	ISimpleOverlay2 *pISO2 = 0;
	HRESULT hr = pOverlay->QueryInterface(__uuidof(ISimpleOverlay2), (void **)&pISO2);
	if (SUCCEEDED(hr))
	{
		pISO2->put_Parent(this);
		pISO2->Release();
	}

	return ArrayPtrVariant<IOverlay, SimpleOverlayImpl2<B> >::Add(pOverlay);
}

template <class B>
STDMETHODIMP CompositeOverlayImpl<B>::AddAt(/*[in]*/ IOverlay *pOverlay, /*[in]*/ VARIANT index)
{
	if (model)
		pOverlay->put_Model(model);

	ISimpleOverlay2 *pISO2 = 0;
	HRESULT hr = pOverlay->QueryInterface(__uuidof(ISimpleOverlay2), (void **)&pISO2);
	if (SUCCEEDED(hr))
	{
		pISO2->put_Parent(this);
		pISO2->Release();
	}

	return ArrayPtrVariant<IOverlay, SimpleOverlayImpl2<B> >::AddAt(pOverlay, index);
}

template <class B>
STDMETHODIMP CompositeOverlayImpl<B>::Remove(/*[in]*/ VARIANT index)
{
	IOverlay *pOverlay;
	Item(index, &pOverlay);

	HRESULT hr = ArrayPtrVariant<IOverlay, SimpleOverlayImpl2<B> >::Remove(index);
	pOverlay->Release();
	return hr;
}

template <class B>
STDMETHODIMP CompositeOverlayImpl<B>::Item(VARIANT index, IOverlay **pRet)
{
	BSTR bstr;
	short count = 0;

	if (SUCCEEDED(SysStatsUtils::VariantToBSTR(&index, &bstr)))
	{
		Count(&count);
		for (int i=0; i < count; i++)
		{
			IOverlay *pOverlay;
			ArrayPtrVariant<IOverlay, SimpleOverlayImpl2<B> >::Item(CComVariant(i), &pOverlay);
			BSTR name;
			pOverlay->get_Name(&name);
			if (wcscmp(name, bstr) == 0)
			{
				*pRet = pOverlay;
				::SysFreeString(name);
				return S_OK;
			}
			::SysFreeString(name);
			pOverlay->Release();
		}

		return S_OK;
	}
	else
		return ArrayPtrVariant<IOverlay, SimpleOverlayImpl2<B> >::Item(index, pRet);

	return E_INVALIDARG;
}

template <class B>
STDMETHODIMP CompositeOverlayImpl<B>::get_Overlays(LPSAFEARRAY *pVal)
{
	short count;
	Count(&count);
	SAFEARRAY *pArray = ::SafeArrayCreateVector(VT_DISPATCH, 0, count);

	IOverlay *pOverlay;
	long indices[1];
	for (int i=0; i<count; i++)
	{
		indices[0] = i;
		Item(CComVariant(i), &pOverlay);
		::SafeArrayPutElement(pArray, indices, pOverlay);
		pOverlay->Release();
	}

	*pVal = pArray;

	return S_OK;
}

template <class B>
STDMETHODIMP CompositeOverlayImpl<B>::put_Overlays(LPSAFEARRAY newVal)
{
	ClearAndDestroy();
	long lb = 0;
	long ub = 0;
	if( !(newVal->fFeatures & FADF_DISPATCH) )
		return E_INVALIDARG;

	HRESULT hr = ::SafeArrayGetUBound(newVal, 1, &ub);
	hr = ::SafeArrayGetLBound(newVal, 1, &lb);

	IDispatch *pDisp;
	IOverlay *pOverlay;
	long indices[1];
	for (int i=lb; i<=ub; i++)
	{
		indices[0] = i;
		pDisp = 0;
		::SafeArrayGetElement(newVal, indices, &pDisp);
		if (pDisp)
		{
			if (SUCCEEDED(pDisp->QueryInterface(__uuidof(IOverlay), (void**)&pOverlay)))
			{
				AtlTrace("Added overlay to composite\n");
				Add(pOverlay);
				pOverlay->Release();
			}
			pDisp->Release();
		}
	}

	return S_OK;
}

template <class B>
STDMETHODIMP CompositeOverlayImpl<B>::put_Model(IClientModel *newVal)
{
	SimpleOverlayImpl2<B>::put_Model(newVal);

	short count = 0;
	Count(&count);

	for (int i=0; i<count; i++)
	{
		IOverlay *pItem;
		Item(CComVariant(i), &pItem);
		pItem->put_Model(newVal);
		pItem->Release();
	}

	return S_OK;
}
