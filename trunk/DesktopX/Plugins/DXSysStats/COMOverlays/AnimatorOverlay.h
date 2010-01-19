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

// AnimatorOverlay.h : Declaration of the CAnimatorOverlay

#ifndef __ANIMATOROVERLAY_H_
#define __ANIMATOROVERLAY_H_

#include "resource.h"       // main symbols
#include "SysStatsOverlayCategory.h"
#include "CompositeOverlayImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CAnimatorOverlay
class ATL_NO_VTABLE CAnimatorOverlay :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAnimatorOverlay, &CLSID_AnimatorOverlay>,
	public ISupportErrorInfo,
	public IDispatchImpl<CompositeOverlayImpl<IAnimatorOverlay>, &IID_IAnimatorOverlay, &LIBID_COMOVERLAYSLib>
{
public:
	CAnimatorOverlay();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();

DECLARE_REGISTRY_RESOURCEID(IDR_ANIMATOROVERLAY)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CAnimatorOverlay)
	COM_INTERFACE_ENTRY(IAnimatorOverlay)
	COM_INTERFACE_ENTRY(ICompositeOverlay)
	COM_INTERFACE_ENTRY(ISimpleOverlay2)
	COM_INTERFACE_ENTRY(ISimpleOverlay)
	COM_INTERFACE_ENTRY(IOverlay)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CAnimatorOverlay)
	IMPLEMENTED_CATEGORY(CATID_SysStatsOverlayCategory)
END_CATEGORY_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IAnimatorOverlay
public:
	STDMETHOD(get_Alpha)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_Alpha)(/*[in]*/ BYTE newVal);
	STDMETHOD(get_VScaleEnd)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_VScaleEnd)(/*[in]*/ double newVal);
	STDMETHOD(get_HScaleEnd)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_HScaleEnd)(/*[in]*/ double newVal);
	STDMETHOD(get_RotationEnd)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_RotationEnd)(/*[in]*/ long newVal);
	STDMETHOD(get_CenterYEnd)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_CenterYEnd)(/*[in]*/ long newVal);
	STDMETHOD(get_CenterXEnd)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_CenterXEnd)(/*[in]*/ long newVal);
	STDMETHOD(get_YEnd)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_YEnd)(/*[in]*/ long newVal);
	STDMETHOD(get_XEnd)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_XEnd)(/*[in]*/ long newVal);
	STDMETHOD(get_AlphaEnd)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_AlphaEnd)(/*[in]*/ BYTE newVal);

// ISimpleOverlay2
	STDMETHOD(CumulativeTransform)(/*[out, retval]*/ float *elements);
//	STDMETHOD(HitTest)(/*[in]*/ short X, /*[in]*/ short Y, /*[out, retval]*/ VARIANT_BOOL *retVal);

// IOverlay
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(clone)(IOverlay * * pRet);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * Meters, LONG hDlg);
	STDMETHOD(Render)(LONG hdc);

protected:
	virtual IOverlay *createInstance();
	const Matrix& getCurTransform();

	BYTE alpha, alphaEnd;
	long rotationEnd, centerYEnd, centerXEnd, yEnd, xEnd;
	double hScaleEnd, vScaleEnd;

	IUnknown *pWeakTarget;

	long framecount;
	long currentframe;

private:
	Matrix matrixCur;
};

#endif //__ANIMATOROVERLAY_H_
