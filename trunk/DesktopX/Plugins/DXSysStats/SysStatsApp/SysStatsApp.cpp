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

#include "SysStatsApp.h"

#include <direct.h>
#include <stdlib.h>
#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <commdlg.h>

#include "SysStatsClientWindow.h"
#include "MenuActionT.h"
#include "ExitMenuAction.h"
#include "CloseMenuAction.h"
#include "Menu.h"
#include "SysStatsUtils.h"

const char *SysStatsApp::CLIENT_COUNT_PROPERTY = "Clients";
const char *SysStatsApp::CLIENT_PREFIX = "client";
const char *SysStatsApp::CONFIG_FILE_PROPERTY = "ConfigFile";
const char *SysStatsApp::GENERAL = "general";
const char *SysStatsApp::UNKNOWN = "unknown";
static const _bstr_t INI = "SysStats.ini";
static const _bstr_t DEFAULT_ROOT = "configs\\default";

SysStatsApp::SysStatsApp(LPSTR lpCmdLine, int nCmdShow) :
	lpCmdLine(lpCmdLine), nCmdShow(nCmdShow), shellMenu(NULL), appConfig(NULL)
{
	HRESULT hr = appConfig.CreateInstance("SysStatsCOM.AppConfig", NULL, CLSCTX_INPROC_SERVER);
	if (!SUCCEEDED(hr))
		SysStatsUtils::checkError();
}

SysStatsApp::~SysStatsApp()
{
	models.ClearAndDestroy();
	windows.clearAndDestroy();
	delete shellMenu;
}

STDMETHODIMP SysStatsApp::HandleEvent()
{
//	save();
	return S_OK;
}

/*
 * Called from outside the class to load all known client windows.
 */
void SysStatsApp::load()
{
	BSTR path;
	appConfig->CanonicalPath(INI, &path);
	_bstr_t path_t = path;
	char *appConfigFile = path_t;

	// Load the clients
	int count = GetPrivateProfileInt(GENERAL, CLIENT_COUNT_PROPERTY, 0, appConfigFile);

	char clientConfigFile[MAX_PATH];
	char name[64];
	strcpy(name, CLIENT_PREFIX);
	int offset = strlen(name);
	for (int i=0; i<count; i++)
	{
		itoa(i, name+offset, 10);

		GetPrivateProfileString(name, CONFIG_FILE_PROPERTY, UNKNOWN, clientConfigFile, MAX_PATH, appConfigFile);
		if (strcmp(clientConfigFile, UNKNOWN) != 0)
		{
			SysStatsClientWindow *win = new SysStatsClientWindow(this);
			SIZE size;
			size.cx = 128;
			size.cy = 128;
			HWND hWnd = win->create(size);
			if (hWnd)
			{
				win->load(appConfigFile, name);
				IClientModel *model = loadClient(win, clientConfigFile);
				add(win, model);
				model->Release();
			}
			else
			{
				delete win;
			}
		}
	}
}

void SysStatsApp::saveTheme()
{
	BSTR path;
	appConfig->CanonicalPath(INI, &path);
	_bstr_t path_t = path;
	char *appConfigFile = path_t;

	// Clear the skin file of all the crap
	int fd = _open(appConfigFile, _O_CREAT | _O_TRUNC | O_RDWR, _S_IWRITE);
	_close(fd);

	char val[32];
	WritePrivateProfileString(GENERAL, CLIENT_COUNT_PROPERTY, itoa(windows.count(), val, 10), appConfigFile);

	char name[64];
	strcpy(name, CLIENT_PREFIX);
	int offset = strlen(name);
	for (int i=0; i<windows.count(); i++)
	{
		itoa(i, name+offset, 10);
		windows[i]->save(appConfigFile, name);
		IClientModel *pModel;
		models.Item(CComVariant(i), &pModel);
		BSTR cf;
		pModel->get_ConfigFile(&cf);
		BSTR ncf;
		appConfig->NormalizePath(cf, &ncf);
		::SysFreeString(cf);
		WritePrivateProfileString(name, CONFIG_FILE_PROPERTY, _bstr_t(ncf, false), appConfigFile);
		pModel->Release();
	}
}

void SysStatsApp::save()
{
	BSTR path;
	appConfig->CanonicalPath(INI, &path);
	_bstr_t path_t = path;
	char *appConfigFile = path_t;

	// Clear the skin file of all the crap
	int fd = _open(appConfigFile, _O_CREAT | _O_TRUNC | O_RDWR, _S_IWRITE);
	_close(fd);

	char val[32];
	WritePrivateProfileString(GENERAL, CLIENT_COUNT_PROPERTY, itoa(windows.count(), val, 10), appConfigFile);

	char name[64];
	strcpy(name, CLIENT_PREFIX);
	int offset = strlen(name);
	for (int i=0; i<windows.count(); i++)
	{
		itoa(i, name+offset, 10);
		windows[i]->save(appConfigFile, name);
		IClientModel *pModel;
		models.Item(CComVariant(i), &pModel);
		pModel->Save(/* not interactive */ false);
		BSTR cf;
		pModel->get_ConfigFile(&cf);
		BSTR ncf;
		appConfig->NormalizePath(cf, &ncf);
		::SysFreeString(cf);
		WritePrivateProfileString(name, CONFIG_FILE_PROPERTY, _bstr_t(ncf, false), appConfigFile);
		pModel->Release();
	}
}

