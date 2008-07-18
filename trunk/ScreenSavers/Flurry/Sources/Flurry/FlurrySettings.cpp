///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : Settings class
//
// Screen saver settings code, specific to Windows platform
//
// Copyright (c) 2003, Matt Ginzton
// Copyright (c) 2005-2008, Julien Templier 
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// o Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// o Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// o Neither the name of the author nor the names of its contributors may
//   be used to endorse or promote products derived from this software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
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

	// Read settings - we have to read visuals and monitors still, as they are initialized theres
	if (opened) {
		reg.QueryValue("Buffering mode", NULL, &settingBufferMode, &formatLen);
		reg.QueryValue("Multimon behavior", NULL, &multiMonPosition, &formatLen);
		reg.QueryValue("FPS display", NULL, &showFPSIndicator, &formatLen);
		reg.QueryValue("Freakshow: Block mode", NULL, &bugBlockMode, &formatLen);
		reg.QueryValue("Freakshow: Whiteout doublebuffer", NULL, &bugWhiteout, &formatLen);
		reg.QueryValue("Preset", NULL, &globalPreset, &formatLen);
		reg.QueryValue("Shrink by %", NULL, &shrinkPercentage, &formatLen);
		reg.QueryValue("Max frame progress", NULL, &maxFrameProgressInMs, &formatLen);
	}

	// read per-monitor and per-preset settings
	ReadVisuals(reg);
	ReadMonitors(reg);
}

////////////////////////////////////////////////////////////////////////////////////////
void Settings::Write()
{
	CRegKey reg;

	// create the registry key
	if (ERROR_SUCCESS != reg.Create(HKEY_CURRENT_USER, flurryRegistryKey))
		return;

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
		if (preset->IsValid()) {
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
	if (ERROR_SUCCESS != presets.Open(reg, "Presets", KEY_READ))
		return;

	while (true) {
		char customFormat[2000];
		DWORD formatLen = sizeof customFormat;

		char nextValue[20];
		_snprintf_s(nextValue, sizeof nextValue, "preset%02d", i++);
		
		//LONG result = presets.QueryValue(customFormat, nextValue, &formatLen);
		LONG result = presets.QueryValue(nextValue, NULL, customFormat, &formatLen);
		if (result != ERROR_SUCCESS) {
			_RPT2(_CRT_WARN, "ReadVisuals: failed to read %s: %d\n", nextValue, result);
			break;
		}

		Spec *preset = new Spec(customFormat);
		if (preset->IsValid()) {
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
	// Cleanup Presets key
	reg.DeleteSubKey("Presets");

	CRegKey presets;
	presets.Create(reg, "Presets");

	for (int i = 0; i < (signed)visuals.size(); i++) {
		
		if (visuals[i]->IsValid()) {
			
			string format = visuals[i]->GetTemplate();

			char nextValue[20];
			_snprintf_s(nextValue, sizeof nextValue, "preset%02d", i);
			//presets.SetValue(format, nextValue);
			presets.SetValue(nextValue, REG_SZ, format.c_str(), format.size() );
		} else {
			_RPT1(_CRT_WARN, "WriteVisuals: Can't write preset %d!\n", i);
		}
	}	
}


////////////////////////////////////////////////////////////////////////////////////////
void Settings::ReadMonitors(CRegKey& reg)
{
	int iMonitor = 0;

	if (reg == NULL)
		return;

	CRegKey monitors;	
	if (ERROR_SUCCESS != monitors.Open(reg, "Monitors", KEY_READ))
		return;

	while (true) {
		char nextValue[20];
		_snprintf_s(nextValue, sizeof nextValue, "monitor%02dPreset", iMonitor++, 20*sizeof(char));
		
		DWORD presetForMonitor;
		DWORD formatLen = sizeof(presetForMonitor);
		
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

		monitors.SetValue(nextValue, REG_DWORD, &multiMonPreset[iMonitor], sizeof(int) );
	}
}

int Settings::GetPresetForMonitor(int monitor)
{
	// return 0 (default preset), if we are passed a wrong parameter
	if (monitor < 0)
		return 0;

	//  if there is no preset for that monitor, create a default one
	if (monitor >= (signed)multiMonPreset.size())	
		multiMonPreset.push_back(0);
	
	return multiMonPreset[monitor];
}


//////////////////////////////////////////////////////////////////////////
// Delete the visual at index
// if the current preset is the deleted one, reset to default preset
void Settings::DeleteVisual(int index)
{

	visuals.erase(visuals.begin() + index);

	// Global preset
	if ((signed)globalPreset == index)
		globalPreset = 0;

	for (int i = 0; i < (signed)multiMonPreset.size(); i++) {

		if (multiMonPreset[i] != index)
			break;

		multiMonPreset[i] = 0;
	}

}

