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

#import "SysStatsCOM.tlb" no_namespace raw_interfaces_only

#include "CSubject.h"
#include "Dialog.h"
#include "ODResource.h"
#include "PickList.h"

class AppearanceDialog : public Dialog, public CSubject
{
public:
	AppearanceDialog(ILabel *app, IObserver *observer, IMeterHome *meters, WORD idd=IDD_APPEARANCE);
	virtual ~AppearanceDialog();

protected:
	void resetSelectorCombo(long meterIndex);

	PickList<long> meterList;
	PickList<_bstr_t> selectorList;
	_bstr_t xml;

	ILabel *app;
	IMeterHome *meters;

	LRESULT setMeter(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT selector(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT format(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(AppearanceDialog)
		CHAIN_MSG_MAP(Dialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_COMBO_SELECTOR, selector)
		COMMAND_HANDLER(IDC_FORMAT, EN_CHANGE, format)
		COMMAND_ID_HANDLER(IDC_COMBO_METER, setMeter)
	END_MSG_MAP()
};