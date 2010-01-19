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

// Menu.cpp: implementation of the Menu class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_DEBUG)
#define AFX_CDECL __cdecl


// Memory tracking allocation
void* AFX_CDECL operator new(size_t nSize, const char* lpszFileName, int nLine);
#define DEBUG_NEW new(THIS_FILE, __LINE__)
#if _MSC_VER >= 1200
void AFX_CDECL operator delete(void* p, const char* lpszFileName, int nLine);
#endif

#endif // _DEBUG

#include "Menu.h"

int Menu::ids = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Menu::Menu(const char *_name, HWND owner) : menuHandle(0), owner(owner)
{
	strncpy(name, _name, sizeof(name));
}

Menu::~Menu()
{
	DestroyMenu(menuHandle);
	actions.clearAndDestroy();
}

int Menu::addAction(MenuAction *action)
{
	int id = ++ids;
	MENUITEMINFO miInfo;

	miInfo.cbSize = sizeof(miInfo);
	miInfo.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_ID | MIIM_STRING;
	miInfo.wID = id;
	miInfo.dwItemData = (ULONG_PTR)action;
	miInfo.fType = MFT_STRING;
	miInfo.dwTypeData = (char*)action->getName();

	InsertMenuItem(getMenuHandle(), GetMenuItemCount(getMenuHandle()), TRUE, &miInfo);

	action->setId(id);
	actions.add(action);

	return id;
}

void Menu::removeSubMenu(Menu *menu)
{
	RemoveMenu(getMenuHandle(), menu->getId(), MF_BYCOMMAND);
}

int Menu::addSubMenu(Menu *menu)
{
	int id = ++ids;
	MENUITEMINFO miInfo;

	miInfo.cbSize = sizeof(miInfo);
	miInfo.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
	miInfo.wID = id;
	miInfo.dwItemData = (ULONG_PTR)menu;
	miInfo.hSubMenu = menu->getMenuHandle();
	miInfo.fType = MFT_STRING;
	miInfo.dwTypeData = (char*)menu->getName();

	InsertMenuItem(getMenuHandle(), GetMenuItemCount(getMenuHandle()), TRUE, &miInfo);
	menu->setId(id);

	return id;
}

void Menu::action()
{
	POINT ptMenu;
	GetCursorPos(&ptMenu);
	TrackPopupMenu(getMenuHandle(), TPM_LEFTALIGN | TPM_TOPALIGN, ptMenu.x, ptMenu.y, 0, owner, NULL);
}

void Menu::action(int id)
{
	MENUITEMINFO miInfo;

	miInfo.cbSize = sizeof(miInfo);
	miInfo.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_ID | MIIM_SUBMENU;

	if (GetMenuItemInfo(getMenuHandle(), id, FALSE, &miInfo) && miInfo.fType == MFT_STRING)
	{
		MenuAction *action = (MenuAction*)miInfo.dwItemData;
		if (action)
			action->action();
	}
}

const char* Menu::getName() const
{
	return name;
}

HMENU Menu::getMenuHandle()
{
	if (menuHandle == NULL)
		menuHandle = CreatePopupMenu();

	return menuHandle;
}
