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

//#import "C:\Program Files\Microsoft Visual Studio\MyProjects\SysStatsCOM\Debug\SysStatsCOM.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids 

#include "CSubject.h"
#include "Dialog.h"
#include "Picklist.h"
#include "TreeViewCombo.h"

template <class I> class ArrayPtr;
struct IMeter;
struct ISimpleOverlay;

class OverlayDialog : public Dialog, public CSubject
{
public:
	virtual ~OverlayDialog();

	static COLORREF custColors[];

protected:
	OverlayDialog(ISimpleOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd);
	void resetSelectorCombo(long meterIndex);

	virtual void resetOverlayList();
	HTREEITEM drawOverlayList(ICompositeOverlay *root, HTREEITEM parent, const IOverlay *selected, TVINSERTSTRUCT &itvIns);

	ISimpleOverlay *overlay;
	IMeterHome *meters;
	PickList<long> meterList;
	PickList<long> modeList;
	PickList<_bstr_t> selectorList;
	_bstr_t xml;
	TreeViewCombo overlaysCombo;

	LRESULT setMeter(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT selector(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT x(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT y(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT xRot(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT yRot(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT rotation(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT hScale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT vScale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT mode(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT setOverlay(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(OverlayDialog)
		CHAIN_MSG_MAP(Dialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_COMBO_SELECTOR, selector)
		COMMAND_HANDLER(IDC_XPOS, EN_CHANGE, x)
		COMMAND_HANDLER(IDC_YPOS, EN_CHANGE, y)
		COMMAND_HANDLER(IDC_CXPOS, EN_CHANGE, xRot)
		COMMAND_HANDLER(IDC_CYPOS, EN_CHANGE, yRot)
		COMMAND_HANDLER(IDC_ROT, EN_CHANGE, rotation)
		COMMAND_HANDLER(IDC_XSCALE, EN_CHANGE, hScale)
		COMMAND_HANDLER(IDC_YSCALE, EN_CHANGE, vScale)
		COMMAND_HANDLER(IDC_COMBO_METER, CBN_SELENDOK, setMeter)
		COMMAND_HANDLER(IDC_COMBO_MODE, CBN_SELENDOK, mode)
		COMMAND_HANDLER(IDC_COMBO_EFFECTOVERLAY, CBN_SELENDOK, setOverlay)
	END_MSG_MAP()
};