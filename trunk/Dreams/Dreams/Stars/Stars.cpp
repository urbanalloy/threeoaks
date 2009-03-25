///////////////////////////////////////////////////////////////////////////////////////////////
//
// Stars Dream
//
// Copyright (c) 2008, Julien Templier
// All rights reserved.
//
// Adapted from the Stars Screensaver for XBox Media Center
// Copyright (c) 2004 Team XBMC
//
// Thanks goes to Warren for his 'TestXBS' program!
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
////////////////////////////////////////////////////////////////////////////////////////////////

#include "Utils/stdafx.h"
#include <DreamSDK.h>
#include <commctrl.h>
#include <stdio.h>

#include "Utils/registry.h"
#include "Utils/dialogs.h"

#include "Stars.h"
#include "StarField.h"
#include "resource.h"
#include "version.h"

static BOOL IsDreamPaused = FALSE;
static BOOL IsDreamInitialized = FALSE;

CStarField* g_pStarField = NULL;

struct ST_SETTINGS g_Settings = 
{
	0, 0, 1000, 1.f, 0.2f, 10.0f, 1.5f, 1.5f, true
};

LPDIRECT3DDEVICE9 g_pd3dDevice;

//////////////////////////////////////////////////////////////////////////
// Query default settings for the Dream
// return the value for the type of setting
QUERY_DEFAULT_SETTINGS
{
	switch (type)
	{
	case QUERY_D3DFORMAT:
	case QUERY_VIDEOTEXTURE:
	case QUERY_4:
	case QUERY_5:
		return value;
		break;		

	case QUERY_SCREENUPDATEFREQUENCY:
		return DEFAULT_SCREENUPDATEFREQUENCY;
		break;
	}

	return value;
}

//////////////////////////////////////////////////////////////////////////
// Notification when:
//  - started
//	- paused
//	- resumed
//  - stopped
//	- settings have changed
NOTIFICATION_PROCEDURE
{
	switch(type) {

		default: 
			break;

		case NOTIFY_STARTED:
			IsDreamPaused = FALSE;
			break;

		case NOTIFY_STOPPED:
			IsDreamPaused = TRUE;
			break;

		case NOTIFY_PAUSED:
			IsDreamPaused = TRUE;
			break;

		case NOTIFY_PLAYING:
			IsDreamPaused = FALSE;
			break;

		case NOTIFY_5:
			break;

		case NOTIFY_SETTINGS_CHANGED:
			break;
	}
}

//////////////////////////////////////////////////////////////////////////
// Setup the plugin
// load Direct3D resources, update registry keys
SETUP_PLUGIN
{
	g_Settings.iWidth = rect.right - rect.left;
	g_Settings.iHeight = rect.bottom - rect.top;

	SetDefaults();
	LoadSettings();

	srand(timeGetTime());

	g_pStarField = new CStarField(g_Settings.iNumStars,
		g_Settings.fGamma,
		g_Settings.fBrightness,
		g_Settings.fSpeed,
		g_Settings.fZoom,
		g_Settings.fExpanse,
		g_Settings.bMoveCamera);

	if (g_pStarField)
	{
		g_pStarField->SetD3DDevice(g_pd3dDevice);
		g_pStarField->Create(g_Settings.iWidth, g_Settings.iHeight);
	}

	IsDreamInitialized = TRUE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Update the scene in memory
UPDATE_SCENE
{

}

//////////////////////////////////////////////////////////////////////////
// Render the updated scene on screen
RENDER_SCENE_ON_SCREEN
{
	if (!g_pStarField)
		return 0;

	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	pd3dDevice->BeginScene();

		g_pStarField->SetD3DDevice(pd3dDevice);

		g_pStarField->RenderFrame();

	pd3dDevice->EndScene();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Free plugin resources
FREE_PLUGIN
{
	SAFE_DELETE(g_pStarField);
}

//////////////////////////////////////////////////////////////////////////
// Configure Dream
extern HINSTANCE hInst;
CONFIG_PLUGIN
{
	if (IsDreamInitialized == FALSE) 
		return FALSE;

	return (BOOL)DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_CONFIG), hWndParent, ConfigureDream, (LPARAM)&g_Settings);
}

