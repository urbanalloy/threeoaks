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

#pragma once

#include "stdafx.h"
#include <comsvcs.h>

#include "COMError.h"
#include "DXSystemEx.h"
#include "resource.h"

#include "MonitorInfo.h"

#include <string>
#include <vector>
using namespace std;

#define ACQUIRE_MUTEX(mutex) \
	DWORD dwWaitMutex = WaitForSingleObject(mutex, INFINITE); \
	switch (dwWaitMutex) \
{ \
	case WAIT_OBJECT_0: \
{

#define RELEASE_MUTEX(mutex) \
	ReleaseMutex(mutex); \
	break; \
} \
	default: \
	break; \
}

// CAeroColor
class ATL_NO_VTABLE CSystemEx :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSystemEx, &CLSID_SystemEx>,
	public IDispatchImpl<ISystemEx, &IID_ISystemEx, &LIBID_DXSystemExLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public ISupportErrorInfo
{
public:
	CSystemEx() {}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		hConfigMutex = NULL;

		return S_OK;
	}

	void FinalRelease() 
	{
		if (hConfigMutex != NULL)
			CloseHandle(hConfigMutex);
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SystemEx)

DECLARE_NOT_AGGREGATABLE(CSystemEx)

BEGIN_COM_MAP(CSystemEx)
	COM_INTERFACE_ENTRY(ISystemEx)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

	private:	
		HANDLE hConfigMutex;
		vector<pair<RECT, bool>> m_monitors;	

		static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

		
	public:

		void Init(DWORD objID, HWND hwnd);
		void Cleanup();

		void UpdateMonitorInfo();
		
		//////////////////////////////////////////////////////////////////////////
		// ISupportErrorInfo
		//////////////////////////////////////////////////////////////////////////
		STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);	

		//////////////////////////////////////////////////////////////////////////
		// ISystemEx
		//////////////////////////////////////////////////////////////////////////
		STDMETHOD(get_NumberOfScreens)(int* numberOfScreens);
		STDMETHOD(get_Screens)(VARIANT* screens);
		STDMETHOD(GetScreen)(int index, IMonitorInfo** info);
};

OBJECT_ENTRY_AUTO(__uuidof(SystemEx), CSystemEx)
