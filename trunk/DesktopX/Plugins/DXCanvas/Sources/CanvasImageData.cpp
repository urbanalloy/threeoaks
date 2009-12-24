///////////////////////////////////////////////////////////////////////////////////////////////
//
// Canvas Plugin for DesktopX
//
// Copyright (c) 2008-2010, Julien Templier
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//  3. The name of the author may not be used to endorse or promote products derived from this
//     software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
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