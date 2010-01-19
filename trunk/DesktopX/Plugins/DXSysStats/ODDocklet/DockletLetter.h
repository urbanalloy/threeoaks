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

// DockletLetter.h: interface for the DockletLetter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOCKLETLETTER_H__5758707B_6228_4021_A257_805F32FAB6AA__INCLUDED_)
#define AFX_DOCKLETLETTER_H__5758707B_6228_4021_A257_805F32FAB6AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma once

#include <windows.h>
#include <GdiplusH.h>

struct SkinInfo;

// return constants for OnGetSkinW
const AVEDESK_NO_MORE_SKINS	= FALSE;
const AVEDESK_MORE_SKINS	= TRUE;
const AVEDESK_BUF_TOO_SMALL	= -1;

//********* End of AD1.2 stuff **********//

class Docklet;
class CSkinEnumerator;

/*
 * The Docklet letter base class.
 */
class DockletLetter
{
public:
	//--------- Constructors -------------//
	DockletLetter(HWND hwndDocklet, HINSTANCE hInstance);

	// Destructor
	virtual ~DockletLetter();

	//--------- Exported static methods - should override -------------//
	static Docklet* CALLBACK create(HWND hwndDocklet, HINSTANCE hInstance, char *szIni, char *szIniGroup);
	static void CALLBACK getInformation(char *szName, char *szAuthor, int *iVersion, char *szNotes);

	//--------- AveDesk 1.2 exported static methods - should override ----------//
    static CSkinEnumerator* CALLBACK createSkinEnumerator(HINSTANCE hInstance, const WCHAR* deskletPath);
	static void CALLBACK destroySkinEnumerator(CSkinEnumerator* se);
	static INT CALLBACK getSkin(CSkinEnumerator* se, SkinInfo* info);
	static Docklet* CALLBACK createFromSkin(HWND hwnd, HINSTANCE hInstance, CSkinEnumerator* se, const SkinInfo* info);

	//--------- Exported virtual methods - override for non-default behavior -----//
	virtual void save(char *szIni, char *szIniGroup, BOOL bIsForExport);
	virtual BOOL exportFiles(char *szFileRelativeOut, int iteration);
	virtual void processMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void configure();
	virtual BOOL leftButtonClick(POINT *ptCursor, SIZE *sizeDocklet);
	virtual BOOL leftButtonHeld(POINT *ptCursor, SIZE *sizeDocklet);
	virtual BOOL rightButtonClick(POINT *ptCursor, SIZE *sizeDocklet);
	virtual BOOL doubleClick(POINT *ptCursor, SIZE *sizeDocklet);
	virtual void handleEvent();

	//--------- AveDesk 1.2 xported virtual methods - override for non-default behavior -----//
	virtual LRESULT processMessageEx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL *bHandled);
	virtual BOOL getModel(IDispatch **pVal);
	virtual BOOL visibilityNotify(BOOL becomesVisible, DWORD flags);
	virtual BOOL setSkin(CSkinEnumerator* se, const SkinInfo* info);

protected:
	//--------- Accessors ----------//
	HWND getHWND();
	HINSTANCE getHInstance();

private:
	//------------------------- Data Members -------------------------//
	HWND hwndDocklet;
	HINSTANCE hInstanceDll;
};


#endif // !defined(AFX_DOCKLETLETTER_H__5758707B_6228_4021_A257_805F32FAB6AA__INCLUDED_)
