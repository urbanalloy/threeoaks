///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXVistaAeroColor - Set the color intensity, color, and opacity of glass on Vista.
//
// Copyright (c) 2006-2010, Julien Templier
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
#include "AeroColor.h"

#define DEFAULT_OPACITY 0xFF

#define ARGB(a,r,g,b) (((a)<<24)|((r)<<16)|((g)<<8)|(b))

#define INV_ARGB(color,a,r,g,b) \
	int a = (color & 0xFF000000) >> 24;\
	int r = (color & 0x00FF0000) >> 16;\
	int g = (color & 0x0000FF00) >> 8;\
	int b = (color & 0x000000FF);

#define TO_VARIANT(var, value) \
	var->vt = VT_I4; \
	var->lVal = value;

#define ERROR_COMPOSITION_NOT_ENABLED 0x80263001
typedef HRESULT (*DwmpSetColorizationFunction) (DWORD dwColorizationColor, BOOL bTransparencyEnabled, BYTE cOpacity);
typedef HRESULT (*DwmGetColorizationFunction) (DWORD* pcrColorization, BOOL* pfOpaqueBlend);

// Default colors
// Aero / Graphite / Blue / Teal / Red / Orange / Pink / Frost
const DWORD CAeroColor::colors[] = {1161862910, 2734686208, 2818591454, 2181074085, 2630749199, 2801760000, 1241071335, 3438278647};


// CAeroColor
STDMETHODIMP CAeroColor::SetDefaultColor(int id){
	if (id < 0 || id > 7){
		return S_FALSE;
	}

	return SetColorizationFunction(colors[id], FALSE, DEFAULT_OPACITY);
}

STDMETHODIMP CAeroColor::GetDefaultColor(int id, VARIANT* alpha, VARIANT* red, VARIANT* green, VARIANT* blue){
	if (id < 0 || id > 7){
		return S_FALSE;
	}

	INV_ARGB(colors[id], a, r, g, b)

	TO_VARIANT(alpha, a)
	TO_VARIANT(red, r)
	TO_VARIANT(green, g)
	TO_VARIANT(blue, b)

	return S_OK;
}

STDMETHODIMP CAeroColor::SetColorization(int alpha, int red, int green, int blue, BOOL transparencyEnabled) {
	return this->SetColorization(alpha, red, green, blue, transparencyEnabled, DEFAULT_OPACITY);
}

STDMETHODIMP CAeroColor::SetColorization(int alpha, int red, int green, int blue, BOOL transparencyEnabled, int opacity)
{
	// Check parameters
	if (alpha > 255 || alpha < 0 ||
		red   > 255 || red   < 0 ||
		green > 255 || green < 0 ||
		blue  > 255 || blue  < 0 ||
		opacity > 255 || opacity < 0)
	{
		return S_FALSE;
	}

	// Inverted
	BOOL transparency = TRUE;
	if (transparencyEnabled == -1) // FALSE (VB)
		transparency = FALSE;

	DWORD color = ARGB(alpha, red, green, blue);

	return SetColorizationFunction(color, transparency, opacity);
}

STDMETHODIMP CAeroColor::GetColorization(VARIANT* alpha, VARIANT* red, VARIANT* green, VARIANT* blue, VARIANT* transparencyEnabled) {

	DWORD color = 0;
	BOOL opaque = FALSE;

	HMODULE hMod = LoadLibrary(TEXT("dwmapi.dll"));
	DwmGetColorizationFunction DwmGetColorization = (DwmGetColorizationFunction) GetProcAddress(hMod, "DwmGetColorizationColor");

	HRESULT hr = (*DwmGetColorization)(&color, &opaque);
	if (SUCCEEDED(hr))
	{
		INV_ARGB(color, a, r, g, b)

		TO_VARIANT(alpha, a)
		TO_VARIANT(red, r)
		TO_VARIANT(green, g)
		TO_VARIANT(blue, b)

		if (opaque = FALSE) {
			TO_VARIANT(transparencyEnabled, 0)
		} else {
			TO_VARIANT(transparencyEnabled, -1) // FALSE (VB)
		}

		return S_OK;
	}

	return S_FALSE;

}

HRESULT CAeroColor::SetColorizationFunction(DWORD color, BOOL isOpaque, int opacity)
{
	// Find our private function (ordinal 104)
	HMODULE hMod = LoadLibrary(TEXT("dwmapi.dll"));
	DwmpSetColorizationFunction DwmpSetColorization = (DwmpSetColorizationFunction) GetProcAddress(hMod, (LPCSTR)LOWORD(104));

	// Did we get the address?
	if(!DwmpSetColorization)
		return S_FALSE;

	HRESULT ret = (*DwmpSetColorization)(color, isOpaque, (BYTE) opacity);

	if (ret == ERROR_SUCCESS)
		return S_OK;
	else if (ret == ERROR_COMPOSITION_NOT_ENABLED)
		return S_FALSE;

	return S_OK;
}
