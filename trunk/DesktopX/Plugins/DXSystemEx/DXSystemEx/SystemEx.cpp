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
#include "SystemEx.h"

#include <wtypes.h>
#include <winerror.h>
#include <math.h>
#include <comdef.h>
#include <Shellapi.h>

// Volume
#include <ks.h>
#include <mmreg.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <EndpointVolume.h>
#include "Volume/MixerAPI.h"

#include "Utils/VersionCheck.h"

// HACK !
static CSystemEx *pSystemEx;
static HANDLE configMutex;

void CSystemEx::Init(DWORD objID, string guiId, HWND hwnd)
{
	m_objID = objID;
	m_guiID = guiId;
	m_hwnd = hwnd;

	// Init instance information
	UpdateInstanceInfo();

	// Init the config mutex
	if (m_hConfigMutex == NULL) {
		char name[MAX_PATH];
		sprintf_s(name, "DXSystemExMutex-%d", objID);
		m_hConfigMutex = CreateMutex(NULL, false, name);
	}

	pSystemEx = this;
	configMutex = m_hConfigMutex;

	UpdateMonitorInfo();
}

void CSystemEx::Terminate()
{
	if (m_pFileDownloader)
		m_pFileDownloader->Cleanup();
}

void CSystemEx::Cleanup()
{
	SAFE_DELETE(m_singleInstance);
	SAFE_DELETE(m_pFileDownloader);

	if (m_hConfigMutex != NULL)
		CloseHandle(m_hConfigMutex);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CSystemEx::UpdateInstanceInfo()
{
	m_singleInstance = new CSingleInstance(m_guiID);
	m_singleInstance->ActivateInstance();

	// Notify the first instance
	if (!m_singleInstance->IsFirstInstance())
	{
		m_singleInstance->NotifyFirstInstance(m_hwnd, GetCommandLine());
		return;
	}

	// This is the first instance, initialize data
	m_singleInstance->CreateFirstInstanceData(m_hwnd);
}

void CSystemEx::UpdateMonitorInfo()
{
	ACQUIRE_MUTEX(m_hConfigMutex)
	m_monitors.clear();
	RELEASE_MUTEX(m_hConfigMutex)

	EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)&CSystemEx::MonitorEnumProc, NULL);
}

//////////////////////////////////////////////////////////////////////////
// Enumerate monitors
BOOL CALLBACK CSystemEx::MonitorEnumProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM)
{
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMonitor, &info);

	ACQUIRE_MUTEX(configMutex)
	pSystemEx->m_monitors.push_back(pair<RECT, bool>(info.rcMonitor, (info.dwFlags == MONITORINFOF_PRIMARY) ? true : false));
	RELEASE_MUTEX(configMutex)

	return true;
}

// Extract command line info
HRESULT CSystemEx::ExtractCommandLine(LPWSTR commandLine, VARIANT* pArgs, bool extractArgs)
{
	USES_CONVERSION;
	HRESULT hr = S_OK;

	// Get command line
	int numArgs = 0;
	LPWSTR* args = CommandLineToArgvW(commandLine, &numArgs);

	// ignore the first element (exe)
	int startIndex = 1;

	// Check for single-exe gadgets
	wstring path((PWSTR)args[0]);
	bool isSingleExe = false;
	size_t slash = path.find_last_of('\\', (size_t)-1);
	if (slash != -1)
	{
		wstring exe = path.substr(slash);

		for (int i = 1; i < numArgs; i++)
		{
			if (hasEnding(wstring((PWSTR)args[i]), exe)) {
				isSingleExe = true;
				startIndex++;
			}
		}
	}

	// Extract exe name and folder
	if (m_executableDirectory.empty()) {
		wstring exeCommand;
		isSingleExe ? exeCommand = wstring((PWSTR)args[1]) : exeCommand = wstring((PWSTR)args[0]);

		size_t slash_end = exeCommand.find_last_of('\\', (size_t)-1);

		m_executableName = wstring(exeCommand.substr(slash_end + 1));
		m_executableDirectory = wstring(exeCommand.substr(0, slash_end + 1));
	}

	if (!extractArgs)
		return S_OK;

	// Create SafeArray of VARIANT BSTRs
	SAFEARRAY *pSA;
	SAFEARRAYBOUND aDim[1];

	aDim[0].lLbound = 0;
	aDim[0].cElements = numArgs - startIndex;

	pSA = SafeArrayCreate(VT_VARIANT, 1, aDim);

	if (pSA != NULL) {
		variant_t vOut;

		for (long l = aDim[0].lLbound; l < (signed)(aDim[0].cElements + aDim[0].lLbound); l++) {
			vOut = args[l + startIndex];

			HRESULT hr = SafeArrayPutElement(pSA, &l, &vOut);

			if (FAILED(hr)) {
				SafeArrayDestroy(pSA); // does a deep destroy
				return hr;
			}
		}
	}

	// return SafeArray as VARIANT
	V_VT(pArgs) = VT_ARRAY | VT_VARIANT;
	V_ARRAY(pArgs)= pSA;

	return hr;
}

