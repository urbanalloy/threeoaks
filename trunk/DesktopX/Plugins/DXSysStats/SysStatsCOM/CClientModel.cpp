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

// ClientModel.cpp : Implementation of CSysStatsCOMApp and DLL registration.

#include "stdafx.h"
#include <locale.h>
#include "SysStatsCOM.h"
#include "CClientModel.h"
#include "ConfigureDialog.h"
#include "CControllerFactory.h"

#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <CommDlg.h>

static _bstr_t EMPTY_BSTR = "";

// Some string constants
const _bstr_t CClientModel::OVERLAY_PREFIX = "overlay";
const _bstr_t CClientModel::CONTROLLER_PREFIX = "control";
const _bstr_t CClientModel::METER_PREFIX = "meter";
const _bstr_t CClientModel::LABEL_PREFIX = "label";
const _bstr_t CClientModel::CONFIG_GENERAL = "General";
const _bstr_t CClientModel::WINDOW_WIDTH = "width";
const _bstr_t CClientModel::WINDOW_HEIGHT = "height";
const _bstr_t CClientModel::WINDOW_X = "x";
const _bstr_t CClientModel::WINDOW_Y = "y";
const _bstr_t CClientModel::VERSION_KEY = "version";
const _bstr_t CClientModel::NAME = "name";
const _bstr_t CClientModel::DESCRIPTION = "description";
const _bstr_t CClientModel::DVERSION = "dversion";
const _bstr_t CClientModel::AUTHOR = "author";
const _bstr_t CClientModel::FITTOWINDOW = "fitToWindow";
const int CClientModel::VERSION = 200;

CSimpleMap<HWND, IClientModel*> CClientModel::subclassMap;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//

STDMETHODIMP CClientModel::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IClientModel,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CClientModel::CClientModel() :
	configFile(""),
	appConfig(0),
	m_hWnd(0),
	d(0),
	plugins(PluginManager::getManager()),
	timerService(NULL),
	pSubclass(0),
	pSuperProc(0),
	name(""),
	description(""),
	version(""),
	author(""),
	fitToWindow(false),
	active(true)
{
	HRESULT hr = ::CoCreateInstance(CLSID_MeterHome, NULL, CLSCTX_INPROC_SERVER, IID_IMeterHome, (void**)&pMeters);
	hr = ::CoCreateInstance(CLSID_OverlayHome, NULL, CLSCTX_INPROC_SERVER, IID_IOverlayHome, (void**)&pOverlays);
	hr = ::CoCreateInstance(CLSID_ControllerHome, NULL, CLSCTX_INPROC_SERVER, IID_IControllerHome, (void**)&pControllers);
	hr = ::CoCreateInstance(CLSID_Label, NULL, CLSCTX_INPROC_SERVER, IID_ILabel, (void**)&pLabel);

	width=128;
	height=128;

	dirty = true;
}

CClientModel::~CClientModel()
{
}

void CClientModel::FinalRelease()
{
	AtlTrace("CClientModel - FinalRelease\n");
	if (d)
		delete d;

	pControllers->Release();
	pOverlays->Release();
	pLabel->Release();
	pMeters->Release();
	if (appConfig)
		appConfig->Release();
}

// Temporary method
IController* CClientModel::addShortcut()
{
	short count = 0;
	pControllers->Count(&count);
	AtlTrace("addShortcut %d\n", count);
	IController *shortcut = 0;
	if (count == 0)
	{
		AtlTrace("addShortcut %d\n", count);
		shortcut = CControllerFactory::createInstance("Shortcut Class", this);
		if (shortcut)
		{
			shortcut->put_Name(_bstr_t("Shortcut"));
			pControllers->Add(shortcut);
		}
	}
	else
	{
		pControllers->Item(CComVariant(0), &shortcut);
		shortcut->Release();
	}

	return shortcut;
}

STDMETHODIMP CClientModel::get_Meters(IMeterHome **pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = pMeters;
	pMeters->AddRef();

	return S_OK;
}

