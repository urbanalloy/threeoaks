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

// SysDocklet.cpp: implementation of the SysDocklet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <direct.h>
#include <stdlib.h>
#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "DeskletSDK.h"
#include "SysDocklet.h"
#include "SizeDialog.h"
#include "SkinEnumerator.hpp"
#include "MenuActionT.h"
#include "SysStatsClientModelConfigure.h"
#include "AppearanceDialog.h"
#include "HDCImage.h"

static const char* IMAGE_ROOT = "docklets\\sysstats";
SysStatsUtils::CriticalSection SysDocklet::registerCS;
bool SysDocklet::dllRegistered = false;

const _bstr_t SysDocklet::APPLICATION_KEY = "\\Software\\SysStatsOD\\2.0";
const int SysDocklet::VERSION = 250;

#define IS_MOUSE_EVENT(uMsg) ((uMsg >= WM_MOUSEFIRST) && (uMsg <= WM_MOUSELAST))


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
 * Return basic information about the plugin.
 */
void CALLBACK SysDocklet::getInformation(char *szName, char *szAuthor, int *iVersion, char *szNotes)
{
	AtlTrace("SysDocklet::getInformtion - in\n");
	strcpy(szName, "SysStats");
	strcpy(szAuthor, "Paul Andrews");
	(*iVersion) = VERSION;
	strcpy(szNotes,   "http://webpages.charter.net/joolsie/SysStats.htm");
	AtlTrace("SysDocklet::getInformtion - out\n");
}

typedef HRESULT (*DLLFunc)();

// Register any COM objects in SysStatsCOM.dll.
void SysDocklet::registerDLL()
{
	SysStatsUtils::Lock lock(registerCS);

	if (!dllRegistered)
	{
		HMODULE hDLL = GetModuleHandle("SysStatsCOM.dll");
		if (hDLL)
		{
			DLLFunc pFunc = (DLLFunc)GetProcAddress(hDLL, "DllRegisterServer");
			if (pFunc)
			{
				// call the function
				AtlTrace("Registering SysStatsCOM server\n");
				HRESULT hr = pFunc();
				if (!SUCCEEDED(hr))
				{
					AtlTrace("RegisterServer failed=%x\n", hr);
				}
				else
					dllRegistered = true;
			}
		}
		else
		{
			AtlTrace("Could not retrieve handle to SysStatsCOM.dll\n");
		}
	}
	else
	{
		AtlTrace("SysStatsCOM.dll already registered\n");
	}
}

/*
 * In the OnCreate, we will be passed the handle to the window which owns this docklet.
 * In ObjectDock, each icon/image is its own window, thus the window passed is the actual
 * window that the image is being displayed on. Thus, it is recommended
 * that you not move or otherwise manually change the label of this window.
 * However! You will need to record this window handle, as it is through that that
 * you will identify yourself to ObjectDock in callbacks.
 *
 * An hInstance is passed giving this DLL's instance, as well as a location of an ini
 * and the specific ini group to load from. If szIni and szIniGroup are NULL, this
 * plugin is being created from scratch, and default options should be set/loaded. 
 * If they are not null, the plugin should load its settings from the ini given
 * by szIni, from the ini group given by szIniGroup. See examples.
 */

Docklet* CALLBACK SysDocklet::create(HWND hwndDocklet, HINSTANCE hInstance, char *szIni, char *szIniGroup)
{
	// First off, register any COM objects in SysStatsCOM.dll.
	// Has to be called after CoInitialize
	registerDLL();

	SysDocklet *self = new SysDocklet(hwndDocklet, hInstance, szIni, szIniGroup);

	return new Docklet(self);
}

CSkinEnumerator* CALLBACK SysDocklet::createSkinEnumerator(HINSTANCE hInstance, const WCHAR* deskletPath)
{
	return new CSkinEnumerator(hInstance, deskletPath);
}

void CALLBACK SysDocklet::destroySkinEnumerator(CSkinEnumerator* se)
{
	delete se;
}