bool CSystemEx::hasEnding(std::wstring const &fullString, std::wstring const &ending)
{
	unsigned int lastMatchPos = fullString.rfind(ending); // Find the last occurrence of ending
	bool isEnding = lastMatchPos != std::string::npos; // Make sure it's found at least once

	// If the string was found, make sure that any characters that follow it are the ones we're trying to ignore
	for( int i = lastMatchPos + ending.length(); (i < (signed)fullString.length()) && isEnding; i++)
	{
		if( (fullString[i] != '\n') &&
			(fullString[i] != '\r') )
		{
			isEnding = false;
		}
	}

	return isEnding;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISupportErrorInfo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSystemEx::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_ISystemEx
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISystemEx
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* HTTP Download                                                        */
/************************************************************************/
STDMETHODIMP CSystemEx::DownloadFile(int id, BSTR remoteUrl, BSTR localPath)
{
	// Check input
	if (CComBSTR(remoteUrl) == CComBSTR(""))
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_SystemEx, _T("Invalid remote url"), "Remote url is empty!", 0, NULL);

	if (CComBSTR(localPath) == CComBSTR(""))
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_SystemEx, _T("Invalid file path"), "File path is empty!", 0, NULL);

	if (m_pFileDownloader == NULL)
		m_pFileDownloader = new FileDownloader(m_objID);

	USES_CONVERSION;
	if (!m_pFileDownloader->IsValid(id))
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_SystemEx, _T("ID already exists"), "A download is already in progress with this ID", 0, NULL);

	// Add to the list of file to download
	m_pFileDownloader->Download(id, OLE2A(remoteUrl), OLE2A(localPath));

	return S_OK;
}

STDMETHODIMP CSystemEx::StopDownload(int id)
{
	if (m_pFileDownloader)
		m_pFileDownloader->StopDownload(id);

	return S_OK;
}

/************************************************************************/
/* Signature                                                            */
/************************************************************************/

STDMETHODIMP CSystemEx::VerifySignature(BSTR path, BSTR signature, int type, VARIANT_BOOL* isValid)
{
	// Check input
	if (CComBSTR(path) == CComBSTR(""))
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_SystemEx, _T("Invalid file path"), "File path is empty!", 0, NULL);

	if (CComBSTR(signature) == CComBSTR(""))
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_SystemEx, _T("Invalid signature"), "Signature is empty!", 0, NULL);

	// Only allow SHA1 signature type at this moment
	if (type != 0)
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_SystemEx, _T("Invalid signature type"), "Signature type is invalid!", 0, NULL);

	USES_CONVERSION;

	HCRYPTPROV hProv = NULL;
	HCRYPTHASH hHash = NULL;
	HANDLE hFile = INVALID_HANDLE_VALUE;

	BYTE *pbHash = NULL;
	string hashValue;
	const int BUFFER_SIZE = 4096;
	BYTE pbBuffer[BUFFER_SIZE];

	DWORD dwBytesRead, dwCount;
	DWORD dwHashLen;

	BOOL fResult;
	BOOL fFinished = FALSE;

	// Open file to verify
	hFile = CreateFile(OLE2A(path),
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_SystemEx, _T("Invalid file path"), "File cannot be opened. Check that the path entered is valid.", 0, NULL);

	// Get CryptoAPI context
	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		goto INTERNAL_ERROR;

	// Create hash
	if(!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash))
		goto INTERNAL_ERROR;

	// Loop through file and hash file contents
	do
	{
		dwBytesRead = 0;

		fResult = ReadFile(hFile, pbBuffer, BUFFER_SIZE, &dwBytesRead, NULL);

		if (dwBytesRead == 0) break;

		if (!fResult)
			goto INTERNAL_ERROR;

		fFinished = (dwBytesRead < BUFFER_SIZE);

		fResult = CryptHashData(hHash, pbBuffer, dwBytesRead, 0);
		if (!fResult)
			goto INTERNAL_ERROR;

	} while (fFinished == FALSE);

	// Get the hash value
	dwCount = sizeof(DWORD);
	if(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&dwHashLen, &dwCount, 0))
		goto INTERNAL_ERROR;

	if ((pbHash = (unsigned char*)malloc(dwHashLen)) == NULL)
		goto INTERNAL_ERROR;

	memset(pbHash, 0, dwHashLen);

	if(!CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &dwHashLen, 0))
		goto INTERNAL_ERROR;

	// Convert to string
	char ch[3];
	for(int i = 0; i < (signed)dwHashLen; ++i)
	{
		sprintf_s(ch, 3, "%02x", pbHash[i]);
		hashValue += ch;
	}

	// Check hash against signature
	if (hashValue.compare(string(OLE2A(signature))) == 0)
		*isValid = VARIANT_TRUE;
	else
		*isValid = VARIANT_FALSE;

	// Cleanup
	free(pbHash);
	CloseHandle(hFile);
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);

	return S_OK;

