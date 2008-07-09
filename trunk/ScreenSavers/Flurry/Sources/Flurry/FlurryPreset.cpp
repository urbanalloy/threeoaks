///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : Preset class
//
// Flurry preset specification
//
// (c) 2003 Matt Ginzton (magi@cs.stanford.edu)
// (c) 2006-2008 Julien Templier
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <string.h>
#include <crtdbg.h>
#include <assert.h>

#include "FlurryPreset.h"

using namespace Flurry;

/*
 * Implementation
 */

Spec::Spec(char *format)
{
	name = NULL;
	valid = ParseFromString(format);
}


Spec::~Spec(void)
{
	free(name);
}


bool Spec::ParseFromString(char *format)
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
	ClusterSpec cluster;
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
		cluster.color = ColorModeFromName(color);
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


bool Spec::WriteToString(char *format, int formatLen)
{
	_snprintf(format, formatLen, "%s:", name);
	format += strlen(format);
	formatLen -= strlen(format);

	for (int i = 0; i < (signed)clusters.size(); i++) {
		bool more = (i + 1 < (signed)clusters.size());
		const char *color = ColorModeToName(clusters[i].color);

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




// keep parallel with typedef enum _ColorModes in GL_saver.h
const char* Spec::colorTable[] = {
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

int Spec::ColorModeFromName(char *colorName)
{

	for (int iColor = 0; iColor < nColors; iColor++) {
		if (!strcmp(colorName, colorTable[iColor])) {
			return iColor;
		}
	}

	return -1;
}


const char* Spec::ColorModeToName(int colorMode)
{
	if (colorMode >= nColors) {
		colorMode = 0;
	}
	return colorTable[colorMode];
}