INT CALLBACK SysDocklet::getSkin(CSkinEnumerator* se, SkinInfo *info /* UINT num, WCHAR* wSkin, UINT* pMaxSkinLength */)
{
	if (info->dwSize != sizeof(SkinInfo))
		return -2;	// We want a skininfo that matches our expectations.

	if(info->num >= se->size() )
		return AVEDESK_NO_MORE_SKINS;

	const _bstr_t& key = se->getKey(info->num);
	if(info->maxKeyLength < key.length()+1 )
	{
		// the output buffer is 
		info->maxKeyLength = key.length() + 1;
		return AVEDESK_BUF_TOO_SMALL;
	}

	wcscpy(info->wKey, key);

	const CSkinValue& value = se->getValue(key);

	if(info->maxNameLength < value.name.length()+1 )
	{
		// the output buffer is 
		info->maxNameLength = value.name.length() + 1;
		return AVEDESK_BUF_TOO_SMALL;
	}

	wcscpy(info->wName, value.name);

	if(info->maxNotesLength < value.description.length()+1 )
	{
		// the output buffer is 
		info->maxNotesLength = value.description.length() + 1;
		return AVEDESK_BUF_TOO_SMALL;
	}

	wcscpy(info->wNotes, value.description);

	if(info->maxAuthorLength < value.author.length()+1 )
	{
		// the output buffer is 
		info->maxAuthorLength = value.author.length() + 1;
		return AVEDESK_BUF_TOO_SMALL;
	}

	wcscpy(info->wAuthor, value.author);

	if(info->maxVersionStringLength < value.version.length()+1 )
	{
		// the output buffer is 
		info->maxVersionStringLength = value.version.length() + 1;
		return AVEDESK_BUF_TOO_SMALL;
	}

	wcscpy(info->versionString, value.version);

	return AVEDESK_MORE_SKINS;
}

Docklet* CALLBACK SysDocklet::createFromSkin(HWND hwndDocklet, HINSTANCE hInstance, CSkinEnumerator* se, const SkinInfo *info /* const WCHAR* wKey, UINT num */)
{
	registerDLL();

	const CSkinValue& value = se->getValue(info->wKey);

	SysDocklet *docklet = NULL;

	// Hack to create a new SysStats instance in AD1.2
	if (value.name == _bstr_t("_New"))
		docklet = new SysDocklet(hwndDocklet, hInstance, NULL, NULL);
	else
		docklet = new SysDocklet(hwndDocklet, hInstance, value.filename);

	return new Docklet(docklet);
}

void SysDocklet::preInit()
{
//	::CoInitialize(NULL);

	HRESULT hr = ::CoCreateInstance(CLSID_AppConfig, NULL, CLSCTX_INPROC_SERVER, IID_IAppConfig, (void**)&appConfig);
	if (!SUCCEEDED(hr))
		SysStatsUtils::checkError();
	else if (getHWND() != 0)
	{
		char dockletFolder[MAX_PATH];

		// The ObjectDock root folder
		DockletGetRootFolder(getHWND(), dockletFolder);
		appConfig->put_InstallDir(_bstr_t(dockletFolder));
		appConfig->put_AltRoot(_bstr_t(dockletFolder));

		// This docklet's root folder relative to the OD root folder
		DockletGetRelativeFolder(getHWND(), dockletFolder);

		// Concatenate the two
		_bstr_t rel(dockletFolder);
		BSTR path;
		appConfig->CanonicalPath(rel, &path);

		// Set as our installdir!
		appConfig->put_InstallDir(_bstr_t(path, false));
	}

	hr = ::CoCreateInstance(CLSID_ClientModel, NULL, CLSCTX_INPROC_SERVER, IID_IClientModel, (void**)&model);
	if (!SUCCEEDED(hr))
		SysStatsUtils::checkError();
	else
	{
		model->put_AppConfig(appConfig);
		model->put_hWnd((long)getHWND());
	}

	RECT r;

	if (DeskletGetRect(getHWND(), &r))
	{
		isAD1_2 = true;	// Until proven otherwise
		isDesklet = true;
	}
}

void SysDocklet::postInit()
{
	adjustWindow(getHWND());

	contextMenu = new Menu("Main Menu", getHWND());
	contextMenu->addAction(new MenuActionT<SysDocklet>("Label...", this, &SysDocklet::configureLabel));
	contextMenu->addAction(new SysStatsClientModelConfigure("Configure...", model, this, getHWND()));
	contextMenu->addAction(new MenuActionT<SysDocklet>("Size...", this, &SysDocklet::configureSize));
	contextMenu->addAction(new MenuActionT<SysDocklet>("Load...", this, &SysDocklet::loadConfigFile));
	contextMenu->addAction(new MenuActionT<SysDocklet>("Save", this, &SysDocklet::Save));
	contextMenu->addAction(new MenuActionT<SysDocklet>("Save as...", this, &SysDocklet::SaveAs));
	contextMenu->addAction(new MenuActionT<SysDocklet>("Screenshot...", this, &SysDocklet::Screenshot));
	contextMenu->addAction(new MenuActionT<SysDocklet>("Update...", this, &SysDocklet::HandleEvent));
	contextMenu->addAction(new MenuActionT<SysDocklet>("Help...", this, &SysDocklet::showHelp));

	updateMeters();
	updateOverlays();
	timerService.setHWND(getHWND());
//	timerService.SetTimerClient(this);
}