INTERNAL_ERROR:
	// Clean up
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	if (hHash)
		CryptDestroyHash(hHash);
	if (hProv)
		CryptReleaseContext(hProv, 0);
	if (pbHash != NULL)
		free(pbHash);

	return CCOMError::DispatchError(INTERNAL_ERR, CLSID_SystemEx, _T("Internal error"), "Internal error while checking signature.", 0, NULL);
}

/************************************************************************/
/* Command line and single instance                                     */
/************************************************************************/

STDMETHODIMP CSystemEx::get_IsFirstInstance(VARIANT_BOOL* isFirstInstance)
{
	if (m_singleInstance->IsFirstInstance())
		*isFirstInstance = VARIANT_TRUE;
	else
		*isFirstInstance = VARIANT_FALSE;

	return S_OK;
}

/*************************************
* Command Line
*************************************/
STDMETHODIMP CSystemEx::get_CommandLine(BSTR* commandLine)
{
	ClearBSTR(*commandLine);

	LPSTR str = GetCommandLine();

	CComBSTR bstr(str);
	*commandLine = bstr.Detach();

	return S_OK;
}

STDMETHODIMP CSystemEx::get_CommandLineArgs(VARIANT* pArgs)
{
	return ExtractCommandLine(GetCommandLineW(), pArgs, true);
}

STDMETHODIMP CSystemEx::get_ExecutableFolder(BSTR* directory)
{
	ClearBSTR(*directory);

	if (m_executableDirectory.empty())
		ExtractCommandLine(GetCommandLineW(), NULL, false);

	CComBSTR bstr(m_executableDirectory.c_str());
	*directory = bstr.Detach();

	return S_OK;
}

STDMETHODIMP CSystemEx::get_ExecutableName(BSTR* name)
{
	ClearBSTR(*name);

	if (m_executableName.empty())
		ExtractCommandLine(GetCommandLineW(), NULL, false);

	CComBSTR bstr(m_executableName.c_str());
	*name = bstr.Detach();

	return S_OK;
}

/************************************************************************/
/* Monitors                                                             */
/************************************************************************/

STDMETHODIMP CSystemEx::get_NumberOfMonitors(int* numberOfMonitors)
{
	ACQUIRE_MUTEX(m_hConfigMutex)
	*numberOfMonitors = m_monitors.size();
	RELEASE_MUTEX(m_hConfigMutex)

	return S_OK;
}

