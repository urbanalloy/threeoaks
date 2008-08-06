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

#include <wtypes.h>
#include <winerror.h>
#include <math.h>

#include <ks.h>
#include <mmreg.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <EndpointVolume.h>

#include "VistaVolume.h"

#include "MixerAPI.h"
#include "VersionCheck.h"

//////////////////////////////////////////////////////////////////////////
// Master volume
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CVistaVolume::put_Volume(int volume)
{
	// volume should be between 0 and 100
	if (volume < 0)
		volume = 0;
	if (volume > 100)
		volume = 100;

	if (Is_WinVista_or_Later())
		return Vista_put_Volume(volume);
	else
		return XP_put_Volume(volume);
}

STDMETHODIMP CVistaVolume::get_Volume(int *volume)
{
	if (Is_WinVista_or_Later())
		return Vista_get_Volume(volume);
	else
		return XP_get_Volume(volume);
}


//////////////////////////////////////////////////////////////////////////
// Muting state
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CVistaVolume::put_Mute(BOOL isMuted)
{
	// We are getting a VBScript BOOL, convert it to a C++ BOOL
	BOOL muted = TRUE;
	isMuted == VARIANT_FALSE ? muted = FALSE : muted = TRUE;

	if (Is_WinVista_or_Later())
		return Vista_put_Mute(muted);
	else
		return XP_put_Mute(muted);
	
}

STDMETHODIMP CVistaVolume::get_Mute(VARIANT *isMuted)
{
	if (Is_WinVista_or_Later())
		return Vista_get_Mute(isMuted);
	else
		return XP_get_Mute(isMuted);
	
}


//////////////////////////////////////////////////////////////////////////
// Peak level
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CVistaVolume::get_PeakValue(int *value)
{
	if (Is_WinVista_or_Later())
		return Vista_get_PeakValue(value);
	else
		return XP_get_PeakValue(value);
	
}

//////////////////////////////////////////////////////////////////////////
// Vista
//////////////////////////////////////////////////////////////////////////

#define EXIT_ON_ERROR(hres)\
	if (FAILED(hres)) { goto Exit; }

#define SAFE_RELEASE(punk)\
	if ((punk) != NULL)\
	{ (punk)->Release(); (punk) = NULL; }
#define TO_VARIANT(var, value)\
	var->vt = VT_I4;\
	var->lVal = value;


// Master volume
HRESULT CVistaVolume::Vista_put_Volume(int volume)
{
	IMMDeviceEnumerator* pEnumerator = NULL;

	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
								  __uuidof(IMMDeviceEnumerator), // IID_IMMDeviceEnumerator,
								  (void**)&pEnumerator);
	EXIT_ON_ERROR(hr)

	IMMDevice* pDevice = NULL;

	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	EXIT_ON_ERROR(hr)

	IAudioEndpointVolume* pClient = NULL;

	hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), // IID_IAudioClient
						   CLSCTX_INPROC_SERVER, NULL, (void **)&pClient);
	EXIT_ON_ERROR(hr)

	// set volume
	pClient->SetMasterVolumeLevelScalar((float) volume/100, NULL);

Exit:
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pDevice)
	SAFE_RELEASE(pClient)

	return S_OK;
}

HRESULT CVistaVolume::Vista_get_Volume(int *volume)
{
	IMMDeviceEnumerator* pEnumerator = NULL;

	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
							  	  __uuidof(IMMDeviceEnumerator), // IID_IMMDeviceEnumerator,
							  	  (void**)&pEnumerator);
	EXIT_ON_ERROR(hr)

	IMMDevice* pDevice = NULL;

	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	EXIT_ON_ERROR(hr)

	IAudioEndpointVolume* pClient = NULL;

	hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), // IID_IAudioClient
						   CLSCTX_INPROC_SERVER, NULL, (void **)&pClient);
	EXIT_ON_ERROR(hr)

	// get volume
	float volumeLevel = NULL;
	hr = pClient->GetMasterVolumeLevelScalar(&volumeLevel);
	EXIT_ON_ERROR(hr)

	int scaledVolume = (int) ceil(volumeLevel*100);
	*volume = scaledVolume;

Exit:
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pDevice)
	SAFE_RELEASE(pClient)

	return S_OK;
}


// Muting state
HRESULT CVistaVolume::Vista_put_Mute(BOOL isMuted)
{
	IMMDeviceEnumerator* pEnumerator = NULL;

	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
								  __uuidof(IMMDeviceEnumerator), // IID_IMMDeviceEnumerator,
								  (void**)&pEnumerator);
	EXIT_ON_ERROR(hr)

	IMMDevice* pDevice = NULL;

	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	EXIT_ON_ERROR(hr)

	IAudioEndpointVolume* pClient = NULL;

	hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), // IID_IAudioClient
						   CLSCTX_INPROC_SERVER, NULL, (void **)&pClient);
	EXIT_ON_ERROR(hr)

	// set mute
	pClient->SetMute(isMuted, NULL);

