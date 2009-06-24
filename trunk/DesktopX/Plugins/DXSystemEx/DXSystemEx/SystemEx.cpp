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
#include "SystemEx.h"

// HACK !
static CSystemEx *pSystemEx;
static HANDLE configMutex;

void CSystemEx::Init(DWORD objID, HWND hwnd)
{
	// init the config mutex
	if (hConfigMutex == NULL) {
		char name[MAX_PATH];
		sprintf_s(name, "DXSystemExMutex-%d", objID);
		hConfigMutex = CreateMutex(NULL, false, name);
	}

	pSystemEx = this;	
	configMutex = hConfigMutex;

	UpdateMonitorInfo();
}

void CSystemEx::Cleanup()
{

}

void CSystemEx::UpdateMonitorInfo()
{
	ACQUIRE_MUTEX(hConfigMutex)
	m_monitors.clear();
	RELEASE_MUTEX(hConfigMutex)

	EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)&CSystemEx::MonitorEnumProc, NULL);
}

//////////////////////////////////////////////////////////////////////////
// Enumerate monitors
BOOL CALLBACK CSystemEx::MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMonitor, &info);

	ACQUIRE_MUTEX(configMutex)
	pSystemEx->m_monitors.push_back(pair<RECT, bool>(info.rcMonitor, (info.dwFlags == MONITORINFOF_PRIMARY) ? true : false));
	RELEASE_MUTEX(configMutex)

	return true;
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

STDMETHODIMP CSystemEx::get_NumberOfScreens(int* numberOfScreens)
{
	ACQUIRE_MUTEX(hConfigMutex)
	*numberOfScreens = m_monitors.size();
	RELEASE_MUTEX(hConfigMutex)

	return S_OK;
}

STDMETHODIMP CSystemEx::get_Screens(VARIANT* screens)
{
	ACQUIRE_MUTEX(hConfigMutex)

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

				ReleaseMutex(hConfigMutex);
				return hr;
			}
			
			VariantClear(&vOut);
		}
	}

	// return SafeArray as VARIANT
	V_VT(screens) = VT_ARRAY | VT_VARIANT;
	V_ARRAY(screens)= pSA;

	RELEASE_MUTEX(hConfigMutex)

	return S_OK;
}

STDMETHODIMP CSystemEx::GetScreen(int index, IMonitorInfo** info)
{
	if (index < 0 || index > (signed)m_monitors.size() - 1)		
		return CCOMError::DispatchError(1, CLSID_SystemEx, _T("Error getting monitor info!"), "Monitor index is invalid.", 0, NULL);	

	ACQUIRE_MUTEX(hConfigMutex)

	CComObject<CMonitorInfo>* pMonitorInfo;
	CComObject<CMonitorInfo>::CreateInstance(&pMonitorInfo);
	pMonitorInfo->Init(m_monitors[index]);
	pMonitorInfo->QueryInterface(IID_IMonitorInfo, (void**)info);

	RELEASE_MUTEX(hConfigMutex)

	return S_OK;

}
