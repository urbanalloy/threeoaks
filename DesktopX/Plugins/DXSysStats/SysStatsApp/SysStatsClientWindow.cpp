/*
 * SysStats Widget Framework
 * Copyright (C) 2002-2006 Paul Andrews
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"

#include "AppResources.h"
#include "CloseMenuAction.h"
#include "MenuActionT.h"
#include "SysStatsClientModelConfigure.h"
#include "SysStatsClientWindow.h"
#include "WindowDialog.h"
#include "HDCImage.h"
#define __SysStatsCOM_h__
//#include "QueueTimerService.h"
#include "WindowTimerService.h"

#include <gdiplusH.h>

const char *SysStatsClientWindow::MENU_RESOURCE_NAME = "MainMenu";				// Name of menu resource
const char *SysStatsClientWindow::WINDOW_CLASS_NAME  = "SysStatsClientWindow";	// Name of window class
const char *SysStatsClientWindow::WINDOW_NAME  = "WindowName";
const char *SysStatsClientWindow::WINDOW_NAME_DEFAULT = "New Window";
const char *SysStatsClientWindow::WINDOW_X = "X";
const char *SysStatsClientWindow::WINDOW_Y = "Y";

#define IS_MOUSE_EVENT(uMsg) ((uMsg >= WM_MOUSEFIRST) && (uMsg <= WM_MOUSELAST))

SysStatsClientWindow::SysStatsClientWindow(IObserver *observer) :
	contextMenu(NULL), model(NULL), observer(observer), timerService(new WindowTimerService())
{
//	timerService->SetTimerClient(this);
}

SysStatsClientWindow::~SysStatsClientWindow()
{
	delete contextMenu;
	timerService->ClearAllTimers();
	DestroyWindow();
	if (model)
		model->Release();
	delete timerService;
}

STDMETHODIMP SysStatsClientWindow::saveModel()
{
	model->Save(/* interactive */ true);

	return S_OK;
}

void SysStatsClientWindow::save(const char *szIni, const char *szIniGroup)
{
	WritePrivateProfileString(szIniGroup, WINDOW_NAME, name, szIni);
	char val[32];
	RECT rect;
	GetWindowRect(&rect);
	WritePrivateProfileString(szIniGroup, WINDOW_X, itoa(rect.left, val, 10), szIni);
	WritePrivateProfileString(szIniGroup, WINDOW_Y, itoa(rect.top, val, 10), szIni);
}

void SysStatsClientWindow::load(const char *szIni, const char *szIniGroup)
{
	GetPrivateProfileString(szIniGroup, WINDOW_NAME, WINDOW_NAME_DEFAULT, name, sizeof(name), szIni);
	int x = GetPrivateProfileInt(szIniGroup, WINDOW_X, 0, szIni);
	int y = GetPrivateProfileInt(szIniGroup, WINDOW_Y, 0, szIni);

	RECT rect;
	GetWindowRect(&rect);
	rect.bottom = rect.bottom - rect.top + y;
	rect.right = rect.right - rect.left + x;
	rect.left=x;
	rect.top=y;
	MoveWindow(&rect);
}

HWND SysStatsClientWindow::create(SIZE &size)
{
	RECT rect;
	rect.top	= 0;
	rect.bottom	= size.cy;
    rect.left	= 0;
	rect.right	= size.cx;

	HWND ret = Create((HWND)NULL, rect, "SysStats Client", WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_LAYERED);
//	hdc = CreateCompatibleDC(0);
	return ret;
}

void SysStatsClientWindow::setModel(IClientModel *model)
{
	// What to do if there is already a model set?
	model->put_TimerService(timerService);
	model->put_hWnd((long)m_hWnd);
	if (this->model)
		this->model->Release();
	this->model = model;
	model->AddRef();
}

/*
 * Deprecated
 */
STDMETHODIMP SysStatsClientWindow::TimerFired()
{
//	updateOverlays();

	return S_OK;
}

/*
 * Called when a dialog changes something.
 */
