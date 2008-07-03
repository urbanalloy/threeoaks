///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXVolumeControl - Vista Master Volume Control for DesktopX
//
// Copyright (c)2006-2007, Julien Templier
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

#include "DXVolumeControl.h"
#include "resource.h"       // main symbols
#include <comsvcs.h>
#include "_IVolumeEvents_CP.H"

// CVistaVolume
class ATL_NO_VTABLE CVistaVolume :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CVistaVolume, &CLSID_VistaVolume>,
	public IDispatchImpl<IVistaVolume, &IID_IVistaVolume, &LIBID_DXVolumeControlLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IConnectionPointContainerImpl<CVistaVolume>,
	public CProxy_IVolumeEvents<CVistaVolume>,
	public IProvideClassInfo2Impl<&CLSID_VistaVolume, &DIID__IVolumeEvents, &LIBID_DXVolumeControlLib>
{
public:
	CVistaVolume() {}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_VISTAVOLUME)

DECLARE_NOT_AGGREGATABLE(CVistaVolume)

BEGIN_COM_MAP(CVistaVolume)
	COM_INTERFACE_ENTRY(IVistaVolume)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

private:
	// VISTA
	HRESULT Vista_get_Volume(int *volume);
	HRESULT Vista_put_Volume(int volume);

	HRESULT Vista_get_Mute(VARIANT *isMuted);
	HRESULT Vista_put_Mute(BOOL isMuted);

	HRESULT Vista_get_PeakValue(int *level);

	// XP
	HRESULT XP_get_Volume(int *volume);
	HRESULT XP_put_Volume(int volume);

	HRESULT XP_get_Mute(VARIANT *isMuted);
	HRESULT XP_put_Mute(BOOL isMuted);

	HRESULT XP_get_PeakValue(int *level);

// IVistaVolume
public:
	BEGIN_CONNECTION_POINT_MAP(CVistaVolume)
		CONNECTION_POINT_ENTRY(__uuidof(_IVolumeEvents))
	END_CONNECTION_POINT_MAP()

	// Master Volume
	STDMETHOD(get_Volume)(/*[out, retval]*/ int *volume);
	STDMETHOD(put_Volume)(/*[in]*/ int volume);

	// Muting state
	STDMETHOD(get_Mute)(/*[out, retval]*/ VARIANT *isMuted);
	STDMETHOD(put_Mute)(/*[in]*/ BOOL isMuted);

	// Peak level
	STDMETHOD(get_PeakValue)(/*[out, retval]*/ int *level);
};

OBJECT_ENTRY_AUTO(__uuidof(VistaVolume), CVistaVolume)
