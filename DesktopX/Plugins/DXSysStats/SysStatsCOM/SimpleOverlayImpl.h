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

// SimpleOverlayImpl.h: interface for the Overlay template.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OverlayImpl_H__EDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_)
#define AFX_OverlayImpl_H__EDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <GdiplusH.h>
#include <comutil.h>
#include "OverlayImpl.h"

struct IObserver;
struct IMeterHome;
struct IOverlay;
struct IMeter;

template <class B>
class SimpleOverlayImpl : public OverlayImpl<B>
{
public:
	SimpleOverlayImpl();
	virtual ~SimpleOverlayImpl();

public:
	STDMETHOD(get_Type)(/*[out, retval]*/ BSTR *retVal) = 0;
	STDMETHOD(clone)(/*[out, retval]*/ IOverlay **pRet);
	STDMETHOD(get_Rotation)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Rotation)(/*[in]*/ long newVal);
	STDMETHOD(get_Y)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Y)(/*[in]*/ long newVal);
	STDMETHOD(get_X)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_X)(/*[in]*/ long newVal);
	STDMETHOD(get_CenterY)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_CenterY)(/*[in]*/ long newVal);
	STDMETHOD(get_CenterX)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_CenterX)(/*[in]*/ long newVal);
	STDMETHOD(get_HAlign)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_HAlign)(/*[in]*/ long newVal);
	STDMETHOD(get_VAlign)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_VAlign)(/*[in]*/ long newVal);
	STDMETHOD(get_HScale)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_HScale)(/*[in]*/ double newVal);
	STDMETHOD(get_VScale)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_VScale)(/*[in]*/ double newVal);
	STDMETHOD(get_selector)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_selector)(/*[in]*/ BSTR newVal);

protected:
	virtual IOverlay *createInstance() = 0;
	const Matrix& getTransform();

protected:
	long x;
	long y;
	long rotation;
	long hAlign;
	long vAlign;
	long centerX;
	long centerY;
	double hScale;
	double vScale;

	_bstr_t selector;

private:
	Matrix matrix;
};

#endif // !defined(AFX_OverlayImpl_H__EDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_)
