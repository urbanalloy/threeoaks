///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXVolumeControl - Vista Master Volume Control for DesktopX
//
// Copyright (c)2006-2007, Julien Templier
//
// This work is licensed under the Creative Commons
// Attribution-ShareAlike License. To view a copy of this license, visit
// http://creativecommons.org/licenses/by-sa/2.0/ or send a letter to
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"

#include <SDPlugin.h>
#include <SDScriptedPlugin.h>
#include "Config.h"
#include "resource.h"
#include "DXVolumeControl.h"
#include "dlldatax.h"

#include "VersionCheck.h"
#include "VistaVolume.h"
#include "CallbackSetup.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DesktopX Plugin
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD) = NULL;

static HINSTANCE dllInstance = NULL;
static HANDLE processHandle = NULL;

DECLARE_DXPLUGIN_READTYPEINFO(ReadVistaVolumeTypeInfo, IID_IVistaVolume)

BOOL SDMessage(DWORD objID, DWORD *pluginIndex, UINT messageID, DWORD param1, DWORD param2)
{
	switch (messageID)
	{
		case SD_LOAD_DATA:
			return TRUE;

		case SD_SAVE_DATA:
			return TRUE;

		case SD_INITIALIZE_PLUGIN:
			return TRUE;

		case SD_TERMINATE_PLUGIN:
			return TRUE;

		case SD_DUPLICATE_PLUGIN:
			return TRUE;

		case SD_CREATE_PLUGIN:
			{
		        CComObject<CVistaVolume>* pVistaVolume;
		        CComObject<CVistaVolume>::CreateInstance(&pVistaVolume);	

		        *pluginIndex = (DWORD)pVistaVolume;
				SCRIPTABLEPLUGIN sp;
				strcpy(sp.szName, "VistaVolume");
				pVistaVolume->QueryInterface(IID_IUnknown, (void**)&sp.pUnk);
				sp.pTI =  ReadVistaVolumeTypeInfo(dllInstance);
				SDHostMessage(SD_REGISTER_SCRIPTABLE_PLUGIN, objID, (DWORD)&sp);
        
				DWORD *flags = (DWORD *) param1;
				*flags = SD_FLAG_CUSTOM_STATES|
						 SD_FLAG_NO_BUILDER_CONFIG|
						 SD_FLAG_NO_USER_CONFIG;

				if (Is_WinVista_or_Later())
					pVistaVolumeCallback->addID(objID);
			}
			return TRUE;

		case SD_DESTROY_PLUGIN:
			{
				CComObject<CVistaVolume>* pVistaVolume = (CComObject<CVistaVolume>*) *pluginIndex;				
				pVistaVolume->Release();	

				if (Is_WinVista_or_Later())
					pVistaVolumeCallback->removeID(objID);
			}
			return TRUE;

		case SD_QUERY_PLUGIN_INFO:
			{
				SD_PLUGIN_INFO *pi = (SD_PLUGIN_INFO*) param1;
				lstrcpy(pi->plugin_name, PLUGIN_NAME);
				lstrcpy(pi->plugin_author, PLUGIN_AUTHOR);
				lstrcpy(pi->plugin_capability, PLUGIN_CAPABILITY);
				pi->plugin_version = VERSION_MAJOR*100 + VERSION_MINOR*10 + VERSION_BUILD;
				pi->supported_platforms = PLUGIN_PLATFORM;
				pi->supported_hosts = PLUGIN_HOSTS;
			}
			return TRUE;

		case SD_INITIALIZE_MODULE:
			{
				dllInstance = (HINSTANCE) param2;
				SDHostMessage = (BOOL (__stdcall *)(UINT, DWORD, DWORD)) param1;

				if (Is_WinVista_or_Later())
					RegisterCallBack();
			}
			return TRUE;

		case SD_TERMINATE_MODULE:
			{		
				if (Is_WinVista_or_Later())
					UnregisterCallBack();
			}
			return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COM
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_VistaVolume, CVistaVolume)
END_OBJECT_MAP()


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DLL
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DECLARE_DXPLUGIN_DLLFUNCTIONS(LIBID_DXVolumeControlLib,
							  "DXVolumeControl.dll",
							  "{F36B9A47-AF5B-4AE2-A039-48CC18FEEA4C}",
							  "DXVolumeControl.VistaVolume.1",
							  "{6444B45E-8B01-4F6C-88E0-C954A017C7C4}",
							  "DXVolumeControl.VistaVolume",
							  "1.0",
							  "DXVolumeControl 1.0 Type Library")
