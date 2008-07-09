///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : Flurry for Windows
//
// Screen saver host code, specific to Windows platform.
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

#include "FlurryGroup.h"
#include "FlurrySettings.h"
#include "FlurryPreset.h"
#include "TimeSupport.h"
#include "AboutBox.h"

///////////////////////////////////////////////////////////////////////////
//
// data types
//

#define RECTWIDTH(rc)  ((rc).right - (rc).left)
#define RECTHEIGHT(rc) ((rc).bottom - (rc).top)

/*
* frames-per-second calculation
*/
#define FPS_SAMPLES 20
typedef struct {
	DWORD startTime;			// in ticks
	DWORD samples[FPS_SAMPLES];	// in ticks
	int nextSample;				// we use samples array as ring buffer
	int nSamples;				// to tell if early/late, and running average
} FPS;

/*
* Per-monitor flurry info.  In multi-monitor mode, we have one of these for
* each monitor; in single-mon mode, we just have one (which might represent
* the entire desktop, and might represent just the primary monitor).
*/
typedef struct {
	int id;				// ordinal just for debugging convenience
	char *device;		// name of display device, or NULL in single-monitor mode
	int updateInterval;	// time between refreshes
	HWND hWnd;			// handle to child window for this monitor
	RECT rc;			// child window rectangle in screen coordinates
	HGLRC hglrc;		// handle to OpenGL rendering context for this window
	HDC hdc;			// handle to DC used by hglrc
	FPS fps;			// frames per second info
	Group *flurry;		// the data structure with info on the flurry clusters
} FlurryAnimateChildInfo;

///////////////////////////////////////////////////////////////////////////
//
// local function prototypes

static void ScreensaverCommonInit(void);
static void ScreensaverRuntimeInit(HWND hWnd);
static void AttachGLToWindow(FlurryAnimateChildInfo *child);
static void DetachGLFromWindow(FlurryAnimateChildInfo *child);
static void CopyFrontBufferToBack(HWND hWnd);
static LRESULT WINAPI FlurryAnimateChildWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void SettingsDialogInit(HWND hWnd);
static void SettingsDialogEnableControls(HWND hWnd);
static void SettingsToDialog(HWND hWnd);
static void SettingsFromDialog(HWND hWnd);
static void ScreenSaverCreateChildren(HWND hWndParent);
static BOOL CALLBACK ScreenSaverCreateChildrenCb(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
static void ScreenSaverCreateChild(HWND hWndParent, RECT *rc, int iMonitor, char *device);
static void ScreenSaverUpdateFpsIndicator(FlurryAnimateChildInfo *child);
static void DoTestScreenSaver(void);