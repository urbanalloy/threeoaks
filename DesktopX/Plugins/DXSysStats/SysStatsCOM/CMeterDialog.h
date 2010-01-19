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

#pragma once

#include "CSubject.h"
#include "Dialog.h"
#include "resource.h"

struct IMeter;
//struct IOverlay;
struct IClientModel;
struct IMeterHome;
struct IOverlayHome;
struct ICompositeOverlay;
template <class I> class ArrayPtr;

class CMeterDialog : public CSubject, public Dialog
{
public:
	CMeterDialog(IClientModel *model, IObserver *observer, WORD idd=IDD_METERHOMEPROPS);
	virtual ~CMeterDialog();

protected:
	void remove(ICompositeOverlay *root, IMeter *pMeter);
	void enable(int item);
	void disable();

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT add(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT remove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT rename(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT configure(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT setInterval(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT listSelChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT OnMeterType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	
	// Specific to this class
	virtual void resetList(int selected);

	IClientModel *model;
	IMeterHome *meters;
	IOverlayHome *overlayRoot;
	HWND parentHWnd;

public:
	BEGIN_MSG_MAP(CMeterDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_METER_LIST, LBN_SELCHANGE, listSelChanged)
		COMMAND_HANDLER(IDC_METER_DELETE, BN_CLICKED, remove)
		COMMAND_HANDLER(IDC_METER_RENAME, BN_CLICKED, rename)
		COMMAND_HANDLER(IDC_METER_ADD, BN_CLICKED, add)
		COMMAND_HANDLER(IDC_METER_CONFIGURE, BN_CLICKED, configure)
		COMMAND_HANDLER(IDC_METER_TYPE, CBN_SELENDOK, OnMeterType)
		COMMAND_HANDLER(IDC_METER_INTERVAL, EN_CHANGE, setInterval)
		CHAIN_MSG_MAP(Dialog)
	END_MSG_MAP()
};