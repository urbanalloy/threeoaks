/*
 * Flurry for Windows.
 *
 * Screen saver settings code, specific to Windows platform.
 *
 * Created 4/17/2003 by Matt Ginzton, magi@cs.stanford.edu
 */

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