/*
 * Constructor. Called from create().
 */
SysDocklet::SysDocklet(HWND hwndDocklet, HINSTANCE hInstance, WCHAR* fileName) :
	DockletLetter(hwndDocklet, hInstance),
	contextMenu(0),
	isDesklet(true),
	visible(true),
	timerSet(false)
{
	preInit();

	load((char*)_bstr_t(fileName), NULL);

	postInit();
}

/*
 * Constructor. Called from create().
 */
SysDocklet::SysDocklet(HWND hwndDocklet, HINSTANCE hInstance, char *szIni, char *szIniGroup) :
	DockletLetter(hwndDocklet, hInstance),
	contextMenu(0),
	isDesklet(false),
	visible(true),
	timerSet(false)
{
	preInit();

	if(szIni && szIniGroup)
	{
		// Been created with a saved config
		load(szIni, szIniGroup);
	}
	else
	{
		// Brand new - initialize some stuff
		model->put_TimerService(&timerService);
	}

	postInit();
}

/*
 * Destructor. Called from destroy().
 */
SysDocklet::~SysDocklet()
{
	// Its important that we clear all the timers first.
	timerService.ClearAllTimers();

	if (model)
		model->Release();

	if (appConfig)
		appConfig->Release();

	delete contextMenu;

//	::CoUninitialize();
}

void SysDocklet::adjustWindow(HWND hwndDocklet)
{
	RECT r;

	if (DeskletGetRect(hwndDocklet, &r))
	{
		model->put_Width(r.right-r.left);
		model->put_Height(r.bottom-r.top);
		model->put_X(r.left);
		model->put_Y(r.top);
	}
}

/*
 * Each icon in the dock is its own window. The OnProcessMessage function
 * recieves every message that gets sent to that window (the HWND passed on OnCreate),
 * including any timer messages, which we will make use of here
 */
void SysDocklet::processMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL handled;
	isAD1_2 = false;

	processMessageEx(hwnd, uMsg, wParam, lParam, &handled);
}

/*
 * Each icon in the dock is its own window. The OnProcessMessage function
 * recieves every message that gets sent to that window (the HWND passed on OnCreate),
 * including any timer messages, which we will make use of here
 */
LRESULT SysDocklet::processMessageEx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL *bHandled)
{
	VARIANT_BOOL done = false;
	*bHandled = false;

	switch (uMsg)
	{
	case WM_USER + 144:
		/*
		Want to set the timer if it isn't already. Need to do this atomically as the timer handler
		should unset it.
		*/
		updateOverlays();
/*
		if (!timerSet)
		{
			timerService.SetTimerClient(this);
			timerSet = true;
		}
*/		return 1;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		// These are handled specially in docklets and AD1.2!
		// So if we are in AD1.1 carry on, otherwise return
		if (!isDesklet)
			return 0;

		break;

	case WM_COMMAND:
		if (HIWORD(wParam) == 0)
		{
			contextMenu->action(LOWORD(wParam));
			*bHandled = true;
		}
		return 1;

	case WM_SIZE:
	case WM_MOVE:
		adjustWindow(hwnd);
		if (uMsg == WM_SIZE)
			model->put_dirty(true);
		break;
	}

	if (model)
	{
		if (uMsg == WM_MOUSEMOVE)
		{
			TRACKMOUSEEVENT trackDef = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, HOVER_DEFAULT };
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

		scalePoint(&pos);
		model->HandleMessage(uMsg, wParam, lParam, pos.x, pos.y, &done);
		*bHandled = done;
	}

	return 1;
}

/*
 * Arguments to this are for the main OD config file.
 */
