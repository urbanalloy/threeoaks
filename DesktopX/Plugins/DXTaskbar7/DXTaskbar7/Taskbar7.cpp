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

#include "stdafx.h"
#include "Taskbar7.h"
#include "SDPlugin.h"
#include "dwmapi.h"

// HACK !
static CTaskbar7 *pTaskbar7;
static HHOOK messageHook;

extern BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initialization
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTaskbar7::Init()
{
	messageHook = NULL;

	// Create an instance of ITaskbarList3
	HRESULT hr = CoCreateInstance(CLSID_TaskbarList, 				
		NULL, 
		CLSCTX_INPROC_SERVER, 
		IID_PPV_ARGS(&_pTaskbarList));

	// Initialize instance
	if (SUCCEEDED(hr))
		hr = _pTaskbarList->HrInit();

	// Get the main window handle	
	GetMainWindowHandle();
}

void CTaskbar7::Cleanup()
{	
	// Remove hook
	if (messageHook)
		UnhookWindowsHookEx(messageHook);

	// Unregister this tab
	if (_mainHwnd)
		_pTaskbarList->UnregisterTab(_hwnd);

	SAFE_RELEASE(_pTaskbarList);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Getters & Setters
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTaskbar7::SetHWND(HWND hwnd)
{
	this->_hwnd = hwnd;
}

void CTaskbar7::SetMainHwnd(HWND hwnd)
{
	this->_mainHwnd = hwnd;
}

void CTaskbar7::SetObjectID(DWORD id)
{
	this->_objectID = id;
}

DWORD CTaskbar7::GetProcessID()
{
	return _processID;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main window handling
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTaskbar7::GetMainWindowHandle()
{
	// Get the process ID	
	GetWindowThreadProcessId(_hwnd, &_processID);

	// Enumerate windows and get the main controller window
	EnumWindows((WNDENUMPROC)&CTaskbar7::EnumWindowsProc, (LPARAM) this);
}

void CTaskbar7::HookMainWindowMessages()
{
	DWORD threadID = GetWindowThreadProcessId(_mainHwnd, NULL);

	// Setup our hook
	pTaskbar7 = this;	
	messageHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)&CTaskbar7::WindowProc, NULL, threadID);
}

BOOL CALLBACK CTaskbar7::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	CTaskbar7* taskbar = (CTaskbar7*) lParam;

	// Get the processID of the window
	DWORD processID;
	GetWindowThreadProcessId(hwnd, &processID);

	// Check if this windows is owned by our process
	if (processID != taskbar->GetProcessID())
		return true;

	// Get the class name
	char buffer[1024];
	GetClassName(hwnd, (LPTSTR)&buffer, 1024);

	string className(buffer);
	std::string::size_type endPos = className.rfind("-Ctrl", className.size() - 1);  // search from back to see if it ends in "-Ctrl"
	if (endPos != className.size() - 5)
		return true;

	// this is our controller window!
	taskbar->SetMainHwnd(hwnd);
	taskbar->RegisterTab();

	return false;
}

LRESULT CALLBACK CTaskbar7::WindowProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(messageHook, nCode, wParam, lParam);

	if (nCode == HC_ACTION)
	{
		PMSG msg = (PMSG) lParam;

		switch(msg->message)
		{
			case WM_COMMAND:
			{

				if (HIWORD(msg->wParam) == THBN_CLICKED)
				{
					SD_SCRIPTABLE_EVENT se;
					se.cbSize = sizeof(SD_SCRIPTABLE_EVENT);
					lstrcpy(se.szEventName, "Taskbar_OnButtonClicked");
					se.flags=0;

					memset(&se.dp, 0, sizeof(DISPPARAMS));

					se.dp.cArgs = 1;
					VARIANT* lpvt = (VARIANT*)malloc(sizeof(VARIANT)*1);
					VariantInit(&lpvt[0]);	
					lpvt[0].vt = VT_I4;
					lpvt[0].lVal = LOWORD(msg->wParam); // button id

					se.dp.rgvarg = lpvt;

					SDHostMessage(SD_SCRIPTABLE_PLUGIN_EVENT, pTaskbar7->_objectID, (DWORD) &se);

					free(se.dp.rgvarg);	
				}
				break;
			}
		}

	}

	return CallNextHookEx(messageHook, nCode, wParam, lParam);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions for image loading
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CTaskbar7::LoadImage(wstring path, Bitmap** bitmap)
{
	char error[1000];

	if (path.empty()) {		
		sprintf_s(error, "Path is not valid: %S", path.c_str());		
		return CCOMError::DispatchError(PATH_NOT_FOUND_ERR, CLSID_Taskbar7, _T("Path is not valid"), error, 0, NULL);
	}

	*bitmap = Bitmap::FromFile(path.c_str(), TRUE);

	if (GetLastError() != S_OK) 
	{
		sprintf_s(error, "There was an error loading the image: %S", path.c_str());
		return CCOMError::DispatchError(NOT_SUPPORTED_ERR, CLSID_Taskbar7, _T("Error loading image"), error, 0, NULL);
	}

	return S_OK;
}

