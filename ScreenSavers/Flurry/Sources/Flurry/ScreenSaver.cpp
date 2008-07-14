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

#include <windows.h>
#include <windowsx.h>
#undef SubclassWindow // collision with atlwin.h
#include <crtdbg.h>
#include <scrnsave.h>
#include <GL/GL.h>
#include <stdio.h>
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>

#include "ScreenSaver.h"
#include "Editor.h"
#include "AboutBox.h"
#include "resource.h"

using namespace Flurry;

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"") 
#pragma warning(disable:4100)

///////////////////////////////////////////////////////////////////////////
//
// shared globals
//

CComModule _Module;
Settings* settings;
HINSTANCE hInstance;


///////////////////////////////////////////////////////////////////////////
//
// module globals
//

static int g_nMonitors = 0;
static int g_iMonitor = 0;
static bool g_bPreviewMode = false;
static bool g_bThumbnailMode = false;


///////////////////////////////////////////////////////////////////////////
//
// public functions


/*
 * ScreensaverCommonInit
 *
 * Init code used by both personalities (configuration and screensaver)
 * when invoked with /s.
 */
static void ScreensaverCommonInit()
{
	// count monitors
	g_nMonitors = GetSystemMetrics(SM_CMONITORS);
}


/*
 * ScreensaverRuntimeInit
 *
 * Init code used only when running as screensaver, not during configuration
 */
static void ScreensaverRuntimeInit(HWND hWnd)
{
	// we're going to use rand(), so...
	srand(GetTickCount());

	// register child window class
	WNDCLASS wc = { 0 };
	wc.lpszClassName = "FlurryAnimateChild";
	wc.lpfnWndProc = FlurryAnimateChildWindowProc;
	RegisterClass(&wc);

	// detect if we're in the desktop properties preview pane...
	// if so, use the window we're given (disable per-monitor behavior)
	RECT rc;
	GetWindowRect(hWnd, &rc);
	_RPT4(_CRT_WARN, "Init in window 0x%08x, parent 0x%08x: %d, %d\n", hWnd, GetParent(hWnd), rc.left, rc.top);

	if (GetParent(hWnd)) {
		settings->multiMonPosition = settings->MULTIMON_ALLMONITORS;
		g_bThumbnailMode = true;
	}

	// initialize timer module
	TimeSupport_Init();

#ifdef _DEBUG
	// debugging behind top-level windows is a pain :P
	SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
#endif
}


/*
 * ScreenSaverProc
 *
 * Scrnsave.lib entry point: called as WndProc for screen saver window
 * when invoked with /s.
 *
 * We implement one big background window, spanning the whole desktop,
 * which has 1 or more children (typically one for each monitor), each
 * of which renders a FlurryGroup.
 */
LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_CREATE:
			if (settings == NULL)
				settings = new Settings();

			if (!g_bPreviewMode) {
				// initialization, since this is the first chunk of our code to run
				ScreensaverCommonInit();
				settings->Read();
			} else {
				// we've already done the common init via the configuration dialog,
				// but we need to set a timer to disable the ignore-input code.
				SetTimer(hWnd, 0, 500, NULL);
			}
			ScreensaverRuntimeInit(hWnd);

			// prepare child windows for rendering
			ScreenSaverCreateChildren(hWnd);
			return 0;

		case WM_KEYDOWN:
			if (wParam == 'F') {
				settings->showFPSIndicator = !settings->showFPSIndicator;
			}
			break;

		case WM_TIMER:
			// cancel ignoring of input
			g_bPreviewMode = false;
			break;

		case WM_SETCURSOR:
			if (!g_bThumbnailMode) {
				SetCursor(NULL);
			}
			break;
	}

	// avoid bailing on input when right control key is down,
	// or in first 1/2 second of preview mode
	bool bIgnoreInput = (GetKeyState(VK_RCONTROL) >> 31) || g_bPreviewMode;

	return (bIgnoreInput ?
		DefWindowProc :
		DefScreenSaverProc)(hWnd, message, wParam, lParam);
}


/*
 * Prepare child windows for rendering
 */
