///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXTaskbar7 - Extended Taskbar Support for Windows 7
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
#include "DXTaskbar7.h"
#include "Taskbar7Errors.h"
#include "resource.h"
#include "shobjidl.h"

#include <gdiplus.h>
using namespace Gdiplus;

#include <string>
#include <vector>
using namespace std;

// CAeroColor
class ATL_NO_VTABLE CTaskbar7 :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTaskbar7, &CLSID_Taskbar7>,
	public IDispatchImpl<ITaskbar7, &IID_ITaskbar7, &LIBID_DXTaskbar7Lib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public ISupportErrorInfo
{
public:
	CTaskbar7() {}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		GdiplusStartup(&_gdiplusToken, &_gdiplusStartupInput, NULL);

		_pTaskbarList = NULL;
		_pCustomDestinationList = NULL;
		_pApplicationDestinations = NULL;
		_hwnd = NULL;
		_objectID = NULL;
		_processID = NULL;
		_mainHwnd = NULL;

		_isTabRegistered = false;
		_isAppIdSet = false;
		_isBeginList = false;

		_buttons = new vector<ThumbButton>();
		return S_OK;
	}

	void FinalRelease() 
	{
		GdiplusShutdown(_gdiplusToken);

		delete _buttons;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_TASKBAR7)

DECLARE_NOT_AGGREGATABLE(CTaskbar7)

BEGIN_COM_MAP(CTaskbar7)
	COM_INTERFACE_ENTRY(ITaskbar7)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

	private:	
		struct ThumbButton
		{
			int id;
			wstring image;
			wstring tooltip;
			int flags;
		};

		ULONG_PTR _gdiplusToken;
		GdiplusStartupInput _gdiplusStartupInput;

		// The COM instances
		ITaskbarList3* _pTaskbarList;
		ICustomDestinationList* _pCustomDestinationList;
		IApplicationDestinations* _pApplicationDestinations;		
		
		// Handle to the DX window
		HWND _hwnd;
		DWORD _objectID;
		
		// Main process window (the one with the taskbar icon)
		HWND _mainHwnd;
		DWORD _processID;

		bool _isTabRegistered;
		bool _isAppIdSet;
		bool _isBeginList;

		// The list of thumbbar buttons to add
		vector<ThumbButton>* _buttons;
	
		void GetMainWindowHandle();	
		static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
		static LRESULT CALLBACK WindowProc(int nCode, WPARAM wParam, LPARAM lParam);

		void RegisterTab();

		HRESULT LoadImage(wstring path, Bitmap** bitmap);
		HRESULT LoadButton(int id, wstring path, wstring tooltip, int flags, THUMBBUTTON* button);
	public:

		// Used by DXTaskbar
		void SetHWND(HWND hwnd);				
		void SetObjectID(DWORD id);
		void Init();
		void Cleanup();

		// Internal use by static methods
		void SetMainHwnd(HWND hwnd);
		DWORD GetProcessID();
		void HookMainWindowMessages();
		
		//////////////////////////////////////////////////////////////////////////
		// ISupportErrorInfo
		//////////////////////////////////////////////////////////////////////////
		STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);	

		//////////////////////////////////////////////////////////////////////////
		// ITaskbar7
		//////////////////////////////////////////////////////////////////////////
		STDMETHOD(get_TabHwnd)(LONG* hwnd);
		STDMETHOD(SetTabsIcon)(BSTR icon);
		STDMETHOD(ConfigureTab)(BSTR name, LONG after);
		STDMETHOD(SetTabActive)();
		STDMETHOD(RemoveTab)();

		// ThumbBar
		STDMETHOD(SetupButton)(int id, BSTR image, BSTR tooltip, int flags);	
		STDMETHOD(UpdateButton)(int id, BSTR image, BSTR tooltip, int flags);	
		STDMETHOD(AddButtons)();

		// Overlay
		STDMETHOD(SetOverlayIcon)(BSTR path, BSTR description);

		// Progress
		STDMETHOD(SetProgressState)(int flag);
		STDMETHOD(SetProgressValue)(ULONGLONG ullCompleted, ULONGLONG ullTotal);

		// Tasks and destinations
		STDMETHOD(SetAppID)(BSTR appID);
		STDMETHOD(RemoveAllDestinations)();

		STDMETHOD(BeginList)(int* maxSlots);		
		STDMETHOD(CommitList)();
		STDMETHOD(AbortList)();
		STDMETHOD(DeleteList)(BSTR appID);
		STDMETHOD(AddUserTask)(VARIANT tasks);
		STDMETHOD(AppendCategory)(BSTR category, VARIANT items);	
		STDMETHOD(AppendKnownCategory)(int knownDestCategory);

};

OBJECT_ENTRY_AUTO(__uuidof(Taskbar7), CTaskbar7)
