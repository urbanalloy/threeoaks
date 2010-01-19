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

// CompositeOverlayImpl.h: interface for the CompositeOverlay template.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CompositeOverlayImpl_H__EDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_)
#define AFX_CompositeOverlayImpl_H__EDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SimpleOverlayImpl2.h"
#include "ArrayPtrVariant.cpp"
struct IOverlay;

template <class B>
class CompositeOverlayImpl : public ArrayPtrVariant<IOverlay, SimpleOverlayImpl2<B> >
{
public:
	CompositeOverlayImpl();
	virtual ~CompositeOverlayImpl();

	STDMETHOD(Remove)(/*[in]*/ VARIANT index);
	STDMETHOD(Add)(/*[in]*/ IOverlay *pOverlay);
	STDMETHOD(AddAt)(/*[in]*/ IOverlay *pOverlay, /*in*/ VARIANT index);
	STDMETHOD(Item)(VARIANT, IOverlay**);

	// IOverlay
public:
	STDMETHOD(clone)(/*[out, retval]*/ IOverlay **pRet);
	STDMETHOD(Render)(/*[in]*/ long hdc);
	STDMETHOD(get_Overlays)(/*[out, retval]*/ LPSAFEARRAY *pVal);
	STDMETHOD(put_Overlays)(/*[in]*/ LPSAFEARRAY newVal);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel *newVal);

// ISimpleOverlay2
	STDMETHOD(HitTest)(/*[in]*/ short X, /*[in]*/ short Y, /*[out, retval]*/ VARIANT_BOOL *retVal);

protected:
	STDMETHOD(ModernRender)(long _hdc);

};

#endif // !defined(AFX_CompositeOverlayImpl_H__EDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_)