//////////////////////////////////////////////////////////////////////////
// Configuration
//////////////////////////////////////////////////////////////////////////
INT_PTR CALLBACK ConfigureDream(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	ST_SETTINGS *config = (ST_SETTINGS *) GetProp(hDlg, "config");

	switch(iMsg)
	{
		case WM_INITDIALOG:
		{
			// Set the version text
			char version[VERSION_SIZE];
			sprintf_s(version, VERSION_SIZE*sizeof(char), "v%i.%i Build %i",VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
			SetDlgItemText(hDlg, IDC_BUILD,version);

			// Get Data
			SetProp(hDlg, "config", (HANDLE) (char*) lParam);
			config = (ST_SETTINGS *) GetProp(hDlg, "config");
				
			if(!config) {
				MessageBox(hDlg, "Error initializing Dialog: config not present", "Initialization Error", MB_OK|MB_ICONERROR);
				EndDialog(hDlg, 0);
			}

			SET_ITEM(IDC_STARS, STARS_MIN, STARS_MAX, STARS_FREQ, config->iNumStars);
			SET_ITEM(IDC_GAMMA, GAMMA_MIN, GAMMA_MAX, GAMMA_FREQ, (int)(config->fGamma*GAMMA_MULT));
			SET_ITEM(IDC_BRIGHTNESS, BRIGHTNESS_MIN, BRIGHTNESS_MAX, BRIGHTNESS_FREQ, (int)(config->fBrightness*BRIGHTNESS_MULT));
			SET_ITEM(IDC_SPEED, SPEED_MIN, SPEED_MAX, SPEED_FREQ, (int)(config->fSpeed*SPEED_MULT));
			SET_ITEM(IDC_ZOOM, ZOOM_MIN, ZOOM_MAX, ZOOM_FREQ, (int)(config->fZoom*ZOOM_MULT));
			SET_ITEM(IDC_EXPANSE, EXPANSE_MIN, EXPANSE_MAX, EXPANSE_FREQ, (int)(config->fExpanse*EXPANSE_MULT));
			SendDlgItemMessage(hDlg, IDC_CAMERA, BM_SETCHECK, config->bMoveCamera ? BST_CHECKED : BST_UNCHECKED, NULL);					

			break;
		}	
		
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_DEFAULT:
					RESET_ITEM(IDC_STARS, STARS_DEFAULT);
					RESET_ITEM(IDC_GAMMA, (int)(GAMMA_DEFAULT*GAMMA_MULT));
					RESET_ITEM(IDC_BRIGHTNESS, (int)(BRIGHTNESS_DEFAULT*BRIGHTNESS_MULT));
					RESET_ITEM(IDC_SPEED, (int)(SPEED_DEFAULT*SPEED_MULT));
					RESET_ITEM(IDC_ZOOM, (int)(ZOOM_DEFAULT*ZOOM_MULT));
					RESET_ITEM(IDC_EXPANSE, (int)(EXPANSE_DEFAULT*EXPANSE_MULT));
					SendDlgItemMessage(hDlg, IDC_CAMERA, BM_SETCHECK, CAMERA_DEFAULT ? BST_CHECKED : BST_UNCHECKED, NULL);					
					break;

				case IDOK:
					if(!config)
						break;	

					config->iNumStars = (int)SendDlgItemMessage(hDlg, IDC_STARS, TBM_GETPOS, NULL, NULL);
					config->fGamma = (float)SendDlgItemMessage(hDlg, IDC_GAMMA, TBM_GETPOS, NULL, NULL)/GAMMA_MULT;
					config->fBrightness = (float)SendDlgItemMessage(hDlg, IDC_BRIGHTNESS, TBM_GETPOS, NULL, NULL)/BRIGHTNESS_MULT;
					config->fSpeed = (float)SendDlgItemMessage(hDlg, IDC_SPEED, TBM_GETPOS, NULL, NULL)/SPEED_MULT;
					config->fZoom = (float)SendDlgItemMessage(hDlg, IDC_ZOOM, TBM_GETPOS, NULL, NULL)/ZOOM_MULT;
					config->fExpanse = (float)SendDlgItemMessage(hDlg, IDC_EXPANSE, TBM_GETPOS, NULL, NULL)/EXPANSE_MULT;
					config->bMoveCamera = (SendDlgItemMessage(hDlg, IDC_CAMERA, BM_GETCHECK, NULL, NULL) == BST_CHECKED ? true : false);

					SaveSettings();	
					EndDialog(hDlg, 1);
					return TRUE;
					break;	

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

//////////////////////////////////////////////////////////////////////////
// Settings
//////////////////////////////////////////////////////////////////////////
void SetDefaults()
{
	g_Settings.iNumStars = STARS_DEFAULT;
	g_Settings.fGamma = GAMMA_DEFAULT;
	g_Settings.fBrightness = BRIGHTNESS_DEFAULT;
	g_Settings.fSpeed = SPEED_DEFAULT;
	g_Settings.fZoom = ZOOM_DEFAULT;
	g_Settings.fExpanse = EXPANSE_DEFAULT;	
	g_Settings.bMoveCamera = CAMERA_DEFAULT;
}

#define DREAM_NAME "Stars"

//////////////////////////////////////////////////////////////////////////
// Load configuration from registry
void LoadSettings()
{
	HKEY key;	
	DWORD size, val;
	DWORD type = REG_SZ;
	char buffer[MAX_PATH];

	char configPath[MAX_PATH];	
	sprintf((char*)&configPath, "%s\\%s", DESKSCAPES_KEY, DREAM_NAME);

	// Open parent key
	if (RegOpenKeyEx(HKEY_CURRENT_USER, configPath, NULL, KEY_READ, &key) != ERROR_SUCCESS)
		return;

	int moveCamera = -1;

	// Load & check config keys	
	LOAD_KEY("NumStars", g_Settings.iNumStars);	
	LOAD_KEY("MoveCamera", moveCamera);	
	LOAD_KEY_FLOAT("Gamma", g_Settings.fGamma);
	LOAD_KEY_FLOAT("Brightness", g_Settings.fBrightness);
	LOAD_KEY_FLOAT("Speed", g_Settings.fSpeed);
	LOAD_KEY_FLOAT("Zoom", g_Settings.fZoom);
	LOAD_KEY_FLOAT("Expanse", g_Settings.fExpanse);

	if (moveCamera != -1)
		g_Settings.bMoveCamera = (moveCamera == 1 ? true : false);

	RegCloseKey(key);
}


//////////////////////////////////////////////////////////////////////////
// Save configuration to registry
void SaveSettings() 
{
	HKEY key;
	DWORD val;
	char buffer[MAX_PATH];

	char configPath[MAX_PATH];	
	sprintf((char*)&configPath, "%s\\%s", DESKSCAPES_KEY, DREAM_NAME);

	// Open parent key
	if (RegCreateKeyEx(HKEY_CURRENT_USER, configPath, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, NULL) != ERROR_SUCCESS)
		return;

	// Save config
	SAVE_KEY("NumStars", g_Settings.iNumStars);
	SAVE_KEY("MoveCamera", g_Settings.bMoveCamera ? 1 : 0);
	SAVE_KEY_FLOAT("Gamma", g_Settings.fGamma);
	SAVE_KEY_FLOAT("Brightness", g_Settings.fBrightness);
	SAVE_KEY_FLOAT("Speed", g_Settings.fSpeed);
	SAVE_KEY_FLOAT("Zoom", g_Settings.fZoom);
	SAVE_KEY_FLOAT("Expanse", g_Settings.fExpanse);

	RegCloseKey(key);
}