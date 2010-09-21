///////////////////////////////////////////////////////////////////////////////////////////////
//
// Canvas Plugin for DesktopX
//
// Copyright (c) 2008-2010, Julien Templier
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
#include <SDScriptedPlugin.h>
#include "Config.h"
#include "resource.h"
#include "DXCanvas.h"
#include "Canvas.h"
#include "dlldatax.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Configuration Dialog
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Size of the version text
#define VERSION_SIZE 30
#define MAX_NUMBER_SIZE 5

INT_PTR CALLBACK ConfigurePlugin(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{

	CCanvas::CONFIG_CANVAS* config = (CCanvas::CONFIG_CANVAS*) GetProp(hDlg, "config");

	switch(iMsg)
	{
		case WM_INITDIALOG:
		{
			// Set the version text
			char version[VERSION_SIZE];
			sprintf_s(version, VERSION_SIZE*sizeof(char), "v%i.%i Build %i",VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
			SetDlgItemText(hDlg, IDC_BUILD,version);

			// Get Data
			SetProp(hDlg, "config", (HANDLE) lParam);
			config = (CCanvas::CONFIG_CANVAS*) GetProp(hDlg, "config");

			if(!config) {
				MessageBox(hDlg, "Error initializing Dialog: config not present", "Initialization Error", MB_OK|MB_ICONERROR);
				EndDialog(hDlg, 0);
			}

			// Init & fill the controls
			SendDlgItemMessage(hDlg, IDC_WIDTH, EM_SETLIMITTEXT, MAX_NUMBER_SIZE, (LPARAM)NULL);
			SendDlgItemMessage(hDlg, IDC_HEIGHT, EM_SETLIMITTEXT, MAX_NUMBER_SIZE, (LPARAM)NULL);

			SetDlgItemInt(hDlg, IDC_WIDTH, config->width, FALSE);
			SetDlgItemInt(hDlg, IDC_HEIGHT, config->height, FALSE);
			CheckDlgButton(hDlg, IDC_ALPHABLEND, config->alphablend);

#ifdef DEBUG
			SetDlgItemText(hDlg, IDC_BETA, "BETA - DO NOT REDISTRIBUTE");
#endif

			break;
		}

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{

				case IDOK:
				{
					if(!config)
						break;

					// Input boxes are set to number, so no need to check for conversion (famous last words :P)
					config->width = GetDlgItemInt(hDlg, IDC_WIDTH, NULL, FALSE);
					config->height = GetDlgItemInt(hDlg, IDC_HEIGHT, NULL, FALSE);
					IsDlgButtonChecked(hDlg, IDC_ALPHABLEND) ? config->alphablend = true : config->alphablend = false;
				}

				case IDCANCEL:
					EndDialog(hDlg, 0);
					return TRUE;
					break;

			}
			break;

		case WM_DESTROY:
			RemoveProp(hDlg, "config");
			break;

		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return FALSE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DesktopX Plugin
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD) = NULL;

static HINSTANCE dllInstance = NULL;

DECLARE_DXPLUGIN_READTYPEINFO(ReadCanvasTypeInfo, IID_ICanvas)
int WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int iValue, LPCTSTR lpFileName);

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

	        CComObject<CCanvas>* pCanvas;
	        CComObject<CCanvas>::CreateInstance(&pCanvas);
			pCanvas->setObjID(objID);

	        *pluginIndex = (DWORD)pCanvas;

			SCRIPTABLEPLUGIN sp;
			strcpy_s(sp.szName, "canvas");
			pCanvas->QueryInterface(IID_IUnknown, (void**)&sp.pUnk);
			sp.pTI =  ReadCanvasTypeInfo(dllInstance);
			SDHostMessage(SD_REGISTER_SCRIPTABLE_PLUGIN, objID, (DWORD)&sp);

			DWORD *flags = (DWORD *)param1;
			*flags = SD_FLAG_DRAW |
					 SD_FLAG_DRAW_PPALPHA |
					 SD_FLAG_SET_SIZE |
					 SD_FLAG_NO_USER_CONFIG;

			return TRUE;

#ifdef DEBUG
label_expiration:
			*pluginIndex = NULL;

			char message[2000];
			sprintf_s(message, "This beta version of DXCanvas expired on %d/%d/%d.\n\n Please check http://julien.wincustomize.com or http://www.templier.info for new versions.", EXPIRATION_MONTH, EXPIRATION_DAY, EXPIRATION_YEAR);
			MessageBox(NULL, (char *)message, "Beta version expiration!", MB_ICONERROR|MB_OK);

			return FALSE;
#endif
		}


		// Load saved plugin data
		case SD_LOAD_DATA:
		{
			CComObject<CCanvas>* pCanvas = (CComObject<CCanvas>*) *pluginIndex;

			if (pCanvas == NULL)
				return FALSE;

			// Get our ini file
			char objectDirectory[MAX_PATH];
			char iniFile[MAX_PATH];
			SDHostMessage(SD_GET_OBJECT_DIRECTORY, (DWORD) objectDirectory, 0);
			sprintf_s(iniFile, "%s\\DXCanvas-%s.ini", objectDirectory, (char *) param1);

			// Save configuration
			pCanvas->config->width = GetPrivateProfileInt("Config", "Width", DEFAULT_WIDTH, iniFile);
			pCanvas->config->height = GetPrivateProfileInt("Config", "Height", DEFAULT_HEIGHT, iniFile);
			pCanvas->config->alphablend = (GetPrivateProfileInt("Config", "AlphaBlend", 1, iniFile) == 1);

			// Update the plugin flags - by default we use alpha blending, so only set it to false if needed
			DWORD *plugin_flags = (DWORD*) param2;
			if (!pCanvas->config->alphablend)
				*plugin_flags &= ~SD_FLAG_DRAW_PPALPHA;

			return TRUE;
		}


		// Configure this instance
		case SD_CONFIGURE:
		{
			CComObject<CCanvas>* pCanvas = (CComObject<CCanvas>*) *pluginIndex;

			if (pCanvas == NULL)
				return FALSE;

			// Show the config for the current instance
			DialogBoxParam(dllInstance, MAKEINTRESOURCE(IDD_CONFIG), (HWND)param2, ConfigurePlugin, (LPARAM)pCanvas->config);

			// Update flags
			DWORD* plugin_flags = (DWORD*) param1;

			// Check that it's not already set, as we may have modified it during LOAD_DATA
			if ((*plugin_flags & SD_FLAG_DRAW_PPALPHA) == SD_FLAG_DRAW_PPALPHA)
			{
				if (!pCanvas->config->alphablend) // remove flag
					*plugin_flags &= ~SD_FLAG_DRAW_PPALPHA;
			} else {
				if (pCanvas->config->alphablend) // add flag
					*plugin_flags ^= SD_FLAG_DRAW_PPALPHA;
			}

			return TRUE;
		}


		// Duplicate this instance data
		// Only the size of the object as set in the configuration dialog is duplicated
		// All data related to the cairo surface is lost
		case SD_DUPLICATE_PLUGIN:
		{
			CComObject<CCanvas>* pCanvas = (CComObject<CCanvas>*) *pluginIndex;
			CComObject<CCanvas>* pOriginalCanvas = (CComObject<CCanvas>*) param2;

			if (pOriginalCanvas == NULL)
				return FALSE;

			pCanvas->config->alphablend = pOriginalCanvas->config->alphablend;
			pCanvas->config->width = pOriginalCanvas->config->width;
			pCanvas->config->height = pOriginalCanvas->config->height;
			strcpy_s(pCanvas->config->objectDirectory, pOriginalCanvas->config->objectDirectory);

			return TRUE;
		}


		// Get the object size of this instance
		// The plugin has already been configured
		case SD_GET_OBJECT_SIZE:
		{
			CComObject<CCanvas>* pCanvas = (CComObject<CCanvas>*) *pluginIndex;

			if (pCanvas == NULL)
				return FALSE;

			SIZE *sz = (SIZE *) param1;
			sz->cx = pCanvas->config->width;
			sz->cy = pCanvas->config->height;

			return TRUE;
		}


		// Start running this instance of the plugin
		case SD_INITIALIZE_PLUGIN:
		{
			CComObject<CCanvas>* pCanvas = (CComObject<CCanvas>*) *pluginIndex;

			if (pCanvas == NULL)
				return FALSE;

			char objectDirectory[MAX_PATH];
			SDHostMessage(SD_GET_OBJECT_DIRECTORY, (DWORD) objectDirectory, 0);
			strcpy_s(pCanvas->config->objectDirectory, objectDirectory);

			// Retrieve object name
			char objectName[100];
			strcpy_s(objectName, "");
			SDHostMessage(SD_GUIID_FROM_OBJID, objID, (DWORD)objectName);
			strcpy_s(pCanvas->config->objectName, objectName);

			pCanvas->initCairo();

			return TRUE;
		}


		// Draw the contents of the canvas into the passed bitmap
		case SD_DRAW:
		{
			SD_DRAW_INFO* drawInfo = (SD_DRAW_INFO*) param1;
			CComObject<CCanvas>* pCanvas = (CComObject<CCanvas>*) *pluginIndex;

			if (pCanvas == NULL)
				return FALSE;

			pCanvas->draw(drawInfo->hdc, drawInfo->hBitmap);
		}
		return TRUE;


		// Save the plugin data before unload
		case SD_SAVE_DATA:
		{
			// We need to come up with an instance ID and register our config file
			// we don't care whether we are in export mode or not
			CComObject<CCanvas>* pCanvas = (CComObject<CCanvas>*) *pluginIndex;

			if (pCanvas == NULL)
				return FALSE;

			// Use the object ID pointer as our instance ID
			char instanceID[20];
			_ltoa_s((long) objID, instanceID, 10);
			lstrcpy((char *) param1, instanceID);

			char path[MAX_PATH], name[MAX_PATH];
			SDHostMessage(SD_GET_OBJECT_DIRECTORY, (DWORD) path, 0);
			sprintf_s(name, "%s\\DXCanvas-%s.ini", path, instanceID);

			// Save configuration
			WritePrivateProfileInt("Config", "Width", pCanvas->config->width, iniFile);
			WritePrivateProfileInt("Config", "Height", pCanvas->config->width, iniFile);
			WritePrivateProfileInt("Config", "AlphaBlend", pCanvas->config->alphablend ? 1 : 0, iniFile);

			SDHostMessage(SD_REGISTER_FILE, (DWORD) name, 0);

			return TRUE;
		}


		// Stop running this instance of the plugin
		case SD_TERMINATE_PLUGIN:
		{
			CComObject<CCanvas>* pCanvas = (CComObject<CCanvas>*) *pluginIndex;

			if (pCanvas == NULL)
				return FALSE;

			pCanvas->destroyCairo();

			return TRUE;
		}


		// Destroy this instance of the plugin
		case SD_DESTROY_PLUGIN:
		{
			CComObject<CCanvas>* pCanvas = (CComObject<CCanvas>*) *pluginIndex;

			if (pCanvas != NULL)
				pCanvas->killthreadQueueDraw();

			SAFE_RELEASE(pCanvas);

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
	OBJECT_ENTRY(CLSID_Canvas, CCanvas)
	OBJECT_ENTRY(CLSID_CanvasGradient, CCanvasGradient)
	OBJECT_ENTRY(CLSID_CanvasPattern, CCanvasPattern)
	OBJECT_ENTRY(CLSID_CanvasRenderingContext2D, CCanvasRenderingContext2D)
END_OBJECT_MAP()

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DLL
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(push)
#pragma warning(disable: 4100)
DECLARE_DXPLUGIN_DLLFUNCTIONS(LIBID_DXCanvasLib,
							  "DXCanvas.dll",
							  "{9215BB4D-AFC4-4b09-B39E-1967072BDD69}",
							  "DXCanvas.Canvas.1",
							  "{4AB89FAE-77C2-4204-827B-17BF775B43F6}",
							  "DXCanvas.Canvas",
							  "1.0",
							  "DXCanvas 1.0 Type Library")

#pragma warning(pop)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registry
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int iValue, LPCTSTR lpFileName)
{
	//////////////////////////////////////////////////////////////////////////
	///Helper function included to quickly & easily save integers to an Ini
	char szNumber[100];
	strcpy_s(szNumber, 100, "");
	_itoa_s(iValue, szNumber, 100, 10);
	return WritePrivateProfileString(lpAppName, lpKeyName, szNumber, lpFileName);
}