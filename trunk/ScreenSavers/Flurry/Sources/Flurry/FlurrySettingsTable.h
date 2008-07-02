/*
 * Flurry for Windows.
 *
 * Screen saver settings code, specific to Windows platform.
 *
 * Created 4/17/2003 by Matt Ginzton, magi@cs.stanford.edu
 */

#ifndef _FLURRY_SETTINGS_TABLE_H_
#define _FLURRY_SETTINGS_TABLE_H_

/*
 * This part of the header only gets included once per source file
 */

enum { BUFFER_MODE_SINGLE, BUFFER_MODE_FAST_DOUBLE, BUFFER_MODE_SAFE_DOUBLE };
enum { MULTIMON_ALLMONITORS, MULTIMON_PRIMARY, MULTIMON_PERMONITOR };

#endif // _FLURRY_SETTINGS_TABLE_H_

/*
 * This part of the header may get included several times per source file
 */

DECLARE_SETTINGS_VAR(DWORD, iSettingBufferMode, "Buffering mode", BUFFER_MODE_SINGLE)
DECLARE_SETTINGS_VAR(DWORD, iMultiMonPosition, "Multimon behavior", MULTIMON_ALLMONITORS)
DECLARE_SETTINGS_VAR(DWORD, iShowFPSIndicator, "FPS display", 0)
DECLARE_SETTINGS_VAR(DWORD, iBugBlockMode, "Freakshow: Block mode", 0)
DECLARE_SETTINGS_VAR(DWORD, iBugWhiteout, "Freakshow: Whiteout doublebuffer", 0)
DECLARE_SETTINGS_VAR(DWORD, iFlurryPreset, "Preset", 0)
DECLARE_SETTINGS_VAR(DWORD, iFlurryShrinkPercentage, "Shrink by %", 0)
DECLARE_SETTINGS_VAR(DWORD, iMaxFrameProgressInMs, "Max frame progress", 30)
