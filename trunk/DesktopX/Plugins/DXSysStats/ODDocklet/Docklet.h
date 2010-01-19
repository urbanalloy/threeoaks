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

// Docklet.h: interface for the Docklet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOCKLET_H__D7BB6BF5_CCBC_4FCF_B30C_9CA13EC0221F__INCLUDED_)
#define AFX_DOCKLET_H__D7BB6BF5_CCBC_4FCF_B30C_9CA13EC0221F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DockletLetter.h"

/*
 * Docklet class.
 */
class Docklet
{
public:
	Docklet(DockletLetter *docklet);
	~Docklet();

	//--------- Exported virtual methods - override for non-default behavior -----//
	void CALLBACK save(char *szIni, char *szIniGroup, BOOL bIsForExport);
	BOOL CALLBACK exportFiles(char *szFileRelativeOut, int iteration);
	void CALLBACK destroy(HWND hwndDocklet);
	void CALLBACK processMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void CALLBACK configure();
	BOOL CALLBACK leftButtonClick(POINT *ptCursor, SIZE *sizeDocklet);
	BOOL CALLBACK leftButtonHeld(POINT *ptCursor, SIZE *sizeDocklet);
	BOOL CALLBACK rightButtonClick(POINT *ptCursor, SIZE *sizeDocklet);
	BOOL CALLBACK doubleClick(POINT *ptCursor, SIZE *sizeDocklet);

	// AD 1.2 extras
	LRESULT CALLBACK processMessageEx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL *bHandled);
	BOOL CALLBACK getModel(IDispatch **pVal);
	BOOL CALLBACK visibilityNotify(BOOL becomesVisible, DWORD flags);
	BOOL CALLBACK setSkin(CSkinEnumerator* se, const SkinInfo* info);

private:
	DockletLetter *docklet;
};

#endif // !defined(AFX_DOCKLET_H__D7BB6BF5_CCBC_4FCF_B30C_9CA13EC0221F__INCLUDED_)