Exit:
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pDevice)
	SAFE_RELEASE(pClient)

	return S_OK;
}

HRESULT CVistaVolume::Vista_get_Mute(VARIANT *isMuted)
{
	IMMDeviceEnumerator* pEnumerator = NULL;

	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
							      __uuidof(IMMDeviceEnumerator), // IID_IMMDeviceEnumerator,
								  (void**)&pEnumerator);
	EXIT_ON_ERROR(hr)

	IMMDevice* pDevice = NULL;

	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	EXIT_ON_ERROR(hr)

	IAudioEndpointVolume* pClient = NULL;

	hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), // IID_IAudioClient
						   CLSCTX_INPROC_SERVER, NULL, (void **)&pClient);
	EXIT_ON_ERROR(hr)

	// Get the muting state
	BOOL mutingState = NULL;
	hr = pClient->GetMute(&mutingState);
	EXIT_ON_ERROR(hr)

	if (mutingState == TRUE) 
	{
		TO_VARIANT(isMuted, VARIANT_TRUE)
	}
	else
	{
		TO_VARIANT(isMuted, VARIANT_FALSE)
	}

Exit:	
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pDevice)
	SAFE_RELEASE(pClient)

	return S_OK;
}


// Peak level
HRESULT CVistaVolume::Vista_get_PeakValue(int *value)
{
	IMMDeviceEnumerator* pEnumerator = NULL;

	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
								  __uuidof(IMMDeviceEnumerator), // IID_IMMDeviceEnumerator,
								  (void**)&pEnumerator);
	EXIT_ON_ERROR(hr)

	IMMDevice* pDevice = NULL;

	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	EXIT_ON_ERROR(hr)

	IAudioMeterInformation* pClient = NULL;

	hr = pDevice->Activate(__uuidof(IAudioMeterInformation), // IID_IAudioClient
						   CLSCTX_INPROC_SERVER, NULL, (void **)&pClient);
	EXIT_ON_ERROR(hr)

	// Get the peak value
	float peak = NULL;
	hr = pClient->GetPeakValue(&peak);
	EXIT_ON_ERROR(hr)

	int scaledValue = (int) ceil(peak*100);
	*value = scaledValue;

Exit:	
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pDevice)
	SAFE_RELEASE(pClient)

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
// XP
//////////////////////////////////////////////////////////////////////////

// Master Volume
HRESULT CVistaVolume::XP_put_Volume(int volume)
{
	MixerAPI mixer(MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
				   NO_SOURCE,
				   MIXERCONTROL_CONTROLTYPE_VOLUME);

	if (!mixer.IsOk()) 
		return S_FALSE;
	
	// Volume should be between 0 and 0xFFFF
	DWORD vol = (volume*0xFFFF)/100;
	mixer.SetControlValue(&vol);

	return S_OK;
}


HRESULT CVistaVolume::XP_get_Volume(int *volume)
{
	MixerAPI mixer(MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
				   NO_SOURCE,
				   MIXERCONTROL_CONTROLTYPE_VOLUME);

	if (!mixer.IsOk()) 
		return S_FALSE;

	DWORD* results;
	mixer.GetControlValue(&results);

	// Volume is between 0 and 0xFFF
	double value = (double)*results;
	int scaledVolume = (int) ceil(value*100/0xFFFF);
	*volume = scaledVolume;

	// cleanup
	delete[] results;

	return S_OK;
}


// Muting state
HRESULT CVistaVolume::XP_put_Mute(BOOL isMuted)
{
	MixerAPI mixer(MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
				   NO_SOURCE,
				   MIXERCONTROL_CONTROLTYPE_MUTE);

	if (!mixer.IsOk()) 
		return S_FALSE;

	if (isMuted)
		mixer.Off();
	else
		mixer.On();

	return S_OK;
}

HRESULT CVistaVolume::XP_get_Mute(VARIANT *isMuted)
{
	MixerAPI mixer(MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
				   NO_SOURCE,
				   MIXERCONTROL_CONTROLTYPE_MUTE);

	if (!mixer.IsOk()) 
		return S_FALSE;

	LONG results;
	BOOL ret = mixer.GetMuteValue(&results);
	if (ret == FALSE)
		return S_FALSE;

	if (results == 1) 
	{
		TO_VARIANT(isMuted, VARIANT_TRUE)
	}
	else
	{
		TO_VARIANT(isMuted, VARIANT_FALSE)
	}

	return S_OK;
}


// Peak level
HRESULT CVistaVolume::XP_get_PeakValue(int *level)
{
	MixerAPI mixer(MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT,
				   NO_SOURCE,
				   MIXERCONTROL_CONTROLTYPE_PEAKMETER);

	if (!mixer.IsOk())
	{
		*level = 100;
		return S_FALSE;
	}

	DWORD* results;
	mixer.GetControlValue(&results);

	*level = (int)*results;

	return S_OK;
}