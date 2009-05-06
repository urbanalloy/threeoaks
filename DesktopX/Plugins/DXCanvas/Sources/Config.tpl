///////////////////////////////////////////////////////////////////////////////////////////////
// This file is used as a template by SubWCRev
// 
// DO NOT MODIFY, IT WILL BE OVERWRITTEN AT BUILD TIME
// MAKE MODIFICATIONS TO .tpl file
//
//	
// SubWCRev.exe $(ProjectDir) "$(ProjectDir)/Sources/Config.tpl" "$(ProjectDir)/Sources/Config.h"
///////////////////////////////////////////////////////////////////////////////////////////////


#define PLUGIN_NAME			"Canvas Control"
#define PLUGIN_AUTHOR		"Julien Templier"
#define PLUGIN_CAPABILITY	"Scriptable free-form vector drawing surface"
#define PLUGIN_PLATFORM		SD_OS_XP
#define PLUGIN_HOSTS		SD_HOST_DESKTOPX | SD_HOST_OBJECTBAR

#define VERSION_MAJOR 1  
#define VERSION_MINOR 2
#define VERSION_BUILD $WCREV$

#ifdef DEBUG
#define EXPIRATION_YEAR 2009
#define EXPIRATION_MONTH 7
#define EXPIRATION_DAY 1
#endif