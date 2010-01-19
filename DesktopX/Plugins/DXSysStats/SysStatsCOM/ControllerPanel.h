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

// ControllerPanel.h: interface for the CControllerPanel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLLERPANEL_H__7AE05973_9708_42B2_A605_92E9CA657BEA__INCLUDED_)
#define AFX_CONTROLLERPANEL_H__7AE05973_9708_42B2_A605_92E9CA657BEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CSubject.h"
#include "Dialog.h"
#include "TreeViewCombo.h"
#include "PickList.h"

struct IMeterHome;
struct IControllerHome;
struct IObserver;

class CControllerPanel :
	public CSubject,
	public Dialog
{
public:
	CControllerPanel(IClientModel *model, IObserver *observer, WORD idd=IDD_CONTROLLERHOMEPROPS);
	virtual ~CControllerPanel();

protected:
	void enable(int item);
	void disable();

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT setMessage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT setOverlay(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT setKey(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT setCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT setKeyKey(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT add(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT remove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT rename(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT moveUp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT moveDown(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT listSelChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT OnControllerType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	virtual LRESULT configure(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT passThrough(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	// Specific to this class
	virtual void resetList(int selected);
	virtual void resetOverlayList(const IOverlay *selected=NULL);

	HTREEITEM drawOverlayList(ICompositeOverlay *root, HTREEITEM parent, const IOverlay *selected, TVINSERTSTRUCT &itvIns);

	IClientModel *model;
	IMeterHome *meters;
	IControllerHome *controllers;

	TreeViewCombo overlaysCombo;
	PickList<WPARAM> keyList;
	PickList<UINT> messageList;
	PickList<UINT> commandList;

public:
	BEGIN_MSG_MAP(CControllerPanel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_HANDLER(IDC_CONTROLLER_TYPE, CBN_SELENDOK, OnControllerType)
		COMMAND_HANDLER(IDC_CONTROLLER_LIST, LBN_SELCHANGE, listSelChanged)
		COMMAND_HANDLER(IDC_EDIT_KEY, EN_CHANGE, setKeyKey)
		COMMAND_ID_HANDLER(IDC_COMBO_MESSAGE, setMessage)
		COMMAND_HANDLER(IDC_COMBO_KEY, CBN_SELENDOK, setKey)
		COMMAND_HANDLER(IDC_COMBO_COMMAND, CBN_SELENDOK, setCommand)
		COMMAND_HANDLER(IDC_COMBO_OVERLAY, CBN_SELENDOK, setOverlay)
		COMMAND_HANDLER(IDC_CONTROLLER_MOVEUP, BN_CLICKED, moveUp)
		COMMAND_HANDLER(IDC_CONTROLLER_MOVEDOWN, BN_CLICKED, moveDown)
		COMMAND_HANDLER(IDC_CONTROLLER_DELETE, BN_CLICKED, remove)
		COMMAND_HANDLER(IDC_CONTROLLER_RENAME, BN_CLICKED, rename)
		COMMAND_HANDLER(IDC_CONTROLLER_ADD, BN_CLICKED, add)
		COMMAND_HANDLER(IDC_CONTROLLER_CONFIGURE, BN_CLICKED, configure)
		COMMAND_HANDLER(IDC_CHECK_PASSTHROUGH, BN_CLICKED, passThrough)
		CHAIN_MSG_MAP(Dialog)
		REFLECT_NOTIFICATIONS()	// So that TreeViewCombo can handle its own DrawItem code.
	END_MSG_MAP()
};

#endif // !defined(AFX_CONTROLLERPANEL_H__7AE05973_9708_42B2_A605_92E9CA657BEA__INCLUDED_)
