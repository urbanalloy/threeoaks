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

#if defined(_DEBUG)
#define AFX_CDECL __cdecl


// Memory tracking allocation
void* AFX_CDECL operator new(size_t nSize, const char* lpszFileName, int nLine);
#define DEBUG_NEW new(THIS_FILE, __LINE__)
#if _MSC_VER >= 1200
void AFX_CDECL operator delete(void* p, const char* lpszFileName, int nLine);
#endif

#endif // _DEBUG


#include <stdio.h>
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include "resource.h"

#include "Dialog.h"

Dialog::Dialog(WORD idd) :
	CommonDialog<>(idd), inited(FALSE)
{
}

Dialog::~Dialog()
{
}

LRESULT Dialog::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    EndDialog(wID);

	return 1;
}

LRESULT Dialog::OnCMD(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Don't let any command messages through until the dialog has been initialized
	bHandled = !(inited && IsWindowVisible());

	return 1;
}

LRESULT Dialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    EndDialog((WORD)uMsg);

	return 1;
}

LRESULT Dialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	inited = TRUE;
	bHandled = FALSE;

	return 1;
}