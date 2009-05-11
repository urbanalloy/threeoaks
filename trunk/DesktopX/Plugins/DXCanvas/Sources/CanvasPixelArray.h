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

class ATL_NO_VTABLE CCanvasPixelArray :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCanvasPixelArray, &CLSID_CanvasPixelArray>,
	public IDispatchImpl<ICanvasPixelArray, &IID_ICanvasPixelArray, &LIBID_DXCanvasLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public ISupportErrorInfo
{
public:
	CCanvasPixelArray() : length(0),
						  data(NULL) {}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease() 
	{		
	}

DECLARE_NOT_AGGREGATABLE(CCanvasPixelArray)

DECLARE_REGISTRY_RESOURCEID(IDR_CANVASPIXELARRAY)

BEGIN_COM_MAP(CCanvasPixelArray)
	COM_INTERFACE_ENTRY(ICanvasPixelArray)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

private:

	unsigned long length;
	// data is an array of [r, g, b, a] pixels
	unsigned char* data;

public:

	void setLength(long length);
	void setData(unsigned char* data);
	unsigned char* getData();

	//////////////////////////////////////////////////////////////////////////
	// ISupportErrorInfo
	//////////////////////////////////////////////////////////////////////////
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	//////////////////////////////////////////////////////////////////////////
	// ICanvasPixelArray
	//////////////////////////////////////////////////////////////////////////
	STDMETHOD(get_length)(unsigned long* length);
	STDMETHOD(XXX5)(unsigned long index, unsigned char* value);
	STDMETHOD(XXX6)(unsigned long index, int value);

};

OBJECT_ENTRY_AUTO(__uuidof(CanvasPixelArray), CCanvasPixelArray)