///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : Settings class
//
// Screen saver settings code, specific to Windows platform
//
// (c) 2003 Matt Ginzton (magi@cs.stanford.edu)
// (c) 2006-2008 Julien Templier
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern void Settings_Read();
extern void Settings_Write();

#define DECLARE_SETTINGS_VAR(type, name, regName, defVal)	\
	extern type name;

#include "FlurrySettingsTable.h"

#undef DECLARE_SETTINGS_VAR

#ifdef __cplusplus
}
#endif