STDMETHODIMP CClientModel::get_Controllers(IControllerHome **pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = pControllers;
	pControllers->AddRef();

	return S_OK;
}

STDMETHODIMP CClientModel::get_Overlays(IOverlayHome **pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = pOverlays;
	pOverlays->AddRef();

	return S_OK;
}

STDMETHODIMP CClientModel::put_TimerService(ICOMTimerService *timerService)
{
	this->timerService = timerService;
	pMeters->put_TimerService(timerService);

	return S_OK;
}

STDMETHODIMP CClientModel::get_TimerService(ICOMTimerService **pRet)
{
	*pRet = timerService;

	return S_OK;
}

// CClientModel.cpp: implementation of the CClientModel class.
//
//////////////////////////////////////////////////////////////////////

STDMETHODIMP CClientModel::get_ConfigFile(BSTR *pVal)
{
	appConfig->NormalizePath(configFile, pVal);

	return S_OK;
}

STDMETHODIMP CClientModel::put_ConfigFile(BSTR newVal)
{
	BSTR tmp;
	appConfig->CanonicalPath(newVal, &tmp);
	configFile = tmp;
	::SysFreeString(tmp);

	return S_OK;
}

STDMETHODIMP CClientModel::UpdateMeters()
{
	long _dirty = false;

	pMeters->Update(&_dirty);
	if (_dirty)
		put_dirty(_dirty);

	return S_OK;
}

STDMETHODIMP CClientModel::UpdateOverlays(long hdc)
{
	if (active)
		pOverlays->Render(hdc);

	return S_OK;
}

STDMETHODIMP CClientModel::Load()
{
	if (configFile == _bstr_t(L""))
	{
		return S_OK;
	}

	IIINIPersister *persister;
	HRESULT hr = ::CoCreateInstance(CLSID_IINIPersister, NULL, CLSCTX_INPROC_SERVER, IID_IIINIPersister, (void**)&persister);

	ICOMINIFile *iniFile;
	hr = ::CoCreateInstance(CLSID_COMINIFile, NULL, CLSCTX_INPROC_SERVER, IID_ICOMINIFile, (void**)&iniFile);

	char *oldLocale = ::strdup(::setlocale(LC_ALL, "English"));

	iniFile->Read(configFile);

	// Only version with no version number is 104
	int writingVersion = 200;
	iniFile->GetInt(CONFIG_GENERAL, VERSION_KEY, 104, &writingVersion);

	// Load our stuff
	iniFile->GetLong(CONFIG_GENERAL, WINDOW_X, 0, &x);
	iniFile->GetLong(CONFIG_GENERAL, WINDOW_Y, 0, &y);
	iniFile->GetLong(CONFIG_GENERAL, WINDOW_WIDTH, 128, &width);
	iniFile->GetLong(CONFIG_GENERAL, WINDOW_HEIGHT, 128, &height);

	BSTR tmpStr;

	iniFile->GetString(CONFIG_GENERAL, NAME, L"", &tmpStr);
	name.Assign(tmpStr);

	iniFile->GetString(CONFIG_GENERAL, DESCRIPTION, L"", &tmpStr);
	description.Assign(tmpStr);

	iniFile->GetString(CONFIG_GENERAL, DVERSION, L"", &tmpStr);
	version.Assign(tmpStr);

	iniFile->GetString(CONFIG_GENERAL, AUTHOR, L"", &tmpStr);
	author.Assign(tmpStr);

	// Load the meters
	pMeters->put_Model(this);
	if (writingVersion == 104)
		persister->Configure(iniFile, CONFIG_GENERAL, FALSE, EMPTY_BSTR, this, writingVersion == 104);
	else
		persister->Configure(iniFile, METER_PREFIX, FALSE, EMPTY_BSTR, this, writingVersion == 104);

	persister->Load(pMeters);

	// Prime their pumps
	UpdateMeters();

	// Load the overlays
	pOverlays->put_Model(this);
	persister->Configure(iniFile, OVERLAY_PREFIX, FALSE, EMPTY_BSTR, this, writingVersion == 104);
	persister->Load(pOverlays);
	if (writingVersion == 104)
	{
		// Read background image
		IOverlay *background = COverlayFactory::createInstance("ImageOverlay Class", this);
		background->put_Name(_bstr_t("Background_1.0.4"));
		persister->Configure(iniFile, CONFIG_GENERAL, FALSE, EMPTY_BSTR, this, writingVersion == 104);
		persister->Load(background);
		pOverlays->Add(background);
	}

	// Load shortcut stuff
	pControllers->put_Model(this);
	if (writingVersion == 104)
	{
		// Load old shortcut stuff
		IController *pShortcut = addShortcut();
		persister->Configure(iniFile, CONFIG_GENERAL, FALSE, EMPTY_BSTR, this, writingVersion == 104);
		persister->Load(pShortcut);
	} else {
		persister->Configure(iniFile, CONTROLLER_PREFIX, FALSE, EMPTY_BSTR, this, writingVersion == 104);
		persister->Load(pControllers);
//		addShortcut();
	}

	// Load the label!
	pLabel->put_Model(this);
	if (writingVersion == 104)
		persister->Configure(iniFile, CONFIG_GENERAL, FALSE, EMPTY_BSTR, this, writingVersion == 104);
	else
		persister->Configure(iniFile, LABEL_PREFIX, FALSE, EMPTY_BSTR, this, writingVersion == 104);

	persister->Load(pLabel);

	iniFile->Release();
	persister->Release();

	::setlocale(LC_ALL, oldLocale);
	::free(oldLocale);

	return S_OK;
}

