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


#include <assert.h>

#include "FlurrySettings.h"
#include "FlurryPreset.h"

using namespace Flurry;

static const char flurryRegistryKey[] = "Software\\Flurry";

////////////////////////////////////////////////////////////////////////////////////////
Settings::Settings() : settingBufferMode(BUFFER_MODE_SINGLE),
					   multiMonPosition(MULTIMON_ALLMONITORS),
					   showFPSIndicator(0),
					   bugBlockMode(0),
					   bugWhiteout(0),
					   globalPreset(0),
					   shrinkPercentage(0),
					   maxFrameProgressInMs(30)
{}


////////////////////////////////////////////////////////////////////////////////////////
void Settings::Read()
{
	CRegKey reg;
	DWORD formatLen = sizeof(DWORD);

	// open the registry key
	BOOL opened = (ERROR_SUCCESS == reg.Open(HKEY_CURRENT_USER, flurryRegistryKey, KEY_READ));

	if (!opened) {
		return;
	}
	
	reg.QueryValue("Buffering mode", NULL, &settingBufferMode, &formatLen);
	reg.QueryValue("Multimon behavior", NULL, &multiMonPosition, &formatLen);
	reg.QueryValue("FPS display", NULL, &showFPSIndicator, &formatLen);
	reg.QueryValue("Freakshow: Block mode", NULL, &bugBlockMode, &formatLen);
	reg.QueryValue("Freakshow: Whiteout doublebuffer", NULL, &bugWhiteout, &formatLen);
	reg.QueryValue("Preset", NULL, &globalPreset, &formatLen);
	reg.QueryValue("Shrink by %", NULL, &shrinkPercentage, &formatLen);
	reg.QueryValue("Max frame progress", NULL, &maxFrameProgressInMs, &formatLen);

	// read per-monitor and per-preset settings
	ReadVisuals(reg);
	ReadMonitors(reg);
}

////////////////////////////////////////////////////////////////////////////////////////
void Settings::Write()
{
	CRegKey reg;

	// create the registry key
	if (ERROR_SUCCESS != reg.Create(HKEY_CURRENT_USER, flurryRegistryKey)) {
		return;
	}

	reg.SetValue("Buffering mode", REG_DWORD, &settingBufferMode, sizeof(DWORD));
	reg.SetValue("Multimon behavior", REG_DWORD, &multiMonPosition, sizeof(DWORD));
	reg.SetValue("FPS display", REG_DWORD, &showFPSIndicator, sizeof(DWORD));
	reg.SetValue("Freakshow: Block mode", REG_DWORD, &bugBlockMode, sizeof(DWORD));
	reg.SetValue("Freakshow: Whiteout doublebuffer", REG_DWORD, &bugWhiteout, sizeof(DWORD));
	reg.SetValue("Preset", REG_DWORD, &globalPreset, sizeof(DWORD));
	reg.SetValue("Shrink by %", REG_DWORD, &shrinkPercentage, sizeof(DWORD));
	reg.SetValue("Max frame progress", REG_DWORD, &maxFrameProgressInMs, sizeof(DWORD));

	// write per-monitor and per-preset settings
	WriteVisuals(reg);
	WriteMonitors(reg);
}

