///////////////////////////////////////////////////////////////////////////////////////////////
//
// DockletX - Docklet support plugin for DesktopX
//
// Copyright (c) 2006-2010, Julien Templier
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
#include <time.h>

#include <SDPlugin.h>
#include "DockletX.h"
#include "Docklet.h"
#include "Host.h"
#include "Dialogs.h"
#include "version.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DesktopX Plugin
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HINSTANCE dllInstance = NULL;
BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD) = NULL;
map<HWND, Docklet*> dockletMap;

static PLUGIN_DATA* pluginData = NULL;

static ULONG_PTR m_gdiplusToken;
static GdiplusStartupInput gdiplusStartupInput;


void LoadPlugins()
{
	pluginData = new PLUGIN_DATA;
	pluginData->index = -1;

	char rootPath[MAX_PATH];
	Docklet::GetObjectDockFolder(rootPath);

	pluginData->docklets.clear();

	// Check for each docklet ini file
	WIN32_FIND_DATAA FindFileData;
	char docklet_path[MAX_PATH];

	char search[MAX_PATH];
	sprintf_s(search, "%s\\Docklets\\*", rootPath);

	HANDLE hFind = FindFirstFileA(search, &FindFileData);
	while (hFind != INVALID_HANDLE_VALUE && (GetLastError() != ERROR_NO_MORE_FILES) ) {

		// For each directory
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY || FindFileData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
			sprintf_s(docklet_path, "%s\\Docklets\\%s\\*.dll", rootPath, FindFileData.cFileName);

			WIN32_FIND_DATA FindDllData;
			HANDLE hFindDll = FindFirstFileA(docklet_path, (LPWIN32_FIND_DATAA)&FindDllData);
			if (hFindDll != INVALID_HANDLE_VALUE)
			{
				DOCKLET_LIST item;
				sprintf_s(item.path, "%s\\Docklets\\%s\\%s", rootPath, FindFileData.cFileName, FindDllData.cFileName);
				item.info = Docklet::GetInformationFromDll((char*)item.path);
				if (item.info.valid)
					pluginData->docklets.push_back(item);
			}
			FindClose(hFindDll);
		}

		FindNextFileA(hFind, (LPWIN32_FIND_DATAA)&FindFileData);
	}
	FindClose(hFind);
}

void ShowContextMenu(DWORD objID, Docklet* pDocklet, POINT &pt, LPMSG msg )
{
	// Get object position
	RECT objRect;
	SDHostMessage(SD_GET_ABSOLUTE_RECT, objID, (DWORD)&objRect);

	// Show default context menu
	HMENU hMenu = CreatePopupMenu();

	AppendMenu(hMenu, MF_STRING | MF_ENABLED, 1, L"Change docklet");
	AppendMenu(hMenu, MF_STRING | MF_ENABLED, 2, L"Display Properties");
	if (pDocklet->IsLoaded()) {
		AppendMenu(hMenu, MF_SEPARATOR, 3, L"");
		AppendMenu(hMenu, MF_STRING | MF_ENABLED, 4, L"Configure");
	}

	int iCommand = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, objRect.left + pt.x, objRect.top + pt.y, 0, msg->hwnd, NULL);

	DestroyMenu(hMenu);

	if(iCommand <= 0)
		return;

	switch(iCommand)
	{
		case 1:
		{
			// Show the config for the current instance
			DialogBoxParam(dllInstance, MAKEINTRESOURCE(IDD_CONFIG), (HWND)NULL, ConfigurePlugin, (LPARAM)pluginData);

			// Load the selected plugin
			if (pluginData->index != -1) {
				// Clean previous docklet
				pDocklet->Stop();
				pDocklet->SetDockletFile(pluginData->docklets[pluginData->index].path);
				pDocklet->Start(msg->hwnd);
			}
			break;
		}

		case 2:
			pDocklet->DefaultConfigDialog();
			break;

		case 4:
			pDocklet->Configure();
			break;
	}
}


