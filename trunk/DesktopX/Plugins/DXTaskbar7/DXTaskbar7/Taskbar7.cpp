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

	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return;

	// Create an instance of ITaskbarList3
	HRESULT hr = CoCreateInstance(CLSID_TaskbarList, 				
								  NULL, 
								  CLSCTX_INPROC_SERVER, 
								  IID_PPV_ARGS(&m_pTaskbarList));

	// Initialize instance
	if (SUCCEEDED(hr))
		hr = m_pTaskbarList->HrInit();

	// Get the main window handle	
	GetMainWindowHandle();

	// Create instance of ICustomDestinationList  and IApplicationDestinations  
	CoCreateInstance(CLSID_DestinationList, 				
					 NULL, 
					 CLSCTX_INPROC_SERVER, 
					 IID_PPV_ARGS(&m_pCustomDestinationList));

	CoCreateInstance(CLSID_ApplicationDestinations, 				
					 NULL, 
					 CLSCTX_INPROC_SERVER, 
					 IID_PPV_ARGS(&m_pApplicationDestinations));
}

void CTaskbar7::Cleanup()
{	
	// Remove hook
	if (messageHook)
		UnhookWindowsHookEx(messageHook);

	// Unregister this tab
	if (m_parentHwnd)
		m_pTaskbarList->UnregisterTab(m_hwnd);

	SAFE_RELEASE(m_pTaskbarList);
	SAFE_RELEASE(m_pCustomDestinationList);
	SAFE_RELEASE(m_pApplicationDestinations);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Getters & Setters
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTaskbar7::SetHWND(HWND hwnd)
{
	this->m_hwnd = hwnd;
}

void CTaskbar7::SetMainHwnd(HWND hwnd)
{
	this->m_parentHwnd = hwnd;
}

void CTaskbar7::SetObjectID(DWORD id)
{
	this->m_objectID = id;
}

DWORD CTaskbar7::GetProcessID()
{
	return m_processID;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main window handling
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTaskbar7::GetMainWindowHandle()
{
	// Get the process ID	
	GetWindowThreadProcessId(m_hwnd, &m_processID);

	// Enumerate windows and get the main controller window
	EnumWindows((WNDENUMPROC)&CTaskbar7::EnumWindowsProc, (LPARAM) this);
}

void CTaskbar7::HookMainWindowMessages()
{
	DWORD threadID = GetWindowThreadProcessId(m_parentHwnd, NULL);

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

					SDHostMessage(SD_SCRIPTABLE_PLUGIN_EVENT, pTaskbar7->m_objectID, (DWORD) &se);

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
HRESULT CTaskbar7::LoadImageFromFile(wstring path, Bitmap** bitmap)
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
	HRESULT hr = LoadImageFromFile(path, &bitmap);

	if (!SUCCEEDED(hr))
		return hr;

	button->dwMask = THB_ICON | THB_TOOLTIP | THB_FLAGS;
	button->iId = id;	
	bitmap->GetHICON(&button->hIcon);
	wcscpy_s((wchar_t *)&button->szTip, 260, tooltip.c_str());
	button->dwFlags = (THUMBBUTTONFLAGS)flags;
	
	delete bitmap;

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tab Handling
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CTaskbar7::RegisterTab()
{
	if (m_isTabRegistered)
		return;

	m_pTaskbarList->RegisterTab(m_hwnd, m_parentHwnd);	
	m_isTabRegistered = true;
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

STDMETHODIMP CTaskbar7::get_TabHwnd(LONG* hwnd)
{
	*hwnd = (LONG)m_hwnd;

	return S_OK;
}

STDMETHODIMP CTaskbar7::SetTabsIcon(BSTR icon)
{
	// Remove icon if icon path is empty
	if (CComBSTR(icon) == CComBSTR(""))
	{	
		if (m_isWindows7)
			SetClassLong(m_hwnd, GCL_HICON, NULL);

		return S_OK;
	}

	USES_CONVERSION;
	Bitmap* bitmap = NULL;
	HRESULT hr = LoadImageFromFile(OLE2W(icon), &bitmap);

	// return a proper error
	if (!SUCCEEDED(hr))
		return hr;

	// Do nothing on XP & Vista
	if (!m_isWindows7) {
		delete bitmap;
		return S_OK;
	}

	HICON hIcon;
	bitmap->GetHICON(&hIcon);
	SetClassLong(m_hwnd, GCL_HICON, (LONG)hIcon);

	// Cleanup the HICON & Bitmap
	DestroyIcon(hIcon); 
	delete bitmap;

	return S_OK;
}

STDMETHODIMP CTaskbar7::ConfigureTab(BSTR name, LONG after)
{
	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	// If running anything other than a gadget, do nothing
	if (!m_parentHwnd)
		return S_OK;

	// Update name
	USES_CONVERSION;
	SetWindowText(m_hwnd, OLE2CA(name));

	RegisterTab();
	HRESULT hr = m_pTaskbarList->SetTabOrder(m_hwnd, (HWND)after);	

	return hr;
}

STDMETHODIMP CTaskbar7::SetTabActive()
{
	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	// Do nothing if we don't have the main hwnd
	if (!m_parentHwnd)
		return S_OK;

	RegisterTab();
	HRESULT hr = m_pTaskbarList->SetTabActive(m_hwnd, m_parentHwnd, 0);

	return hr;
}

STDMETHODIMP CTaskbar7::RemoveTab()
{
	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	// Do nothing if we don't have the main hwnd
	if (!m_isTabRegistered)
		return S_OK;
	
	HRESULT hr = m_pTaskbarList->UnregisterTab(m_hwnd);
	m_isTabRegistered = false;
	
	return hr;
}


/*************************************
* ThumbBar
*************************************/
STDMETHODIMP CTaskbar7::SetupButton(int id, BSTR image, BSTR tooltip, int flags)
{
	// TODO check args!
	if (m_buttons->size() == 7)
		return CCOMError::DispatchError(INDEX_SIZE_ERR, CLSID_Taskbar7, _T("Error setting up buttons!"), "You cannot have more than 7 buttons on a thumbbar.", 0, NULL);

	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	USES_CONVERSION;
	ThumbButton button;
	button.id = id;
	button.image = OLE2W(image);
	button.tooltip = OLE2W(tooltip);
	button.flags = flags;

	m_buttons->push_back(button);

	return S_OK;
}

STDMETHODIMP CTaskbar7::UpdateButton(int id,BSTR image, BSTR tooltip, int flags)
{
	// TODO check args! (id exists...)

	if (!m_pTaskbarList)
		return S_OK;
	
	USES_CONVERSION;

	THUMBBUTTON button;
	HRESULT hr = LoadButton(id, OLE2W(image), OLE2W(tooltip), flags, &button);
	if (!SUCCEEDED(hr))
		return hr;

	// Do nothing on XP & Vista
	if (m_isWindows7)
		hr = m_pTaskbarList->ThumbBarUpdateButtons(m_hwnd, 1, &button);	

	// Cleanup the HICON
	DestroyIcon(button.hIcon);

	return hr;
}

STDMETHODIMP CTaskbar7::AddButtons()
{
	int size = m_buttons->size();

	// Size > 7 is forbidden by AddButton
	if (size == 0)
		return CCOMError::DispatchError(INDEX_SIZE_ERR, CLSID_Taskbar7, _T("Error adding buttons!"), "You need to setup at least one button before adding buttons to the thumbbar.", 0, NULL);	

	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	if (!m_pTaskbarList)
		return S_OK;

	// Create buttons array
	LPTHUMBBUTTON buttons;
	buttons = (LPTHUMBBUTTON) malloc(sizeof(THUMBBUTTON) * size);

	// Load all buttons
	int index = 0;
	for (vector<ThumbButton>::iterator it = m_buttons->begin(); it != m_buttons->end(); ++it) {			
		LoadButton(((ThumbButton)*it).id, ((ThumbButton)*it).image, ((ThumbButton)*it).tooltip, ((ThumbButton)*it).flags, &buttons[index]);
		index++;
	}

	HRESULT hr = m_pTaskbarList->ThumbBarAddButtons(m_hwnd, size, buttons);	

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
* Overlay
*************************************/
STDMETHODIMP CTaskbar7::SetOverlayIcon(BSTR path, BSTR description)
{
	if (!m_pTaskbarList || m_parentHwnd == NULL)
		return S_OK;

	USES_CONVERSION;
	wstring image(OLE2W(path));
	wstring desc(OLE2W(description));

	// if no path is given, we remove the overlay
	if (image.empty())
	{
		if (m_isWindows7)
			return m_pTaskbarList->SetOverlayIcon(m_parentHwnd, NULL, L"");
		
		return S_OK;
	}

	Bitmap* bitmap = NULL;
	HRESULT hr = LoadImageFromFile(image, &bitmap);

	if (!SUCCEEDED(hr))
		return hr;

	// Do nothing on XP & Vista
	if (!m_isWindows7)
	{
		delete bitmap;
		return S_OK;
	}

	HICON pIcon;
	bitmap->GetHICON(&pIcon);

	// Sets the overlay icon
	hr = m_pTaskbarList->SetOverlayIcon(m_parentHwnd, pIcon, desc.c_str());

	DestroyIcon(pIcon);
	delete bitmap;

	return hr;
}

/*************************************
* Progress
*************************************/

STDMETHODIMP CTaskbar7::SetProgressState(int flag)
{
	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	if (!m_pTaskbarList || m_parentHwnd == NULL)
		return S_OK;

	HRESULT hr = m_pTaskbarList->SetProgressState(m_parentHwnd, (TBPFLAG) flag);

	return hr;
}

STDMETHODIMP CTaskbar7::SetProgressValue(ULONGLONG completed, ULONGLONG total)
{
	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	if (!m_pTaskbarList || m_parentHwnd == NULL)
		return S_FALSE;

	// Update the progress. This call to SetProgressValue cancels the
	// current state and puts the button into normal progress mode.
	HRESULT hr = m_pTaskbarList->SetProgressValue(m_parentHwnd, completed, total);

	return hr;
}

/*************************************
* Tasks and destinations
*************************************/

STDMETHODIMP CTaskbar7::SetAppID(BSTR appID)
{
	// Check appID is valid
	if (CComBSTR(appID) == CComBSTR(""))
		return CCOMError::DispatchError(INDEX_SIZE_ERR, CLSID_Taskbar7, _T("Error setting Application ID!"), "appID cannot be NULL or empty", 0, NULL);	

	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	USES_CONVERSION;
	HRESULT hr = m_pCustomDestinationList->SetAppID(OLE2W(appID));

	if (FAILED(hr))
		return hr;

	hr = m_pApplicationDestinations->SetAppID(OLE2W(appID));

	if (FAILED(hr))
		return hr;

	m_isAppIdSet = true;

	return hr;
}

STDMETHODIMP CTaskbar7::RemoveAllDestinations()
{
	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	return m_pApplicationDestinations->RemoveAllDestinations();
}


STDMETHODIMP CTaskbar7::BeginList(int* maxSlots)
{
	// Default is 10
	*maxSlots = 10;

	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	if (!m_isAppIdSet)
		return S_OK;

	UINT uMaxSlots;
	IObjectArray *poaRemoved;

	HRESULT hr = m_pCustomDestinationList->BeginList(&uMaxSlots, IID_PPV_ARGS(&poaRemoved));

	if (SUCCEEDED(hr)) {
		m_isBeginList = true;
		poaRemoved->Release();
	}

	return hr;
}		

STDMETHODIMP CTaskbar7::CommitList()
{
	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	m_isBeginList = false;

	return m_pCustomDestinationList->CommitList();
}

STDMETHODIMP CTaskbar7::AbortList()
{
	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	m_isBeginList = false;

	return m_pCustomDestinationList->AbortList();
}

STDMETHODIMP CTaskbar7::DeleteList(BSTR appID)
{
	if (CComBSTR(appID) == CComBSTR(""))
		return CCOMError::DispatchError(INDEX_SIZE_ERR, CLSID_Taskbar7, _T("Error deleting list!"), "appID cannot be NULL or empty", 0, NULL);	

	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	USES_CONVERSION;
	return m_pCustomDestinationList->DeleteList(OLE2W(appID));
}


STDMETHODIMP CTaskbar7::AddUserTask(VARIANT tasks)
{
	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	return S_FALSE;
}

STDMETHODIMP CTaskbar7::AppendCategory(BSTR category, VARIANT items)
{
	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	return S_FALSE;
}

STDMETHODIMP CTaskbar7::AppendKnownCategory(int knownDestCategory)
{
	// Do nothing on XP & Vista
	if (!m_isWindows7)
		return S_OK;

	return m_pCustomDestinationList->AppendKnownCategory((KNOWNDESTCATEGORY)knownDestCategory);
}