///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : Flurry for Windows
//
// Screen saver host code, specific to Windows platform.
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

#include "FlurrySettings.h"
#include "FlurryGroup.h"
#include "TimeSupport.h"

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
typedef struct FPS{
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
typedef struct FlurryAnimateChildInfo {
	int id;				// ordinal just for debugging convenience
	char *device;		// name of display device, or NULL in single-monitor mode
	int updateInterval;	// time between refreshes
	HWND hWnd;			// handle to child window for this monitor
	RECT rc;			// child window rectangle in screen coordinates
	HGLRC hglrc;		// handle to OpenGL rendering context for this window
	HDC hdc;			// handle to DC used by hglrc
	FPS fps;			// frames per second info
	Flurry::Group *flurry;		// the data structure with info on the flurry clusters
} FlurryAnimateChildInfo;

///////////////////////////////////////////////////////////////////////////
//
// local function prototypes

static void ScreensaverCommonInit();
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
static void DoTestScreenSaver();
static void LoadDialogPresets(HWND hWnd);
static void UpdateEditButtons(HWND hWnd);
static BOOL WINAPI AssignDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);