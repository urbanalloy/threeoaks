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

// TextOverlay.h : Declaration of the CTextOverlay

#ifndef __TEXTOVERLAY_H_
#define __TEXTOVERLAY_H_

#include <commdlg.h>
#include <Gdiplus.h>
using namespace Gdiplus;

#include "COMOverlays.h"       // main symbols
#include "resource.h"       // main symbols
#include "SysStatsOverlayCategory.h"
#include "SimpleOverlayImpl2.h"

/////////////////////////////////////////////////////////////////////////////
// CTextOverlay
class ATL_NO_VTABLE CTextOverlay :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTextOverlay, &CLSID_TextOverlay>,
	public ISupportErrorInfo,
	public IDispatchImpl<SimpleOverlayImpl2<ITextOverlay>, &IID_ITextOverlay, &LIBID_COMOVERLAYSLib>
{
public:
	CTextOverlay();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_REGISTRY_RESOURCEID(IDR_TEXTOVERLAY)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CTextOverlay)
	COM_INTERFACE_ENTRY(ITextOverlay)
	COM_INTERFACE_ENTRY(ISimpleOverlay2)
	COM_INTERFACE_ENTRY(ISimpleOverlay)
	COM_INTERFACE_ENTRY(IOverlay)
 	COM_INTERFACE_ENTRY(ISupportErrorInfo)
 	COM_INTERFACE_ENTRY(IDispatch)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CTextOverlay)
	IMPLEMENTED_CATEGORY(CATID_SysStatsOverlayCategory)
END_CATEGORY_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// ITextOverlay
public:
	STDMETHOD(get_BlurColor)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_BlurColor)(/*[in]*/ long newVal);
	STDMETHOD(get_TextContrast)(/*[out, retval]*/ UINT *pVal);
	STDMETHOD(put_TextContrast)(/*[in]*/ UINT newVal);
	STDMETHOD(get_TextRenderingHint)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_TextRenderingHint)(/*[in]*/ short newVal);
	STDMETHOD(get_Vertical)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_Vertical)(/*[in]*/ short newVal);
	STDMETHOD(get_Wrap)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_Wrap)(/*[in]*/ short newVal);
	STDMETHOD(get_AlternateRender)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_AlternateRender)(/*[in]*/ short newVal);
	STDMETHOD(get_Height)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Height)(/*[in]*/ long newVal);
	STDMETHOD(get_Width)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Width)(/*[in]*/ long newVal);
	STDMETHOD(get_Format)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Format)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Radius)(/*[out, retval]*/ int *pVal);
	STDMETHOD(put_Radius)(/*[in]*/ int newVal);
	STDMETHOD(get_Alpha)(/*[out, retval]*/ BYTE *pVal);
	STDMETHOD(put_Alpha)(/*[in]*/ BYTE newVal);
	STDMETHOD(get_FontColor)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_FontColor)(/*[in]*/ long newVal);
	STDMETHOD(get_FontSize)(/*[out, retval]*/ float *pVal);
	STDMETHOD(put_FontSize)(/*[in]*/ float newVal);
	STDMETHOD(get_FontStyle)(/*[out, retval]*/ int *pVal);
	STDMETHOD(put_FontStyle)(/*[in]*/ int newVal);
	STDMETHOD(get_FontName)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_FontName)(/*[in]*/ BSTR newVal);

// ISimpleOverlay2
	STDMETHOD(HitTest)(/*[in]*/ short X, /*[in]*/ short Y, /*[out, retval]*/ VARIANT_BOOL *retVal);

// IOverlay
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(clone)(IOverlay * * pRet);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * Meters, LONG hDlg);
	STDMETHOD(Render)(LONG hdc);

	CHOOSEFONT *getChooseFont() { return &chooseFont; }

protected:
	virtual IOverlay *createInstance();

	CHOOSEFONT chooseFont;
	LOGFONT logFont;
	char fontStyle[128];	// Buffer for font style name
	Color color;
	int radius;	// for blur effect
	_bstr_t format;
	long width;
	long height;
	RectF bounds;
	short alternateRender;
	short wrap;
	short vertical;
	short textRenderingHint;
	UINT textContrast;
	long blurColor;

	static const char* FORMAT_PROPERTY;

	IUnknown *pWeakTarget;
};

#endif //__TEXTOVERLAY_H_
