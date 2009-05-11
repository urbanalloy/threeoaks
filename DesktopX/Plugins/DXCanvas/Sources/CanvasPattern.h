///////////////////////////////////////////////////////////////////////////////////////////////
//
// Canvas Plugin for DesktopX
//
// Copyright (c) 2008-2009, Three Oaks Crossing
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

class ATL_NO_VTABLE CCanvasPattern :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCanvasPattern, &CLSID_CanvasPattern>,
	public IDispatchImpl<ICanvasPattern, &IID_ICanvasPattern, &LIBID_DXCanvasLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public ISupportErrorInfo
{
public:
	CCanvasPattern() : pattern(NULL) {}

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

DECLARE_NOT_AGGREGATABLE(CCanvasPattern)

DECLARE_REGISTRY_RESOURCEID(IDR_CANVASPATTERN)

BEGIN_COM_MAP(CCanvasPattern)
	COM_INTERFACE_ENTRY(ICanvasPattern)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

private:

	cairo_pattern_t* pattern;

public:

	void setPattern(cairo_pattern_t* pattern);
	void setExtend(cairo_extend_t extend);
	void apply(cairo_t *context);

	//////////////////////////////////////////////////////////////////////////
	// ISupportErrorInfo
	//////////////////////////////////////////////////////////////////////////
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	//////////////////////////////////////////////////////////////////////////
	// ICanvasPattern
	//////////////////////////////////////////////////////////////////////////
};

OBJECT_ENTRY_AUTO(__uuidof(CanvasPattern), CCanvasPattern)
