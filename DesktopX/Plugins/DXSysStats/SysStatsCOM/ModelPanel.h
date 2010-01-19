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

// ModelPanel.h: interface for the CModelPanel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELPANEL_H__7AE05973_9708_42B2_A605_92E9CA657BEA__INCLUDED_)
#define AFX_MODELPANEL_H__7AE05973_9708_42B2_A605_92E9CA657BEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlapp.h>
#include <atlmisc.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <atlctrlx.h>

#include "CSubject.h"
#include "Dialog.h"

struct IClientModel;
struct IObserver;

class CModelPanel :
	public CSubject,
	public Dialog
{
public:
	CModelPanel(IClientModel *model, IObserver *observer, WORD idd=IDD_MODEL);
	virtual ~CModelPanel();

protected:
	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT setName(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT setDescription(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT setVersion(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT setAuthor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT hScale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT vScale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	CHyperLink link;
	IClientModel *model;

public:
	BEGIN_MSG_MAP(CModelPanel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_HANDLER(IDC_MODELNAME, EN_CHANGE, setName)
		COMMAND_HANDLER(IDC_MODELDESC, EN_CHANGE, setDescription)
		COMMAND_HANDLER(IDC_MODELVERSION, EN_CHANGE, setVersion)
		COMMAND_HANDLER(IDC_MODELAUTHOR, EN_CHANGE, setAuthor)
		COMMAND_HANDLER(IDC_XSCALE, EN_CHANGE, hScale)
		COMMAND_HANDLER(IDC_YSCALE, EN_CHANGE, vScale)
		CHAIN_MSG_MAP(Dialog)
	END_MSG_MAP()
};

#endif // !defined(AFX_MODELPANEL_H__7AE05973_9708_42B2_A605_92E9CA657BEA__INCLUDED_)
