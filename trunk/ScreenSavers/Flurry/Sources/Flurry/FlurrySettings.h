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
		DWORD iSettingBufferMode;				// "Buffering mode"
		DWORD iMultiMonPosition;				// "Multimon behavior"
		DWORD iShowFPSIndicator;				// "FPS display"
		DWORD iBugBlockMode;					// "Freakshow: Block mode"
		DWORD iBugWhiteout;						// "Freakshow: Whiteout double buffer"
		DWORD iFlurryPreset;					// "Preset"
		DWORD iFlurryShrinkPercentage;			// "Shrink by %"
		DWORD iMaxFrameProgressInMs;			// "Max frame progress"

		// Constructor/Destructor
		Settings();
		~Settings();

		// Functions
		void Read();
		void Write();
	};

}
