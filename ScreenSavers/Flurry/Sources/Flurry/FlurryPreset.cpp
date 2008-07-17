///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : Preset class
//
// Flurry preset specification
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

#include <windows.h>
#include <string.h>
#include <crtdbg.h>
#include <assert.h>

#include "FlurryPreset.h"

using namespace Flurry;

/*
 * Implementation
 */

Spec::Spec(char *format) : specTemplate(string(format))						  
{
	ParseTemplate();
}

Spec::Spec(const Spec& arg) :  specTemplate(arg.specTemplate)					 
{
	ParseTemplate();
}


Spec::~Spec(void)
{	
}


void Spec::ParseTemplate()
{
	// format is: name:{streams,color,thickness,speed}(;stream)+
	// try to parse this out; if at any time we fail, exit leaving valid == false
	const char *format = specTemplate.c_str();

	// find name
	const char *psz = strchr(format, ':');
	if (!psz) {
		_RPT1(_CRT_WARN, "PresetParse: no name in '%s'\n", format);
		isValid = false;
		return;
	}

	// copy name from format
	name = string(format).substr(0, psz - format);	

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
			isValid = false;
			return;
		}
		cluster.color = ColorModeFromName(color);
		if (cluster.color < 0) {
			_RPT1(_CRT_WARN, "PresetParse: bad color name '%s'\n", color);
			isValid = false;
			return;
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
	_RPT2(_CRT_WARN, "Read preset '%s': %d clusters\n", name.c_str(), clusters.size());
	isValid = true;	
}


bool Spec::WriteToString(char *format, int formatLen)
{
	_snprintf(format, formatLen, "%s:", name.c_str());
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
