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

#include "SysStatsUtils.h"
#include "ArrayPtr.h"
#include "ArrayPtr.cpp"
#include "ArrayPtrVariant.h"
#include "ArrayPtrVariant.cpp"
#include "ShellIconHelper.h"
#include "ShellIconHelper.cpp"

#import "SysStatsCOM.tlb" no_namespace raw_interfaces_only

class SysStatsClientWindow;
class MenuAction;
class Menu;

/*
 * The business end of the SysStats application.
 */
class SysStatsApp : public CShellIconHelper<SysStatsApp>, IObserver
{
public:
	SysStatsApp(LPSTR lpCmdLine, int nCmdShow);
	virtual ~SysStatsApp();

	// Override Observer methods - called when a client config changes.
	STDMETHOD(HandleEvent)();

	// Our own methods
	virtual void load();
	virtual void save();
	virtual void saveTheme();
	STDMETHOD(loadClient)();
	STDMETHOD(newClient)();
	virtual void close(SysStatsClientWindow *window);
	virtual Menu* getShellMenu();
	virtual int showShellMenu();

	// Those pesky IUnknown methods:
	DUMMY_DUAL

protected:
	virtual IClientModel *loadClient(SysStatsClientWindow *win, const char* configFile);
	virtual void add(SysStatsClientWindow *win, IClientModel *model);

	virtual LRESULT OnUserCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT command(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	ArrayPtrVariant<IClientModel> models;
	ArrayPtr<SysStatsClientWindow> windows;
	ArrayPtr<MenuAction> shellMenuActions;

	LPSTR lpCmdLine;
	int nCmdShow;
	Menu *shellMenu;
	IAppConfigPtr appConfig;

	static const char *CLIENT_COUNT_PROPERTY;
	static const char *CLIENT_PREFIX;
	static const char *CONFIG_FILE_PROPERTY;
	static const char *GENERAL;
	static const char *UNKNOWN;

BEGIN_MSG_MAP(SysStatsApp)
    MESSAGE_HANDLER(WM_USER, OnUserCommand)
    MESSAGE_HANDLER(WM_COMMAND, command)
END_MSG_MAP()
};