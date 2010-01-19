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

// CCompositeOverlay.h : Declaration of the CCompositeOverlay

#ifndef __COMPOSITEOVERLAY_H_
#define __COMPOSITEOVERLAY_H_

#include "resource.h"       // main symbols
#include "CompositeOverlayImpl.cpp"
#include "SysStatsOverlayCategory.cpp"

/////////////////////////////////////////////////////////////////////////////
// CCompositeOverlay
class ATL_NO_VTABLE CCompositeOverlay : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCompositeOverlay, &CLSID_CompositeOverlay>,
	public ISupportErrorInfo,
	public IDispatchImpl<CompositeOverlayImpl<ICompositeOverlay>, &IID_ICompositeOverlay, &LIBID_SYSSTATSCOMLib>
{
public:
	CCompositeOverlay();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_REGISTRY_RESOURCEID(IDR_COMPOSITEOVERLAY)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCompositeOverlay)
	COM_INTERFACE_ENTRY(ICompositeOverlay)
	COM_INTERFACE_ENTRY(ISimpleOverlay2)
	COM_INTERFACE_ENTRY(ISimpleOverlay)
	COM_INTERFACE_ENTRY(IOverlay)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CCompositeOverlay)
	IMPLEMENTED_CATEGORY(CATID_SysStatsOverlayCategory)
END_CATEGORY_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

public:
	STDMETHOD(get_Type)(/*[out, retval]*/ BSTR *retVal);
	STDMETHOD(Configure)(/*[in]*/ IObserver *observer, /*[in]*/ IMeterHome *meters, /*[in]*/ long hWnd);

protected:
	// From OverlayImpl<>, used by clone()
	virtual IOverlay *createInstance();

	IUnknown *pWeakTarget;
};

#endif //__COMPOSITEOVERLAY_H_
