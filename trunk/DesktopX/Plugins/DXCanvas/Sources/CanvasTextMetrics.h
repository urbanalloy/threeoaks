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

class ATL_NO_VTABLE CCanvasTextMetrics :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCanvasTextMetrics, &CLSID_CanvasTextMetrics>,
	public IDispatchImpl<ICanvasTextMetrics, &IID_ICanvasTextMetrics, &LIBID_DXCanvasLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public ISupportErrorInfo
{
public:
	CCanvasTextMetrics() : width(0) {}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease() 
	{
	}

DECLARE_NOT_AGGREGATABLE(CCanvasTextMetrics)

DECLARE_REGISTRY_RESOURCEID(IDR_CANVASTEXTMETRICS)

BEGIN_COM_MAP(CCanvasTextMetrics)
	COM_INTERFACE_ENTRY(ICanvasTextMetrics)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

private:

	float width;

public:

	void setWidth(float width);

	//////////////////////////////////////////////////////////////////////////
	// ISupportErrorInfo
	//////////////////////////////////////////////////////////////////////////
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	//////////////////////////////////////////////////////////////////////////
	// ICanvasTextMetrics
	//////////////////////////////////////////////////////////////////////////
	STDMETHOD(get_width)(float* width);
};

OBJECT_ENTRY_AUTO(__uuidof(CanvasTextMetrics), CCanvasTextMetrics)