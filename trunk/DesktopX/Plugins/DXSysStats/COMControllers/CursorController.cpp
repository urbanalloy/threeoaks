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

// Cursor.cpp : Implementation of CCursorController
#include "stdafx.h"
#include "ControllerImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#include "SysStatsUtils.h"
#include "COMControllers.h"
#include "CursorController.h"
#include "CursorControllerDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CCursorController

STDMETHODIMP CCursorController::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_ICursorController
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/*
 * Constructor
 */
CCursorController::CCursorController() :
	cursorID((long)IDC_ARROW)
{
}

/*
 * This method is called when a windows message of the correct type has been posted.
 */
STDMETHODIMP CCursorController::HandleMessage(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled)
{
	*handled = true;

	HCURSOR hCursor = ::LoadCursor(NULL, (LPCTSTR)cursorID);
	::SetCursor(hCursor);

	// Provide implementation.
	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the controller should
 * be displayed so that the user can configure the controller to meet their
 * requirements.
 */
STDMETHODIMP CCursorController::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	CursorControllerDialog *d = new CursorControllerDialog(this, observer, meters);
	d->DoModal((HWND)hDlg);
	delete d;

	return S_OK;
}

/*
 * Return a string representation of the controller’s type.
 */
STDMETHODIMP CCursorController::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_CursorController, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CCursorController::get_CursorID(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = cursorID;

	return S_OK;
}

STDMETHODIMP CCursorController::put_CursorID(long newVal)
{
	cursorID = newVal;

	return S_OK;
}
