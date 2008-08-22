
#pragma once

typedef struct SD_PLUGIN_INIT_
{
	HWND hwnd;			// the object 's hwnd
	RECT rcObject;		// the portion of the window occupied by the object
} SD_PLUGIN_INIT;

typedef struct SD_DRAW_INFO_
{
	HDC hdc;
	HBITMAP hBitmap;
	RECT rcObject;
	char *szCurrentState;
} SD_DRAW_INFO;

typedef struct SD_SCRIPTABLE_EVENTtag  
{
	DWORD cbSize;
	DISPPARAMS dp;
	char szEventName[64];
	DWORD flags;
}SD_SCRIPTABLE_EVENT;

typedef struct tagSCRIPTABLEPLUGIN
{ 
	CHAR szName[50]; 
	IUnknown* pUnk;
	ITypeInfo* pTI;
} SCRIPTABLEPLUGIN, *LPSCRIPTABLEPLUGIN;

#define SD_SOP_MOVE		0x00000001
#define SD_SOP_SIZE		0x00000002
#define SD_SOP_SHOW		0x00000004
#define SD_SOP_HIDE		0x00000008
#define SD_SOP_ONTOP	0x00000010

typedef struct SD_SOP_INFO_
{
	int x;
	int y;
	int width;
	int height;
	DWORD flags;
} SD_SOP_INFO;

typedef struct SD_IMAGE_INFO_
{
	char fileName[MAX_PATH];
	HBITMAP hb;
	int width;
	int height;
	BOOL bHasAlpha;
} SD_IMAGE_INFO;

#define SD_HOST_DESKTOPX  0x00000001
#define SD_HOST_OBJECTBAR 0x00000002

#define SD_OS_ALL  0x00000000
#define SD_OS_95   0x00000001
#define SD_OS_98   0x00000002
#define SD_OS_ME   0x00000004
#define SD_OS_NT4  0x00000008
#define SD_OS_2000 0x00000010
#define SD_OS_XP   0x00000020

#define SD_FLAG_DRAW				 0x00000001  // the plugin will draw directly the object appearance, it is a drawing plugin (i.e. analog clock). If a drawing plugin is selected for an object, the user won't be able to directly setup the messages/states in the GUI. The appearance will be only configurable through the plugin configuration dialog.
#define SD_FLAG_SET_SIZE			 0x00000002  // This flag can be used by drawing plugins, if the plugin needs to determine the initial object size, instead of using the one specified by the user in the object size settings (that will be disabled). In this case the plugin will receive SD_GET_OBJECT_SIZE when the host needs to know those values to create or modify the window or do other internal adjustements.
#define SD_FLAG_CUSTOM_ACTION		 0x00000004  // the plugin will perform a custom action (i.e. control Winamp). It is a command plugin. If a command plugin is selected for an object, the command class will switch automatically to "Custom class" and the "Configure" button will open the plugin configuration panel. See SD_COMMAND below for more details.
#define SD_FLAG_CUSTOM_STATES		 0x00000010  // the plugin has custom states that it wants host to observe
#define SD_FLAG_ONLY_CUSTOM_STATES	 0x00000020  // items/objects using this plugin should only show the custom states when the users is editing the item's appearance.
#define SD_FLAG_FILTER_STATE_CHANGES 0x00000040
#define SD_FLAG_SUBCLASS			 0x00000080
#define SD_FLAG_DRAW_PPALPHA		 0x00000100
#define SD_FLAG_DRAW_DOUBLE_HEIGHT	 0x00000200
#define SD_FLAG_MORE_INSTANCES		 0x00000400
#define SD_FLAG_NO_BUILDER_CONFIG    0x00002000
#define SD_FLAG_NO_USER_CONFIG       0x00004000

typedef struct SD_PLUGIN_INFO_
{
	DWORD host;					// set by the host to identify whether the host is OB or DX
								// SD_HOST_DESKTOPX or SD_HOST_OBJECTBAR
	DWORD host_version; 		// set by the host, specifies the version of the program
								// 100 = 1.0, 101 = 1.01, 110 = 1.10
	char plugin_name[256];
	char plugin_author[256];

	DWORD plugin_version;		// 100 = 1.0, 101 = 1.01, 110 = 1.10

	DWORD supported_platforms;	// SD_OS_ALL or any combination of the following values:
								// SD_OS_95, SD_OS_98, SD_OS_ME, SD_OS_NT4, SD_OS_2000, SD_OS_XP

	DWORD supported_hosts;		// SD_HOST_DESKTOPX and/or SD_HOST_OBJECTBAR	

	char plugin_capability[256];

	BOOL bDistributable;

} SD_PLUGIN_INFO;

#define SD_INITIALIZE_MODULE	 1
#define SD_TERMINATE_MODULE		 2
#define SD_QUERY_PLUGIN_INFO	 3
#define SD_QUERY_CUSTOM_STATES	 4
#define SD_GET_OBJECT_SIZE		 5
#define SD_CREATE_PLUGIN		 6
#define SD_DESTROY_PLUGIN		 7
#define SD_DUPLICATE_PLUGIN	 	 8
#define SD_INITIALIZE_PLUGIN	 9
#define SD_TERMINATE_PLUGIN		10
#define SD_LOAD_DATA			11
#define SD_SAVE_DATA			12
#define SD_CONFIGURE			13
#define SD_STATE_CHANGING		14
#define SD_STATE_CHANGED		15
#define SD_DRAW					16
#define SD_WINDOW_MESSAGE		18
#define SD_COMMAND				19
#define SD_MODE_CHANGED			20

#define SD_GET_OBJECT_DIRECTORY		1
#define SD_GET_PLUGINS_DIRECTORY	2
#define SD_REDRAW					3
#define SD_SET_STATE				4
#define SD_MOVE						5
#define SD_GET_RELATIVE_RECT		6
#define SD_GET_ABSOLUTE_RECT		7
#define SD_SET_OBJECT_POS			8
#define SD_LOAD_IMAGE				9
#define SD_SET_TOOLTIP_TEXT			10
#define SD_PACK_ALPHA				11
#define SD_CREATE_OBJECT			12
#define SD_REGISTER_FILE			13
#define SD_OBJID_FROM_GUIID			14
#define SD_ENUMERATE_OBJECTS		15
#define SD_DELETE_OBJECT			16
#define SD_GET_STATE				17
#define SD_GUIID_FROM_OBJID			18
#define SD_SET_OBJECT_GUIID			19
#define SD_USER_MODE				20

#define SD_REGISTER_SCRIPTABLE_PLUGIN 55
#define SD_SCRIPTABLE_PLUGIN_EVENT  75

#define SD_OBJECT_STATE_MOUSEDOWN "Mouse down"
#define SD_OBJECT_STATE_MOUSEUP	  "Mouse up"
#define SD_OBJECT_STATE_MOUSEOVER "Mouse over"
#define SD_OBJECT_STATE_AWAY	  "Mouse away"
#define SD_OBJECT_STATE_SHOW	  "Show"
#define SD_OBJECT_STATE_HIDE	  "Hide"
#define SD_OBJECT_STATE_COMMAND	  "Command executed"