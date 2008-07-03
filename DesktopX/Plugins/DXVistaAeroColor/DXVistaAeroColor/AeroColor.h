///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXVistaAeroColor - Set the color intensity, color, and opacity of glass on Vista.
//
// (c) 2006 - Julien Templier
//
// This work is licensed under the Creative Commons
// Attribution-ShareAlike License. To view a copy of this license, visit
// http://creativecommons.org/licenses/by-sa/2.0/ or send a letter to
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
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
