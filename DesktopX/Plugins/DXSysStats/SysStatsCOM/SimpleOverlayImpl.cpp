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

#include <comdef.h>
#include "OverlayImpl.cpp"
#include "SimpleOverlayImpl.h"
#include "SysStatsUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template <class B>
SimpleOverlayImpl<B>::SimpleOverlayImpl() :
	x(0),
	y(0),
	centerX(0),
	centerY(0),
	hAlign(0),
	vAlign(0),
	hScale(1.0),
	vScale(1.0),
	rotation(0),
	selector("")
{

}

template <class B>
SimpleOverlayImpl<B>::~SimpleOverlayImpl()
{
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::clone(IOverlay **pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	// Clone all the superclass's state
	OverlayImpl<B>::clone(pRet);

	SimpleOverlayImpl<B> *o = (SimpleOverlayImpl<B>*) *pRet;

	o->put_selector(selector);
	o->put_X(x);
	o->put_Y(y);
	o->put_CenterX(centerX);
	o->put_CenterY(centerY);
	o->put_HAlign(hAlign);
	o->put_VAlign(vAlign);
	o->put_HScale(hScale);
	o->put_VScale(vScale);
	o->put_Rotation(rotation);

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::get_X(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = x;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::put_X(long newVal)
{
	x = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::get_Y(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = y;
		
	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::put_Y(long newVal)
{
	y = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::get_CenterX(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = centerX;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::put_CenterX(long newVal)
{
	centerX = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::get_CenterY(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = centerY;
		
	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::put_CenterY(long newVal)
{
	centerY = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::get_HAlign(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = hAlign;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::put_HAlign(long newVal)
{
	hAlign = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::get_VAlign(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = vAlign;
		
	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::put_VAlign(long newVal)
{
	vAlign = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::get_HScale(double *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = hScale;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::put_HScale(double newVal)
{
	hScale = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::get_VScale(double *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = vScale;
		
	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::put_VScale(double newVal)
{
	vScale = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::get_Rotation(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = rotation;
		
	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::put_Rotation(long newVal)
{
	rotation = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::get_selector(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = selector.copy();

	return S_OK;
}

template <class B>
STDMETHODIMP SimpleOverlayImpl<B>::put_selector(BSTR newVal)
{
	selector = newVal;

	return S_OK;
}

template <class B>
const Matrix &SimpleOverlayImpl<B>::getTransform()
{
	matrix.Reset();

	matrix.Translate((float)(x), (float)(y));
	matrix.Rotate((float)rotation);
	matrix.Scale((float)hScale, (float)vScale);
	matrix.Translate((float)(-centerX), (float)(-centerY));

	return matrix;
}