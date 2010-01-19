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

// OverlayHome.h: Definition of the COverlayHome class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OVERLAYHOME_H__EE4999AE_A771_43C1_A6A1_9F44C83AFACF__INCLUDED_)
#define AFX_OVERLAYHOME_H__EE4999AE_A771_43C1_A6A1_9F44C83AFACF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <GdiplusH.h>

#include "CompositeOverlayImpl.cpp"
#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// COverlayHome

class COverlayHome : 
	public IDispatchImpl<CompositeOverlayImpl<IOverlayHome>, &IID_IOverlayHome, &LIBID_SYSSTATSCOMLib>, 
	public ISupportErrorInfo,
	public CComObjectRoot,
	public CComCoClass<COverlayHome,&CLSID_OverlayHome>
{
public:
	COverlayHome() {}

BEGIN_COM_MAP(COverlayHome)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IOverlayHome)
	COM_INTERFACE_ENTRY(ICompositeOverlay)
	COM_INTERFACE_ENTRY(ISimpleOverlay2)
	COM_INTERFACE_ENTRY(ISimpleOverlay)
	COM_INTERFACE_ENTRY(IOverlay)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(COverlayHome) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_REGISTRY_RESOURCEID(IDR_OverlayHome)
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IOverlay
public:
	STDMETHOD(Render)(long _hdc);
	STDMETHOD(get_Type)(/*[out, retval]*/ BSTR *retVal);
	STDMETHOD(Configure)(/*[in]*/ IObserver *observer, /*[in]*/ IMeterHome *meters, /*[in]*/ long hWnd);

	// ICompositeOverlay
public:

	//IOverlayHome
public:
	STDMETHOD(Find)(/*[in]*/ LPSAFEARRAY path, /*[out, retval]*/ IOverlay **pRet);

// non-COM part
public:
	STDMETHOD(GetPath)(/*[in]*/ IOverlay *pOverlay, /*[out]*/ LPSAFEARRAY *pRet);
	virtual ~COverlayHome();

protected:
	// From OverlayImpl<>
	virtual IOverlay *createInstance();

	HRESULT DepthFirstSearch(ICompositeOverlay *coll, IOverlay *target, LPSAFEARRAY *pRet, int depth);
};

#endif // !defined(AFX_OVERLAYHOME_H__EE4999AE_A771_43C1_A6A1_9F44C83AFACF__INCLUDED_)
