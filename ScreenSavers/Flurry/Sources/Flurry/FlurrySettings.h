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

#pragma once

#include <atlbase.h>
#include <vector>
using std::vector;

#include "FlurryPreset.h"

#define SHRINK_MIN 0
#define SHRINK_MAX 50
#define SHRINK_FREQ 5

#define PRESETS_READONLY 5 // the first 5 presets are read-only

namespace Flurry {

	class Settings 
	{

		private:

			void ReadMonitors(CRegKey& reg);
			void WriteMonitors(CRegKey& reg);

			void ReadVisuals(CRegKey& reg);
			void WriteVisuals(CRegKey& reg);

			void ResetPresetSettings(CRegKey& reg, int index);			

		public:

			// Double Buffering support
			enum { 
				BUFFER_MODE_SINGLE,
				BUFFER_MODE_FAST_DOUBLE,
				BUFFER_MODE_SAFE_DOUBLE 
			};

			// Multi monitor support
			enum {
				MULTIMON_ALLMONITORS,
				MULTIMON_PRIMARY,
				MULTIMON_PERMONITOR
			};

			// Parameters
			DWORD settingBufferMode;			// "Buffering mode"
			DWORD multiMonPosition;				// "Multimon behavior"
			DWORD showFPSIndicator;				// "FPS display"
			DWORD bugBlockMode;					// "Freakshow: Block mode"
			DWORD bugWhiteout;					// "Freakshow: Whiteout double buffer"
			DWORD globalPreset;					// "Preset"
			DWORD shrinkPercentage;				// "Shrink by %"
			DWORD maxFrameProgressInMs;			// "Max frame progress"

			// Visuals
			vector<Spec *> visuals;

			// Per-monitor presets
			vector<int> multiMonPreset;

			// Constructor/Destructor
			Settings();
			~Settings();

			// Functions
			void Read();
			void Write();

			void ReloadVisuals();
			void DeleteVisual(int index);

			int GetPresetForMonitor(int monitor);
	};

}
