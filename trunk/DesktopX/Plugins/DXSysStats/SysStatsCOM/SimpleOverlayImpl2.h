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

// SimpleOverlayImpl2.h: interface for the Overlay template.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OverlayImpl2_H__EDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_)
#define AFX_OverlayImpl2_H__EDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <comutil.h>
#include "SimpleOverlayImpl.h"
#include <Gdiplus.h>
using namespace Gdiplus;

template <class B>
class SimpleOverlayImpl2 : public SimpleOverlayImpl<B>
{
public:
	SimpleOverlayImpl2();
	virtual ~SimpleOverlayImpl2();

public:
	STDMETHOD(clone)(/*[out, retval]*/ IOverlay **pRet);
	STDMETHOD(get_Parent)(/*[out, retval]*/ ISimpleOverlay2* *pVal);
	STDMETHOD(put_Parent)(/*[in]*/ ISimpleOverlay2* newVal);
	STDMETHOD(HitTest)(short X, short Y, VARIANT_BOOL *retVal) = 0;
	STDMETHOD(CumulativeTransform)(/*[out, retval]*/ float *elements);
	STDMETHOD(get_EffectOverlayPath)(/*[out, retval]*/ LPSAFEARRAY *pVal);
	STDMETHOD(put_EffectOverlayPath)(/*[in]*/ LPSAFEARRAY pVal);
	STDMETHOD(get_EffectOverlay)(/*[out, retval]*/ IOverlay* *pVal);
	STDMETHOD(put_EffectOverlay)(/*[in]*/ IOverlay* pVal);
	STDMETHOD(get_EffectOverlayEffect)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_EffectOverlayEffect)(/*[in]*/ long pVal);

protected:
	void ResolveEffectPath();

private:
	ISimpleOverlay2 *parent;
	IWeakReference *effectOverlay;
	LPSAFEARRAY cachedPath;
	long effectOverlayEffect;
};

#endif // !defined(AFX_OverlayImpl2_H__EDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_)
