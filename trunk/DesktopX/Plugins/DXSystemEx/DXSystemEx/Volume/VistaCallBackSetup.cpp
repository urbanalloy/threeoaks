///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXSystemEx - Extended System Information
//
// Copyright (c) 2009-2010, Julien Templier
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
#include "VistaCallBackSetup.h"
#include <wtypes.h>
#include <winerror.h>

#include <ks.h>
#include <mmreg.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <Endpointvolume.h>

CVistaVolumeCallback* pVistaVolumeCallback = NULL;
static IAudioEndpointVolume* pClient = NULL;

void RegisterCallBack() {

	if (pVistaVolumeCallback != NULL)
		return;

	IMMDeviceEnumerator* pEnumerator = NULL;
	IMMDevice* pDevice = NULL;

    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
								  NULL,
								  CLSCTX_INPROC_SERVER,
                                  __uuidof(IMMDeviceEnumerator), // IID_IMMDeviceEnumerator,
                                  (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)

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