STDMETHODIMP SysStatsClientWindow::HandleEvent()
{
	if (model)
	{
		RECT rect;
		GetWindowRect(&rect);
		model->put_Width(rect.right - rect.left);
		model->put_Height(rect.bottom - rect.top);
		model->put_X(rect.left);
		model->put_Y(rect.top);
		model->UpdateMeters();
	}

	ICOMTimerService *ts = NULL;
	model->get_TimerService(&ts);
	if (ts)
	{
		ts->UpdateTimerClients();
		ts->Release();
	}

	// Force a redraw
	model->put_dirty(true);
	updateOverlays();

	// Notify OUR observer that something has changed
	observer->HandleEvent();

	return S_OK;
}

STDMETHODIMP SysStatsClientWindow::configure()
{
	WindowDialog *pDialog = new WindowDialog(this, this);
	pDialog->DoModal(m_hWnd);
	delete pDialog;

	return S_OK;
}

int SysStatsClientWindow::draw(HDC hdc)
{
	BLENDFUNCTION blend;
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.AlphaFormat = AC_SRC_ALPHA;
    blend.SourceConstantAlpha = 255;

	RECT rect;
	GetWindowRect(&rect);
	SIZE FSize;
	FSize.cx = rect.right-rect.left;
	FSize.cy = rect.bottom-rect.top;

	POINT ptSrc;
	ptSrc.x = 0;
	ptSrc.y = 0;
 
	BOOL ret = FALSE;

    if (!(ret = UpdateLayeredWindow(
		m_hWnd,	// Window handle
		NULL,	// Screen device context (used for palette)
		NULL,	// We're not changing the window position
		&FSize,	// We're not changing the window size
		hdc,	// The source device context (aka image).
		&ptSrc,	// The offset of the source image in the source context
		NULL,	// Transparent color (none)
        &blend,	// Alpha blending
		ULW_ALPHA)))
		SysStatsUtils::checkError();

	return TRUE;
}

Menu *SysStatsClientWindow::getContextMenu(HWND owner)
{
	if (owner == NULL)
		return contextMenu;

	if (contextMenu == NULL)
	{
		contextMenu = new Menu(getName(), owner);
		
		contextMenu->addAction(new SysStatsClientModelConfigure("Configure contents...", model, this, m_hWnd));
		contextMenu->addAction(new MenuActionT<SysStatsClientWindow>("Configure window...", this, &SysStatsClientWindow::configure));
		contextMenu->addAction(new MenuActionT<SysStatsClientWindow>("Save", this, &SysStatsClientWindow::saveModel));
		contextMenu->addAction(new MenuActionT<IClientModel>("Save as...", model, &IClientModel::SaveAs));
	}

	return contextMenu;
}

int SysStatsClientWindow::showContextMenu()
{
	if (contextMenu != NULL)
		contextMenu->action();

	return 0;
}

int SysStatsClientWindow::updateOverlays()
{
#ifdef MEM_CHECKS
	static time_t lasttime = time(NULL);

	if (time(NULL) > lasttime+60)
	{
		SysStatsUtils::SetLFHeaps();
		SysStatsUtils::CompactHeaps();
		::CoFreeUnusedLibraries();
		lasttime = time(NULL);
	}
#endif
	if (model)
	{
		long dirty;
		model->get_dirty(&dirty);
		if (dirty)
		{
			char labelText[MAX_PATH];
			ILabel *label;
			model->get_Label(&label);
			BSTR s;
			label->GetLabel(&s);
			strncpy(labelText, _bstr_t(s, false), MAX_PATH);
			label->Release();

			RECT rect;
			GetWindowRect(&rect);
			HDCImage hdc(GetDC(), rect.right-rect.left, rect.bottom-rect.top);
			model->UpdateOverlays(hdc);

			draw(hdc);

			model->put_dirty(false);
		}
	}

	return 0;
}

