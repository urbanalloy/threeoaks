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

#include "stdafx.h"

#include <stdio.h>
#include <shlwapi.h>
#include <shlobj.h>
#include "resource.h"

#include "ArrayPtr.h"
#include "ArrayPtr.cpp"

#include "Shortcut.h"
#include "ShortcutDialog.h"
#include "PickList.cpp"

extern void ToValL(long *out, char *s);
extern void ToStrL(char *buf, int maxlen, long val);

template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}


ShortcutDialog::ShortcutDialog(IShortcut *pController, IObserver *observer, IMeterHome *meters, WORD idd) :
	pController(pController),
	meters(meters),
	Dialog(idd),
	meterList(Identity<long>, ToStrL, ToValL, /* fixed */ true)
{
}

ShortcutDialog::~ShortcutDialog()
{
	shortcuts.clearAndDestroy();
}

LRESULT ShortcutDialog::setShortcut(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DWORD value;
	LRESULT res = SendDlgItemMessage(wID,CB_GETCURSEL,0,0);
	if (res != CB_ERR)
	{
		value = SendDlgItemMessage(wID,CB_GETITEMDATA,(WPARAM)res,0);
		pController->put_Shortcut(_bstr_t(shortcuts[value]));
	}
	else
	{
		char buf[MAX_PATH];
		*buf = 0;
		GetDlgItemText(wID, buf, MAX_PATH);
		pController->put_Shortcut(_bstr_t(buf));
	}

	return 1;
}

LRESULT ShortcutDialog::args(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char buf[MAX_PATH];
	*buf = 0;
	GetDlgItemText(IDC_COMMANDLINE, buf, MAX_PATH);
	pController->put_Args(_bstr_t(buf));

	return 1;
}

LRESULT ShortcutDialog::chooseShortcut(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	OPENFILENAME of;
    memset(&of,0,sizeof(OPENFILENAME));   
    of.lStructSize=sizeof(OPENFILENAME);
	char app[MAX_PATH];
	*app = 0;
	of.lpstrFile = app;
	of.nMaxFile = sizeof(app);
	of.lpstrTitle = "Choose application to run";
	of.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	of.hwndOwner = m_hWnd;

	if (GetOpenFileName(&of))
	{
		SetDlgItemText(IDC_COMBO_APP, of.lpstrFile);
		pController->put_Shortcut(_bstr_t(of.lpstrFile));
	}

	return 1;
}

LRESULT ShortcutDialog::chooseStartIn(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BROWSEINFO bi = { 0 };
    bi.lpszTitle = "Start Directory";
	bi.hwndOwner = m_hWnd;
    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
    if ( pidl != 0 )
    {
        // get the name of the folder
        TCHAR path[MAX_PATH];
        SHGetPathFromIDList ( pidl, path );
		SetDlgItemText(IDC_DIRECTORY, path);

        // free memory used
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free ( pidl );
            imalloc->Release ( );
        }
	}

	return 1;
}

LRESULT ShortcutDialog::setMeter(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val = -1;
	meterList.getData(&val, -1 /* default value */);
	pController->put_MeterIndex(val);

	return 1;
}

LRESULT ShortcutDialog::selector(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[MAX_PATH];
	*text = 0;

	GetDlgItemText(wID, text, sizeof(text));
	pController->put_Selector(_bstr_t(text));

	return 1;
}

LRESULT ShortcutDialog::startIn(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[MAX_PATH];
	*text = 0;

	GetDlgItemText(wID, text, sizeof(text));
	pController->put_StartIn(_bstr_t(text));

	return 1;
}


/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT ShortcutDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BSTR s;
	pController->get_Shortcut(&s);
	SetDlgItemText(IDC_COMBO_APP, _bstr_t(s, false));

	IClientModel *pModel;
	meters->get_Model(&pModel);
	IAppConfig *pAppConfig;
	pModel->get_AppConfig(&pAppConfig);

	_bstr_t shortcutName("system\\shortcuts.txt");
	BSTR configFile;
	pAppConfig->CanonicalPath(shortcutName, &configFile);

	FILE *shortcutFile = _wfopen(configFile, L"r");
	::SysFreeString(configFile);
	if (shortcutFile)
	{
		shortcuts.clearAndDestroy();
		char key[MAX_PATH];
		char value[MAX_PATH];
		*key=0;
		*value=0;
		while (fscanf(shortcutFile, "%[^=]=%[^\n]\n", key, value) == 2)
		{
			// Need to hold onto the values so we can delete them later.
			char *realValue = new char[strlen(value)+1];
			strcpy(realValue, value);
			int i = shortcuts.add(realValue);
			int index = SendDlgItemMessage(IDC_COMBO_APP,CB_ADDSTRING,0,(LPARAM)key);
			SendDlgItemMessage(IDC_COMBO_APP,CB_SETITEMDATA,index,(LPARAM)i);

			BSTR shortcut;
			pController->get_Shortcut(&shortcut);
			_bstr_t bShortcut(shortcut, false);
			_bstr_t bRealValue = realValue;
			if (bRealValue == bShortcut)
				SendDlgItemMessage(IDC_COMBO_APP,CB_SETCURSEL,(WPARAM)index,0);
		}

		fclose(shortcutFile);
	}

	pController->get_Selector(&s);
	SetDlgItemText(IDC_SELECTOR, _bstr_t(s, false));

	long current = -1;
	pController->get_MeterIndex(&current);

	meterList.init(m_hWnd, IDC_COMBO_METER, meters, current);

	pController->get_Args(&s);
	SetDlgItemText(IDC_COMMANDLINE, _bstr_t(s, false));

	pController->get_StartIn(&s);
	SetDlgItemText(IDC_DIRECTORY, _bstr_t(s, false));

	pAppConfig->Release();
	pModel->Release();

	bHandled = FALSE;

	return TRUE;
}
