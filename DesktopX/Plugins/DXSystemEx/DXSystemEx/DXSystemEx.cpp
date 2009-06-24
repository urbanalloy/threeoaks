///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXSystemEx - Extended System Information
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
#include "DXSystemEx.h"
#include "dlldatax.h"
#include "SystemEx.h"

#include "VersionCheck.h"
#include "Volume/VistaCallBackSetup.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DesktopX Plugin
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD) = NULL;

static HINSTANCE dllInstance = NULL;
static HANDLE processHandle = NULL;

DECLARE_DXPLUGIN_READTYPEINFO(ReadSystemExTypeInfo, IID_ISystemEx);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Plugin-specific data
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WHEEL_DEFAULT_VALUE 3
#define WHEEL_PAGE_LINES	10

// Dragging
struct MouseWheelData
{
	DWORD objID;
	POINTS current;
	bool isMouseDragging;
	bool ignoreNext;

	MouseWheelData() : objID(NULL),
					   isMouseDragging(false)
	{}

	~MouseWheelData() {}

	void Reset()
	{
		objID = NULL;
		isMouseDragging = false;
	}
};

static MouseWheelData mouseWheelData;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SDMessage
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

			if (Is_WinVista_or_Later())
				RegisterCallBack();

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

			CComObject<CSystemEx>* pSystemEx;
			CComObject<CSystemEx>::CreateInstance(&pSystemEx);	

			*pluginIndex = (DWORD)pSystemEx;

			SCRIPTABLEPLUGIN sp;
			strcpy_s(sp.szName, "SystemEx");
			pSystemEx->QueryInterface(IID_IUnknown, (void**)&sp.pUnk);	        
			sp.pTI =  ReadSystemExTypeInfo(dllInstance);
			SDHostMessage(SD_REGISTER_SCRIPTABLE_PLUGIN, objID, (DWORD)&sp);

			return TRUE;

