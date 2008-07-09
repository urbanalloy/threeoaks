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

#include <atlbase.h>
#include <vector>
using std::vector;

#include "FlurryPreset.h"

#define SHRINK_MIN 0
#define SHRINK_MAX 50
#define SHRINK_FREQ 5

namespace Flurry {

	class Settings 
	{

		private:

			void ReadMonitors(CRegKey& reg);
			void WriteMonitors(CRegKey& reg);

			void ReadVisuals(CRegKey& reg);
			void WriteVisuals(CRegKey& reg);

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
	};

}
