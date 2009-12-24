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

#pragma once
#include "DXVistaAeroColor.h"
#include "resource.h"       // main symbols
#include <comsvcs.h>


// CAeroColor
class ATL_NO_VTABLE CAeroColor :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAeroColor, &CLSID_AeroColor>,
	public IDispatchImpl<IAeroColor, &IID_IAeroColor, &LIBID_DXVistaAeroColorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CAeroColor() {}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease() {}

DECLARE_REGISTRY_RESOURCEID(IDR_AEROCOLOR)

DECLARE_NOT_AGGREGATABLE(CAeroColor)

BEGIN_COM_MAP(CAeroColor)
	COM_INTERFACE_ENTRY(IAeroColor)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

private:

	HRESULT SetColorizationFunction(DWORD color, int opacity, BOOL isOpaque);

	// Set the color intensity, color, and opacity of glass
	STDMETHOD(SetColorization)(int alpha, int red, int green, int blue, BOOL transparencyEnabled, int opacity);

	const static DWORD colors[];

// IAeroColor
public:

	// Set default color
	STDMETHOD(SetDefaultColor)(int id);

	// Get Default color
	STDMETHOD(GetDefaultColor)(int id, VARIANT* alpha, VARIANT* red, VARIANT* green, VARIANT* blue);

	// Set the color intensity and color of glass
	STDMETHOD(SetColorization)(int alpha, int red, int green, int blue, BOOL transparencyEnabled);

	// Get the color of glass
	STDMETHOD(GetColorization)(VARIANT* alpha, VARIANT* red, VARIANT* green, VARIANT* blue, VARIANT* transparencyEnabled);

};

OBJECT_ENTRY_AUTO(__uuidof(AeroColor), CAeroColor)
