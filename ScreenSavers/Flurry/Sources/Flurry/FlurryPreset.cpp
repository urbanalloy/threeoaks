/*
 * Flurry for Windows.
 *
 * Flurry preset specification
 *
 * Created 5/31/2003 by Matt Ginzton, magi@cs.stanford.edu
 */

#include <windows.h>
#include <string.h>
#include <crtdbg.h>
#include <assert.h>
#include "FlurryPreset.h"
#include "FlurrySettings.h"

/*
 * Module globals
 */

vector<int> g_multiMonPreset;
vector<FlurrySpec *> g_visuals;


/*
 * Local functions
 */

static int FlurryColorModeFromName(char *colorName);
static const char *FlurryColorModeToName(int colorMode);


/*
 * Implementation
 */

FlurrySpec::FlurrySpec(char *format)
{
	name = NULL;
	valid = ParseFromString(format);
}


FlurrySpec::~FlurrySpec(void)
{
	free(name);
}


bool
FlurrySpec::ParseFromString(char *format)
{
	// format is: name:{streams,color,thickness,speed}(;stream)+
	// try to parse this out; if at any time we fail, exit leaving valid == false

	// find name
	char *psz = strchr(format, ':');
	if (!psz) {
		_RPT1(_CRT_WARN, "PresetParse: no name in '%s'\n", format);
		return false;
	}

	// copy name from format
	int len = psz - format;
	name = (char *)malloc(len + 1);
	strncpy(name, format, len);
	name[len] = 0;

	// find streams
	FlurryClusterSpec cluster;
	format = psz + 1; // just past the colon
	
	while (true) {
		// parse current stream
		char color[26];
		int parsed = sscanf(format, "{%d,%25[^,],%f,%f}",
							&cluster.nStreams, color, &cluster.thickness,
							&cluster.speed);
		if (parsed != 4) {
			_RPT2(_CRT_WARN, "PresetParse: no stream match (%d of 4) @ '%s'\n",
				  parsed, format);
			return false;
		}
		cluster.color = FlurryColorModeFromName(color);
		if (cluster.color < 0) {
			_RPT1(_CRT_WARN, "PresetParse: bad color name '%s'\n", color);
			return false;
		}

		// if it looks good, add it
		clusters.push_back(cluster);

		// and move on to the next one
		psz = strchr(format, ';');
		if (psz) {
			format = psz + 1;
		} else {
			break;
		}
	}

	// hooray, we win!
	_RPT2(_CRT_WARN, "Read preset '%s': %d clusters\n", name, clusters.size());
	return true;
}


bool
FlurrySpec::WriteToString(char *format, int formatLen)
{
	_snprintf(format, formatLen, "%s:", name);
	format += strlen(format);
	formatLen -= strlen(format);

	for (int i = 0; i < (signed)clusters.size(); i++) {
		bool more = (i + 1 < (signed)clusters.size());
		const char *color = FlurryColorModeToName(clusters[i].color);

		_snprintf(format, formatLen, "{%d,%s,%g,%g}%s",
				  clusters[i].nStreams, color, clusters[i].thickness, clusters[i].speed,
				  more ? ";" : "");
		format += strlen(format);
		formatLen -= strlen(format);

		if (formatLen <= 0) {
			return false;
		}
	}

	return true;
}


void
Visuals_Read(CRegKey& reg)
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
		FlurrySpec *preset = new FlurrySpec(PresetVisuals[i]);
		if (preset->valid) {
			g_visuals.push_back(preset);
		} else {
			_RPT0(_CRT_WARN, "Things are really messed up... preset doesn't match!\n");
			delete preset;
		}
	}
	assert(g_visuals.size() > 0);

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
			_RPT2(_CRT_WARN, "Visuals_Read: failed to read %s: %d\n", nextValue, result);
			break;
		}

		FlurrySpec *preset = new FlurrySpec(customFormat);
		if (preset->valid) {
			g_visuals.push_back(preset);
		} else {
			_RPT0(_CRT_WARN, "Visuals_Read: preset not parseable; WARNING will be removed on ok\n");
			_RPT1(_CRT_WARN, "  '%s'\n", customFormat);
			delete preset;
		}
	}
}


void
Visuals_Write(CRegKey& reg)
{
	char format[2000];

	CRegKey presets;
	presets.Create(reg, "Presets");

	for (int i = 0; i < (signed)g_visuals.size(); i++) {
		if (g_visuals[i]->WriteToString(format, sizeof format)) {
			char nextValue[20];
			_snprintf(nextValue, sizeof nextValue, "preset%02d", i);
			//presets.SetValue(format, nextValue);
			presets.SetValue(nextValue, REG_SZ, format, sizeof(format) );
		} else {
			_RPT1(_CRT_WARN, "XXX: Can't write preset %d!\n", i);
		}
	}
}


void
Monitors_Read(CRegKey& reg)
{
	// first monitor = already known
	g_multiMonPreset.push_back(iFlurryPreset);
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
			_RPT2(_CRT_WARN, "Visuals_Read: failed to read %s: %d\n", nextValue, result);
			break;
		}

		if (presetForMonitor >= g_visuals.size()) {
			presetForMonitor = iFlurryPreset;
		}

		_RPT2(_CRT_WARN, "Monitors_Read: monitor %d using visual %d\n",
			  g_multiMonPreset.size(), presetForMonitor);
		g_multiMonPreset.push_back(presetForMonitor);
	}
}


void
Monitors_Write(CRegKey& reg)
{
	CRegKey monitors;
	monitors.Create(reg, "Monitors");

	for (int iMonitor = 0; iMonitor < (signed)g_multiMonPreset.size(); iMonitor++) {
		char nextValue[20];
		_snprintf_s(nextValue, sizeof nextValue, "monitor%02dPreset", iMonitor, 20*sizeof(char));
		//monitors.SetValue(g_multiMonPreset[iMonitor], nextValue);
		monitors.SetValue(nextValue, REG_DWORD, &g_multiMonPreset[iMonitor], sizeof(int) );
	}
}


// keep parallel with typedef enum _ColorModes in GL_saver.h
const char *colorTable[] = {
	"red",
	"magenta",
	"blue",
	"cyan",
	"green",
	"yellow",
	"slowCyclic",
	"cyclic",
	"tiedye",
	"rainbow",
	"white",
	"multi",
	"darkColorMode"
};
#define nColors (sizeof colorTable / sizeof colorTable[0])

static int
FlurryColorModeFromName(char *colorName)
{

	for (int iColor = 0; iColor < nColors; iColor++) {
		if (!strcmp(colorName, colorTable[iColor])) {
			return iColor;
		}
	}

	return -1;
}


static const char *
FlurryColorModeToName(int colorMode)
{
	if (colorMode >= nColors) {
		colorMode = 0;
	}
	return colorTable[colorMode];
}
