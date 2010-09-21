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

// GaugeOverlay.h : Declaration of the CGaugeOverlay

#ifndef __GAUGEOVERLAY_H_
#define __GAUGEOVERLAY_H_

#include <commdlg.h>
#include <Gdiplus.h>
using namespace Gdiplus;

#include "COMOverlays.h"       // main symbols
#include "resource.h"       // main symbols
#include "SysStatsOverlayCategory.h"
#include "SimpleOverlayImpl2.h"

/////////////////////////////////////////////////////////////////////////////
// CGaugeOverlay
class ATL_NO_VTABLE CGaugeOverlay :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CGaugeOverlay, &CLSID_GaugeOverlay>,
	public ISupportErrorInfo,
	public IDispatchImpl<SimpleOverlayImpl2<IGaugeOverlay>, &IID_IGaugeOverlay, &LIBID_COMOVERLAYSLib>
{
public:
	CGaugeOverlay();
	virtual ~CGaugeOverlay();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_REGISTRY_RESOURCEID(IDR_GAUGEOVERLAY)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CGaugeOverlay)
	COM_INTERFACE_ENTRY(IGaugeOverlay)
	COM_INTERFACE_ENTRY(IMeterOverlay)
	COM_INTERFACE_ENTRY(ISimpleOverlay2)
	COM_INTERFACE_ENTRY(ISimpleOverlay)
	COM_INTERFACE_ENTRY(IOverlay)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CGaugeOverlay)
	IMPLEMENTED_CATEGORY(CATID_SysStatsOverlayCategory)
END_CATEGORY_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IGaugeOverlay
public:
	STDMETHOD(get_Span)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Span)(/*[in]*/ long newVal);

	// Like the ISimpleOverlay2 method, but not quite :)
	STDMETHOD(CumulativeTransform)(long size, long offset, /*[out, retval]*/ REAL *elements);

// IMeterOverlay
	STDMETHOD(get_Alpha)(/*[out, retval]*/ BYTE *pVal) { return proxy->get_Alpha(pVal); }
	STDMETHOD(put_Alpha)(/*[in]*/ BYTE newVal) { return proxy->put_Alpha(newVal); }
	STDMETHOD(get_Color)(/*[out, retval]*/ long *pVal) { return proxy->get_Color(pVal); }
	STDMETHOD(put_Color)(/*[in]*/ long newVal) { return proxy->put_Color(newVal); }
	STDMETHOD(get_Thickness)(/*[out, retval]*/ long *pVal) { return proxy->get_Thickness(pVal); }
	STDMETHOD(put_Thickness)(/*[in]*/ long newVal) { return proxy->put_Thickness(newVal); }
	STDMETHOD(get_Length)(/*[out, retval]*/ long *pVal) { return proxy->get_Length(pVal); }
	STDMETHOD(put_Length)(/*[in]*/ long newVal) { return proxy->put_Length(newVal); }
	STDMETHOD(get_Max)(/*[out, retval]*/ long *pVal) { return proxy->get_Max(pVal); }
	STDMETHOD(put_Max)(/*[in]*/ long newVal) { return proxy->put_Max(newVal); }
	STDMETHOD(get_Min)(/*[out, retval]*/ long *pVal) { return proxy->get_Min(pVal); }
	STDMETHOD(put_Min)(/*[in]*/ long newVal) { return proxy->put_Min(newVal); }

// ISimpleOverlay2
	STDMETHOD(HitTest)(/*[in]*/ short X, /*[in]*/ short Y, /*[out, retval]*/ VARIANT_BOOL *retVal);

// IOverlay
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(clone)(IOverlay * * pRet);
	STDMETHOD(Configure)(IObserver *observer, IMeterHome *Meters, LONG hDlg);
	STDMETHOD(Render)(LONG hdc);

protected:
	virtual IOverlay *createInstance();

	IMeterOverlay *proxy;

	long span;

	IUnknown *pWeakTarget;
};

#endif //__GAUGEOVERLAY_H_
