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

#include "OverlayDialog.h"

class Meter;
class CImageOverlay;

class ImageOverlayDialog : public OverlayDialog
{
public:
	ImageOverlayDialog(CImageOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd=IDD_IMAGEOVERLAY);
	virtual ~ImageOverlayDialog();

protected:
	LRESULT alpha(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT path(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT setAnimationMeter(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT chooseImage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT keepOpen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT limitWidth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT limitHeight(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT alignment(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	PickList<long> animationMeterList;
	PickList<long> alignmentList;

public:
	BEGIN_MSG_MAP(ImageOverlayDialog)
		CHAIN_MSG_MAP(OverlayDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_ALPHA, EN_CHANGE, alpha)
		COMMAND_HANDLER(IDC_LIMITWIDTH, EN_CHANGE, limitWidth)
		COMMAND_HANDLER(IDC_LIMITHEIGHT, EN_CHANGE, limitHeight)
		COMMAND_HANDLER(IDC_COMBO_ALIGNMENT, CBN_SELENDOK, alignment)
		COMMAND_HANDLER(IDC_FORMAT, EN_CHANGE, path)
		COMMAND_ID_HANDLER(IDC_BROWSE, chooseImage)
		COMMAND_HANDLER(IDC_COMBO_ANIMATIONMETER, CBN_SELENDOK, setAnimationMeter)
		COMMAND_HANDLER(IDC_CHECK_KEEPOPEN, BN_CLICKED, keepOpen)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		REFLECT_NOTIFICATIONS()	// So that TreeViewCombo can handle its own DrawItem code.
	END_MSG_MAP()
};