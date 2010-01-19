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

// ScriptController.cpp : Implementation of ScriptControllerDialog.cpp
#include "stdafx.h"
#include "COMControllers.h"
#include "ScriptController.h"
#include "ScriptControllerDialog.h"

static const char *typeExtensions[] =
{
	"js",
	"pl",
	"php",
	"py",
	"rb",
	"vbs",
	NULL
};

static const char *typeLabels[] =
{
	"Java Script",
	"Perl",
	"PHP",
	"Python",
	"Ruby",
	"VB Script",
	NULL
};

static char* typeData[] =
{
	"JavaScript",
	"Perl",
	"ActivePHP",
	"Python",
	"RubyScript",
	"vbScript"
};

ScriptControllerDialog::ScriptControllerDialog(CScriptController *pController, IObserver *observer, IMeterHome *meters, WORD idd) :
	pController(pController), meters(meters), Dialog(idd), CSubject(observer)
{
}

ScriptControllerDialog::~ScriptControllerDialog()
{
}

LRESULT ScriptControllerDialog::showConsole(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	pController->ShowConsole(true);

	return 1;
}

LRESULT ScriptControllerDialog::chooseScript(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	OPENFILENAME of;
    memset(&of,0,sizeof(OPENFILENAME));   
    of.lStructSize=sizeof(OPENFILENAME);
	char _path[MAX_PATH];
	*_path = 0;
	of.lpstrFile = _path;
	of.nMaxFile = sizeof(_path);
	of.lpstrTitle = "Choose script...";
	of.lpstrFilter = "All Scripts\0*.js;*.pl;*.php;*.py;*.rb;*.vbs\0Java Script\0*.js\0Perl\0*.pl\0PHP\0*.php\0Python\0*.py\0Ruby\0*.rb\0VB Script\0*.vbs\0All\0*.*\0\0\0";
	of.lpstrDefExt = "*.js;*.pl;*.php;*.py;*.rb;*.vbs";
	of.nFilterIndex = 1;
	of.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	of.hwndOwner = m_hWnd;

	if (GetOpenFileName(&of))
	{
		SetDlgItemText(IDC_SCRIPT_NAME, of.lpstrFile);
		char *pExt = of.lpstrFile + of.nFileExtension;
		for (int i=0; typeExtensions[i]; i++)
		{
			if (strcmp(pExt, typeExtensions[i]) == 0)
			{
				SendDlgItemMessage(IDC_COMBO_SCRIPT_TYPE,CB_SETCURSEL,(WPARAM)i,0);
			}
		}
	}

	return 1;
}

LRESULT ScriptControllerDialog::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    EndDialog(wID);
	return 1;
}

LRESULT ScriptControllerDialog::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char *type;
	char buf[MAX_PATH];

	LRESULT res = SendDlgItemMessage(IDC_COMBO_SCRIPT_TYPE,CB_GETCURSEL,0,0);
	if (res != CB_ERR)
	{
		type = (char*)SendDlgItemMessage(IDC_COMBO_SCRIPT_TYPE,CB_GETITEMDATA,(WPARAM)res,0);
	}
	else
	{
		GetDlgItemText(IDC_COMBO_SCRIPT_TYPE, buf, sizeof(buf));
		type = buf;
	}

	pController->put_ScriptType(_bstr_t(type));

	::GetDlgItemText(m_hWnd, IDC_SCRIPT_NAME, buf, sizeof(buf));
	IClientModel *model;
	pController->get_Model(&model);
	IAppConfig *appConfig;
	model->get_AppConfig(&appConfig);
	BSTR path;
	appConfig->NormalizePath(_bstr_t(buf), &path);
	pController->put_FileName(path);
	::SysFreeString(path);
	appConfig->Release();
	model->Release();

    EndDialog(wID);
	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT ScriptControllerDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BSTR owned;
	pController->get_FileName(&owned);
	SetDlgItemText(IDC_SCRIPT_NAME, _bstr_t(owned, false));

	pController->get_ScriptType(&owned);
	_bstr_t st(owned, false);

	for (int i=0; typeLabels[i]; i++)
	{
		int index = SendDlgItemMessage(IDC_COMBO_SCRIPT_TYPE,CB_ADDSTRING,0,(LPARAM)typeLabels[i]);
		SendDlgItemMessage(IDC_COMBO_SCRIPT_TYPE,CB_SETITEMDATA,index,(LPARAM)typeData[i]);
		if (!strcmp(typeData[i], st))
			SendDlgItemMessage(IDC_COMBO_SCRIPT_TYPE,CB_SETCURSEL,(WPARAM)index,0);
	}

	bHandled = FALSE;	// Make CWindow go through all the message maps.

	return 1;
}