STDMETHODIMP CSystemEx::get_Monitors(VARIANT* monitors)
{
	ACQUIRE_MUTEX(m_hConfigMutex)

	// Create SafeArray of MonitorInfos
	SAFEARRAY *pSA;
	SAFEARRAYBOUND aDim[1];

	aDim[0].lLbound = 0;
	aDim[0].cElements = m_monitors.size();

	pSA = SafeArrayCreate(VT_VARIANT, 1, aDim);

	if (pSA != NULL) {

		for (long l = aDim[0].lLbound; l < (signed)(aDim[0].cElements + aDim[0].lLbound); l++) {

			VARIANT vOut;
			VariantInit(&vOut);
			vOut.vt = VT_DISPATCH;

			// Init MonitorInfo
			CComObject<CMonitorInfo>* pMonitorInfo;
			CComObject<CMonitorInfo>::CreateInstance(&pMonitorInfo);
			pMonitorInfo->Init(m_monitors[l]);
			pMonitorInfo->QueryInterface(IID_IMonitorInfo, (void**)&vOut.pdispVal);

			HRESULT hr = SafeArrayPutElement(pSA, &l, &vOut);

			if (FAILED(hr)) {
				VariantClear(&vOut);
				SafeArrayDestroy(pSA); // does a deep destroy of source VARIANT

				ReleaseMutex(m_hConfigMutex);
				return hr;
			}

			VariantClear(&vOut);
		}
	}

	// return SafeArray as VARIANT
	V_VT(monitors) = VT_ARRAY | VT_VARIANT;
	V_ARRAY(monitors)= pSA;

	RELEASE_MUTEX(m_hConfigMutex)

	return S_OK;
}

STDMETHODIMP CSystemEx::GetMonitor(int index, IMonitorInfo** info)
{
	if (index < 0 || index > (signed)m_monitors.size() - 1)
		return CCOMError::DispatchError(SYNTAX_ERR, CLSID_SystemEx, _T("Error getting monitor info!"), "Monitor index is invalid.", 0, NULL);

	ACQUIRE_MUTEX(m_hConfigMutex)

	CComObject<CMonitorInfo>* pMonitorInfo;
	CComObject<CMonitorInfo>::CreateInstance(&pMonitorInfo);
	pMonitorInfo->Init(m_monitors[index]);
	pMonitorInfo->QueryInterface(IID_IMonitorInfo, (void**)info);

	RELEASE_MUTEX(m_hConfigMutex)

	return S_OK;
}

/************************************************************************/
/* Volume                                                             */
/************************************************************************/

// Master volume
STDMETHODIMP CSystemEx::put_Volume(int volume)
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

STDMETHODIMP CSystemEx::get_Volume(int *volume)
{
	if (Is_WinVista_or_Later())
		return Vista_get_Volume(volume);
	else
		return XP_get_Volume(volume);
}


// Muting state
STDMETHODIMP CSystemEx::put_Mute(VARIANT_BOOL isMuted)
{
	if (Is_WinVista_or_Later())
		return Vista_put_Mute(isMuted);
	else
		return XP_put_Mute(isMuted);

}

STDMETHODIMP CSystemEx::get_Mute(VARIANT_BOOL *isMuted)
{
	if (Is_WinVista_or_Later())
		return Vista_get_Mute(isMuted);
	else
		return XP_get_Mute(isMuted);

}


// Peak level
STDMETHODIMP CSystemEx::get_PeakValue(int *value)
{
	if (Is_WinVista_or_Later())
		return Vista_get_PeakValue(value);
	else
		return XP_get_PeakValue(value);

}

//////////////////////////////////////////////////////////////////////////
// Vista
//////////////////////////////////////////////////////////////////////////

// Master volume
HRESULT CSystemEx::Vista_put_Volume(int volume)
{
	IMMDeviceEnumerator* pEnumerator = NULL;
	IAudioEndpointVolume* pClient = NULL;
	IMMDevice* pDevice = NULL;

	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
								  __uuidof(IMMDeviceEnumerator), // IID_IMMDeviceEnumerator,
								  (void**)&pEnumerator);
	EXIT_ON_ERROR(hr);

	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	EXIT_ON_ERROR(hr);

	hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), // IID_IAudioClient
						   CLSCTX_INPROC_SERVER,
						   NULL,
						   (void **)&pClient);
	EXIT_ON_ERROR(hr);

	// set volume
	pClient->SetMasterVolumeLevelScalar((float) volume/100, NULL);

Exit:
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pDevice)
	SAFE_RELEASE(pClient)

	return S_OK;
}

