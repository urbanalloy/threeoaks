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

#include <SDPlugin.h>
#include <SDScriptedPlugin.h>

#include <time.h>
#include "Config.h"
#include "resource.h"
#include "DXTaskbar7.h"
#include "dlldatax.h"
#include "Taskbar7.h"
#include "shobjidl.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DesktopX Plugin
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD) = NULL;

static HINSTANCE dllInstance = NULL;
static HANDLE processHandle = NULL;

DECLARE_DXPLUGIN_READTYPEINFO(ReadTaskbar7TypeInfo, IID_ITaskbar7)

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
			lstrcpy(pi->plugin_name, PLUGIN_NAME);
			lstrcpy(pi->plugin_author, PLUGIN_AUTHOR);
			lstrcpy(pi->plugin_capability, PLUGIN_CAPABILITY);
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

			return TRUE;
		}
		

		// Create a new instance of the plugin
		case SD_CREATE_PLUGIN:
		{
			*pluginIndex = NULL;

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

			DWORD *flags = (DWORD *) param1;
			*flags = SD_FLAG_SUBCLASS | 
					 SD_FLAG_NO_BUILDER_CONFIG|
					 SD_FLAG_NO_USER_CONFIG;

			// Check that we are running on Windows 7, otherwise, use stub instance
			OSVERSIONINFO versionInfo;
			versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

			GetVersionEx(&versionInfo);

			// XP or lower
			if (versionInfo.dwMajorVersion < 6)
				return FALSE;

			// Vista
			if (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion < 1)
				return FALSE;

	        CComObject<CTaskbar7>* pTaskbar;
	        CComObject<CTaskbar7>::CreateInstance(&pTaskbar);	
			
	        *pluginIndex = (DWORD)pTaskbar;

			SCRIPTABLEPLUGIN sp;
			strcpy_s(sp.szName, "Taskbar");
			pTaskbar->QueryInterface(IID_IUnknown, (void**)&sp.pUnk);
			sp.pTI =  ReadTaskbar7TypeInfo(dllInstance);
			SDHostMessage(SD_REGISTER_SCRIPTABLE_PLUGIN, objID, (DWORD)&sp);

			return TRUE;

#ifdef DEBUG
label_expiration:		
			char message[2000];
			sprintf_s(message, "This beta version of DXTaskbar7 expired on %d/%d/%d.\n\n Please check http://julien.wincustomize.com or http://www.templier.info for new versions.", EXPIRATION_MONTH, EXPIRATION_DAY, EXPIRATION_YEAR);
			MessageBox(NULL, (char *)message, "Beta version expiration!", MB_ICONERROR|MB_OK);

			return FALSE;
#endif
		}
		

		// Load saved plugin data
		case SD_LOAD_DATA:
		{
			return TRUE;
		}
		

		// Configure this instance
		case SD_CONFIGURE:
		{
			return TRUE;
		}


		// Duplicate this instance data
		case SD_DUPLICATE_PLUGIN:
		{
			return TRUE;
		}		

		// Start running this instance of the plugin
		case SD_INITIALIZE_PLUGIN:
		{
			CComObject<CTaskbar7>* pTaskbar7 = (CComObject<CTaskbar7>*) *pluginIndex;	

			if (pTaskbar7 == NULL)
				return FALSE;

			// Store our window handle
			SD_PLUGIN_INIT* pluginInit = (SD_PLUGIN_INIT*)param1;
			pTaskbar7->SetHWND(pluginInit->hwnd);
			pTaskbar7->SetObjectID(objID);

			pTaskbar7->Init();

			return TRUE;
		}

		case SD_WINDOW_MESSAGE:
		{
			LPMSG msg = reinterpret_cast<LPMSG>(param1);

			switch(msg->message)
			{
				case WM_CLOSE:
				{
					SD_SCRIPTABLE_EVENT se;
					se.cbSize = sizeof(SD_SCRIPTABLE_EVENT);
					lstrcpy(se.szEventName, "Taskbar_OnCloseTab");
					se.flags=0;

					memset(&se.dp, 0, sizeof(DISPPARAMS));

					se.dp.cArgs = 0;

					SDHostMessage(SD_SCRIPTABLE_PLUGIN_EVENT, objID, (DWORD) &se);

					free(se.dp.rgvarg);						

					return TRUE;
				}
			}

			return FALSE;
		}

		// Save the plugin data before unload
		case SD_SAVE_DATA:
		{
			return TRUE;
		}
		

		// Stop running this instance of the plugin
		case SD_TERMINATE_PLUGIN:
		{
			return TRUE;
		}


		// Destroy this instance of the plugin
		case SD_DESTROY_PLUGIN:
		{
			CComObject<CTaskbar7>* pTaskbar7 = (CComObject<CTaskbar7>*) *pluginIndex;				
			
			if (pTaskbar7 != NULL)
				pTaskbar7->Cleanup();

			SAFE_RELEASE(pTaskbar7);	
			
			return TRUE;
		}


		// Unload the plugin dll
		case SD_TERMINATE_MODULE:
			return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COM
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_Taskbar7, CTaskbar7)
END_OBJECT_MAP()

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DLL
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DECLARE_DXPLUGIN_DLLFUNCTIONS(LIBID_DXTaskbar7Lib,
							  "DXTaskbar7.dll",
							  "{20BD78A3-CDA1-47f9-A505-93D1990252C9}", // class
							  "DXTaskbar7.Taskbar7.1",
							  "{8C650B9D-E2A4-4980-9CED-3A38D91F022B}", // type library
							  "DXTaskbar7.Taskbar7",
							  "1.0",
							  "DXTaskbar7 1.0 Type Library")