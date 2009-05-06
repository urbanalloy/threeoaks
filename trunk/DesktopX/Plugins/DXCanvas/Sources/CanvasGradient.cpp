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
#include "CanvasGradient.h"
#include "CSSColorParser.h"

#include "COMError.h"

void CCanvasGradient::apply(cairo_t *context)
{
	if (pattern)
		cairo_set_source(context, pattern);
}

void CCanvasGradient::setPattern(cairo_pattern_t* pat)
{
	pattern = pat;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISupportErrorInfo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCanvasGradient::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ICanvasGradient
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ICanvasGradient
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCanvasGradient::addColorStop(double offset, BSTR color)
{
	if (offset < 0 || offset > 1)
		return CCOMError::DispatchError(E_FAIL, CLSID_CanvasGradient, _T("Invalid parameter"), __FUNCTION__ ": offset should be between 0 and 1.", 0, NULL);

	USES_CONVERSION;

	CCSSColorParser* parser = new CCSSColorParser();
	RGBAColor* rgba = new RGBAColor();

	char* colorT = OLE2T(color);
	if (!parser->parseColor(colorT, rgba))
	{
		delete parser;
		delete rgba;	
		char error[1000];
		sprintf_s(error, __FUNCTION__ ": Failed to parse color value (value: %s)", colorT);
		return CCOMError::DispatchError(E_FAIL, CLSID_CanvasGradient, _T("Failed to parse color value"), error, 0, NULL);
	}

	if (pattern)
		cairo_pattern_add_color_stop_rgba(pattern, offset, rgba->r/255.0, rgba->g/255.0, rgba->b/255.0, rgba->a);

	delete parser;
	delete rgba;
	return S_OK;
}