STDMETHODIMP CClientModel::Save(short interactive)
{
	if (configFile == _bstr_t(L""))
	{
		if (interactive)
			SaveAs();

		return S_OK;
	}

	IIINIPersister *persister;
	HRESULT hr = ::CoCreateInstance(CLSID_IINIPersister, NULL, CLSCTX_INPROC_SERVER, IID_IIINIPersister, (void**)&persister);

	ICOMINIFile *iniFile;
	hr = ::CoCreateInstance(CLSID_COMINIFile, NULL, CLSCTX_INPROC_SERVER, IID_ICOMINIFile, (void**)&iniFile);

	char *oldLocale = ::strdup(::setlocale(LC_ALL, "English"));


	iniFile->PutInt((BSTR)CONFIG_GENERAL, (BSTR)VERSION_KEY, VERSION);
	iniFile->PutInt((BSTR)CONFIG_GENERAL, (BSTR)WINDOW_X, x);
	iniFile->PutInt((BSTR)CONFIG_GENERAL, (BSTR)WINDOW_Y, y);
	iniFile->PutInt((BSTR)CONFIG_GENERAL, (BSTR)WINDOW_WIDTH, width);
	iniFile->PutInt((BSTR)CONFIG_GENERAL, (BSTR)WINDOW_HEIGHT, height);

	iniFile->PutString((BSTR)CONFIG_GENERAL, (BSTR)NAME, name);
	iniFile->PutString((BSTR)CONFIG_GENERAL, (BSTR)DESCRIPTION, description);
	iniFile->PutString((BSTR)CONFIG_GENERAL, (BSTR)DVERSION, version);
	iniFile->PutString((BSTR)CONFIG_GENERAL, (BSTR)AUTHOR, author);

	// Save the meters
	persister->Configure(iniFile, METER_PREFIX, FALSE, EMPTY_BSTR, this, false);
	pMeters->put_Model(this);
	persister->Save(pMeters);
	
	// Save controllers
	persister->Configure(iniFile, CONTROLLER_PREFIX, FALSE, EMPTY_BSTR, this, false);
	pControllers->put_Model(this);
	persister->Save(pControllers);

	// Save overlays
	persister->Configure(iniFile, OVERLAY_PREFIX, FALSE, EMPTY_BSTR, this, false);
	pOverlays->put_Model(this);
	persister->Save(pOverlays);

	// Save label
	persister->Configure(iniFile, LABEL_PREFIX, FALSE, EMPTY_BSTR, this, false);
	pLabel->put_Model(this);
	persister->Save(pLabel);

	iniFile->Save(configFile);

	persister->Release();
	iniFile->Release();

	::setlocale(LC_ALL, oldLocale);
	::free(oldLocale);

	return S_OK;
}

