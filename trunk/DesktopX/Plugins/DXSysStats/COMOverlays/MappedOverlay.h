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

// MappedOverlay.h : Declaration of the CMappedOverlay

#ifndef __MAPPEDOVERLAY_H_
#define __MAPPEDOVERLAY_H_

#include <commdlg.h>
#include <Gdiplus.h>
using namespace Gdiplus;

#include "COMOverlays.h"       // main symbols
#include "resource.h"       // main symbols
#include "SysStatsOverlayCategory.h"
#include "SimpleOverlayImpl2.h"
#include "RemoteBitmap.h"

class HDCImage;

/////////////////////////////////////////////////////////////////////////////
// CMappedOverlay
class ATL_NO_VTABLE CMappedOverlay :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMappedOverlay, &CLSID_MappedOverlay>,
	public ISupportErrorInfo,
	public IDispatchImpl<SimpleOverlayImpl2<IMappedOverlay>, &IID_IMappedOverlay, &LIBID_COMOVERLAYSLib>,
	public RemoteBitmap::Notifier
{
public:
	CMappedOverlay();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_REGISTRY_RESOURCEID(IDR_MAPPEDOVERLAY)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMappedOverlay)
	COM_INTERFACE_ENTRY(IMappedOverlay)
	COM_INTERFACE_ENTRY(ISimpleOverlay2)
	COM_INTERFACE_ENTRY(ISimpleOverlay)
	COM_INTERFACE_ENTRY(IOverlay)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CMappedOverlay)
	IMPLEMENTED_CATEGORY(CATID_SysStatsOverlayCategory)
END_CATEGORY_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IMappedOverlay
public:
	STDMETHOD(get_Alignment)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_Alignment)(/*[in]*/ short newVal);
	STDMETHOD(get_LimitHeight)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_LimitHeight)(/*[in]*/ long newVal);
	STDMETHOD(get_LimitWidth)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_LimitWidth)(/*[in]*/ long newVal);
	STDMETHOD(get_Saturation)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_Saturation)(/*[in]*/ double newVal);
	STDMETHOD(get_HueShift)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_HueShift)(/*[in]*/ long newVal);
	STDMETHOD(Dispose)();
	STDMETHOD(GetPixelValue)(/*[in]*/ long X, /*[in]*/ long Y, /*[out, retval]*/ DWORD *value);
	STDMETHOD(get_ImageWidth)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ImageHeight)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Alpha)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_Alpha)(/*[in]*/ BYTE newVal);
	STDMETHOD(get_MapPath)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_MapPath)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ImagePath)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ImagePath)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Max)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Max)(/*[in]*/ long newVal);
	STDMETHOD(get_Min)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Min)(/*[in]*/ long newVal);

// ISimpleOverlay2
	STDMETHOD(HitTest)(/*[in]*/ short X, /*[in]*/ short Y, /*[out, retval]*/ VARIANT_BOOL *retVal);

// IOverlay
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(clone)(IOverlay * * pRet);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * Meters, LONG hDlg);
	STDMETHOD(Render)(LONG hdc);

protected:
	virtual IOverlay *createInstance();
	void UnloadImage();

	Image *LoadCombinedImage();
	void LoadImage(_bstr_t path, RemoteBitmap &_bmp);
	void loaded();

	RemoteBitmap bmp;
	RemoteBitmap map;
	HDCImage *hdcImage;
	float oldFactor;

	BYTE alpha;
	long hueShift;
	double saturation;

	_bstr_t imagePath;
	_bstr_t mapPath;
	long min;
	long max;
	long imageHeight;
	long imageWidth;

	long limitWidth;
	long limitHeight;
	short alignment;

	IUnknown *pWeakTarget;
};

#endif //__MAPPEDOVERLAY_H_
