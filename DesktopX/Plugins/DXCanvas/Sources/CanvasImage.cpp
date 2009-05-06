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

	// Copy our image to the surface
	HDC cairoDC = cairo_win32_surface_get_dc(surface);
	if (cairoDC == NULL)
		return CCOMError::DispatchError(E_FAIL, CLSID_CanvasImage, _T("Internal error"), __FUNCTION__ ": failed to get the DC to the image surface", 0, NULL);

	Graphics graphics(cairoDC);
	graphics.DrawImage(bitmap, 0, 0);

	// Create a new pattern
	pattern = cairo_pattern_create_for_surface(surface);

	// Cleamup
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