STDMETHODIMP CClientModel::SaveAs()
{
	char cf[MAX_PATH] = { 0 };
	char id[MAX_PATH];
	BSTR bf;
	appConfig->CanonicalPath(L"configs", &bf);
	strncpy(id, _bstr_t(bf, false), sizeof(id));
	OPENFILENAME of;
	memset(&of,0,sizeof(of));   
    of.lStructSize=sizeof(of);
	of.lpstrFile = cf;
	of.lpstrInitialDir = id;
	of.nMaxFile = sizeof(cf);
	of.lpstrTitle = "Save As: choose/create a config file...";
	of.lpstrFilter = "Config\0*.ini\0All\0*.*\0\0\0";
	of.lpstrDefExt = "ini";
	of.nFilterIndex = 1;
	of.Flags = OFN_CREATEPROMPT | OFN_NOREADONLYRETURN | OFN_HIDEREADONLY;

	if (GetSaveFileName(&of))
	{
		configFile = cf;
		Save(false);
	}

	return S_OK;
}

#define NONMODAL

STDMETHODIMP CClientModel::Configure(IObserver *observer, long hWnd)
{
#ifdef NONMODAL
	if (d)
	{
		if (::IsWindow(d->m_hWnd))
		{
			d->ShowWindow(SW_SHOWNORMAL);
			d->BringWindowToTop();
			return S_OK;
		}
		else
		{
			delete d;
		}
	}

	pMeters->put_Model(this);
	pOverlays->put_Model(this);
	pControllers->put_Model(this);
	d = new CConfigureDialog(this, observer);
	d->Create(0);
	d->ShowWindow(SW_SHOWNORMAL);
#else
	CConfigureDialog *cd = new CConfigureDialog(this, observer);
	cd->DoModal((HWND)hWnd);
	delete cd;

#endif

	return S_OK;
}

STDMETHODIMP CClientModel::get_dirty(long *pVal)
{
	*pVal = dirty;

	return S_OK;
}

STDMETHODIMP CClientModel::put_dirty(long newVal)
{
	dirty = newVal;

	MSG msg;
	if (dirty && !::PeekMessage(&msg, m_hWnd, WM_USER+144, WM_USER+144, PM_NOREMOVE | PM_NOYIELD | PM_QS_POSTMESSAGE))
	{
		if (!PostMessage(WM_USER+144, 0, 0))
			SysStatsUtils::checkError();
	}

	return S_OK;
}

STDMETHODIMP CClientModel::get_AppConfig(IAppConfig **pVal)
{
	appConfig->AddRef();
	*pVal = appConfig;
	
	return S_OK;
}

STDMETHODIMP CClientModel::put_AppConfig(IAppConfig *newVal)
{
	newVal->AddRef();
	if (appConfig)
		appConfig->Release();

	appConfig = newVal;
	plugins->put_AppConfig(appConfig);

	return S_OK;
}

STDMETHODIMP CClientModel::HandleMessage(short messageID, UINT wParam, UINT lParam, short x, short y, VARIANT_BOOL *handled)
{
	AtlTrace("HandleMessage=0x%0.4x\n", messageID);
	pControllers->HandleMessage(messageID, wParam, lParam, x, y, handled);

	return S_OK;
}

STDMETHODIMP CClientModel::get_Label(ILabel **pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = pLabel;
	pLabel->AddRef();

	return S_OK;
}

STDMETHODIMP CClientModel::get_X(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = x;

	return S_OK;
}

STDMETHODIMP CClientModel::put_X(long newVal)
{
	x = newVal;

	return S_OK;
}