void SysDocklet::load(char *szIni, char *szIniGroup)
{
	char configFile[MAX_PATH];

	if (szIni && szIniGroup)
	{
		char cf[MAX_PATH];
		GetPrivateProfileString(szIniGroup, "config", "", cf, MAX_PATH, szIni);
		if (strnicmp(cf, IMAGE_ROOT, strlen(IMAGE_ROOT)) == 0)
		{
			// We have an old relative path
			strcpy(configFile, cf+strlen(IMAGE_ROOT)+1);
		}
		else
			strcpy(configFile, cf);

		// Only version that doesn't have a version string is 104
		int version = GetPrivateProfileInt(szIniGroup, "version", 104, szIni);
	}
	else if (!szIniGroup)
		strncpy(configFile, szIni, sizeof(configFile));

	model->put_ConfigFile(_bstr_t(configFile));
	model->put_TimerService(&timerService);
	model->Load();

	// For AveDesk.
	if (isDesklet)
	{
		SIZE size;
		POINT pos;
		model->get_X(&pos.x);
		model->get_Y(&pos.y);
		model->get_Width(&size.cx);
		model->get_Height(&size.cy);

		DeskletSetPosition(getHWND(), &pos);
		DeskletSetSize(getHWND(), &size);

		char buf[1024];
		SkinInfo *info = new SkinInfo();
		info->dwSize = sizeof (SkinInfo);
		info->wKey = NULL;
		info->maxKeyLength = 0;
		info->num = 0;
		info->version = 1;

		::GetPrivateProfileString("General", "name", "", buf, sizeof(buf), configFile);
		_bstr_t _name(buf);
		info->wName = _name;
		info->maxNameLength = ::wcslen(info->wName);

		::GetPrivateProfileString("General", "author", "", buf, sizeof(buf), configFile);
		_bstr_t _author(buf);
		info->wAuthor = _author;
		info->maxAuthorLength = ::wcslen(info->wAuthor);

		::GetPrivateProfileString("General", "description", "", buf, sizeof(buf), configFile);
		_bstr_t _description(buf);
		info->wNotes = _description;
		info->maxNotesLength = ::wcslen(info->wNotes);

		::GetPrivateProfileString("General", "dversion", "", buf, sizeof(buf), configFile);
		_bstr_t _version(buf);
		info->versionString = _version;
		info->maxVersionStringLength = ::wcslen(info->versionString);

		::PathRemoveExtension(configFile);
		_bstr_t _key(configFile);
		info->wKey = _key;
		info->maxKeyLength = ::wcslen(info->wKey);

		DeskletSetSkinInfoW(getHWND(), info);

		delete info;
	}
}

/*
 * Called when we need to persist our state. Arguments are for main OD config file
 */
void SysDocklet::save(char *szIni, char *szIniGroup, BOOL bIsForExport)
{
	AtlTrace("Save %s[%s]\n", szIni, szIniGroup);

	BSTR cFile;
	model->get_ConfigFile(&cFile);
	_bstr_t _ncf(cFile, false);

//	I think this could return the config directory if the configfile is the empty string.

	if (szIni && szIniGroup)
	{
		WritePrivateProfileString(szIniGroup, "config", _ncf, szIni);
		WritePrivateProfileInt(szIniGroup, "version", VERSION, szIni);
	}

	// model->Save() trashes the config file so we have to do it before any other saves
	model->Save(/* not interactive */ false);
}

void SysDocklet::save()
{
	// Not called any more?
	save(NULL, NULL, FALSE);
}

void SysDocklet::scalePoint(POINT *pt)
{
	RECT rect;
	DockletGetRect(getHWND(), &rect);
	if ((rect.right != rect.left) && (rect.top != rect.bottom))
	{
		long width, height;
		model->get_Width(&width);
		model->get_Height(&height);
		pt->x = pt->x * width / (rect.right-rect.left);
		pt->y = pt->y * height / (rect.bottom-rect.top);
	}
}

/*
 * Try and execute an application, if there is one.
 */
BOOL SysDocklet::leftButtonClick(POINT *ptCursor, SIZE *sizeDocklet)
{
	VARIANT_BOOL handled = false;
	if (!isAD1_2)
	{
		scalePoint(ptCursor);
		model->HandleMessage(WM_LBUTTONDOWN, 0, 0, ptCursor->x, ptCursor->y, &handled);

		if (handled)
			DockletDoAttentionAnimation(getHWND());
	}

	return handled;
}

/*
 * Left button held or ctrl-left click!
 */
