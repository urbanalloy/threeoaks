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

#include <vector>
#include <EndpointVolume.h>

class CVistaVolumeCallback : public IAudioEndpointVolumeCallback
{

private :
	ULONG _ref_count;

	// Mutex
	HANDLE hObject;

	// DesktopX Object Ids
	// TODO: dot not use a vector here
	std::vector<DWORD> *objectsId;

public:
	CVistaVolumeCallback() :
			_ref_count(1),
			hObject(NULL)
	{		
		objectsId = new std::vector<DWORD>();
		
		hObject   = CreateMutex(NULL, false, "ObjectMutex");
	}

	~CVistaVolumeCallback()	{
		delete(objectsId);
		CloseHandle(hObject);
	};

	void addID(DWORD objID);
	void removeID(DWORD objID);

	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID iid, void** ppvObject );
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	virtual HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
};
