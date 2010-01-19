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

// MeterOverlay.h : Declaration of the CMeterOverlay

#ifndef __METEROVERLAY_H_
#define __METEROVERLAY_H_

#include <commdlg.h>
#include <GdiplusH.h>

#include "COMOverlays.h"       // main symbols
#include "resource.h"       // main symbols
#include "SysStatsOverlayCategory.h"
#include "SimpleOverlayImpl2.h"

/////////////////////////////////////////////////////////////////////////////
// CMeterOverlay
class ATL_NO_VTABLE CMeterOverlay : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMeterOverlay, &CLSID_MeterOverlay>,
	public ISupportErrorInfo,
	public IDispatchImpl<SimpleOverlayImpl2<IMeterOverlay>, &IID_IMeterOverlay, &LIBID_COMOVERLAYSLib>
{
public:
	CMeterOverlay();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_REGISTRY_RESOURCEID(IDR_METEROVERLAY)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMeterOverlay)
	COM_INTERFACE_ENTRY(IMeterOverlay)
	COM_INTERFACE_ENTRY(ISimpleOverlay2)
	COM_INTERFACE_ENTRY(ISimpleOverlay)
	COM_INTERFACE_ENTRY(IOverlay)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CMeterOverlay)
	IMPLEMENTED_CATEGORY(CATID_SysStatsOverlayCategory)
END_CATEGORY_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IMeterOverlay
public:
	STDMETHOD(get_Alpha)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_Alpha)(/*[in]*/ BYTE newVal);
	STDMETHOD(get_Color)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Color)(/*[in]*/ long newVal);
	STDMETHOD(get_Thickness)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Thickness)(/*[in]*/ long newVal);
	STDMETHOD(get_Length)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Length)(/*[in]*/ long newVal);
	STDMETHOD(get_Max)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Max)(/*[in]*/ long newVal);
	STDMETHOD(get_Min)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Min)(/*[in]*/ long newVal);

// ISimpleOverlay2
	STDMETHOD(HitTest)(/*[in]*/ short X, /*[in]*/ short Y, /*[out, retval]*/ VARIANT_BOOL *retVal);

// IOverlay
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(clone)(IOverlay * * pRet);
	STDMETHOD(Configure)(IObserver *observer, IMeterHome *Meters, LONG hDlg);
	STDMETHOD(Render)(LONG hdc);

protected:
	virtual IOverlay *createInstance();

	long min, max, length, thickness;
	Color color;

	IUnknown *pWeakTarget;
};

#endif //__METEROVERLAY_H_