STDMETHODIMP CClientModel::get_Y(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = y;

	return S_OK;
}

STDMETHODIMP CClientModel::put_Y(long newVal)
{
	y = newVal;

	return S_OK;
}

STDMETHODIMP CClientModel::get_Width(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = width;

	return S_OK;
}

STDMETHODIMP CClientModel::put_Width(long newVal)
{
	width = newVal;

	return S_OK;
}

STDMETHODIMP CClientModel::get_Height(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = height;

	return S_OK;
}

STDMETHODIMP CClientModel::put_Height(long newVal)
{
	height = newVal;

	return S_OK;
}

STDMETHODIMP CClientModel::PostMessage(short messageID, UINT wParam, UINT lParam)
{
	BOOL ret = ::PostMessage(m_hWnd, messageID, wParam, lParam);

	if (!ret)
		SysStatsUtils::checkError();

	return S_OK;
}

STDMETHODIMP CClientModel::get_hWnd(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = (long)m_hWnd;

	return S_OK;
}

STDMETHODIMP CClientModel::put_hWnd(long newVal)
{
	m_hWnd = (HWND)newVal;

	return S_OK;
}

/*
 * Instance sub-class message handler
 */
STDMETHODIMP CClientModel::HandleSubclassMessage(UINT msg, UINT wparam, UINT lparam)
{
	VARIANT_BOOL bHandled = false;

	if (pSubclass)
		pSubclass->HandleSubclassMessage(msg, wparam, lparam, &bHandled);

	if (!bHandled)
		CallWindowProc(pSuperProc, m_hWnd, msg, wparam, lparam);

	return S_OK;
}

/*
 * Static sub-class message handler - just find and dispatch to instance version
 */
void CClientModel::MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	IClientModel *pModel = subclassMap.Lookup(hwnd);

	if (pModel)
		pModel->HandleSubclassMessage(msg, wparam, lparam);
}

STDMETHODIMP CClientModel::SetWindowSubclass(IController *pSubclass)
{
	RemoveWindowSubclass(this->pSubclass);

	pSuperProc = (WNDPROC)SetWindowLong(m_hWnd, GWL_WNDPROC, (long)MessageHandler);
	this->pSubclass = pSubclass;
	subclassMap.Add(m_hWnd, this);

	return S_OK;
}

STDMETHODIMP CClientModel::RemoveWindowSubclass(IController *pSubclass)
{
	if (pSuperProc)
		SetWindowLong(m_hWnd, GWL_WNDPROC, (long)pSuperProc);

	subclassMap.Remove(m_hWnd);
	pSuperProc = 0;
	pSubclass = 0;

	return S_OK;
}

STDMETHODIMP CClientModel::get_Name(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = name.copy();
		
	return S_OK;
}

STDMETHODIMP CClientModel::put_Name(BSTR newVal)
{
	name = newVal;

	return S_OK;
}

STDMETHODIMP CClientModel::get_Description(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = description.copy();
		
	return S_OK;
}

STDMETHODIMP CClientModel::put_Description(BSTR newVal)
{
	description = newVal;

	return S_OK;
}

STDMETHODIMP CClientModel::get_Version(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = version.copy();
		
	return S_OK;
}

STDMETHODIMP CClientModel::put_Version(BSTR newVal)
{
	version = newVal;

	return S_OK;
}

STDMETHODIMP CClientModel::get_Author(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = author.copy();
		
	return S_OK;
}

STDMETHODIMP CClientModel::put_Author(BSTR newVal)
{
	author = newVal;

	return S_OK;
}

STDMETHODIMP CClientModel::get_FitToWindow(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = fitToWindow;

	return S_OK;
}

STDMETHODIMP CClientModel::put_FitToWindow(short newVal)
{
	fitToWindow = newVal;

	put_dirty(true);

	return S_OK;
}

STDMETHODIMP CClientModel::SetActive(short active)
{
	this->active = active;

	return S_OK;
}
