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

#include "stdafx.h"
#include "VistaVolumeCallback.h"
#include <math.h>
#include <SDPlugin.h>

extern BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD);

// Return a ptr to a different COM interface to this object
HRESULT STDMETHODCALLTYPE CVistaVolumeCallback::QueryInterface( REFIID iid, void** ppvObject )
{
	// Match the interface and return the proper pointer
	if ( iid == IID_IUnknown) {
		*ppvObject = dynamic_cast<IUnknown*>( this );
	} else if ( iid == __uuidof(IAudioEndpointVolumeCallback) ) {
		*ppvObject = dynamic_cast<IAudioEndpointVolumeCallback*>( this );
	} else {
		// It didn't match an interface
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}

	// Increment refcount on the pointer we're about to return
	this->AddRef();
	// Return success
	return S_OK;
}

// Increment ref count
ULONG STDMETHODCALLTYPE CVistaVolumeCallback::AddRef()
{
	return ( ++_ref_count );
}

// Decrement ref count
ULONG STDMETHODCALLTYPE CVistaVolumeCallback::Release()
{
	return ( --_ref_count );
}

// Notify objects
HRESULT STDMETHODCALLTYPE CVistaVolumeCallback::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) {

	// Try to acquire mutex on objectsId
	DWORD dwWaitMutex;
	dwWaitMutex = WaitForSingleObject(hObject, INFINITE);

	switch (dwWaitMutex) 
	{
		case WAIT_OBJECT_0: 
		{
			std::vector <DWORD>::iterator Iter;
			for (Iter = this->objectsId->begin( ); Iter != this->objectsId->end( ); Iter++) {

				SD_SCRIPTABLE_EVENT se;
				se.cbSize = sizeof(SD_SCRIPTABLE_EVENT);
				se.flags=0;
				memset(&se.dp, 0, sizeof(DISPPARAMS));

				se.dp.cArgs = 1;
				VARIANT* lpvt = (VARIANT*)malloc(sizeof(VARIANT)*1);
				VariantInit(&lpvt[0]);							
				lpvt[0].vt = VT_I4;

				// Mute event
				if (pNotify->bMuted == TRUE)
				{
					lstrcpy(se.szEventName, "VistaVolume_OnMuteEvent");
					lpvt[0].lVal = 0;
				}
				else // Standard volume event
				{
					lstrcpy(se.szEventName, "VistaVolume_OnVolumeEvent");
					lpvt[0].lVal = (long) ceil(pNotify->fMasterVolume*100);				
				}
				
				se.dp.rgvarg = lpvt;

				SDHostMessage(SD_SCRIPTABLE_PLUGIN_EVENT, (DWORD) *Iter, (DWORD) &se);
				
				free(se.dp.rgvarg);
			}
			break;
		}					

		default:
			return S_FALSE;
			break;
	}

	ReleaseMutex(hObject);

	return S_OK;
}

void CVistaVolumeCallback::addID(DWORD objID) {
	DWORD dwWaitMutex;
	dwWaitMutex = WaitForSingleObject(hObject, INFINITE);

	switch (dwWaitMutex) 
	{
		case WAIT_OBJECT_0: 
			{
				std::vector <DWORD>::iterator Iter;
				for (Iter = objectsId->begin( ); Iter != objectsId->end( ); Iter++) {
					if (objID == *Iter) {
						ReleaseMutex(hObject);
						return;
					}
				}

				objectsId->push_back(objID);
		
				ReleaseMutex(hObject);
			}
			break; 

		default:
			break;
	}
}

void CVistaVolumeCallback::removeID(DWORD objID) {
	DWORD dwWaitMutex;
	dwWaitMutex = WaitForSingleObject(hObject, INFINITE);

	switch (dwWaitMutex) 
	{
		case WAIT_OBJECT_0: 
			{
				std::vector <DWORD>::iterator Iter;
				for (Iter = objectsId->begin( ); Iter != objectsId->end( ); Iter++) {
					if (objID == *Iter) {
						objectsId->erase(Iter);
						ReleaseMutex(hObject);
						return;
					}
				}
		
				ReleaseMutex(hObject);
			}
			break; 

		default:
			break;
	}
}

