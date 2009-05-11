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

#include "CanvasPixelArray.h"

class ATL_NO_VTABLE CCanvasImageData :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCanvasImageData, &CLSID_CanvasImageData>,
	public IDispatchImpl<ICanvasImageData, &IID_ICanvasImageData, &LIBID_DXCanvasLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public ISupportErrorInfo
{
public:
	CCanvasImageData() 
		: width(0),
		  height(0),
		  data(NULL)
	{}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease() 
	{
		// we are responsible for freeing the data
		free(data);
		data = NULL;
	}

DECLARE_NOT_AGGREGATABLE(CCanvasImageData)

DECLARE_REGISTRY_RESOURCEID(IDR_CANVASIMAGEDATA)

BEGIN_COM_MAP(CCanvasImageData)
	COM_INTERFACE_ENTRY(ICanvasImageData)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

private:
	long int width;
	long int height;		
	unsigned char* data;

public:

	long int getWidth();
	long int getHeight();

	void setData(unsigned char* data, long int width, long int height);
	unsigned char* getData();


	//////////////////////////////////////////////////////////////////////////
	// ISupportErrorInfo
	//////////////////////////////////////////////////////////////////////////
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	//////////////////////////////////////////////////////////////////////////
	// ICanvasImageData
	//////////////////////////////////////////////////////////////////////////
	STDMETHOD(get_width)(unsigned long int* width);
	STDMETHOD(get_height)(unsigned long int* height);
	STDMETHOD(get_data)(ICanvasPixelArray** array);

};

OBJECT_ENTRY_AUTO(__uuidof(CanvasImageData), CCanvasImageData)