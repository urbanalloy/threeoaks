/*
 * Flurry for Windows.
 *
 * Screen saver settings code, specific to Windows platform.
 *
 * Created 4/17/2003 by Matt Ginzton, magi@cs.stanford.edu
 */

#include <windows.h>
#include <atlbase.h>
#include <crtdbg.h>
#include "FlurrySettings.h"
#include "FlurryPreset.h"


static const char flurryRegistryKey[] = "Software\\Flurry";

/*
 * Globals (exported to other modules)
 *
 * automatic via FlurrySettingsTable
 */

#define DECLARE_SETTINGS_VAR(type, name, regName, defVal)	\
	type name;
#include "FlurrySettingsTable.h"
#undef DECLARE_SETTINGS_VAR

/*
 * Settings table (for read/write routines)
 */

typedef struct {
	DWORD *storage;
	char *regName;
	DWORD defaultVal;
} SettingsTable;

static SettingsTable settingsTable[] = {

#define DECLARE_SETTINGS_VAR(type, name, regName, defVal)	\
	{ &name, regName, defVal },
#include "FlurrySettingsTable.h"
#undef DECLARE_SETTINGS_VAR

};

static const int nSettings = sizeof settingsTable / sizeof settingsTable[0];


void
Settings_Read()
{
	CRegKey reg;
	bool opened;
	DWORD formatLen = sizeof(DWORD);

	// open the registry key
	opened = (ERROR_SUCCESS == reg.Open(HKEY_CURRENT_USER, flurryRegistryKey, KEY_READ));

	// read the table-driven settings
	for (int i = 0; i < nSettings; i++) {
		bool read = false;
		if (opened) {
			//read = (ERROR_SUCCESS == reg.QueryValue(*settingsTable[i].storage, settingsTable[i].regName));
			read = (ERROR_SUCCESS == reg.QueryValue(settingsTable[i].regName, NULL, settingsTable[i].storage, &formatLen));
		}

		if (read) {
			_RPT2(_CRT_WARN, "%s read as %u\n", settingsTable[i].regName, *settingsTable[i].storage);
		} else {
			*settingsTable[i].storage = settingsTable[i].defaultVal;
			_RPT2(_CRT_WARN, "%s default to %u\n", settingsTable[i].regName, *settingsTable[i].storage);
		}
	}

	// read per-monitor and per-preset settings
	Visuals_Read(reg);
	Monitors_Read(reg);
}


void
Settings_Write()
{
	CRegKey reg;

	// create the registry key
	if (ERROR_SUCCESS != reg.Create(HKEY_CURRENT_USER, flurryRegistryKey)) {
		return;
	}

	// write the table-driven settings
	for (int i = 0; i < nSettings; i++) {
		//reg.SetValue(*settingsTable[i].storage, settingsTable[i].regName);
		reg.SetValue(settingsTable[i].regName, REG_DWORD, settingsTable[i].storage, sizeof(DWORD) );
	}

	// write per-monitor and per-preset settings
	Visuals_Write(reg);
	Monitors_Write(reg);
}
