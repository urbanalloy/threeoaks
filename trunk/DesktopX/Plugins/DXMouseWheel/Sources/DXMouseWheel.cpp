///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXMouseWheel - MouseWheel Support for DesktopX
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


#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <windowsx.h>

#include "DXMouseWheel.h"

BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD) = NULL;

static HINSTANCE dllInstance = NULL;
static HHOOK mouseHook = NULL;
static DWORD objectId = NULL;

///<summary>
///	Mouse Hook callback
///</summary>
static LRESULT CALLBACK MessageHookProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
    // If the value of nCode is < 0, just pass it on and return 0
    if (nCode < 0)
		return CallNextHookEx(mouseHook, nCode, wParam, lParam);

    int msg = (UINT)wParam;

	if (msg == WM_MOUSEWHEEL) {
		MSLLHOOKSTRUCT* mouseStruct = (MSLLHOOKSTRUCT*)lParam;
		int delta = GET_WHEEL_DELTA_WPARAM(mouseStruct->mouseData);

		if ((delta / WHEEL_DELTA) == 0)
			delta = (delta > 0) ? 120 : -120;

		SD_SCRIPTABLE_EVENT se;
		se.cbSize = sizeof(SD_SCRIPTABLE_EVENT);
		lstrcpy(se.szEventName, "Wheel_OnMouseWheel");
		se.flags=0;

		memset(&se.dp, 0, sizeof(DISPPARAMS));

		se.dp.cArgs = 1;
		VARIANT* lpvt = (VARIANT*)malloc(sizeof(VARIANT)*1);
		VariantInit(&lpvt[0]);							
		lpvt[0].vt = VT_I4;
		lpvt[0].lVal = delta;

		se.dp.rgvarg = lpvt;

		SDHostMessage(SD_SCRIPTABLE_PLUGIN_EVENT, objectId, (DWORD) &se);

		free(se.dp.rgvarg);			
	}

	return CallNextHookEx(mouseHook, nCode,  wParam, lParam);

}


///<summary>
///	Main function
///</summary>
BOOL SDMessage(DWORD objID, DWORD *pluginIndex, UINT messageID, DWORD param1, DWORD param2)
{
	switch (messageID)
	{
		case SD_INITIALIZE_PLUGIN:			
			return TRUE;

		case SD_TERMINATE_PLUGIN:
			return TRUE;

		case SD_DUPLICATE_PLUGIN:
			return TRUE;

		case SD_CREATE_PLUGIN:
			{
				DWORD *flags = (DWORD *) param1;
				*flags = SD_FLAG_SUBCLASS |
						 SD_FLAG_NO_BUILDER_CONFIG |
						 SD_FLAG_NO_USER_CONFIG;
			}
			return TRUE;

		case SD_DESTROY_PLUGIN:
			if (mouseHook != NULL)
				UnhookWindowsHookEx(mouseHook);
			return TRUE;

		case SD_QUERY_CUSTOM_STATES:
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

		case SD_WINDOW_MESSAGE:
			LPMSG msg = reinterpret_cast<LPMSG>(param1);
			
			switch (msg->message)
			{
				// Mouse is inside the window, hook the mouse
				case WM_MOUSEMOVE:
					if (mouseHook == NULL) {
						objectId = objID;
						mouseHook = SetWindowsHookEx(WH_MOUSE_LL, &MessageHookProc, dllInstance, 0);
					}
					break;

				// Mouse leaving the window, unhook
				case WM_MOUSELEAVE:
					if (mouseHook != NULL) {
						UnhookWindowsHookEx(mouseHook);
						mouseHook = NULL;
					}
					break;

				// Middle button click
				case WM_MBUTTONUP:
				case WM_MBUTTONDOWN:
					SD_SCRIPTABLE_EVENT se;
					se.cbSize = sizeof(SD_SCRIPTABLE_EVENT);
					if (msg->message == WM_MBUTTONUP)
						lstrcpy(se.szEventName, "Wheel_OnMButtonUp");
					else
						lstrcpy(se.szEventName, "Wheel_OnMButtonDown");
					se.flags=0;

					memset(&se.dp, 0, sizeof(DISPPARAMS));

					se.dp.cArgs = 2;
					VARIANT* lpvt = (VARIANT*)malloc(sizeof(VARIANT)*2);
					VariantInit(&lpvt[0]);	
					VariantInit(&lpvt[1]);
					lpvt[0].vt = VT_I4;
					lpvt[0].lVal = GET_X_LPARAM(msg->lParam);
					lpvt[1].vt = VT_I4;
					lpvt[1].lVal =  GET_Y_LPARAM(msg->lParam);

					se.dp.rgvarg = lpvt;

					SDHostMessage(SD_SCRIPTABLE_PLUGIN_EVENT, objectId, (DWORD) &se);

					free(se.dp.rgvarg);	
					break;

			}
			
			return FALSE;
	}

	return FALSE;
}
