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

// Shortcut.cpp : Implementation of CShortcut
#include "stdafx.h"

#include "SysStatsUtils.h"
#include "COMControllers.h"
#include "Shortcut.h"
#include "ShortcutDialog.h"
#include "ControllerImpl.cpp"

/////////////////////////////////////////////////////////////////////////////
// CShortcut

CShortcut::CShortcut() : args(""), selector(""), startIn(""), shortcut("")
{
	messageType = WM_LBUTTONDOWN;
}

STDMETHODIMP CShortcut::HandleMessage(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled)
{
	*handled = !passThrough;
	IMeter *meter = 0;
	get_meter(&meter);

	if (shortcut != _bstr_t("")) 
	{
		BSTR text;

		if (!meter)
		{
			text = shortcut.copy();
		}
		else
		{
			_bstr_t tmp("");
			BSTR tCopy = tmp.copy();
			text = tCopy;
			meter->GetAsString(shortcut, selector, &text);
			if (text != tCopy)
				::SysFreeString(tCopy);
		}

		bool itWorked = ((int)ShellExecute(0,
				"open",
				_bstr_t(text),
				args,
				startIn,
				SW_SHOWNORMAL) > 32);

		::SysFreeString(text);
	}

	return S_OK;
}

STDMETHODIMP CShortcut::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	ShortcutDialog *pDialog = new ShortcutDialog(this, observer, meters);
	pDialog->DoModal((HWND)hDlg);
	delete pDialog;

	return S_OK;
}

STDMETHODIMP CShortcut::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_Shortcut, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}


STDMETHODIMP CShortcut::get_Args(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = args.copy();
		
	return S_OK;
}

STDMETHODIMP CShortcut::put_Args(BSTR newVal)
{
	args = newVal;

	return S_OK;
}

STDMETHODIMP CShortcut::get_StartIn(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = startIn.copy();

	return S_OK;
}

STDMETHODIMP CShortcut::put_StartIn(BSTR newVal)
{
	startIn = newVal;

	return S_OK;
}

STDMETHODIMP CShortcut::get_Selector(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = selector.copy();

	return S_OK;
}

STDMETHODIMP CShortcut::put_Selector(BSTR newVal)
{
	selector = newVal;

	return S_OK;
}

STDMETHODIMP CShortcut::get_Shortcut(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = shortcut.copy();

	return S_OK;
}

STDMETHODIMP CShortcut::put_Shortcut(BSTR newVal)
{
	shortcut = newVal;

	return S_OK;
}
