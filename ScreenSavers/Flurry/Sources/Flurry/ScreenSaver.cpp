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

#include <windows.h>
#include <windowsx.h>
#undef SubclassWindow // collision with atlwin.h
#include <crtdbg.h>
#include <scrnsave.h>
#include <GL/GL.h>
#include <stdio.h>
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>

#include "FlurryGroup.h"
#include "FlurrySettings.h"
#include "FlurryPreset.h"
#include "TimeSupport.h"
#include "AboutBox.h"
#include "resource.h"

using namespace Flurry;

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"") 


#pragma warning(disable:4100)

/*
 * Still to do:
 * 2) visual configuration / customization
 * 3) separate support for multiple monitors
 *    XXX: double-buffered flip() without blocking the other monitor
 */


///////////////////////////////////////////////////////////////////////////
//
// shared globals
//

CComModule _Module;
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
// data types
//

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
 * Per-monitor flurry info.  In multimon mode, we have one of these for
 * each monitor; in single-mon mode, we just have one (which might represent
 * the entire desktop, and might represent just the primary monitor).
 */
typedef struct {
	int id;				// ordinal just for debugging convenience
	char *device;		// name of display device, or NULL in single-mon mode
	int updateInterval;	// time between refreshes
	HWND hWnd;			// handle to child window for this monitor
	RECT rc;			// child window rectangle in screen coordinates
	HGLRC hglrc;		// handle to OpenGL rendering context for this window
	HDC hdc;			// handle to DC used by hglrc
	FPS fps;			// frames per second info
	Group *flurry;// the data structure with info on the flurry clusters
} FlurryAnimateChildInfo;

///////////////////////////////////////////////////////////////////////////
//
// local function prototypes

static void ScreensaverCommonInit(void);
static void ScreensaverRuntimeInit(HWND hWnd);
static void AttachGLToWindow(FlurryAnimateChildInfo *child);
static void DetachGLFromWindow(FlurryAnimateChildInfo *child);
static void CopyFrontBufferToBack(HWND hWnd);
static BOOL WINAPI CreditsDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
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


///////////////////////////////////////////////////////////////////////////
//
// public functions


/*
 * ScreensaverCommonInit
 *
 * Init code used by both personalities (configuration and screensaver)
 * when invoked with /s.
 */

static void
ScreensaverCommonInit(void)
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
	_RPT4(_CRT_WARN, "Init in window 0x%08x, parent 0x%08x: %d, %d\n",
		  hWnd, GetParent(hWnd), rc.left, rc.top);
	if (GetParent(hWnd)) {
		iMultiMonPosition = MULTIMON_ALLMONITORS;
		g_bThumbnailMode = true;
	}

	// initialize timer module
	TimeSupport_Init();

#ifdef _DEBUG
	// debugging behind toplevel windows is a pain :P
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
			if (!g_bPreviewMode) {
				// initialization, since this is the first chunk of our code to run
				ScreensaverCommonInit();
				Settings_Read();
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
				iShowFPSIndicator = !iShowFPSIndicator;
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


static void ScreenSaverCreateChildren(HWND hWndParent)
{
	RECT rc;
	g_iMonitor = 0;

	switch (iMultiMonPosition) {
		case MULTIMON_PERMONITOR:
			EnumDisplayMonitors(NULL, NULL, ScreenSaverCreateChildrenCb,
								(LPARAM)hWndParent);
			return;

		case MULTIMON_PRIMARY:
			SetRect(&rc, 0, 0,
					GetSystemMetrics(SM_CXSCREEN),
					GetSystemMetrics(SM_CYSCREEN));
			break;

		case MULTIMON_ALLMONITORS:
		default:
			GetClientRect(hWndParent, &rc);
			break;
	}

	ScreenSaverCreateChild(hWndParent, &rc, 0, NULL);
}


static BOOL CALLBACK ScreenSaverCreateChildrenCb(HMONITOR hMonitor, HDC hdcMonitor,
							LPRECT lprcMonitor, LPARAM dwData)
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

#define RECTWIDTH(rc)  ((rc).right - (rc).left)
#define RECTHEIGHT(rc) ((rc).bottom - (rc).top)

	// 200: n% / 100, and it counts on each size, so / 2 more
	InflateRect(&child->rc,
		        -(int)(iFlurryShrinkPercentage * RECTWIDTH(child->rc) / 200),
		        -(int)(iFlurryShrinkPercentage * RECTHEIGHT(child->rc) / 200));

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
			int preset = child->id < (signed)g_multiMonPreset.size() ?
						g_multiMonPreset[child->id] : iFlurryPreset;
			child->flurry = new Group(preset);
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
			if (wglMakeCurrent(child->hdc, child->hglrc)) {
				PAINTSTRUCT ps;
				BeginPaint(hWnd, &ps);
				CopyFrontBufferToBack(hWnd);	// always call; may do nothing
				child->flurry->AnimateOneFrame();
				if (iSettingBufferMode > BUFFER_MODE_SINGLE) {
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
			Settings_Read();
			SettingsDialogInit(hWnd);
			SettingsDialogEnableControls(hWnd);
			SettingsToDialog(hWnd);
			return TRUE;
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				// command buttons:
				case IDOK:
					SettingsFromDialog(hWnd);
					Settings_Write();
					EndDialog(hWnd, 1);
					return TRUE;
				case IDCANCEL:
					EndDialog(hWnd, 0);
					return TRUE;
				case IDC_CREDITS:
					DialogBoxParam(NULL, MAKEINTRESOURCE(DLG_CREDITS), hWnd, CreditsDialog, 0);
					return TRUE;
				case IDC_ABOUT:
					CAboutBox::AutomaticDoModal();
					return TRUE;
				case IDC_TEST:
					SettingsFromDialog(hWnd);
					DoTestScreenSaver();
					return TRUE;

					// radio buttons:
				case IDC_POSITION_DESKTOP:
				case IDC_POSITION_PRIMARY:
				case IDC_POSITION_PER:
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


static void SettingsDialogEnableControls(HWND hWnd)
{
	// Configure button enabled only when relevant
	EnableWindow(GetDlgItem(hWnd, IDC_POSITION_PER_CONFIGURE),
				 IsDlgButtonChecked(hWnd, IDC_POSITION_PER));

	// but all multimon stuff enabled only on multimon system
	if (g_nMonitors <= 1) {
		EnableWindow(GetDlgItem(hWnd, IDC_POSITION_PRIMARY), FALSE);
		EnableWindow(GetDlgItem(hWnd, IDC_POSITION_PER), FALSE);
		EnableWindow(GetDlgItem(hWnd, IDC_POSITION_PER_CONFIGURE), FALSE);
	}

	// XXX and per-monitor flurry assignment isn't implemented yet anyway
	EnableWindow(GetDlgItem(hWnd, IDC_POSITION_PER_CONFIGURE), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_POSITION_PER), FALSE);
}


static void SettingsDialogInit(HWND hWnd)
{
	HWND hPresetList = GetDlgItem(hWnd, IDC_VISUAL);
	for (int i = 0; i < (signed)g_visuals.size(); i++) {
		ComboBox_AddString(hPresetList, g_visuals[i]->name);
	}

	// Load icon and add to dialog
	HANDLE hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ID_APP));

	if (hIcon == NULL)
		return;

	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

	DestroyIcon((HICON)hIcon);

}


