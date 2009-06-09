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
#include "Config.h"
#include "resource.h"
#include "DXTaskbar7.h"
#include "dlldatax.h"
#include "Taskbar7.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DesktopX Plugin
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD) = NULL;


static HINSTANCE dllInstance = NULL;
static HANDLE processHandle = NULL;

DECLARE_DXPLUGIN_READTYPEINFO(ReadTaskbar7TypeInfo, IID_ITaskbar7)

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
		  //      CComObject<CAeroColor>* pAeroColor;
		  //      CComObject<CAeroColor>::CreateInstance(&pAeroColor);	

		  //      *pluginIndex = (DWORD)pAeroColor;
				//SCRIPTABLEPLUGIN sp;
				//strcpy(sp.szName, "AeroColor");
				//pAeroColor->QueryInterface(IID_IUnknown, (void**)&sp.pUnk);
				//sp.pTI =  ReadAeroColorTypeInfo(dllInstance);
				//SDHostMessage(SD_REGISTER_SCRIPTABLE_PLUGIN, objID, (DWORD)&sp);
        
				DWORD *flags = (DWORD *) param1;
				*flags = SD_FLAG_CUSTOM_STATES|
						 SD_FLAG_NO_BUILDER_CONFIG|
						 SD_FLAG_NO_USER_CONFIG;
			}
			return TRUE;

		case SD_DESTROY_PLUGIN:
			{
				//CComObject<CAeroColor>* pAeroColor = (CComObject<CAeroColor>*) *pluginIndex;				
				//pAeroColor->Release();	
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
	OBJECT_ENTRY(CLSID_Taskbar7, CTaskbar7)
END_OBJECT_MAP()

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DLL
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DECLARE_DXPLUGIN_DLLFUNCTIONS(LIBID_DXTaskbar7Lib,
							  "DXTaskbar7.dll",
							  "{20BD78A3-CDA1-47f9-A505-93D1990252C9}", // class
							  "DXTaskbar7.AeroColor.1",
							  "{8C650B9D-E2A4-4980-9CED-3A38D91F022B}", // type library
							  "DXTaskbar7.AeroColor",
							  "1.0",
							  "DXTaskbar7 1.0 Type Library")