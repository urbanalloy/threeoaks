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

#include "stdafx.h"
#include "CanvasPixelArray.h"

#include "COMError.h"

void CCanvasPixelArray::setLength(long length)
{
	this->length = length;
}

void CCanvasPixelArray::setData(unsigned char* data)
{
	this->data = data;
}

unsigned char* CCanvasPixelArray::getData()
{
	return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISupportErrorInfo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCanvasPixelArray::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ICanvasPixelArray
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ICanvasPixelArray
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCanvasPixelArray::get_length(unsigned long* length)
{
	if (!data)
		return CCOMError::DispatchError(E_FAIL, CLSID_CanvasPixelArray, _T("Internal error"), __FUNCTION__ ": data is not present", 0, NULL);

	*length = this->length;

	return S_OK;
}

STDMETHODIMP CCanvasPixelArray::XXX5(unsigned long index, unsigned char* value)
{
	if (!data)
		return CCOMError::DispatchError(E_FAIL, CLSID_CanvasPixelArray, _T("Internal error"), __FUNCTION__ ": data is not present", 0, NULL);

	if (index > length)
		return CCOMError::DispatchError(INDEX_SIZE_ERR, CLSID_CanvasPixelArray, _T("Index out of bounds"), __FUNCTION__ "Index is out of bounds", 0, NULL);

	*value = data[index];

	return S_OK;
}

STDMETHODIMP CCanvasPixelArray::XXX6(unsigned long index, int value)
{
	if (!data)
		return CCOMError::DispatchError(E_FAIL, CLSID_CanvasPixelArray, _T("Internal error"), __FUNCTION__ ": data is not present", 0, NULL);

	if (index > length)
		return CCOMError::DispatchError(INDEX_SIZE_ERR, CLSID_CanvasPixelArray, _T("Index out of bounds"), __FUNCTION__ "Index is out of bounds", 0, NULL);

	// Clamp value
	data[index] = (char)((value < 0) ? 0 : ((value > 255) ? 255 : value));
	
	return S_OK;
}