static void ScreenSaverCreateChildren(HWND hWndParent)
{
	RECT rc;
	g_iMonitor = 0;

	switch (settings->multiMonPosition) {
		case settings->MULTIMON_PERMONITOR:
			EnumDisplayMonitors(NULL, NULL, ScreenSaverCreateChildrenCb,
								(LPARAM)hWndParent);
			return;

		case settings->MULTIMON_PRIMARY:
			SetRect(&rc, 0, 0,
					GetSystemMetrics(SM_CXSCREEN),
					GetSystemMetrics(SM_CYSCREEN));
			break;

		case settings->MULTIMON_ALLMONITORS:
		default:
			GetClientRect(hWndParent, &rc);
			break;
	}

	ScreenSaverCreateChild(hWndParent, &rc, 0, NULL);
}


/*
 * Create in each Monitor
 */
static BOOL CALLBACK ScreenSaverCreateChildrenCb(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	HWND hWndParent = (HWND)dwData;
	RECT rcMonitorChild;

	MONITORINFOEX mi;
	memset(&mi, 0, sizeof mi);
	mi.cbSize = sizeof mi;
	GetMonitorInfo(hMonitor, (MONITORINFO *)&mi);
	_RPT1(_CRT_WARN, "Found monitor: %s\n", mi.szDevice);

	rcMonitorChild = *lprcMonitor;
	ScreenToClient(hWndParent, ((POINT*)&rcMonitorChild) + 0);
	ScreenToClient(hWndParent, ((POINT*)&rcMonitorChild) + 1);

	ScreenSaverCreateChild(hWndParent, &rcMonitorChild, g_iMonitor++, mi.szDevice);
	return TRUE;
}


static void ScreenSaverCreateChild(HWND hWndParent, RECT *rc, int iMonitor, char *device)
{
	char szName[20];
	FlurryAnimateChildInfo *child;

	wsprintf(szName, "flurryMon%d", iMonitor);
	_RPT2(_CRT_WARN, "Creating child %s on device %s:\n", szName, device);

	child = new FlurryAnimateChildInfo;
	memset(child, 0, sizeof *child);
	child->id = iMonitor;
	child->rc = *rc;
	child->device = device ? _strdup(device) : NULL;
	child->fps.startTime = timeGetTime();

	// 200: n% / 100, and it counts on each size, so / 2 more
	InflateRect(&child->rc,
		-(int)(settings->shrinkPercentage * RECTWIDTH(child->rc) / 200),
		-(int)(settings->shrinkPercentage * RECTHEIGHT(child->rc) / 200));

	_RPT4(_CRT_WARN, "  position %d, %d, %d, %d\n", child->rc.left, child->rc.top, RECTWIDTH(child->rc), RECTHEIGHT(child->rc));

	CreateWindow("FlurryAnimateChild", szName, WS_VISIBLE | WS_CHILD,
		child->rc.left, child->rc.top, RECTWIDTH(child->rc), RECTHEIGHT(child->rc),
		hWndParent, NULL, NULL, child);
}


/*
 * FlurryAnimateChildWindowProc
 *
 * WndProc for the child windows that actually do the drawing.
 * We handle WM_CREATE (setup), various painting and timer messages for
 * animation, and forward mouse and keyboard messages to our parent.
 */
LRESULT WINAPI FlurryAnimateChildWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	FlurryAnimateChildInfo *child = (FlurryAnimateChildInfo *) GetWindowLong(hWnd, GWL_USERDATA);

#ifdef _DEBUG
	static int iFrameCounter = 0;
