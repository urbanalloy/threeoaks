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
#include "CanvasImage.h"
#include "COMError.h"

#include <cairo-win32.h>

#include <gdiplus.h>
using namespace Gdiplus;

int CCanvasImage::getWidth()
{
	return width;
}

int CCanvasImage::getHeight()
{
	return height;
}

cairo_surface_t* CCanvasImage::getSurface()
{
	return surface;
}

void CCanvasImage::setMatrix(cairo_matrix_t* matrix)
{
	cairo_pattern_set_matrix(pattern, matrix);
}

void CCanvasImage::apply(cairo_t *context)
{
	cairo_set_source(context, pattern);
}

HRESULT CCanvasImage::loadImage(wstring image)
{
	Bitmap* bitmap = Bitmap::FromFile(image.c_str(), TRUE);

	if (GetLastError() != S_OK)
	{
		char error[1000];
		sprintf_s(error, "There was an error loading the image: %S", image.c_str());
		return CCOMError::DispatchError(NOT_SUPPORTED_ERR, CLSID_CanvasImage, _T("Error loading image"), error, 0, NULL);
	}

	this->width = bitmap->GetWidth();
	this->height = bitmap->GetHeight();

	// Copy the image to our new cairo surface
	surface = cairo_win32_surface_create_with_dib(CAIRO_FORMAT_ARGB32, this->width, this->height);

	// Check the status of our surface
	if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
		return CCOMError::DispatchError(E_FAIL, CLSID_CanvasImage, _T("Internal error"), __FUNCTION__ ": failed to create a surface from the image", 0, NULL);

	// Copy our image to the surface
	HDC cairoDC = cairo_win32_surface_get_dc(surface);
	if (cairoDC == NULL)
		return CCOMError::DispatchError(E_FAIL, CLSID_CanvasImage, _T("Internal error"), __FUNCTION__ ": failed to get the DC to the image surface", 0, NULL);

	Graphics graphics(cairoDC);
	graphics.DrawImage(bitmap, 0, 0);

	// Create a new pattern
	pattern = cairo_pattern_create_for_surface(surface);

	// Check the status of our pattern
	if (cairo_pattern_status(pattern) != CAIRO_STATUS_SUCCESS)
		return CCOMError::DispatchError(E_FAIL, CLSID_CanvasImage, _T("Internal error"), __FUNCTION__ ": failed to create a pattern from the image", 0, NULL);

	// Cleanup
	delete bitmap;

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISupportErrorInfo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCanvasImage::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_ICanvasImage
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ICanvasImage
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