HRESULT CSystemEx::Vista_get_Volume(int *volume)
{
	IMMDeviceEnumerator* pEnumerator = NULL;
	IMMDevice* pDevice = NULL;
	IAudioEndpointVolume* pClient = NULL;

	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
								  __uuidof(IMMDeviceEnumerator), // IID_IMMDeviceEnumerator,
								  (void**)&pEnumerator);
	EXIT_ON_ERROR(hr);

	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	EXIT_ON_ERROR(hr);

	hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), // IID_IAudioClient
						   CLSCTX_INPROC_SERVER, NULL,
						   (void **)&pClient);
	EXIT_ON_ERROR(hr);

	// get volume
	float volumeLevel = NULL;
	hr = pClient->GetMasterVolumeLevelScalar(&volumeLevel);
	EXIT_ON_ERROR(hr);

	int scaledVolume = (int) ceil(volumeLevel*100);
	*volume = scaledVolume;

Exit:
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pDevice)
	SAFE_RELEASE(pClient)

	return S_OK;
}


// Muting state
HRESULT CSystemEx::Vista_put_Mute(VARIANT_BOOL isMuted)
{
	IMMDeviceEnumerator* pEnumerator = NULL;
	IAudioEndpointVolume* pClient = NULL;
	IMMDevice* pDevice = NULL;

	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
								  __uuidof(IMMDeviceEnumerator), // IID_IMMDeviceEnumerator,
								  (void**)&pEnumerator);
	EXIT_ON_ERROR(hr);

	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	EXIT_ON_ERROR(hr);

	hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), // IID_IAudioClient
						   CLSCTX_INPROC_SERVER, NULL,
						   (void **)&pClient);
	EXIT_ON_ERROR(hr);

	// set mute
	pClient->SetMute(isMuted == VARIANT_TRUE ? TRUE : FALSE, NULL);

Exit:
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pDevice)
	SAFE_RELEASE(pClient)

	return S_OK;
}

HRESULT CSystemEx::Vista_get_Mute(VARIANT_BOOL *isMuted)
{
	IMMDeviceEnumerator* pEnumerator = NULL;
	IMMDevice* pDevice = NULL;
	IAudioEndpointVolume* pClient = NULL;

	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
								  __uuidof(IMMDeviceEnumerator), // IID_IMMDeviceEnumerator,
								  (void**)&pEnumerator);
	EXIT_ON_ERROR(hr);

	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	EXIT_ON_ERROR(hr);

	hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), // IID_IAudioClient
						   CLSCTX_INPROC_SERVER, NULL,
						   (void **)&pClient);
	EXIT_ON_ERROR(hr);

	// Get the muting state
	BOOL mutingState = NULL;
	hr = pClient->GetMute(&mutingState);
	EXIT_ON_ERROR(hr);

	(mutingState == TRUE) ? *isMuted = VARIANT_TRUE : *isMuted = VARIANT_FALSE;

Exit:
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pDevice)
	SAFE_RELEASE(pClient)

	return S_OK;
}


// Peak level
HRESULT CSystemEx::Vista_get_PeakValue(int *value)
{
	IMMDeviceEnumerator* pEnumerator = NULL;
	IMMDevice* pDevice = NULL;
	IAudioMeterInformation* pClient = NULL;

	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
								  __uuidof(IMMDeviceEnumerator), // IID_IMMDeviceEnumerator,
								 (void**)&pEnumerator);
	EXIT_ON_ERROR(hr);

	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	EXIT_ON_ERROR(hr);

	hr = pDevice->Activate(__uuidof(IAudioMeterInformation), // IID_IAudioClient
						   CLSCTX_INPROC_SERVER,
						   NULL,
						   (void **)&pClient);
	EXIT_ON_ERROR(hr);

	// Get the peak value
	float peak = NULL;
	hr = pClient->GetPeakValue(&peak);
	EXIT_ON_ERROR(hr);

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
HRESULT CSystemEx::XP_put_Volume(int volume)
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


HRESULT CSystemEx::XP_get_Volume(int *volume)
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
HRESULT CSystemEx::XP_put_Mute(VARIANT_BOOL isMuted)
{
	MixerAPI mixer(MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
		NO_SOURCE,
		MIXERCONTROL_CONTROLTYPE_MUTE);

	if (!mixer.IsOk())
		return S_FALSE;

	if (isMuted == VARIANT_TRUE)
		mixer.Off();
	else
		mixer.On();

	return S_OK;
}

HRESULT CSystemEx::XP_get_Mute(VARIANT_BOOL *isMuted)
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

	(results == 1) ? *isMuted = VARIANT_TRUE : *isMuted = VARIANT_FALSE;

	return S_OK;
}


// Peak level
HRESULT CSystemEx::XP_get_PeakValue(int *level)
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