#ifdef DEBUG
label_expiration:		
			char message[2000];
			sprintf_s(message, "This beta version of DXSystemEx expired on %d/%d/%d.\n\n Please check http://julien.wincustomize.com or http://www.templier.info for new versions.", EXPIRATION_MONTH, EXPIRATION_DAY, EXPIRATION_YEAR);
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
			CComObject<CSystemEx>* pSystemEx = (CComObject<CSystemEx>*) *pluginIndex;	
			SD_PLUGIN_INIT* pluginInit = (SD_PLUGIN_INIT*)param1;

			if (pSystemEx == NULL)
				return FALSE;

			pSystemEx->Init(objID, pluginInit->hwnd);

			if (Is_WinVista_or_Later())
				pVistaVolumeCallback->addID(objID);

			return TRUE;
		}

		case SD_WINDOW_MESSAGE:
		{
			LPMSG msg = reinterpret_cast<LPMSG>(param1);

			switch(msg->message)
			{
				// Monitor information
				case WM_DISPLAYCHANGE:
				{
					CComObject<CSystemEx>* pSystemEx = (CComObject<CSystemEx>*) *pluginIndex;	

					if (pSystemEx == NULL)
						return FALSE;
					
					pSystemEx->UpdateMonitorInfo();

					break;
				}

				//////////////////////////////////////////////////////////////////////////
				// Middle button drag
				case WM_MOUSEMOVE:
				{
					// FIXME: BROKEN AT THE MOMENT: DISABLE
					return FALSE;

					// Check if we are dragging with the middle mouse button
					if (!((msg->wParam & MK_MBUTTON) == MK_MBUTTON))
						return FALSE;		

					// Ignore any movement from child objects
					if (objID != mouseWheelData.objID)
						return FALSE;

					// Get delta since last move
					int delta_x = GET_X_LPARAM(msg->lParam) - mouseWheelData.current.x;
					int delta_y = GET_Y_LPARAM(msg->lParam) - mouseWheelData.current.y;

					if (delta_x == 0 && delta_y == 0)
						return FALSE;

					// Get the object position
					RECT position;
					SDHostMessage(SD_GET_ABSOLUTE_RECT, objID, (DWORD)&position);

					// Move the object
					SD_SOP_INFO positionInfo;
					positionInfo.x = position.left + delta_x;
					positionInfo.y = position.top + delta_y;
					positionInfo.flags = SD_SOP_MOVE;
					SDHostMessage(SD_SET_OBJECT_POS, objID, (DWORD)&positionInfo);

					// Save position
					mouseWheelData.current = MAKEPOINTS(msg->lParam);

					mouseWheelData.isMouseDragging = true;	

					return TRUE;
					
					break;
				}

				case WM_MOUSELEAVE:
				{
					// Stop mouse dragging
					mouseWheelData.Reset();
					break;
				}

				// Middle Button Up
				case WM_MBUTTONUP:
				{
					SD_SCRIPTABLE_EVENT se;
					se.cbSize = sizeof(SD_SCRIPTABLE_EVENT);
					se.flags=0;
					lstrcpy(se.szEventName, "SystemEx_OnMButtonUp");

					// Message parameters
					memset(&se.dp, 0, sizeof(DISPPARAMS));
					se.dp.cArgs = 3;
					VARIANT* lpvt = (VARIANT*)malloc(sizeof(VARIANT)*3);
					VariantInit(&lpvt[0]);	
					VariantInit(&lpvt[1]);
					VariantInit(&lpvt[2]);
					lpvt[0].vt = VT_BOOL;
					lpvt[0].lVal = mouseWheelData.isMouseDragging ? VARIANT_TRUE : VARIANT_FALSE;
					lpvt[1].vt = VT_I4;
					lpvt[1].lVal =  GET_Y_LPARAM(msg->lParam);
					lpvt[2].vt = VT_I4;
					lpvt[2].lVal = GET_X_LPARAM(msg->lParam);

					se.dp.rgvarg = lpvt;

					SDHostMessage(SD_SCRIPTABLE_PLUGIN_EVENT, objID, (DWORD) &se);

					free(se.dp.rgvarg);

					// Stop mouse dragging
					mouseWheelData.Reset();			

					break;
				}

				// Middle button down
				case WM_MBUTTONDOWN:
				{
					// Start dragging on next MOUSE_MOVE if we do not get a WM_MBUTTONUP
					mouseWheelData.objID = objID;
					mouseWheelData.current = MAKEPOINTS(msg->lParam);					

					SD_SCRIPTABLE_EVENT se;
					se.cbSize = sizeof(SD_SCRIPTABLE_EVENT);
					se.flags=0;
					lstrcpy(se.szEventName, "SystemEx_OnMButtonDown");

					// Message parameters
					memset(&se.dp, 0, sizeof(DISPPARAMS));
					se.dp.cArgs = 2;
					VARIANT* lpvt = (VARIANT*)malloc(sizeof(VARIANT)*2);
					VariantInit(&lpvt[0]);	
					VariantInit(&lpvt[1]);
					lpvt[0].vt = VT_I4;
					lpvt[0].lVal = GET_Y_LPARAM(msg->lParam);
					lpvt[1].vt = VT_I4;
					lpvt[1].lVal =  GET_X_LPARAM(msg->lParam);

					se.dp.rgvarg = lpvt;

					SDHostMessage(SD_SCRIPTABLE_PLUGIN_EVENT, objID, (DWORD) &se);

					free(se.dp.rgvarg);	

					break;
				}

				//////////////////////////////////////////////////////////////////////////
				// Mouse wheel
				case WM_MOUSEWHEEL:
				{
					// Get wheel delta
					int wheel_delta = GET_WHEEL_DELTA_WPARAM(msg->wParam);

					// Compute scroll delta
					// http://blogs.msdn.com/oldnewthing/archive/2003/08/07/54615.aspx

					static int wheel_carryover = 0;

					UINT scroll_lines;
					if (!SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scroll_lines, 0)) {
						scroll_lines = WHEEL_DEFAULT_VALUE;
					}

					// If user specified no wheel scrolling, then don't do wheel scrolling.
					if (scroll_lines == 0)					
						return FALSE;		
					
					// If user specified scrolling by pages, do so.					
					if (scroll_lines == WHEEL_PAGESCROLL)
						scroll_lines = WHEEL_PAGE_LINES;

					// Accumulate wheel motion 
					wheel_delta += wheel_carryover;

					// See how many lines we should scroll. This relies on round-towards-zero.					
					int delta_lines = wheel_delta * (int)scroll_lines / WHEEL_DELTA;

					// Record the unused portion as the next carryover.
					wheel_carryover = wheel_delta - delta_lines * WHEEL_DELTA / (int)scroll_lines;

					// Send the message to the object
					SD_SCRIPTABLE_EVENT se;
					se.cbSize = sizeof(SD_SCRIPTABLE_EVENT);
					lstrcpy(se.szEventName, "SystemEx_OnMouseWheel");
					se.flags=0;

					memset(&se.dp, 0, sizeof(DISPPARAMS));

					se.dp.cArgs = 1;
					VARIANT* lpvt = (VARIANT*)malloc(sizeof(VARIANT)*1);
					VariantInit(&lpvt[0]);							
					lpvt[0].vt = VT_I4;
					lpvt[0].lVal = delta_lines;

					se.dp.rgvarg = lpvt;

					SDHostMessage(SD_SCRIPTABLE_PLUGIN_EVENT, objID, (DWORD) &se);

					free(se.dp.rgvarg);	

					return TRUE;

					break;
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
			if (Is_WinVista_or_Later())
				pVistaVolumeCallback->removeID(objID);

			return TRUE;
		}


		// Destroy this instance of the plugin
		case SD_DESTROY_PLUGIN:
		{			
			CComObject<CSystemEx>* pSystemEx = (CComObject<CSystemEx>*) *pluginIndex;				
			
			if (pSystemEx != NULL)
				pSystemEx->Cleanup();

			SAFE_RELEASE(pSystemEx);

			return TRUE;
		}


		// Unload the plugin dll
		case SD_TERMINATE_MODULE:
		{
			if (Is_WinVista_or_Later())
				UnregisterCallBack();

			return TRUE;
		}
			
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COM
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_SystemEx, CSystemEx)		
	OBJECT_ENTRY(CLSID_MonitorInfo, CMonitorInfo)	
END_OBJECT_MAP()

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DLL
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DECLARE_DXPLUGIN_DLLFUNCTIONS(LIBID_DXSystemExLib,
							  "DXSystemEx.dll",
							  "{65C94091-C8AE-4423-99A2-CEAE94F91E62}", // class
							  "DXSystemEx.SystemEx.1",
							  "{9F3FB81B-658E-4853-82BA-1A263630CFA5}", // type library
							  "DXSystemEx.SystemEx",
							  "1.0",
							  "DXSystemEx 1.0 Type Library")