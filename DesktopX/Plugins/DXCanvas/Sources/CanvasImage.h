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
#include <string>
using namespace std;

#include <cairo.h>

class ATL_NO_VTABLE CCanvasImage :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCanvasImage, &CLSID_CanvasImage>,
	public IDispatchImpl<ICanvasImage, &IID_ICanvasImage, &LIBID_DXCanvasLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public ISupportErrorInfo
{
public:
	CCanvasImage() : pattern(NULL),
					 surface(NULL)
	{}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease() 
	{
		if (pattern)
			cairo_pattern_destroy(pattern);

		if (surface)
			cairo_surface_destroy(surface);
	}

DECLARE_NOT_AGGREGATABLE(CCanvasImage)

DECLARE_REGISTRY_RESOURCEID(IDR_CANVASIMAGE)

BEGIN_COM_MAP(CCanvasImage)
	COM_INTERFACE_ENTRY(ICanvasImage)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

private:

	int width;
	int height;
	cairo_surface_t* surface;
	cairo_pattern_t* pattern;

public:

	int getWidth();
	int getHeight();
	cairo_surface_t* getSurface();

	HRESULT loadImage(wstring path);
	void setMatrix(cairo_matrix_t* matrix);
	void apply(cairo_t *context);

	//////////////////////////////////////////////////////////////////////////
	// ISupportErrorInfo
	//////////////////////////////////////////////////////////////////////////
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	//////////////////////////////////////////////////////////////////////////
	// ICanvasImage
	//////////////////////////////////////////////////////////////////////////
};

OBJECT_ENTRY_AUTO(__uuidof(CanvasImage), CCanvasImage)