LRESULT SysStatsClientWindow::windowCreated(UINT uMsg,	// message identifier
		WPARAM wParam,	// first message parameter
		LPARAM lParam,	// second message parameter
		BOOL &handled)
{
	timerService->setHWND(m_hWnd);

	RECT rect;
	GetWindowRect(&rect);
	HDCImage hdc(0, rect.right-rect.left,rect.bottom-rect.top);
	HICON hIcon = (HICON)LoadImage(      
		_Module.m_hInst,
		MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON,
		128,
		128,
		LR_DEFAULTCOLOR
	);
	DrawIconEx(hdc, 0, 0, hIcon, 128, 128, 0, NULL, DI_NORMAL);
	draw(hdc);

	return 0;
}

LRESULT SysStatsClientWindow::SysStatsDispatch(UINT uMsg,	// message identifier
		WPARAM wParam,	// first message parameter
		LPARAM lParam,	// second message parameter
		BOOL &handled)
{
	VARIANT_BOOL done = false;

	AtlTrace("Dispatch=0x%0.4x\n", uMsg);

	if (uMsg == WM_USER+143)
		AtlTrace("General message handler got a timer message!\n");

	if (model)
	{
		if (uMsg == WM_MOUSEMOVE)
		{
			// We need to be told when the mouse leaves our window.
			TRACKMOUSEEVENT trackDef = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT };
			::TrackMouseEvent(&trackDef);
		}

		POINT pos;
		if (IS_MOUSE_EVENT(uMsg))
		{
			pos.x = LOWORD(lParam);
			pos.y = HIWORD(lParam);
		}
		else
		{
			::GetCursorPos(&pos);
		}

		model->HandleMessage(uMsg, wParam, lParam, pos.x, pos.y, &done);
	}

	handled = done;

	return 0;
}

LRESULT SysStatsClientWindow::timer(UINT uMsg,	// message identifier
		WPARAM wParam,	// first message parameter
		LPARAM lParam,	// second message parameter
		BOOL &handled)
{
	handled = timerService->handleTimerMessage(wParam);

	return 0;
}

LRESULT SysStatsClientWindow::redraw(UINT uMsg,	// message identifier
		WPARAM wParam,	// first message parameter
		LPARAM lParam,	// second message parameter
		BOOL &handled)
{
	handled = true;
	updateOverlays();

	return 0;
}

LRESULT SysStatsClientWindow::rightButtonDown(UINT uMsg,	// message identifier
		WPARAM wParam,	// first message parameter
		LPARAM lParam,	// second message parameter
		BOOL &handled)
{
	showContextMenu();

	return 0;
}

LRESULT SysStatsClientWindow::leftButtonDown(UINT uMsg,	// message identifier
		WPARAM wParam,	// first message parameter
		LPARAM lParam,	// second message parameter
		BOOL &handled)
{
	// ---------------------------------------------------------
	// this is a common trick for easy dragging of the window.
	// this message fools windows telling that the user is
	// actually dragging the application caption bar.
	// ---------------------------------------------------------
	SendMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, NULL);

	return 0;
}

LRESULT SysStatsClientWindow::windowMoved(UINT uMsg,	// message identifier
		WPARAM wParam,	// first message parameter
		LPARAM lParam,	// second message parameter
		BOOL &handled)
{

	if (model)
	{
		model->put_X(LOWORD(lParam));
		model->put_Y(HIWORD(lParam));
	}

	return 0;
}

CWndClassInfo& SysStatsClientWindow::GetWndClassInfo()
{
	static LOGBRUSH brush;
	ZeroMemory(&brush, sizeof(brush));
	brush.lbStyle = BS_HOLLOW;

	static CWndClassInfo wc =
	{
		{
			sizeof(WNDCLASSEX),
			CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC, 
//			CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_SAVEBITS, 
			StartWindowProc,	// Window proc
			0,					// Extra class memory
			0,					// Extra window memory
			NULL,				// hInstance
			LoadIcon((HINSTANCE) NULL, MAKEINTRESOURCE(IDI_ICON1)),	// Predefined app icon
			NULL,				// Cursor
			CreateBrushIndirect(&brush),
			NULL,				// Window menu resource name
			WINDOW_CLASS_NAME,	// Window class name
			NULL				// Small icon handle
		},
		NULL,
		NULL,
		IDC_ARROW,
		TRUE,
		0,
		_T("")
	};

	return wc;
}
