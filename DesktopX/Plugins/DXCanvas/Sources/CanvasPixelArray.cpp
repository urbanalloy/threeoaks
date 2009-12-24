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
