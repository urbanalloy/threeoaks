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

// ConfigureDialog.h: interface for the CConfigureDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGUREDIALOG_H__8716797A_8082_4428_B55F_34D213AA8373__INCLUDED_)
#define AFX_CONFIGUREDIALOG_H__8716797A_8082_4428_B55F_34D213AA8373__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CSubject.h"
#include "Dialog.h"
#include "ModelPanel.h"
#include "CMeterDialog.h"
#include "COverlayPanel.h"
#include "ControllerPanel.h"

class CConfigureDialog :
	public CSubject,
	public Dialog
{
public:
	CConfigureDialog(IClientModel *model, IObserver *observer, WORD idd=IDD_CONFIGURE);
	virtual ~CConfigureDialog();

protected:
	CModelPanel modelPanel;
	COverlayPanel overlayPanel;
	CMeterDialog meterPanel;
	CControllerPanel controllerPanel;

	Dialog **dialogs;
	int numTabs;

	IClientModel *model;

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSelchangeTab(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	virtual LRESULT OnHelp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void ShowChild(int number);

public:
	BEGIN_MSG_MAP(CConfigureDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(Dialog)
		COMMAND_HANDLER(IDC_BUTTON_HELP, BN_CLICKED, OnHelp)
		NOTIFY_HANDLER(IDC_TAB, TCN_SELCHANGE, OnSelchangeTab)
	END_MSG_MAP()
};

#endif // !defined(AFX_CONFIGUREDIALOG_H__8716797A_8082_4428_B55F_34D213AA8373__INCLUDED_)