///<summary>
///	Main function
///</summary>
BOOL SDMessage(DWORD objID, DWORD *pluginIndex, UINT messageID, DWORD param1, DWORD param2)
{
	switch (messageID)
	{
		// Query info about this plugin
		case SD_QUERY_PLUGIN_INFO:
		{
			SD_PLUGIN_INFO *pi = (SD_PLUGIN_INFO*) param1;
			lstrcpyA(pi->plugin_name, PLUGIN_NAME);
			lstrcpyA(pi->plugin_author, PLUGIN_AUTHOR);
			lstrcpyA(pi->plugin_capability, PLUGIN_CAPABILITY);
			pi->plugin_version = VERSION_MAJOR*100 + VERSION_MINOR;
			pi->supported_platforms = PLUGIN_PLATFORM;
			pi->supported_hosts = PLUGIN_HOSTS;

			return TRUE;
		}


		// Initialize the plugin DLL
		case SD_INITIALIZE_MODULE:
		{
			dllInstance = (HINSTANCE) param2;
			SDHostMessage = (BOOL (__stdcall *)(UINT, DWORD, DWORD)) param1;

			// GDI+
			GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

			// For drag&drop
			OleInitialize(NULL);

			// Load the list of docklets
			LoadPlugins();

			return TRUE;
		}


		// Create a new instance of the plugin
		case SD_CREATE_PLUGIN:
		{
#ifdef DEBUG
			// Check for expiration date
			struct tm now;
			__time64_t nowTime;
			errno_t err;

			_time64(&nowTime);			    // get current time
			err = _localtime64_s(&now, &nowTime); // convert time to structure

			if (err) // there is no bugs in the program, only features
				goto label_expiration;

			if (now.tm_year + 1900 > EXPIRATION_YEAR)
				goto label_expiration;

			if (now.tm_year + 1900 == EXPIRATION_YEAR)
				if (now.tm_mon > EXPIRATION_MONTH-1)
					goto label_expiration;

			if (now.tm_year + 1900 == EXPIRATION_YEAR)
				if (now.tm_mon == EXPIRATION_MONTH-1)
					if (now.tm_mday > EXPIRATION_DAY)
						goto label_expiration;

#endif
	        Docklet* pDocklet = new Docklet(objID);
	        *pluginIndex = (DWORD)pDocklet;

			DWORD *flags = (DWORD *)param1;
			*flags = SD_FLAG_DRAW |
					 SD_FLAG_DRAW_PPALPHA |
					 SD_FLAG_SUBCLASS |
					 SD_FLAG_SET_SIZE |
					 SD_FLAG_NO_USER_CONFIG;

			return TRUE;

#ifdef DEBUG
label_expiration:
			char message[2000];
			sprintf_s(message, "This beta version of DockletX expired on %d/%d/%d.\n\n Please check http://julien.wincustomize.com or http://www.templier.info for new versions.", EXPIRATION_MONTH, EXPIRATION_DAY, EXPIRATION_YEAR);
			MessageBoxA(NULL, (char *)message, "Beta version expiration!", MB_ICONERROR|MB_OK);

			return FALSE;
#endif
		}


		// Load saved plugin data
		case SD_LOAD_DATA:
		{
			Docklet* pDocklet = (Docklet*) *pluginIndex;

			if (pDocklet == NULL)
				return FALSE;

			// Get our ini file
			char objectDirectory[MAX_PATH];
			char iniFile[MAX_PATH];
			SDHostMessage(SD_GET_OBJECT_DIRECTORY, (DWORD) objectDirectory, 0);
			sprintf_s(iniFile, "%s\\DockletX-%s.ini", objectDirectory, (char *) param1);

			// Load configuration
			pDocklet->LoadConfig(iniFile);

			return TRUE;
		}


		// Configure this instance
		case SD_CONFIGURE:
		{
			Docklet* pDocklet = (Docklet*) *pluginIndex;

			if (pDocklet == NULL)
				return FALSE;

			// Show the config for the current instance
			DialogBoxParam(dllInstance, MAKEINTRESOURCE(IDD_CONFIG), (HWND)param2, ConfigurePlugin, (LPARAM)pluginData);

			// Load the selected plugin
			if (pluginData->index != -1)
				pDocklet->SetDockletFile(pluginData->docklets[pluginData->index].path);

			return TRUE;
		}


		// Duplicate this instance data
		// Only the size of the object as set in the configuration dialog is duplicated
		// All data related to the cairo surface is lost
		case SD_DUPLICATE_PLUGIN:
		{
			Docklet* pDocklet = (Docklet*) *pluginIndex;
			Docklet* pOriginalDocklet = (Docklet*) param2;

			if (pOriginalDocklet == NULL)
				return FALSE;

			pDocklet->SetSize(pOriginalDocklet->GetSize());
			pDocklet->SetDockletFile(pOriginalDocklet->GetDockletFile());

			return TRUE;
		}


		// Get the object size of this instance
		// The plugin has already been configured
		case SD_GET_OBJECT_SIZE:
		{
			Docklet* pDocklet = (Docklet*) *pluginIndex;

			if (pDocklet == NULL)
				return FALSE;

			SIZE *sz = (SIZE *) param1;
			sz->cx = pDocklet->GetSize();
			sz->cy = pDocklet->GetSize();

			return TRUE;
		}


		// Start running this instance of the plugin
		case SD_INITIALIZE_PLUGIN:
		{
			SD_PLUGIN_INIT* init = (SD_PLUGIN_INIT*)param1;

			Docklet* pDocklet = (Docklet*) *pluginIndex;
			if (pDocklet == NULL)
				return FALSE;

			dockletMap[init->hwnd] = pDocklet;
			pDocklet->Start(init->hwnd);

			return TRUE;
		}


		// Draw the contents of the canvas into the passed bitmap
		case SD_DRAW:
		{
			SD_DRAW_INFO* drawInfo = (SD_DRAW_INFO*) param1;
			Docklet* pDocklet = (Docklet*) *pluginIndex;

			if (pDocklet == NULL)
				return FALSE;

			pDocklet->Draw(drawInfo->hdc, drawInfo->hBitmap);
		}
		return TRUE;

		//////////////////////////////////////////////////////////////////////////
		// The plugin receives this message from the host if it has registered
		// itself to hook window messages.
		//
		// The message is not yet processed by the host. The plugin can simply
		// retrieve the info or modify the message parameters.
		//
		// Returns TRUE if the message must be not processed by the host.
		// The plugin must write in param2 the desired value to return to
		// the window proc.
		//
		// Returns FALSE if the message must be processed by the host.
		//////////////////////////////////////////////////////////////////////////
		case SD_WINDOW_MESSAGE:
		{
			Docklet* pDocklet = (Docklet*) *pluginIndex;
			LPMSG msg = reinterpret_cast<LPMSG>(param1);

			// Always send misc messages to process (ex: clock)
			pDocklet->ProcessMessage(msg->message, msg->wParam, msg->lParam);

			POINT pt = {GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};

			// Check for special config menu
			if (msg->wParam == (MK_CONTROL|MK_SHIFT) && msg->message == WM_RBUTTONUP)
			{
				ShowContextMenu(objID, pDocklet, pt, msg);

				// Send fake WM_LBUTTONUP to DesktopX to stop the drag
				SendMessage(pDocklet->GetHWND(), WM_LBUTTONUP, NULL, msg->lParam);

				// Stop processing
				return TRUE;
			}

			// In edit mode, ignore all other messages and let DX process the message
			if (msg->wParam != MK_CONTROL)
				return FALSE;

			switch (msg->message)
			{
				// Right click
				case WM_RBUTTONUP:
				{
					if (!pDocklet->RightButtonClick(pt))
						ShowContextMenu(objID, pDocklet, pt, msg);

					// Send fake WM_LBUTTONUP to DesktopX to stop the drag
					SendMessage(pDocklet->GetHWND(), WM_LBUTTONUP, NULL, msg->lParam);
					break;
				}

				// Double click
				// FIXME: Does not work :(
				case WM_LBUTTONDBLCLK:
					pDocklet->DoubleClick(pt);
					break;

				// Middle button click
				case WM_LBUTTONUP:
					pDocklet->LeftButtonClick(pt);

					// Send fake WM_LBUTTONUP to DesktopX to stop the drag
					SendMessage(pDocklet->GetHWND(), WM_LBUTTONUP, NULL, msg->lParam);
					break;

				// Does not work :(
				case WM_LBUTTONDOWN:
					pDocklet->LeftButtonHeld(pt);
					break;
			}

			// Stop processing
			return TRUE;
		}

		// Save the plugin data before unload
		case SD_SAVE_DATA:
		{
			// We need to come up with an instance ID and register our config file
			// we don't care whether we are in export mode or not
			Docklet* pDocklet = (Docklet*) *pluginIndex;

			if (pDocklet == NULL)
				return FALSE;

			// Use the object ID pointer as our instance ID
			char instanceID[20];
			_ltoa_s((long) objID, instanceID, 10);
			lstrcpyA((char *) param1, instanceID);

			char iniFile[MAX_PATH], name[MAX_PATH];
			SDHostMessage(SD_GET_OBJECT_DIRECTORY, (DWORD) iniFile, 0);
			sprintf_s(name, "DockletX-%s.ini", instanceID);
			sprintf_s(iniFile, "%s\\%s", iniFile, name);

			// Save configuration
			pDocklet->SaveConfig(iniFile, (BOOL)param2);

			SDHostMessage(SD_REGISTER_FILE, (DWORD) name, 0);

			return TRUE;
		}


		// Stop running this instance of the plugin
		case SD_TERMINATE_PLUGIN:
		{
			Docklet* pDocklet = (Docklet*) *pluginIndex;

			if (pDocklet == NULL)
				return FALSE;

			dockletMap.erase(pDocklet->GetHWND());
			pDocklet->Stop();

			return TRUE;
		}


		// Destroy this instance of the plugin
		case SD_DESTROY_PLUGIN:
		{
			Docklet* pDocklet = (Docklet*) *pluginIndex;

			SAFE_DELETE(pDocklet);

			return TRUE;
		}


		// Unload the plugin dll
		case SD_TERMINATE_MODULE:
		{
			SAFE_DELETE(pluginData);
			GdiplusShutdown(m_gdiplusToken);

			// Shutdown COM
			OleUninitialize();

			return TRUE;
		}

	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COM
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CComModule _Module;