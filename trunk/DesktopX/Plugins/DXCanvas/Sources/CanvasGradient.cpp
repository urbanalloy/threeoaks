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
#include "CanvasGradient.h"
#include "CSSColorParser.h"
#include "CanvasParameter.h"

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
	if (IS_NAN(offset) || IS_INFINITY(offset) || offset < 0 || offset > 1)
		return CCOMError::DispatchError(INDEX_SIZE_ERR, CLSID_CanvasGradient, _T("Invalid parameter"), __FUNCTION__ ": offset should be between 0 and 1.", 0, NULL);

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
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_CanvasGradient, _T("Failed to parse color value"), error, 0, NULL);
	}

	if (pattern)
		cairo_pattern_add_color_stop_rgba(pattern, offset, rgba->r/255.0, rgba->g/255.0, rgba->b/255.0, rgba->a);

	delete parser;
	delete rgba;
	return S_OK;
}