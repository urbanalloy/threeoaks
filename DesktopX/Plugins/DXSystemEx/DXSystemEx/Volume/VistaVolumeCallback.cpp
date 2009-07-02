///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXSystemEx - Extended System Information
//
// Copyright (c) 2009, Julien Templier
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
	return InterlockedIncrement(&m_RefCount);;
}

// Decrement ref count
ULONG STDMETHODCALLTYPE CVistaVolumeCallback::Release()
{
	LONG ref = InterlockedDecrement(&m_RefCount);  
	if (ref == 0) 
		delete this; 
	return ref; 
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
				
				// Mute event
				if (pNotify->bMuted == TRUE)
				{
					lstrcpy(se.szEventName, "VistaVolume_OnMuteEvent");
					lpvt[0].vt = VT_BOOL;
					lpvt[0].lVal = VARIANT_TRUE;
				}
				else // Standard volume event
				{
					lstrcpy(se.szEventName, "VistaVolume_OnVolumeEvent");
					lpvt[0].vt = VT_I4;
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

