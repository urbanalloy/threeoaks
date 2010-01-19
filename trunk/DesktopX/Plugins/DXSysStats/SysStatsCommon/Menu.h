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

// Menu.h: interface for the Menu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENU_H__D34DCA3F_4138_408D_A629_156A325ADBA7__INCLUDED_)
#define AFX_MENU_H__D34DCA3F_4138_408D_A629_156A325ADBA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

#include "MenuAction.h"
#include "ArrayPtr.cpp"

class Menu : public MenuAction
{
public:
	Menu(const char *name, HWND owner);
	virtual ~Menu();

	virtual int addAction(MenuAction *action);
	virtual int addSubMenu(Menu *menu);
	virtual void removeSubMenu(Menu *menu);
	virtual void action();			// popup the menu
	virtual void action(int id);	// execute an item
	virtual const char* getName() const;
	virtual HMENU getMenuHandle();

private:
	HWND owner;
	HMENU menuHandle;
	char name[80];
	ArrayPtr<MenuAction> actions;

	static int ids;
};

#endif // !defined(AFX_MENU_H__D34DCA3F_4138_408D_A629_156A325ADBA7__INCLUDED_)
