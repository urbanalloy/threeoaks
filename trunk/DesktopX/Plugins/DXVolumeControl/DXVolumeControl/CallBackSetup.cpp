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
#include "CallBackSetup.h"
#include <wtypes.h>
#include <winerror.h>

#include <ks.h>
#include <mmreg.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <Endpointvolume.h>

CVistaVolumeCallback* pVistaVolumeCallback = NULL;
static IAudioEndpointVolume* pClient = NULL;

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }



void RegisterCallBack() {

	if (pVistaVolumeCallback != NULL)
		return;

	IMMDeviceEnumerator* pEnumerator = NULL;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
								  NULL,
								  CLSCTX_INPROC_SERVER,
                                  __uuidof(IMMDeviceEnumerator), // IID_IMMDeviceEnumerator,
                                  (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)

	IMMDevice* pDevice = NULL;
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    EXIT_ON_ERROR(hr)

    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), // IID_IAudioClient
                           CLSCTX_INPROC_SERVER, NULL, (void **)&pClient);
    EXIT_ON_ERROR(hr);

	// Register Callback	
	pVistaVolumeCallback = new CVistaVolumeCallback();
	hr = pClient->RegisterControlChangeNotify(pVistaVolumeCallback);
	if (FAILED(hr))
		SAFE_RELEASE(pVistaVolumeCallback);

	return;

Exit:
	SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
}

void UnregisterCallBack() {
	if (pVistaVolumeCallback == NULL)
		goto Exit;

	// Unregister Callback
	pClient->UnregisterControlChangeNotify(pVistaVolumeCallback);

Exit:
	SAFE_RELEASE(pVistaVolumeCallback);
    SAFE_RELEASE(pClient)
}