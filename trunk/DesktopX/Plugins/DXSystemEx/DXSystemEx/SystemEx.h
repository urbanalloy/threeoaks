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

#pragma once

#include "stdafx.h"
#include <comsvcs.h>

#include "COMError.h"
#include "DXSystemEx.h"

#include "DragDrop/CDropTarget.h"
#include "HTTP/FileDownloader.h"
#include "Instance/CSingleInstance.h"
#include "Monitor/MonitorInfo.h"
#include "Volume/IVolumeEventsConnectionPoint.h"
#include "Archive/Archive.h"

#include <string>
#include <vector>
using namespace std;

// CSystemEx
class ATL_NO_VTABLE CSystemEx :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSystemEx, &CLSID_SystemEx>,
	public IDispatchImpl<ISystemEx, &IID_ISystemEx, &LIBID_DXSystemExLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IConnectionPointContainerImpl<CSystemEx>,
	public CProxy_IVolumeEvents<CSystemEx>,
	public IProvideClassInfo2Impl<&CLSID_SystemEx, &DIID_IVolumeEvents, &LIBID_DXSystemExLib>,
    public ISupportErrorInfo
{
public:
	CSystemEx() {}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_hConfigMutex = NULL;
		m_hwnd = NULL;
		m_objID = NULL;

		m_executableDirectory = L"";
		m_executableName = L"";

		m_pSingleInstance = NULL;
		m_pFileDownloader = NULL;
		m_pDropTarget = NULL;

		config = new Config;

		return S_OK;
	}

	void FinalRelease()
	{
		SAFE_DELETE(config);
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SYSTEMEX)

DECLARE_NOT_AGGREGATABLE(CSystemEx)

BEGIN_COM_MAP(CSystemEx)
	COM_INTERFACE_ENTRY(ISystemEx)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CSystemEx)
	CONNECTION_POINT_ENTRY(__uuidof(IVolumeEvents))
END_CONNECTION_POINT_MAP()

	//////////////////////////////////////////////////////////////////////////
	private:
		enum SignatureType
		{
			SHA1 = 0
		};

		DWORD m_objID;
		string m_guiID;
		HWND m_hwnd;

		// Single instance
		CSingleInstance* m_pSingleInstance;
		wstring m_executableDirectory;
		wstring m_executableName;

		// Drag&Drop
		CDropTarget* m_pDropTarget;

		// Monitors
		HANDLE m_hConfigMutex;
		vector<pair<RECT, bool>> m_monitors;
		static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

		// Downloads
		FileDownloader* m_pFileDownloader;

		// Zip
		CArchive* m_pZipUtility;

		// Misc		
		static bool hasEnding(wstring const &fullString, wstring const &ending);

		//////////////////////////////////////////////////////////////////////////
		// Volume
		//////////////////////////////////////////////////////////////////////////

		// Vista
		HRESULT Vista_get_Volume(int *volume);
		HRESULT Vista_put_Volume(int volume);

		HRESULT Vista_get_Mute(VARIANT_BOOL *isMuted);
		HRESULT Vista_put_Mute(VARIANT_BOOL isMuted);

		HRESULT Vista_get_PeakValue(int *level);

		// XP
		HRESULT XP_get_Volume(int *volume);
		HRESULT XP_put_Volume(int volume);

		HRESULT XP_get_Mute(VARIANT_BOOL *isMuted);
		HRESULT XP_put_Mute(VARIANT_BOOL isMuted);

		HRESULT XP_get_PeakValue(int *level);

	//////////////////////////////////////////////////////////////////////////
	public:
		struct Config {
			bool enableDnD;
			bool enableMonitors;
			bool enableInstance;

			Config() {
				enableDnD = false;
				enableMonitors = true;
				enableInstance = false;
			}
		};

		Config* config;

		void Init(DWORD objID, string guiID, HWND hwnd);
		void Destroy();
		void Terminate();

		void EnableInstance(bool enable);
		void EnableMonitorInfo(bool enable);
		void EnableDragDrop(bool enable);

		HRESULT ExtractCommandLine(LPWSTR commandLine, VARIANT* pArgs, bool extractArgs);

		//////////////////////////////////////////////////////////////////////////
		// ISupportErrorInfo
		//////////////////////////////////////////////////////////////////////////
		STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

		//////////////////////////////////////////////////////////////////////////
		// ISystemEx
		//////////////////////////////////////////////////////////////////////////

		/************************************************************************/
		/* Download                                                             */
		/************************************************************************/
		STDMETHOD(StartDownload)(int id, BSTR remoteUrl, BSTR localPath);
		STDMETHOD(StopDownload)(int id);

		/************************************************************************/
		/* Signature                                                            */
		/************************************************************************/
		STDMETHOD(GetSignature)(BSTR path, int type, BSTR* signature);
		STDMETHOD(VerifySignature)(BSTR path, BSTR signature, int type, VARIANT_BOOL* isValid);

		/************************************************************************/
		/* Command line and single instance                                     */
		/************************************************************************/
		STDMETHOD(get_CommandLine)(BSTR* commandLine);
		STDMETHOD(get_CommandLineArgs)(VARIANT* pArgs);
		STDMETHOD(get_IsFirstInstance)(VARIANT_BOOL* isFirstInstance);
		STDMETHOD(get_ExecutableFolder)(BSTR* executableDirectory);
		STDMETHOD(get_ExecutableName)(BSTR* executableName);

		/************************************************************************/
		/* Monitor                                                              */
		/************************************************************************/

		STDMETHOD(get_NumberOfMonitors)(int* numberOfMonitors);
		STDMETHOD(get_Monitors)(VARIANT* monitors);
		STDMETHOD(GetMonitor)(int index, IMonitorInfo** info);

		/************************************************************************/
		/* Volume                                                               */
		/************************************************************************/

		// Master Volume
		STDMETHOD(get_Volume)(int *volume);
		STDMETHOD(put_Volume)(int volume);

		// Muting state
		STDMETHOD(get_Mute)(VARIANT_BOOL *isMuted);
		STDMETHOD(put_Mute)(VARIANT_BOOL isMuted);

		// Peak level
		STDMETHOD(get_PeakValue)(int *level);

		/************************************************************************/
		/* Zip                                                               */
		/************************************************************************/

		STDMETHOD(GetArchive)(IArchive **zip);
};

OBJECT_ENTRY_AUTO(__uuidof(SystemEx), CSystemEx)
