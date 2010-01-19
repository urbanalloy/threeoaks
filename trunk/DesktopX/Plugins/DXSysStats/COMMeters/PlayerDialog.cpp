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

// Player.cpp : Implementation of PlayerDialog.cpp
#include "stdafx.h"
#include <CommCtrl.h>
#include "COMMeters.h"
#include "Player.h"
#include "PlayerDialog.h"

PlayerDialog::PlayerDialog(CPlayer *pMeter, WORD idd) :
	pMeter(pMeter),
	Dialog(idd)
{
}

PlayerDialog::~PlayerDialog()
{
}

LRESULT PlayerDialog::loop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT state = SendDlgItemMessage(wID, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	pMeter->put_Loop(state == BST_CHECKED);

	return 1;
}

LRESULT PlayerDialog::autostart(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT state = SendDlgItemMessage(wID, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	pMeter->put_AutoStart(state == BST_CHECKED);

	return 1;
}

LRESULT PlayerDialog::choosePath(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	OPENFILENAME of;
    memset(&of,0,sizeof(OPENFILENAME));   
    of.lStructSize=sizeof(OPENFILENAME);
	char _path[MAX_PATH];
	*_path = 0;
	of.lpstrFile = _path;
	of.nMaxFile = sizeof(_path);
	of.lpstrTitle = "Choose file...";
	of.lpstrFilter = "MP3\0*.mp3\0WAV\0*.wav\0AIF\0*.aif\0AIFF\0*.aiff\0AIFC\0*.aifc\0AU\0*.au\0M3U\0*.m3u\0SND\0*.snd\0WMA\0*.wma\0MOV\0.mov\0All\0*.*\0\0\0";
	of.lpstrDefExt = "*.mp3;*.wav;*.aif;*.aifc;*.au;*.m3u;*.snd;*.wma;*.mov";
	of.nFilterIndex = 1;
	of.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	of.hwndOwner = m_hWnd;

	if (GetOpenFileName(&of))
	{
		SetDlgItemText(IDC_PLAYER_PATH, of.lpstrFile);
	}

	return 1;
}

LRESULT PlayerDialog::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char buf[MAX_PATH];

	::GetDlgItemText(m_hWnd, IDC_PLAYER_PATH, buf, sizeof(buf));
	IClientModel *model;
	pMeter->get_Model(&model);
	IAppConfig *appConfig;
	model->get_AppConfig(&appConfig);
	BSTR path = 0;
	appConfig->NormalizePath(_bstr_t(buf), &path);
	pMeter->put_Path(path);
	::SysFreeString(path);
	appConfig->Release();
	model->Release();

    EndDialog(wID);
	return 1;
}

LRESULT PlayerDialog::OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT res = SendMessage((HWND)lParam, (UINT)TBM_GETPOS, 0, 0);
	if (GetDlgItem(IDC_SLIDER_VOLUME) == (HWND)lParam)
	{
		SetDlgItemInt(IDC_STATIC_VOLUME, (UINT)res, TRUE);
		pMeter->put_Volume(res);
	}

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 * Should initialize all of the controls using values from the meter.
 */
LRESULT PlayerDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;	// Make CWindow go through all the message maps.

	short flag = false;
	pMeter->get_Loop(&flag);
	SendDlgItemMessage(IDC_PLAYER_LOOP, BM_SETCHECK, flag ? BST_CHECKED : BST_UNCHECKED, 0);

	pMeter->get_AutoStart(&flag);
	SendDlgItemMessage(IDC_PLAYER_AUTOSTART, BM_SETCHECK, flag ? BST_CHECKED : BST_UNCHECKED, 0);

	BSTR owned = 0;
	pMeter->get_Path(&owned);
	SetDlgItemText(IDC_PLAYER_PATH, _bstr_t(owned, false));

	long l = 0;
	pMeter->get_Volume(&l);
	SendDlgItemMessage(IDC_SLIDER_VOLUME, TBM_SETRANGE, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) MAKELONG(0, 1000));  // min. & max. positions
	
	SendDlgItemMessage(IDC_SLIDER_VOLUME, TBM_SETPOS, 
        (WPARAM) TRUE,                   // redraw flag 
        (LPARAM) l);
	
	SetDlgItemInt(IDC_STATIC_VOLUME, (UINT)l, TRUE);
	
	return 1;
}