BOOL SysDocklet::leftButtonHeld(POINT *ptCursor, SIZE *sizeDocklet)
{
	VARIANT_BOOL handled = false;
	scalePoint(ptCursor);
	model->HandleMessage(WM_LBUTTONDOWN, MK_CONTROL, 0, ptCursor->x, ptCursor->y, &handled);

	return handled;
}

/*
 * Left button double-click
 */
BOOL SysDocklet::doubleClick(POINT *ptCursor, SIZE *sizeDocklet)
{
	VARIANT_BOOL handled = false;
	scalePoint(ptCursor);
	model->HandleMessage(WM_LBUTTONDBLCLK, 0, 0, ptCursor->x, ptCursor->y, &handled);

	return handled;
}

//By implementing the OnRightButtonClick function, ObjectDock will not automatically show a right
// click menu for the item, and thus we must create one. We want one with an Update and a Change location (configure)
BOOL SysDocklet::rightButtonClick(POINT *ptCursor, SIZE *sizeDocklet)
{
	AtlTrace("RBClick\n");
	DockletLockMouseEffect(getHWND(), TRUE);	//Lock the dock's zooming while our menu is in use.
	contextMenu->action();
	DockletLockMouseEffect(getHWND(), FALSE);
	return TRUE;
}

STDMETHODIMP SysDocklet::configureLabel()
{
	ILabel *label;
	model->get_Label(&label);
	label->put_Model(model);
	IMeterHome *meters;
	model->get_Meters(&meters);
	AppearanceDialog *pDialog = new AppearanceDialog(label, this, meters);
	pDialog->DoModal(getHWND());
	delete pDialog;
	meters->Release();
	label->Release();

	return S_OK;
}

STDMETHODIMP SysDocklet::configureSize()
{
	SizeDialog *pDialog = new SizeDialog(model, this);
	pDialog->DoModal(getHWND());
	delete pDialog;

	return S_OK;
}

STDMETHODIMP SysDocklet::loadConfigFile()
{
	char configFile[MAX_PATH];
	BSTR bCF;
	model->get_ConfigFile(&bCF);
	strcpy(configFile, _bstr_t(bCF, false));
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
		load(configFile, NULL);
	}

	return S_OK;
}

STDMETHODIMP SysDocklet::showHelp()
{
	_bstr_t helpFile_t("system\\help.html");
	BSTR helpFile;
	appConfig->CanonicalPath(helpFile_t, &helpFile);

	ShellExecute(getHWND(),
			"open",
			_bstr_t(helpFile, false),
			"",
			"",
			SW_SHOWNORMAL);

	return S_OK;
}

STDMETHODIMP SysDocklet::Save()
{
	model->Save(true);

	return S_OK;
}

STDMETHODIMP SysDocklet::SaveAs()
{
	model->SaveAs();

	return S_OK;
}

static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

STDMETHODIMP SysDocklet::Screenshot()
{
	Image *image = createOverlayImage();

	char cf[MAX_PATH] = { 0 };
	char id[MAX_PATH];
	BSTR bf;
	appConfig->CanonicalPath(L"images", &bf);
	strncpy(id, _bstr_t(bf, false), sizeof(id));
	OPENFILENAME of;
	memset(&of,0,sizeof(of));   
    of.lStructSize=sizeof(of);
	of.lpstrFile = cf;
	of.lpstrInitialDir = id;
	of.nMaxFile = sizeof(cf);
	of.lpstrTitle = "Screenshot: select an image file...";
	of.lpstrFilter = "PNG\0*.png\0\0\0";
	of.lpstrDefExt = "png";
	of.nFilterIndex = 1;
	of.Flags = OFN_CREATEPROMPT | OFN_NOREADONLYRETURN | OFN_HIDEREADONLY;

	if (GetSaveFileName(&of))
	{
		CLSID pngClsid;
		GetEncoderClsid(L"image/png", &pngClsid);
		image->Save(_bstr_t(cf), &pngClsid, NULL);
	}

	delete image;

	return S_OK;
}

/*
 * Called from CnotifyObserver()
 */
STDMETHODIMP SysDocklet::HandleEvent()
{
	AtlTrace("HandleEvent\n");

	ICOMTimerService *ts = NULL;
	model->get_TimerService(&ts);
	if (ts)
	{
		ts->UpdateTimerClients();
		ts->Release();
	}

	updateMeters();
	DockletSetLabel(getHWND(), getLabelText());
	DockletSetImageFile(getHWND(), getImageText());
	DockletSetImageOverlay(getHWND(), createOverlayImage());

	return S_OK;
}

