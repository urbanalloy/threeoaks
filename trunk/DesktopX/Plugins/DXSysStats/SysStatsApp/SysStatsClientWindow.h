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

#pragma once

#include <atlwin.h>

#import "SysStatsCOM.tlb" raw_interfaces_only raw_native_types no_namespace named_guids 

#include "SysStatsUtils.h"
#include "ArrayPtr.h"
//#include "ArrayPtr.cpp"
#include "Menu.h"

class CClientModel;
class MenuAction;
class QueueTimerService;
class WindowTimerService;

/*
 * This class implements two COM interfaces, because its easier that way. These interfaces
 * themselves extend IUnknown - which is intended to handle reference counting and querying
 * for COM objects. However - this object can never be created using the COM framework
 * and is never exposed to other modules. Hence it doesn't need the lifecycle management
 * that COM generally (and IUknown in particular) expose.
 * <P>
 * If that ever changes we will probably have to get to grips with aggregation. In the
 * meantime we will just implement empty IUnknown methods to keep the compiler quiet.
 */
class SysStatsClientWindow : public ITimerClient, public IObserver, public CWindowImpl<SysStatsClientWindow>
{
public:
	SysStatsClientWindow(IObserver *observer);
	virtual ~SysStatsClientWindow();

	// Overrides TimerClient (deprecated)
	STDMETHOD(get_TimerInterval)(double *pRet) { *pRet = 0.01; return S_OK; }
	STDMETHOD(put_TimerInterval)(double) { return E_NOTIMPL; }
	STDMETHOD(TimerFired)();

	// Overrides IObserver
	STDMETHOD(HandleEvent)();

	// Own methods
	virtual Menu* getContextMenu(HWND owner=NULL);
	virtual int draw(HDC hdc);

	STDMETHOD(saveModel)();
	virtual void save(const char *szIni, const char *szIniGroup);
	virtual void load(const char *szIni, const char *szIniGroup);
	virtual HWND create(SIZE &size);

	STDMETHOD(configure)();

	void setModel(IClientModel *model);
	IClientModel *getModel() { return model; }

	void setName(const char *_name) { strncpy(name, _name, sizeof(name)); }
	char *getName() { return name; }

	// Those pesky IUnknown methods:
	DUMMY_DUAL

	static const char *MENU_RESOURCE_NAME;	// Name of menu resource
	static const char *WINDOW_CLASS_NAME;	// Name of window class

protected:
	static const char* WINDOW_X;
	static const char* WINDOW_Y;
	static const char* WINDOW_NAME;
	static const char* WINDOW_NAME_DEFAULT;

	// Methods that sub-classes might want to override
	virtual int showContextMenu();
	virtual int updateOverlays();

	// Default message handler
	virtual LRESULT SysStatsDispatch(UINT uMsg,	// message identifier
		WPARAM wParam,	// first message parameter
		LPARAM lParam,	// second message parameter
		BOOL &handled);	// Flag indicating whether or not we handled it

	// Individual message handling methods
	virtual LRESULT timer(UINT uMsg,	// message identifier
		WPARAM wParam,	// first message parameter
		LPARAM lParam,	// second message parameter
		BOOL &handled);	// Flag indicating whether or not we handled it
	virtual LRESULT redraw(UINT uMsg,	// message identifier
		WPARAM wParam,	// first message parameter
		LPARAM lParam,	// second message parameter
		BOOL &handled);	// Flag indicating whether or not we handled it
	virtual LRESULT windowCreated(UINT uMsg,	// message identifier
		WPARAM wParam,	// first message parameter
		LPARAM lParam,	// second message parameter
		BOOL &handled);	// Flag indicating whether or not we handled it
	virtual LRESULT windowMoved(UINT uMsg,	// message identifier
		WPARAM wParam,	// first message parameter
		LPARAM lParam,	// second message parameter
		BOOL &handled);
	virtual LRESULT leftButtonDown(UINT uMsg,	// message identifier
		WPARAM wParam,	// first message parameter
		LPARAM lParam,	// second message parameter
		BOOL &handled);
	virtual LRESULT rightButtonDown(UINT uMsg,	// message identifier
		WPARAM wParam,	// first message parameter
		LPARAM lParam,	// second message parameter
		BOOL &handled);

private:
//	HDC hdc;
	char name[80];
	Menu *contextMenu;
	WindowTimerService *timerService;
	IClientModel *model;
	IObserver *observer;

	ArrayPtr<MenuAction> contextMenuActions;

public:
	static CWndClassInfo& GetWndClassInfo();

BEGIN_MSG_MAP(SysStatsApp)
    MESSAGE_HANDLER(WM_USER+143, timer)
    MESSAGE_HANDLER(WM_TIMER, timer)
    MESSAGE_HANDLER(WM_USER+144, redraw)
    MESSAGE_HANDLER(WM_CREATE, windowCreated)
	MESSAGE_RANGE_HANDLER(0, -1, SysStatsDispatch)
    MESSAGE_HANDLER(WM_MOVE, windowMoved)
    MESSAGE_HANDLER(WM_LBUTTONDOWN, leftButtonDown)
    MESSAGE_HANDLER(WM_RBUTTONDOWN, rightButtonDown)
END_MSG_MAP()
};