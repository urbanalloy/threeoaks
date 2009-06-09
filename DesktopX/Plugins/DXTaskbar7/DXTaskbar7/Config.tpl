///////////////////////////////////////////////////////////////////////////////////////////////
// This file is used as a template by SubWCRev
// 
// DO NOT MODIFY, IT WILL BE OVERWRITTEN AT BUILD TIME
// MAKE MODIFICATIONS TO .tpl file
//
//	
// SubWCRev.exe $(ProjectDir) "$(ProjectDir)/Sources/Config.tpl" "$(ProjectDir)/Sources/Config.h"
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define PLUGIN_NAME			"Extended Taskbar Support"
#define PLUGIN_AUTHOR		"Julien Templier"
#define PLUGIN_CAPABILITY	"Extended Taskbar Support for Windows 7"
#define PLUGIN_PLATFORM		SD_OS_XP
#define PLUGIN_HOSTS		SD_HOST_DESKTOPX

#define VERSION_MAJOR 1  
#define VERSION_MINOR 0
#define VERSION_BUILD $WCREV$

#ifdef DEBUG
#define EXPIRATION_YEAR 2009
#define EXPIRATION_MONTH 7
#define EXPIRATION_DAY 1
#endif