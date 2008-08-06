///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXVistaAeroColor - Set the color intensity, color, and opacity of glass on Vista.
//
// (c) 2006 - Julien Templier
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
#include "DXVistaAeroColor.h"
#include "AeroColor.h"
#include "dlldatax.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DesktopX Plugin
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD) = NULL;


static HINSTANCE dllInstance = NULL;
static HANDLE processHandle = NULL;

DECLARE_DXPLUGIN_READTYPEINFO(ReadAeroColorTypeInfo, IID_IAeroColor)

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
		        CComObject<CAeroColor>* pAeroColor;
		        CComObject<CAeroColor>::CreateInstance(&pAeroColor);	

		        *pluginIndex = (DWORD)pAeroColor;
				SCRIPTABLEPLUGIN sp;
				strcpy_s(sp.szName, 9*sizeof(char), "AeroColor");
				pAeroColor->QueryInterface(IID_IUnknown, (void**)&sp.pUnk);
				sp.pTI =  ReadAeroColorTypeInfo(dllInstance);
				SDHostMessage(SD_REGISTER_SCRIPTABLE_PLUGIN, objID, (DWORD)&sp);
        
				DWORD *flags = (DWORD *) param1;
				*flags = SD_FLAG_CUSTOM_STATES|
						 SD_FLAG_NO_BUILDER_CONFIG|
						 SD_FLAG_NO_USER_CONFIG;
			}
			return TRUE;

		case SD_DESTROY_PLUGIN:
			{
				CComObject<CAeroColor>* pAeroColor = (CComObject<CAeroColor>*) *pluginIndex;				
				pAeroColor->Release();	
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
			}
			return TRUE;

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
	OBJECT_ENTRY(CLSID_AeroColor, CAeroColor)
END_OBJECT_MAP()

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DLL
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DECLARE_DXPLUGIN_DLLFUNCTIONS(LIBID_DXVistaAeroColorLib,
							  "DXVistaAeroColor.dll",
							  "{AA8B06F5-8188-40BB-B7AE-535827030384}",
							  "DXVistaAeroColor.AeroColor.1",
							  "{0DFF1531-6394-4E04-A09D-8AD69EA4B03A}",
							  "DXVistaAeroColor.AeroColor",
							  "1.0",
							  "DXVistaAeroColor 1.0 Type Library")