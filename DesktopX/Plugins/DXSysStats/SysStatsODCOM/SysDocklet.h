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

// SysDocklet.h: interface for the SysDocklet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSDOCKLET_H__23EBC105_19DC_4E96_A490_F43591C93409__INCLUDED_)
#define AFX_SYSDOCKLET_H__23EBC105_19DC_4E96_A490_F43591C93409__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Docklet.h"
#include "DockletSDK.h"
#include "SkinEnumerator.hpp"
#include "Menu.h"
#include "WindowTimerService.h"
#import "SysStatsCOM.tlb" no_namespace raw_interfaces_only

class SysDocklet : public DockletLetter, public IObserver, public ITimerClient
{
public:
	SysDocklet(HWND hwndDocklet, HINSTANCE hInstance, char *szIni, char *szIniGroup);

	// AveDesk 1.2 constructor
	SysDocklet(HWND hwnd, HINSTANCE hInstance, WCHAR* fileName);

	// Destructor
	virtual ~SysDocklet();

	//--------- Exported static methods - should override -------------//
	static Docklet* CALLBACK create(HWND hwndDocklet, HINSTANCE hInstance, char *szIni, char *szIniGroup);
	static void CALLBACK getInformation(char *szName, char *szAuthor, int *iVersion, char *szNotes);

	//--------- AveDesk 1.2 exported static methods - should override ----------//
    static CSkinEnumerator* CALLBACK createSkinEnumerator(HINSTANCE hInstance, const WCHAR* deskletPath);
	static void CALLBACK destroySkinEnumerator(CSkinEnumerator* se);
	static INT CALLBACK getSkin(CSkinEnumerator* se, SkinInfo* info);
	static Docklet* CALLBACK createFromSkin(HWND hwnd, HINSTANCE hInstance, CSkinEnumerator* se, const SkinInfo* info);

	virtual char *getLabelText();
	virtual char *getImageText();
	virtual Image *createOverlayImage();

	//--------- Menu action methods ----------------//
	STDMETHOD(loadConfigFile)();
	STDMETHOD(showHelp)();
	STDMETHOD(configureLabel)();
	STDMETHOD(configureSize)();
	STDMETHOD(Save)();
	STDMETHOD(SaveAs)();
	STDMETHOD(Screenshot)();

	//--------- Overridden virtual methods ----------//
	virtual void load(char *szIni, char *szIniGroup);
	virtual void save(char *szIni, char *szIniGroup, BOOL bIsForExport);
	virtual BOOL leftButtonClick(POINT *ptCursor, SIZE *sizeDocklet);
	virtual BOOL leftButtonHeld(POINT *ptCursor, SIZE *sizeDocklet);
	virtual BOOL doubleClick(POINT *ptCursor, SIZE *sizeDocklet);
	virtual BOOL rightButtonClick(POINT *ptCursor, SIZE *sizeDocklet);
	virtual void processMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//--------- AveDesk 1.2 methods -----------//
	virtual LRESULT processMessageEx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL *bHandled);
	virtual BOOL getModel(IDispatch **pVal);
	virtual BOOL visibilityNotify(BOOL becomesVisible, DWORD flags);
	virtual BOOL setSkin(CSkinEnumerator* se, const SkinInfo* info);

	// Overrides IObserver
	STDMETHOD(HandleEvent)();

	// Overrides TimerClient
	STDMETHOD(get_TimerInterval)(double *pRet) { *pRet = 0.02; return S_OK; }
	STDMETHOD(put_TimerInterval)(double) { return E_NOTIMPL; }
	STDMETHOD(TimerFired)();

	// Those pesky IUnknown methods:
	DUMMY_DUAL

protected:
	virtual void updateMeters();
	virtual void updateOverlays();
	virtual void adjustWindow(HWND);
	virtual void save();
	virtual void scalePoint(POINT *pt);

	void preInit();
	void postInit();

	static void registerDLL();
	static SysStatsUtils::CriticalSection registerCS;
	static bool dllRegistered;

	IClientModel *model;
	IAppConfig *appConfig;
	char labelText[MAX_PATH];

	Menu *contextMenu;
	WindowTimerService timerService;
	bool isDesklet;
	bool isAD1_2;
	bool visible;
	bool timerSet;

	static const _bstr_t APPLICATION_KEY;
	static const int VERSION;
};

#endif // !defined(AFX_SYSDOCKLET_H__23EBC105_19DC_4E96_A490_F43591C93409__INCLUDED_)