#endif

	switch (message) {
		case WM_CREATE: {
			CREATESTRUCT *create = (CREATESTRUCT *)lParam;
			// initialize per-child struct as window data
			child = (FlurryAnimateChildInfo *)create->lpCreateParams;
			child->hWnd = hWnd;
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)child);
			// initialize flurry struct
			int preset = (settings->multiMonPosition == Settings::MULTIMON_PERMONITOR ?
						  settings->GetPresetForMonitor(child->id) :
						  settings->globalPreset);
			child->flurry = new Group(preset, settings);
			// prepare OpenGL context
			AttachGLToWindow(child);
			// prepare Flurry code --  must come after OpenGL initialization
			child->flurry->PrepareToAnimate();
			// set repaint timer
			SetTimer(hWnd, 1, child->updateInterval, NULL);
			// set up text parameters, in case we want to say anything
			SetTextColor(child->hdc, 0xFFFFFF);
			SetBkColor(child->hdc, 0x000000);
			//SetBkMode(child->hdc, TRANSPARENT);
			return 0;	
		}

		case WM_ERASEBKGND:
			// Never erase, so we get that nice fade effect initially and
			// between frames.
			return 0;

		case WM_PAINT:
			_RPT1(_CRT_WARN, "Start render frame %d\n", iFrameCounter);
			if (wglMakeCurrent(child->hdc, child->hglrc))
			{
				PAINTSTRUCT ps;
				BeginPaint(hWnd, &ps);
				CopyFrontBufferToBack(hWnd);	// always call; may do nothing
				child->flurry->AnimateOneFrame();
				if (settings->settingBufferMode > settings->BUFFER_MODE_SINGLE) {
					// ATI Radeon 9700s seem to get really upset if we call
					// SwapBuffers in a single-buffered context when invoked
					// with /p.  So be careful not to!
					SwapBuffers(ps.hdc);
				}
				EndPaint(hWnd, &ps);
			} else {
				_RPT1(_CRT_WARN, "OnPaint: wglMakeCurrent failed, error %d\n",
					GetLastError());
			}
			ScreenSaverUpdateFpsIndicator(child);
			_RPT1(_CRT_WARN, "End render frame %d\n", iFrameCounter++);

			return 0;			

		case WM_TIMER:
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
			return 0;

		case WM_DESTROY:
			DetachGLFromWindow(child);
			delete child->flurry;
			free(child->device);
			free(child);
			SetWindowLong(hWnd, GWL_USERDATA, 0);
			return 0;

		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			// forward input messages to parent, which will probably dismiss us
			// note that the children don't actually get keyboard messages; the
			// main window has the focus.
			return SendMessage(GetParent(hWnd), message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


/*
 * RegisterDialogClasses
 *
 * Scrnsave.lib entry point: called before ScreenSaverConfigureDialog when
 * invoked with /c.
 */
BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
	_Module.Init(NULL, (HINSTANCE)hInst);
	hInstance = (HINSTANCE)hInst;
	return TRUE;
}

// WGL attach/detach code
static void AttachGLToWindow(FlurryAnimateChildInfo *child)
{
	// find current display settings on this monitor
	DEVMODE mode;
	EnumDisplaySettings(child->device, ENUM_CURRENT_SETTINGS, &mode);
	_RPT4(_CRT_WARN, "  current display settings %dx%dx%dbpp@%dHz\n",
		mode.dmPelsWidth, mode.dmPelsHeight, mode.dmBitsPerPel, mode.dmDisplayFrequency);
	if (mode.dmDisplayFrequency == 0) {	// query failed
		mode.dmDisplayFrequency = 60;	// default to sane value
	}
	_RPT1(_CRT_WARN, "  refresh time = %d ms\n", 1000 / mode.dmDisplayFrequency);
	child->updateInterval = 1000 / mode.dmDisplayFrequency;

	// build a pixel format
	int iPixelFormat;
	RECT rc;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),    // structure size
		1,                                // version number
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL, // flags: support OpenGL rendering to visible window
		PFD_TYPE_RGBA,                    // pixel format: RGBA
		(BYTE)mode.dmBitsPerPel,          // color depth, excluding alpha -- use whatever it's doing now
		0, 0, 0, 0, 0, 0, 8, 0,           // color bits ignored?
		0,                                // no accumulation buffer
		0, 0, 0, 0,                       // accum bits ignored
		16,                               // 16-bit z-buffer
		0,                                // no stencil buffer
		0,                                // no auxiliary buffer
		PFD_MAIN_PLANE,                   // main layer
		0,                                // reserved
		0, 0, 0                           // layer masks ignored
	};

	if (settings->settingBufferMode > settings->BUFFER_MODE_SINGLE) {
		pfd.dwFlags |= PFD_DOUBLEBUFFER;
	}

	// need a DC for the pixel format
	child->hdc = GetDC(child->hWnd);

	// apply pixel format to DC
	iPixelFormat = ChoosePixelFormat(child->hdc, &pfd);
	SetPixelFormat(child->hdc, iPixelFormat, &pfd);

	// then use this to create a rendering context
	child->hglrc = wglCreateContext(child->hdc);
	wglMakeCurrent(child->hdc, child->hglrc);

	// tell Flurry to use the whole window as viewport
	GetClientRect(child->hWnd, &rc);
	child->flurry->SetSize(rc.right - rc.left, rc.bottom - rc.top);

	// some nice debug output
	_RPT4(_CRT_WARN, "  child 0x%08x: hWnd 0x%08x, hdc 0x%08x, hglrc 0x%08x\n",
		child, child->hWnd, child->hdc, child->hglrc);
	_RPT1(_CRT_WARN, "  GL vendor:     %s\n", glGetString(GL_VENDOR));
	_RPT1(_CRT_WARN, "  GL renderer:   %s\n", glGetString(GL_RENDERER));
	_RPT1(_CRT_WARN, "  GL version:    %s\n", glGetString(GL_VERSION));
	_RPT1(_CRT_WARN, "  GL extensions: %s\n", glGetString(GL_EXTENSIONS));
	_RPT0(_CRT_WARN, "\n");
}


