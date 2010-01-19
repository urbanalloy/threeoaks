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

// Docklet.cpp: implementation of the Docklet class.
//
//////////////////////////////////////////////////////////////////////

#include "Docklet.h"
//#include "DockletSDK.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
 * Constructor. Called from create().
 */
Docklet::Docklet(DockletLetter *docklet)
{
	this->docklet = docklet;
}

/*
 * Destructor. Called from destroy().
 */
Docklet::~Docklet()
{
	delete docklet;
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
void CALLBACK Docklet::save(char *szIni, char *szIniGroup, BOOL bIsForExport)
{
	docklet->save(szIni, szIniGroup, bIsForExport);
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
BOOL CALLBACK Docklet::exportFiles(char *szFileRelativeOut, int iteration)
{
	return docklet->exportFiles(szFileRelativeOut, iteration);
}

/*
 * On destruction, stop all timers, kill all threads left running and
 * free the memory allocated for the plugin's personal data.
 */
void CALLBACK Docklet::destroy(HWND hwndDocklet)
{
	delete this;	// Eeeek
}

/*
 * Each icon in the dock is its own window. The OnProcessMessage function
 * recieves every message that gets sent to that window (the HWND passed on OnCreate)
 */
void CALLBACK Docklet::processMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (this)
		docklet->processMessage(hwnd, uMsg, wParam, lParam);
}

/*
 * The OnProcessMessageEx function is called by AveDesk1.2.
 * recieves every message that gets sent to that window (the HWND passed on OnCreate)
 */
LRESULT CALLBACK Docklet::processMessageEx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL *bHandled)
{
	if (this)
		return docklet->processMessageEx(hwnd, uMsg, wParam, lParam, bHandled);
	else
		return 0;
}

/*
 * Called the dock item is called to be configured by ObjectDock, such as would
 * happen if our plugin did not impliment OnRightMouseClick which would result
 * in ObjectDock displaying a menu on right click with a menu option to configure.
 */
void CALLBACK Docklet::configure()
{
	docklet->configure();
}

/* 
 * Process a left mouse button click. Returning TRUE reports to ObjectDock that
 * the event was acted upon. In the case of the weather plugin, if no zipcode
 * has yet been set on a left click, open the Configure dialog.
 * otherwise, open that weather page.
 */
BOOL CALLBACK Docklet::leftButtonClick(POINT *ptCursor, SIZE *sizeDocklet)
{
	return docklet->leftButtonClick(ptCursor, sizeDocklet);
}

/*
 * Process whent he left mouse button has been held. Nothing here yet...
 * Returning TRUE reports to ObjectDock that the event was acted upon.
 */
BOOL CALLBACK Docklet::leftButtonHeld(POINT *ptCursor, SIZE *sizeDocklet)
{
	return docklet->leftButtonHeld(ptCursor, sizeDocklet);
}

/*
 * By implimenting the OnRightButtonClick function, ObjectDock will not
 * automatically show a right click menu for the item, and thus we must
 * create one. We want one with an Update and a Change location (configure).
 */
BOOL CALLBACK Docklet::rightButtonClick(POINT *ptCursor, SIZE *sizeDocklet)
{
	return docklet->rightButtonClick(ptCursor, sizeDocklet);
}

/*
 * Handle a double-click.
 */
BOOL CALLBACK Docklet::doubleClick(POINT *ptCursor, SIZE *sizeDocklet)
{
	return docklet->doubleClick(ptCursor, sizeDocklet);
}

/*
 * AD 1.2 get docklet model
 */
BOOL CALLBACK Docklet::getModel(IDispatch **pVal)
{
	return docklet->getModel(pVal);
}

/*
 * AD 1.2 visibility notification
 */
BOOL CALLBACK Docklet::visibilityNotify(BOOL becomesVisible, DWORD flags)
{
	return docklet->visibilityNotify(becomesVisible, flags);
}

/*
 * AD 1.2 set skin
 */
BOOL CALLBACK Docklet::setSkin(CSkinEnumerator* se, const SkinInfo* info)
{
	return docklet->setSkin(se, info);
}