/*
 * Called to from a menu to allow a user to add an exsiting client config
 * to the overall config.
 */
STDMETHODIMP SysStatsApp::loadClient()
{
	char configFile[MAX_PATH];
	*configFile = 0;

	OPENFILENAME fileDialog;
	memset(&fileDialog,0,sizeof(OPENFILENAME));   
    fileDialog.lStructSize=sizeof(OPENFILENAME);
	fileDialog.lpstrFile = configFile;
	fileDialog.nMaxFile = sizeof(configFile);
	fileDialog.lpstrTitle = "Choose a config file...";
	fileDialog.lpstrFilter = "Config\0*.ini\0All\0*.*\0\0\0";
	fileDialog.lpstrDefExt = "ini";
	fileDialog.nFilterIndex = 1;
	fileDialog.Flags = OFN_CREATEPROMPT | OFN_NOREADONLYRETURN | OFN_HIDEREADONLY;

	if (GetOpenFileName(&fileDialog))
	{
		SysStatsClientWindow *win = new SysStatsClientWindow(this);
		SIZE size;
		size.cx = 128;
		size.cy = 128;
		HWND hWnd = win->create(size);
		if (hWnd)
		{
			win->setName("New Window");
			IClientModel *model = loadClient(win, configFile);
			add(win, model);
			saveTheme();
			model->Release();
		}
		else
		{
			delete win;
		}
	}

	return S_OK;
}

/*
 * Called from a menu to create a new client config.
 */
STDMETHODIMP SysStatsApp::newClient()
{
	IClientModelPtr model = NULL;
	HRESULT hr = model.CreateInstance("SysStatsCOM.ClientModel", NULL, CLSCTX_INPROC_SERVER);

	if (!SUCCEEDED(hr))
	{
		return hr;
	}

	SysStatsClientWindow *win = new SysStatsClientWindow(this);
	long width, height;
	model->get_Width(&width);
	model->get_Height(&height);
	SIZE size;
	size.cx = width;
	size.cy = height;
	HWND hWnd = win->create(size);
	if (hWnd)
	{
		model->put_AppConfig(appConfig);
		win->setName("New Window");
	 	win->setModel(model);	// So that timer gets set up first. Boy, I *must* sort this out.
		add(win, model);
	}
	else
	{
		delete win;
	}

	return S_OK;
}

void SysStatsApp::close(SysStatsClientWindow *window)
{
	getShellMenu()->removeSubMenu(window->getContextMenu());
	int pos = windows.find(window);
	if (pos > -1)
	{
		saveTheme();
		IClientModel *pModel;
		models.Item(CComVariant(pos), &pModel);
		models.Remove(CComVariant(pos));
		pModel->Release();
		delete windows.remove(pos);
	}
}

// Protected methods

/*
 * Load an existing client config.
 */
IClientModel *SysStatsApp::loadClient(SysStatsClientWindow *win, const char *configFile)
{
	IClientModelPtr model = NULL;
	HRESULT hr = model.CreateInstance("SysStatsCOM.ClientModel", NULL, CLSCTX_INPROC_SERVER);

	if (SUCCEEDED(hr))
	{
		model->put_AppConfig(appConfig);
		model->put_ConfigFile(_bstr_t(configFile));
	 	win->setModel(model);	// So that timer gets set up first. Boy, I *must* sort this out.
		model->Load();
	}

	model->AddRef();
	return model;
}

void SysStatsApp::add(SysStatsClientWindow *win, IClientModel *model)
{
	RECT rect;
	model->get_X(&rect.left);
	model->get_Y(&rect.top);
	model->get_Width(&rect.right);
	model->get_Height(&rect.bottom);
	rect.right += rect.left;
	rect.bottom += rect.top;
	win->MoveWindow(&rect);

	windows.add(win);
	models.Add(model);
	win->ShowWindow(nCmdShow);
	Menu *contextMenu = win->getContextMenu(m_hWnd);
	contextMenu->addAction(new CloseMenuAction("Close Window", win, this));
	getShellMenu()->addSubMenu(contextMenu);

	win->HandleEvent();		// Force initial paint - sucks
}

Menu* SysStatsApp::getShellMenu()
{
	if (shellMenu == NULL)
	{
		shellMenu = new Menu("SysStats", m_hWnd);
		shellMenu->addAction(new MenuActionT<SysStatsApp>("New", this, SysStatsApp::newClient));
		shellMenu->addAction(new MenuActionT<SysStatsApp>("Open...", this, SysStatsApp::loadClient));
		shellMenu->addAction(new ExitMenuAction("Exit"));
	}

	return shellMenu;
}

int SysStatsApp::showShellMenu()
{
    ::SetForegroundWindow (m_hWnd);
	getShellMenu()->action();
    // See MS KB article Q135788
    ::PostMessage (m_hWnd, WM_NULL, 0, 0);

	return 0;
}

// This handler is called every time there is a mouse movement.
LRESULT SysStatsApp::OnUserCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    // Show the popup menu.
    if (lParam == WM_RBUTTONDOWN) 
        showShellMenu ();

	return 0;
}

LRESULT SysStatsApp::command(UINT uMsg,	// message identifier
		WPARAM wParam,	// first message parameter
		LPARAM lParam,	// second message parameter
		BOOL &handled)
{
	if (HIWORD(wParam) == 0 && shellMenu)	// Menu command
		shellMenu->action(LOWORD(wParam));
	else
		handled = FALSE;

	return 0;
}