static void DetachGLFromWindow(FlurryAnimateChildInfo *child)
{
	if (child->hglrc == wglGetCurrentContext()) {
		_RPT1(_CRT_WARN, "Evicting context %d\n", child->id);
		wglMakeCurrent(NULL, NULL);
	}
	if (!wglDeleteContext(child->hglrc)) {
		_RPT2(_CRT_WARN, "Failed to delete context for %d: %d\n",
			child->id, GetLastError());
	}
	ReleaseDC(child->hWnd, child->hdc);
}


static void CopyFrontBufferToBack(HWND hWnd)
{
	static BOOL bFirstTime = TRUE;

	// copy front buffer to back buffer, to compensate for Windows'
	// possibly weird implementation of SwapBuffers().  As documented, it
	// reserves the right to leave the back buffer completely undefined
	// after each swap, but on both my ATI Radeon 8500 and NVidia GF4Ti4200
	// it works almost fine to just copy front to back once like this.
	if ((settings->settingBufferMode == settings->BUFFER_MODE_SAFE_DOUBLE) ||
		(settings->settingBufferMode == settings->BUFFER_MODE_FAST_DOUBLE && bFirstTime)) {
			RECT rc;
			GetClientRect(hWnd, &rc);

			glDisable(GL_ALPHA_TEST);
			if (!settings->bugWhiteout) {
				// Found this by accident; Adam likes it.  Freakshow option #1.
				glDisable(GL_BLEND);
			}
			glReadBuffer(GL_FRONT);
			glDrawBuffer(GL_BACK);
			glRasterPos2i(0, 0);
			glCopyPixels(0, 0, rc.right, rc.bottom, GL_COLOR);
			if (!settings->bugWhiteout) {
				glEnable(GL_BLEND);
			}
			glEnable(GL_ALPHA_TEST);

			bFirstTime = FALSE;
	}
}




