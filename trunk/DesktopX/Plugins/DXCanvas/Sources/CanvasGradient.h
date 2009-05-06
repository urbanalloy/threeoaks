///////////////////////////////////////////////////////////////////////////////////////////////
//
// Canvas Plugin for DesktopX
//
// Copyright (c) 2008, Three Oaks Crossing
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "DXCanvas.h"
#include "resource.h" 
#include <comsvcs.h>

#include <cairo.h>

class ATL_NO_VTABLE CCanvasGradient :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCanvasGradient, &CLSID_CanvasGradient>,
	public IDispatchImpl<ICanvasGradient, &IID_ICanvasGradient, &LIBID_DXCanvasLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,	
	public ISupportErrorInfo
{
public:
	CCanvasGradient() : pattern(NULL) {}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		if (pattern)
			cairo_pattern_destroy(pattern);
	}

DECLARE_NOT_AGGREGATABLE(CCanvasGradient)

DECLARE_REGISTRY_RESOURCEID(IDR_CANVASGRADIENT)

BEGIN_COM_MAP(CCanvasGradient)
	COM_INTERFACE_ENTRY(ICanvasGradient)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

protected:

	cairo_pattern_t *pattern;

public:

	void setPattern(cairo_pattern_t* pat);
	void apply(cairo_t* context);

	//////////////////////////////////////////////////////////////////////////
	// ISupportErrorInfo
	//////////////////////////////////////////////////////////////////////////
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	//////////////////////////////////////////////////////////////////////////
	// ICanvasGradient
	//////////////////////////////////////////////////////////////////////////
	STDMETHOD(addColorStop)(double offset, BSTR color);
};

OBJECT_ENTRY_AUTO(__uuidof(CanvasGradient), CCanvasGradient)