static void SettingsToDialog(HWND hWnd)
{
	// visual preset
	ComboBox_SetCurSel(GetDlgItem(hWnd, IDC_VISUAL), iFlurryPreset);

	// multimon options
	if (g_nMonitors <= 1) {
		iMultiMonPosition = MULTIMON_ALLMONITORS;
	}

	CheckRadioButton(hWnd, IDC_POSITION_DESKTOP, IDC_POSITION_PER,
					 IDC_POSITION_DESKTOP + iMultiMonPosition);

	// buffering mode
	CheckRadioButton(hWnd, IDC_DOUBLE_BUFFER_NONE, IDC_DOUBLE_BUFFER_PARANOID,
					 IDC_DOUBLE_BUFFER_NONE + iSettingBufferMode);
}


static void SettingsFromDialog(HWND hWnd)
{
	// visual preset
	iFlurryPreset = ComboBox_GetCurSel(GetDlgItem(hWnd, IDC_VISUAL));

	// multimon options
	if (IsDlgButtonChecked(hWnd, IDC_POSITION_DESKTOP)) {
		iMultiMonPosition = MULTIMON_ALLMONITORS;
	} else if (IsDlgButtonChecked(hWnd, IDC_POSITION_PRIMARY)) {
		iMultiMonPosition = MULTIMON_PRIMARY;
	} else if (IsDlgButtonChecked(hWnd, IDC_POSITION_PER)) {
		iMultiMonPosition = MULTIMON_PERMONITOR;
	}

	// buffering mode
	if (IsDlgButtonChecked(hWnd, IDC_DOUBLE_BUFFER_NONE)) {
		iSettingBufferMode = BUFFER_MODE_SINGLE;
	} else if (IsDlgButtonChecked(hWnd, IDC_DOUBLE_BUFFER_OPTIMISTIC)) {
		iSettingBufferMode = BUFFER_MODE_FAST_DOUBLE;
	} else if (IsDlgButtonChecked(hWnd, IDC_DOUBLE_BUFFER_PARANOID)) {
		iSettingBufferMode = BUFFER_MODE_SAFE_DOUBLE;
	}
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

	if (iSettingBufferMode > BUFFER_MODE_SINGLE) {
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

	if ((iSettingBufferMode == BUFFER_MODE_SAFE_DOUBLE) ||
		(iSettingBufferMode == BUFFER_MODE_FAST_DOUBLE && bFirstTime)) {
		RECT rc;
		GetClientRect(hWnd, &rc);

		glDisable(GL_ALPHA_TEST);
		if (!iBugWhiteout) {
			// Found this by accident; Adam likes it.  Freakshow option #1.
			glDisable(GL_BLEND);
		}
		glReadBuffer(GL_FRONT);
		glDrawBuffer(GL_BACK);
		glRasterPos2i(0, 0);
		glCopyPixels(0, 0, rc.right, rc.bottom, GL_COLOR);
		if (!iBugWhiteout) {
			glEnable(GL_BLEND);
		}
		glEnable(GL_ALPHA_TEST);

		bFirstTime = FALSE;
	}
}

static BOOL WINAPI CreditsDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
	if (!iShowFPSIndicator) {
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

	sprintf(buf, "FPS: Overall %.1f / Recent %.1f / Last %.1f   ",
				 overall, recent, last);
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