static void ScreenSaverUpdateFpsIndicator(FlurryAnimateChildInfo *child)
{
	DWORD now = timeGetTime();
	FPS *fps = &child->fps;
	DWORD prevSample, prevRingSample = fps->samples[fps->nextSample];
	char buf[100];
	double last, recent, overall;

	// always gather data in case they turn on FPS later
	prevRingSample = fps->samples[fps->nextSample];
	prevSample = (fps->nSamples == 0) ? fps->startTime :
		((fps->nextSample == 0) ? fps->samples[FPS_SAMPLES - 1] :
		fps->samples[fps->nextSample - 1]);

	fps->samples[fps->nextSample] = now;
	fps->nextSample = (fps->nextSample + 1) % FPS_SAMPLES;
	fps->nSamples++;

	_RPT3(_CRT_WARN, "Child %d: last render %d ms (target %d ms)\n",
		child->id, now - prevSample, child->updateInterval);

	// but the rest of the work is only necessary if they want to see it
	if (!settings->showFPSIndicator) {
		return;
	}

	// calculate overall, simple average
	overall = 1000.0 * fps->nSamples / (now - fps->startTime);

	// calculate last frame; in ring buffer if more than one, else same
	if (fps->nSamples == 1) {
		last = overall;
	} else {
		last = 1000.0 / (now - prevSample);
	}

	// calculate last 20; in ring buffer if more than 20, else same
	if (fps->nSamples < FPS_SAMPLES) {
		// ring buffer not full yet; just use from front till now
		recent = overall;
	} else {
		// ring buffer has full set of samples; use most recent set
		recent = 1000.0 / (now - prevRingSample) * FPS_SAMPLES;
	}

	sprintf(buf, "FPS: Overall %.1f / Recent %.1f / Last %.1f   ", overall, recent, last);
	TextOut(child->hdc, 5, 5, buf, lstrlen(buf));
}


static void DoTestScreenSaver(void)
{
	// calculate desktop rect
	int screenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int screenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int screenW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int screenH = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	// create fullscreen window
	WNDCLASS wc = { 0 };
	wc.lpszClassName = "FlurryTestWindow";
	wc.lpfnWndProc = ScreenSaverProc;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	RegisterClass(&wc);

	g_bPreviewMode = true;
	CreateWindowEx(WS_EX_TOPMOST, wc.lpszClassName, wc.lpszClassName, WS_VISIBLE | WS_POPUP,
		screenX, screenY, screenW, screenH, NULL, NULL, _Module.m_hInst, NULL);
}


//////////////////////////////////////////////////////////////////////////
//
//  CONFIGURATION
//
//////////////////////////////////////////////////////////////////////////


/*
 * ScreenSaverConfigureDialog
 *
 * Scrnsave.lib entry point: called when invoked with /c.
 */
BOOL WINAPI ScreenSaverConfigureDialog(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_INITDIALOG:
			ScreensaverCommonInit();
			settings = new Settings();
			settings->Read();
			SettingsDialogInit(hWnd);
			SettingsDialogEnableControls(hWnd);
			SettingsToDialog(hWnd);
			UpdateEditButtons(hWnd);
			return TRUE;
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {

				// command buttons:
				case IDOK:
					SettingsFromDialog(hWnd);
					settings->Write();
					EndDialog(hWnd, 1);
					return TRUE;
				case IDCANCEL:
					EndDialog(hWnd, 0);
					return TRUE;
				case IDC_ABOUT:
					CAboutBox::AutomaticDoModal();
					return TRUE;
				case IDC_TEST:
					SettingsFromDialog(hWnd);
					DoTestScreenSaver();
					return TRUE;

				case IDC_FLURRY:
					if (HIWORD(wParam) == CBN_SELCHANGE)
						UpdateEditButtons(hWnd);			
					break;

				case IDC_POSITION_PER_CONFIGURE:
					{																	
						DialogBoxParam(NULL, MAKEINTRESOURCE(DLG_ASSIGN), hWnd, AssignDialog, 0);
					}					
					return TRUE;

				case IDC_FLURRY_NEW:
					{
						int size = settings->visuals.size();
						int index = ComboBox_GetCurSel(GetDlgItem(hWnd, IDC_FLURRY));

						CEditor::AutomaticDoModal(NULL);
						settings->ReloadVisuals();	
						LoadDialogPresets(hWnd);

						// check if a new preset has been added and select it
						int newIndex = (size < (signed)settings->visuals.size() ? settings->visuals.size() - 1 : index);
						ComboBox_SetCurSel(GetDlgItem(hWnd, IDC_FLURRY), newIndex);
					}					
					return TRUE;

				case IDC_FLURRY_EDIT:
					{
						int index = ComboBox_GetCurSel(GetDlgItem(hWnd, IDC_FLURRY));
						CEditor::AutomaticDoModal(settings->visuals[index]);
						settings->ReloadVisuals();	
						LoadDialogPresets(hWnd);

						ComboBox_SetCurSel(GetDlgItem(hWnd, IDC_FLURRY), index);
					}					
					return TRUE;

				case IDC_FLURRY_DELETE:
					{
						int index = ComboBox_GetCurSel(GetDlgItem(hWnd, IDC_FLURRY));

						if (index == -1)
							return TRUE;

						settings->DeleteVisual(index);
						settings->ReloadVisuals();					
						LoadDialogPresets(hWnd);

						// Reselect a preset
						ComboBox_SetCurSel(GetDlgItem(hWnd, IDC_FLURRY), settings->globalPreset);						
					}				
					return TRUE;

					// radio buttons:
				case IDC_POSITION_DESKTOP:
				case IDC_POSITION_PRIMARY:
				case IDC_POSITION_PER:
					SettingsDialogEnableControls(hWnd);
					break;

				case IDC_DOUBLE_BUFFER_NONE:
				case IDC_DOUBLE_BUFFER_PARANOID:
				case IDC_DOUBLE_BUFFER_OPTIMISTIC:
					SettingsDialogEnableControls(hWnd);
					break;
			}
			break; // from WM_COMMAND
		case WM_DESTROY:
			_Module.Term();			
			break;
	}

	return FALSE;
}

