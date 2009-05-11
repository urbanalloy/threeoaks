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
//#define _USE_MATH_DEFINES
//#include <math.h>

#include "CanvasImageData.h"
#include "COMError.h"

long int CCanvasImageData::getWidth()
{
	return width;
}

long int CCanvasImageData::getHeight()
{
	return height;
}

void CCanvasImageData::setData(unsigned char* data, long int width, long int height)
{
	this->width = width;
	this->height = height;
	this->data = data;

	// Process data
	unsigned char temp;
	for (int i = 0; i < width*height*4; i = i+4)
	{
		// Get data in correct order (BGRA -> RGBA)
		temp = data[i+2];
		data[i+2] = data[i];
		data[i] = temp;

		// Convert to non-premultiplied color
		if (data[i+3] != 0) 
		{		
			data[i]   = (data[i]   * 255) / data[i+3];
			data[i+1] = (data[i+1] * 255) / data[i+3];
			data[i+2] = (data[i+2] * 255) / data[i+3];
		}
	}
}

unsigned char* CCanvasImageData::getData()
{
	unsigned char temp;
	for (int i = 0; i < width*height*4; i = i+4)
	{
		// Swap data back to BGRA
		temp = data[i+2];
		data[i+2] = data[i];
		data[i] = temp;

		// Convert to premultiplied color  		
		data[i]   = (data[i]   * data[i+3] + 254) / 255;
		data[i+1] = (data[i+1] * data[i+3] + 254) / 255;
		data[i+2] = (data[i+2] * data[i+3] + 254) / 255;
	}

	return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISupportErrorInfo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCanvasImageData::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ICanvasImageData
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ICanvasImageData
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCanvasImageData::get_width(unsigned long int* width)
{
	*width = this->width;

	return S_OK;
}

STDMETHODIMP CCanvasImageData::get_height(unsigned long int* height)
{
	*height = this->height;

	return S_OK;
}

STDMETHODIMP CCanvasImageData::get_data(ICanvasPixelArray** array)
{
	if (!data)
		return CCOMError::DispatchError(E_FAIL, CLSID_CanvasImageData, _T("Internal error"), __FUNCTION__ ": data is not present", 0, NULL);

	CComObject<CCanvasPixelArray>* pixelArray;
	CComObject<CCanvasPixelArray>::CreateInstance(&pixelArray);
	pixelArray->setData(data);
	pixelArray->setLength(width*height*4);

	pixelArray->QueryInterface(IID_ICanvasPixelArray, (void**)array);

	return S_OK;
}