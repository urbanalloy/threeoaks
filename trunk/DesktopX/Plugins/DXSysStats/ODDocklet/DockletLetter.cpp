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

// DockletLetter.cpp: implementation of the DockletLetter class.
//
//////////////////////////////////////////////////////////////////////

#include "DockletLetter.h"
#include "Docklet.h"

extern void DockletDefaultConfigDialog(HWND hwndDocklet);

/*
 * Return basic information about the plugin.
 */
void CALLBACK DockletLetter::getInformation(char *szName, char *szAuthor, int *iVersion, char *szNotes)
{
	strcpy(szName, "Base Letter C++ Docklet");
	strcpy(szAuthor, "Paul Andrews");
	(*iVersion) = 100;
	strcpy(szNotes,   "Base docklet class");
}

/*
 * In the OnCreate, we will be passed the handle to the window which owns this docklet.
 * In ObjectDock, each icon/image is its own window, thus the window passed is the actual
 * window that the image is being displayed on. Thus, it is recommended
 * that you not move or otherwise manually change the appearance of this window.
 * However! You will need to record this window handle, as it is through that that
 * you will identify yourself to ObjectDock in callbacks.
 *
 * An hInstance is passed giving this DLL's instance, as well as a location of an ini
 * and the specific ini group to load from. If szIni and szIniGroup are NULL, this
 * plugin is being created from scratch, and default options should be set/loaded. 
 * If they are not null, the plugin should load its settings from the ini given
 * by szIni, from the ini group given by szIniGroup. See examples.
 */
Docklet* CALLBACK DockletLetter::create(HWND hwndDocklet, HINSTANCE hInstance, char *szIni, char *szIniGroup)
{
	DockletLetter *docklet = new DockletLetter(hwndDocklet, hInstance);
	return new Docklet(docklet);
}

//--------- AveDesk 1.2 exported static methods - should override ----------//
CSkinEnumerator* CALLBACK DockletLetter::createSkinEnumerator(HINSTANCE hInstance, const WCHAR* deskletPath)
{
	return NULL;
}

void CALLBACK DockletLetter::destroySkinEnumerator(CSkinEnumerator* se)
{
}

INT CALLBACK DockletLetter::getSkin(CSkinEnumerator* se, SkinInfo *info)
{
	return 0;
}

Docklet* CALLBACK DockletLetter::createFromSkin(HWND hwndDocklet, HINSTANCE hInstance, CSkinEnumerator* se, const SkinInfo *info)
{
	DockletLetter *docklet = new DockletLetter(hwndDocklet, hInstance);
	return new Docklet(docklet);
}

/*
 * Constructor. Called from create().
 */
DockletLetter::DockletLetter(HWND hwndDocklet, HINSTANCE hInstance) :
	hwndDocklet(hwndDocklet),
	hInstanceDll(hInstance)
{
}

/*
 * Destructor. Called from destroy().
 */
DockletLetter::~DockletLetter()
{
}

/*
 * When ObjectDock needs to save this plugin's individual properties, it will give
 * you the plugin's data along with an szIni (file) and szIniGroup (ini section)
 * to save the plugin's data to.
 *
 * REMEMBER! Multiple of your same plugin can be added to one dock at once,
 * so if you want each of your plugins to be able to have unique options,
 * follow the example given below.
 *
 * "bIsForExport" lets you know if this save is being done to be saved in a
 * package to distribute. While not important for most docklets, if for instance
 * you have a mail checking plugin you will not want to save any passwords/usernames
 * in the event that this flag is set to TRUE.
 */
void DockletLetter::save(char *szIni, char *szIniGroup, BOOL bIsForExport)
{
}

/*
 * ObjectDock has the capability of automatically packing up individual
 * items as well as entire docks to be easily distruted and automatically
 * installed with one click. If your plugin has files that it uses other
 * than its own DLL and the image it is currently displaying, which are
 * packed up automatically, you will want to tell object dock about any
 * files/folders you want to include in the event your docklet was to be
 * packed up. This includes other images your docklet might use.
 */
BOOL DockletLetter::exportFiles(char *szFileRelativeOut, int iteration)
{
	return FALSE;
}

/*
 * Each icon in the dock is its own window. The OnProcessMessage function
 * recieves every message that gets sent to that window (the HWND passed on OnCreate),
 * including any timer messages, which we will make use of here/
 */
void DockletLetter::processMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
}

/*
 * Each icon in the dock is its own window. The OnProcessMessage function
 * recieves every message that gets sent to that window (the HWND passed on OnCreate),
 * including any timer messages, which we will make use of here/
 */
LRESULT DockletLetter::processMessageEx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL *bHandled)
{
	return 0;
}

/*
 * Called the dock item is called to be configured by ObjectDock, such as would
 * happen if our plugin did not impliment OnRightMouseClick which would result
 * in ObjectDock displaying a menu on right click with a menu option to configure.
 */
void DockletLetter::configure()
{
	DockletDefaultConfigDialog(getHWND());
}

/* 
 * Process a left mouse button click. Returning TRUE reports to ObjectDock that
 * the event was acted upon. In the case of the weather plugin, if no zipcode
 * has yet been set on a left click, open the Configure dialog.
 * otherwise, open that weather page.
 */
BOOL DockletLetter::leftButtonClick(POINT *ptCursor, SIZE *sizeDocklet)
{
	return FALSE;
}

/*
 * Process whent he left mouse button has been held. Nothing here yet...
 * Returning TRUE reports to ObjectDock that the event was acted upon.
 */
BOOL DockletLetter::leftButtonHeld(POINT *ptCursor, SIZE *sizeDocklet)
{
	return FALSE;
}

/*
 * By implimenting the OnRightButtonClick function, ObjectDock will not
 * automatically show a right click menu for the item, and thus we must
 * create one. We want one with an Update and a Change location (configure).
 */
BOOL DockletLetter::rightButtonClick(POINT *ptCursor, SIZE *sizeDocklet)
{
	return FALSE;
}

/*
 */
BOOL DockletLetter::doubleClick(POINT *ptCursor, SIZE *sizeDocklet)
{
	return FALSE;
}

HWND DockletLetter::getHWND()
{
	return hwndDocklet;
}

HINSTANCE DockletLetter::getHInstance()
{
	return hInstanceDll;
}

void DockletLetter::handleEvent()
{
}

BOOL DockletLetter::visibilityNotify(BOOL becomesVisible, DWORD flags)
{
	return false;
}

BOOL DockletLetter::getModel(IDispatch **pVal)
{
	return false;
}

BOOL DockletLetter::setSkin(CSkinEnumerator* se, const SkinInfo* info)
{
	return false;
}