STDMETHODIMP SysDocklet::TimerFired()
{
	AtlTrace("Timer Fired\n");
	MSG msg;
	if (!::PeekMessage(&msg, getHWND(), WM_USER+144, WM_USER+144, PM_REMOVE | PM_QS_POSTMESSAGE))
	{
//		timerService.RemoveTimerClient(this);
//		timerSet = false;
		updateOverlays();
	}

	return S_OK;
}

//-------------------- protected stuff --------------------------//
void SysDocklet::updateMeters()
{
	model->UpdateMeters();
}

void SysDocklet::updateOverlays()
{
	if (model && visible)
	{
		long dirty;
		model->get_dirty(&dirty);
		if (dirty)
		{
			DockletSetLabel(getHWND(), getLabelText());
			DockletSetImageFile(getHWND(), getImageText());
			DockletSetImageOverlay(getHWND(), createOverlayImage());
			model->put_dirty(false);
		}
	}
}

char *SysDocklet::getLabelText()
{
	ILabel *label;
	model->get_Label(&label);
	BSTR s = L"";
	label->GetLabel(&s);
	strncpy(labelText, _bstr_t(s, false), MAX_PATH);
	label->Release();

	return labelText;
}

char *SysDocklet::getImageText()
{
	static char imagePathBlank[MAX_PATH];
	static char imagePath[MAX_PATH];
	static int called = 0;

	if (!called)
	{
		called = 1;

		_bstr_t tName("images\\transparent.png");
		BSTR ncf;

		appConfig->CanonicalPath(tName, &ncf);
		strcpy(imagePathBlank, _bstr_t(ncf, false));
		AtlTrace("ImagePathBlank=%s\n", imagePathBlank);

		tName = "images\\NoOverlays.png";
		appConfig->CanonicalPath(tName, &ncf);
		strcpy(imagePath, _bstr_t(ncf, false));
		AtlTrace("ImagePath=%s\n", imagePath);
	}

	char *path = imagePath;
	IOverlayHome *overlays;
	model->get_Overlays(&overlays);
	short count = 0;
	overlays->Count(&count);
	if (count > 0)
	{
		path = imagePathBlank;

		if (isDesklet)
		{
			// Extra code for AveDesk so that our bottom-most layer gets a shadow rather
			// than the transparent one.
			BOOL success(FALSE);
			UINT layerId ( DeskletLayerGetLayer(getHWND(),"Default.Background",&success) );
			if(success)
			   DeskletLayerSetVisibility(getHWND(),layerId,FALSE,FALSE);
		}
	}

	overlays->Release();

	return path;
}

Image *SysDocklet::createOverlayImage()
{
	model->put_dirty(false);

	long width, height;
	model->get_Width(&width);
	model->get_Height(&height);

	HDCImage hdc(0, width, height);
	model->UpdateOverlays((long)((HDC)hdc));
    
	// Code, mucho thanks to Ave.
	Bitmap* returnBitmap = new Bitmap(width, height);

	Rect rc(0,0, returnBitmap->GetWidth(), returnBitmap->GetHeight() );
	BitmapData bmpData;
	returnBitmap->LockBits(&rc,ImageLockModeWrite,PixelFormat32bppPARGB,&bmpData);

	HBITMAP curhBmp = (HBITMAP)GetCurrentObject(hdc,OBJ_BITMAP);
	GetBitmapBits(curhBmp,returnBitmap->GetWidth() * returnBitmap->GetHeight() *4,bmpData.Scan0);

	returnBitmap->UnlockBits(&bmpData);

	return returnBitmap;
}

BOOL SysDocklet::visibilityNotify(BOOL becomesVisible, DWORD flags)
{
	this->visible = becomesVisible;
	if (model)
		model->SetActive(becomesVisible);

	updateOverlays();

	return true;
}

BOOL SysDocklet::getModel(IDispatch **pVal)
{
	*pVal = model;
	model->AddRef();

	return model != NULL;
}

BOOL SysDocklet::setSkin(CSkinEnumerator* se, const SkinInfo* info)
{
	const CSkinValue& value = se->getValue(info->wKey);

	load(value.filename, NULL);

	return true;
}