HRESULT CTaskbar7::LoadButton(int id, wstring path, wstring tooltip, int flags, THUMBBUTTON* button)
{
	Bitmap* bitmap = NULL;
	HRESULT hr = LoadImage(path, &bitmap);

	if (!SUCCEEDED(hr))
		return hr;

	button->dwMask = THB_ICON | THB_TOOLTIP | THB_FLAGS;
	button->iId = id;	
	bitmap->GetHICON(&button->hIcon);
	wcscpy_s((wchar_t *)&button->szTip, 260, tooltip.c_str());
	button->dwFlags = (THUMBBUTTONFLAGS)flags;

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tab Handling
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTaskbar7::RegisterTab()
{
	if (!_mainHwnd)
		return;

	_pTaskbarList->RegisterTab(_hwnd, _mainHwnd);
	_pTaskbarList->SetTabOrder(_hwnd, NULL);
	_pTaskbarList->SetTabActive(_hwnd, _mainHwnd, NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISupportErrorInfo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CTaskbar7::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ITaskbar7
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ITaskbar7
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*************************************
* Tab management
*************************************/

STDMETHODIMP CTaskbar7::GetTabHwnd(HWND* hwnd)
{
	if (!_mainHwnd)
		return FALSE;

	*hwnd = _mainHwnd;

	return S_OK;
}

STDMETHODIMP CTaskbar7::ConfigureTab(BSTR name, BSTR icon, VARIANT_BOOL visible, HWND after)
{
	return S_OK;
}

STDMETHODIMP CTaskbar7::SetTabActive()
{
	return S_OK;
}

/*************************************
* Thumbnails and live preview
*************************************/
STDMETHODIMP CTaskbar7::SetIconicThumbnail(BSTR image, int flags)
{
	return S_OK;
}

/*************************************
* ThumbBar
*************************************/
STDMETHODIMP CTaskbar7::AddButton(int id, BSTR image, BSTR tooltip, int flags)
{
	if (!_pTaskbarList || _mainHwnd == NULL)
		return S_FALSE;

	// TODO check args!
	USES_CONVERSION;

	ThumbButton button;
	button.id = id;
	button.image = OLE2W(image);
	button.tooltip = OLE2W(tooltip);
	button.flags = flags;

	_buttons->push_back(button);

	return S_OK;
}

STDMETHODIMP CTaskbar7::UpdateButton(int id,BSTR image, BSTR tooltip, int flags)
{
	if (!_pTaskbarList || _mainHwnd == NULL)
		return S_FALSE;
	
	USES_CONVERSION;

	THUMBBUTTON button;
	HRESULT hr = LoadButton(id, OLE2W(image), OLE2W(tooltip), flags, &button);
	if (!SUCCEEDED(hr))
		return hr;

	hr = _pTaskbarList->ThumbBarUpdateButtons(_mainHwnd, 1, &button);	

	// Cleanup the HICON
	DestroyIcon(button.hIcon);

	return S_OK;
}

STDMETHODIMP CTaskbar7::SetupButtons()
{
	if (!_pTaskbarList || _mainHwnd == NULL)
		return S_FALSE;

	int size = _buttons->size();

	// TODO check for number of buttons before adding!
	if (size == 0)
		return CCOMError::DispatchError(INDEX_SIZE_ERR, CLSID_Taskbar7, _T("Error setting up buttons!"), "You need to add at least one button before setting up buttons.", 0, NULL);

	if (size > 7)
		return CCOMError::DispatchError(INDEX_SIZE_ERR, CLSID_Taskbar7, _T("Error setting up buttons!"), "You cannot have more than 7 buttons on a thumbbar.", 0, NULL);

	// Create buttons array
	LPTHUMBBUTTON buttons;
	buttons = (LPTHUMBBUTTON) malloc(sizeof(THUMBBUTTON) * size);

	// Load all buttons
	int index = 0;
	for (vector<ThumbButton>::iterator it = _buttons->begin(); it != _buttons->end(); ++it) {			
		LoadButton(((ThumbButton)*it).id, ((ThumbButton)*it).image, ((ThumbButton)*it).tooltip, ((ThumbButton)*it).flags, &buttons[index]);
		index++;
	}

	HRESULT hr = _pTaskbarList->ThumbBarAddButtons(_hwnd, size, buttons);	

	// Hook messages to be able to get the command messages
	if (SUCCEEDED(hr))
		HookMainWindowMessages();

	// Clean all buttons
	for (int i = 0; i < size; i++) {	
		DestroyIcon(buttons[i].hIcon);
	}

	return hr;
}

/*************************************
* Tasks and destinations
*************************************/


/*************************************
* Overlay
*************************************/
STDMETHODIMP CTaskbar7::SetOverlayIcon(BSTR path, BSTR description)
{
	if (!_pTaskbarList || _mainHwnd == NULL)
		return S_FALSE;

	USES_CONVERSION;
	wstring image(OLE2W(path));
	wstring desc(OLE2W(description));

	// if no path is given, we remove the overlay
	if (image.empty())
	{
		return _pTaskbarList->SetOverlayIcon(_mainHwnd, NULL, L"");
	}

	Bitmap* bitmap = NULL;
	HRESULT hr = LoadImage(image, &bitmap);

	if (!SUCCEEDED(hr))
		return hr;

	HICON pIcon;
	bitmap->GetHICON(&pIcon);

	// Sets the overlay icon
	hr = _pTaskbarList->SetOverlayIcon(_mainHwnd, pIcon, desc.c_str());

	DestroyIcon(pIcon);

	return hr;
}

/*************************************
* Progress
*************************************/

STDMETHODIMP CTaskbar7::SetProgressState(int flag)
{
	if (!_pTaskbarList || _mainHwnd == NULL)
		return S_FALSE;

	HRESULT hr = _pTaskbarList->SetProgressState(_mainHwnd, (TBPFLAG) flag);

	return hr;
}

STDMETHODIMP CTaskbar7::SetProgressValue(ULONGLONG completed, ULONGLONG total)
{
	if (!_pTaskbarList || _mainHwnd == NULL)
		return S_FALSE;

	// Update the progress. This call to SetProgressValue cancels the
	// current state and puts the button into normal progress mode.
	HRESULT hr = _pTaskbarList->SetProgressValue(_mainHwnd, completed, total);

	return hr;
}