//////////////////////////////////////////////////////////////////////////
void Settings::ReadVisuals(CRegKey& reg)
{
	// the first N are hardcoded; we put them in the registry so the user can
	// see what they look like for inspiration, but they can change them till
	// they're blue in the face and we won't care.

	char *PresetVisuals[] = {
		"Classic:{5,tiedye,100,1.0}",
		"RGB:{3,red,100,0.8};{3,blue,100,0.8};{3,green,100,0.8}",
		"Water:{1,blue,100.0,2.0};{1,blue,100.0,2.0};{1,blue,100.0,2.0};{1,blue,100.0,2.0};{1,blue,100.0,2.0};{1,blue,100.0,2.0};{1,blue,100.0,2.0};{1,blue,100.0,2.0};{1,blue,100.0,2.0}",
		"Fire:{12,slowCyclic,10000.0,0.0}",
		"Psychedelic:{10,rainbow,200.0,2.0}"
	};
	static const int nPresets = sizeof PresetVisuals / sizeof PresetVisuals[0];

	int i = 0;
	for (i = 0; i < nPresets; i++) {
		Spec *preset = new Spec(PresetVisuals[i]);
		if (preset->valid) {
			visuals.push_back(preset);
		} else {
			_RPT0(_CRT_WARN, "Things are really messed up... preset doesn't match!\n");
			delete preset;
		}
	}
	assert(visuals.size() > 0);

	// read the rest from the registry
	if (reg == NULL)
		return;

	CRegKey presets;
	presets.Open(reg, "Presets", KEY_READ);

	while (true) {
		char customFormat[2000];
		DWORD formatLen = sizeof customFormat;

		char nextValue[20];
		_snprintf(nextValue, sizeof nextValue, "preset%02d", i++);
		
		//LONG result = presets.QueryValue(customFormat, nextValue, &formatLen);
		LONG result = presets.QueryValue(nextValue, NULL, customFormat, &formatLen);
		if (result != ERROR_SUCCESS) {
			_RPT2(_CRT_WARN, "ReadVisuals: failed to read %s: %d\n", nextValue, result);
			break;
		}

		Spec *preset = new Spec(customFormat);
		if (preset->valid) {
			visuals.push_back(preset);
		} else {
			_RPT0(_CRT_WARN, "ReadVisuals: preset not parseable; WARNING will be removed on ok\n");
			_RPT1(_CRT_WARN, "  '%s'\n", customFormat);
			delete preset;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////
void Settings::WriteVisuals(CRegKey& reg)
{
	char format[2000];

	CRegKey presets;
	presets.Create(reg, "Presets");

	for (int i = 0; i < (signed)visuals.size(); i++) {
		if (visuals[i]->WriteToString(format, sizeof format)) {
			char nextValue[20];
			_snprintf(nextValue, sizeof nextValue, "preset%02d", i);
			//presets.SetValue(format, nextValue);
			presets.SetValue(nextValue, REG_SZ, format, sizeof(format) );
		} else {
			_RPT1(_CRT_WARN, "WriteVisuals: Can't write preset %d!\n", i);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////
void Settings::ReadMonitors(CRegKey& reg)
{
	// first monitor = already known
	multiMonPreset.push_back(globalPreset);
	int iMonitor = 1;

	// other monitors, read from registry
	if (reg == NULL)
		return;

	CRegKey monitors;
	monitors.Open(reg, "Monitors", KEY_READ);

	while (true) {
		char nextValue[20];
		_snprintf_s(nextValue, sizeof nextValue, "monitor%02dPreset", iMonitor++, 20*sizeof(char));
		
		DWORD presetForMonitor;
		DWORD formatLen = sizeof(presetForMonitor);
		//LONG result = monitors.QueryValue(presetForMonitor, nextValue);
		LONG result = monitors.QueryValue(nextValue, NULL, &presetForMonitor, &formatLen);
		if (result != ERROR_SUCCESS) {
			_RPT2(_CRT_WARN, "ReadMonitors: failed to read %s: %d\n", nextValue, result);
			break;
		}

		if (presetForMonitor >= visuals.size()) {
			presetForMonitor = globalPreset;
		}

		_RPT2(_CRT_WARN, "ReadMonitors: monitor %d using visual %d\n",
			  multiMonPreset.size(), presetForMonitor);
		multiMonPreset.push_back(presetForMonitor);
	}
}


////////////////////////////////////////////////////////////////////////////////////////
void Settings::WriteMonitors(CRegKey& reg)
{
	CRegKey monitors;
	monitors.Create(reg, "Monitors");

	for (int iMonitor = 0; iMonitor < (signed)multiMonPreset.size(); iMonitor++) {
		char nextValue[20];
		_snprintf_s(nextValue, sizeof nextValue, "monitor%02dPreset", iMonitor, 20*sizeof(char));
		//monitors.SetValue(g_multiMonPreset[iMonitor], nextValue);
		monitors.SetValue(nextValue, REG_DWORD, &multiMonPreset[iMonitor], sizeof(int) );
	}
}

