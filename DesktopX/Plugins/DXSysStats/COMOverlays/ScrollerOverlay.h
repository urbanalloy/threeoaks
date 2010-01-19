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

// ScrollerOverlay.h : Declaration of the CScrollerOverlay

#ifndef __SCROLLEROVERLAY_H_
#define __SCROLLEROVERLAY_H_

#include "resource.h"       // main symbols
#include "SysStatsOverlayCategory.h"
#include "CompositeOverlayImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CScrollerOverlay
class ATL_NO_VTABLE CScrollerOverlay :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CScrollerOverlay, &CLSID_ScrollerOverlay>,
	public ISupportErrorInfo,
	public IDispatchImpl<CompositeOverlayImpl<IScrollerOverlay>, &IID_IScrollerOverlay, &LIBID_COMOVERLAYSLib>
{
public:
	CScrollerOverlay();
	virtual ~CScrollerOverlay();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();

DECLARE_REGISTRY_RESOURCEID(IDR_SCROLLEROVERLAY)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScrollerOverlay)
	COM_INTERFACE_ENTRY(IScrollerOverlay)
	COM_INTERFACE_ENTRY(ICompositeOverlay)
	COM_INTERFACE_ENTRY(ISimpleOverlay2)
	COM_INTERFACE_ENTRY(ISimpleOverlay)
	COM_INTERFACE_ENTRY(IOverlay)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CScrollerOverlay)
	IMPLEMENTED_CATEGORY(CATID_SysStatsOverlayCategory)
END_CATEGORY_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScrollerOverlay
public:
	STDMETHOD(get_DuplicateLastFrame)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_DuplicateLastFrame)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_SmoothScrollMeter)(/*[out, retval]*/ IMeter* *pVal);
	STDMETHOD(put_SmoothScrollMeter)(/*[in]*/ IMeter* newVal);
	STDMETHOD(get_SmoothScrollMeterIndex)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_SmoothScrollMeterIndex)(/*[in]*/ long newVal);
	STDMETHOD(get_dy)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_dy)(/*[in]*/ long newVal);
	STDMETHOD(get_dx)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_dx)(/*[in]*/ long newVal);
	STDMETHOD(get_Alpha)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_Alpha)(/*[in]*/ BYTE newVal);

// ISimpleOverlay2
//	STDMETHOD(HitTest)(/*[in]*/ short X, /*[in]*/ short Y, /*[out, retval]*/ VARIANT_BOOL *retVal);
	STDMETHOD(CumulativeTransform)(/*[out, retval]*/ float *elements);

// IOverlay
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(clone)(IOverlay * * pRet);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * Meters, LONG hDlg);
	STDMETHOD(Render)(LONG hdc);

protected:
	virtual IOverlay *createInstance();
	virtual void controlMeter(IMeter *iMeter, BSTR selector, BSTR value);
	void getOffset(int *x, int *y) const;
	const Matrix& getCurTransform();

	BYTE alpha;
	long dx, dy;
	VARIANT_BOOL duplicateLastFrame;

	Bitmap *oldImage;

	IUnknown *pWeakTarget;

	Matrix matrixCur;
	long framecount;
	long currentframe;

	long smoothframecount;
	long smoothcurrentframe;
	long smoothMeterPlayState;
	long delta;	// forwards = +1, backwards = -1

private:
	IWeakReference *smoothScrollMeter;
};

#endif //__SCROLLEROVERLAY_H_