// Assign Dialog
static BOOL WINAPI AssignDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_INITDIALOG:
			return TRUE;
			break;

		case WM_COMMAND:
			EndDialog(hDlg, 1);
			break;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// Update controls on settings dialog
//  - enable/disable multi-monitor options
//  - enable/disable fps indicator option
static void SettingsDialogEnableControls(HWND hWnd)
{
	// Configure button enabled only when relevant
	EnableWindow(GetDlgItem(hWnd, IDC_POSITION_PER_CONFIGURE), IsDlgButtonChecked(hWnd, IDC_POSITION_PER));

	// but all multi-monitor stuff enabled only on multi-monitor system
	if (g_nMonitors <= 1) {
		EnableWindow(GetDlgItem(hWnd, IDC_POSITION_PRIMARY), FALSE);
		EnableWindow(GetDlgItem(hWnd, IDC_POSITION_PER), FALSE);
		EnableWindow(GetDlgItem(hWnd, IDC_POSITION_PER_CONFIGURE), FALSE);
	}

	// Enable FPS only in single-buffer mode
	EnableWindow(GetDlgItem(hWnd, IDC_FPS_INDICATOR), IsDlgButtonChecked(hWnd, IDC_DOUBLE_BUFFER_NONE));
}

//////////////////////////////////////////////////////////////////////////
// Load the list of presets
static void LoadDialogPresets(HWND hWnd)
{
	HWND hPresetList = GetDlgItem(hWnd, IDC_FLURRY);

	ComboBox_ResetContent(hPresetList);

	for (int i = 0; i < (signed)settings->visuals.size(); i++) {
		ComboBox_AddString(hPresetList, settings->visuals[i]->name.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////
// Enable/Disable edit buttons for read-only presets and multi-monitor
static void UpdateEditButtons(HWND hWnd)
{
	int index = ComboBox_GetCurSel(GetDlgItem(hWnd, IDC_FLURRY));

	// Disable edit/delete for read-only presets
	BOOL enabled = (index < PRESETS_READONLY ? FALSE : TRUE);
	
	EnableWindow(GetDlgItem(hWnd, IDC_FLURRY_EDIT), enabled);
	EnableWindow(GetDlgItem(hWnd, IDC_FLURRY_DELETE), enabled);

	if (IsWindowEnabled(GetDlgItem(hWnd, IDC_POSITION_PER)) && IsDlgButtonChecked(hWnd, IDC_POSITION_PER))
		EnableWindow(GetDlgItem(hWnd, IDC_POSITION_PER_CONFIGURE), TRUE);

}

//////////////////////////////////////////////////////////////////////////
// Init settings dialog
static void SettingsDialogInit(HWND hWnd)
{
	LoadDialogPresets(hWnd);

	// Init the slider control (Shrink percentage)
	SendDlgItemMessage(hWnd, IDC_SHRINK, TBM_SETRANGE, FALSE, MAKELONG(SHRINK_MIN, SHRINK_MAX));			
	SendDlgItemMessage(hWnd, IDC_SHRINK, TBM_SETTICFREQ, SHRINK_FREQ, NULL);

	// Load icon and add to dialog
	HANDLE hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ID_APP));

	if (hIcon == NULL)
		return;

	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

	DestroyIcon((HICON)hIcon);
}


//////////////////////////////////////////////////////////////////////////
// Load settings into the dialog
static void SettingsToDialog(HWND hWnd)
{
	// visual preset
	ComboBox_SetCurSel(GetDlgItem(hWnd, IDC_FLURRY), settings->globalPreset);

	// multi-monitor options
	if (g_nMonitors <= 1) {
		settings->multiMonPosition = settings->MULTIMON_ALLMONITORS;
	}

	CheckRadioButton(hWnd, IDC_POSITION_DESKTOP, IDC_POSITION_PER,
					 IDC_POSITION_DESKTOP + settings->multiMonPosition);

	// buffering mode
	CheckRadioButton(hWnd, IDC_DOUBLE_BUFFER_NONE, IDC_DOUBLE_BUFFER_PARANOID,
					 IDC_DOUBLE_BUFFER_NONE + settings->settingBufferMode);

	
	// Shrink percentage
	SendDlgItemMessage(hWnd, IDC_SHRINK, TBM_SETPOS, TRUE, (LONG)settings->shrinkPercentage);

	// Block & Whiteout mode
	CheckDlgButton(hWnd, IDC_BLOCK_MODE, settings->bugBlockMode);
	CheckDlgButton(hWnd, IDC_WHITEOUT_MODE, settings->bugWhiteout);

	// FPS Indicator
	EnableWindow(GetDlgItem(hWnd, IDC_FPS_INDICATOR), settings->settingBufferMode == settings->BUFFER_MODE_SINGLE);
	CheckDlgButton(hWnd, IDC_FPS_INDICATOR, settings->showFPSIndicator);
}

//////////////////////////////////////////////////////////////////////////
// Save settings from the dialog
static void SettingsFromDialog(HWND hWnd)
{
	// visual preset
	settings->globalPreset = ComboBox_GetCurSel(GetDlgItem(hWnd, IDC_FLURRY));

	// Update per-monitor presets
	// FIXME: should work through ASSIGN option


	// multi-monitor options
	if (IsDlgButtonChecked(hWnd, IDC_POSITION_DESKTOP)) {
		settings->multiMonPosition = settings->MULTIMON_ALLMONITORS;
	} else if (IsDlgButtonChecked(hWnd, IDC_POSITION_PRIMARY)) {
		settings->multiMonPosition = settings->MULTIMON_PRIMARY;
	} else if (IsDlgButtonChecked(hWnd, IDC_POSITION_PER)) {
		settings->multiMonPosition = settings->MULTIMON_PERMONITOR;
	}

	// buffering mode
	if (IsDlgButtonChecked(hWnd, IDC_DOUBLE_BUFFER_NONE)) {
		settings->settingBufferMode = settings->BUFFER_MODE_SINGLE;
	} else if (IsDlgButtonChecked(hWnd, IDC_DOUBLE_BUFFER_OPTIMISTIC)) {
		settings->settingBufferMode = settings->BUFFER_MODE_FAST_DOUBLE;
	} else if (IsDlgButtonChecked(hWnd, IDC_DOUBLE_BUFFER_PARANOID)) {
		settings->settingBufferMode = settings->BUFFER_MODE_SAFE_DOUBLE;
	}

	// Shrink percentage
	settings->shrinkPercentage = (int)SendDlgItemMessage(hWnd, IDC_SHRINK, TBM_GETPOS, NULL, NULL);

	// FPS indicator
	settings->showFPSIndicator = IsDlgButtonChecked(hWnd, IDC_FPS_INDICATOR);

	// Block & Whiteout mode
	settings->bugBlockMode = IsDlgButtonChecked(hWnd, IDC_BLOCK_MODE);
	settings->bugWhiteout = IsDlgButtonChecked(hWnd, IDC_WHITEOUT_MODE);
}


