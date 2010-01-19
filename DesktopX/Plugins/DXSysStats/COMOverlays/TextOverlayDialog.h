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
class CTextOverlay;

class TextOverlayDialog : public OverlayDialog
{
public:
	TextOverlayDialog(CTextOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd=IDD_TEXTOVERLAY);
	virtual ~TextOverlayDialog();

protected:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT chooseFont(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT chooseColor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT chooseBlurColor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT hAlign(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT vAlign(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT alpha(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT blur(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT format(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT width(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT height(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT wrap(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT vertical(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT alternate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT antialias(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT contrast(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	PickList<long> antiAliasList;

public:
	BEGIN_MSG_MAP(TextOverlayDialog)
		CHAIN_MSG_MAP(OverlayDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_BLUR, EN_CHANGE, blur)
		COMMAND_HANDLER(IDC_WIDTH, EN_CHANGE, width)
		COMMAND_HANDLER(IDC_HEIGHT, EN_CHANGE, height)
		COMMAND_HANDLER(IDC_ALPHA, EN_CHANGE, alpha)
		COMMAND_HANDLER(IDC_HALIGN, CBN_SELENDOK, hAlign)
		COMMAND_HANDLER(IDC_VALIGN, CBN_SELENDOK, vAlign)
		COMMAND_HANDLER(IDC_FORMAT, EN_CHANGE, format)
		COMMAND_ID_HANDLER(IDC_FONT_BUTTON, chooseFont)
		COMMAND_ID_HANDLER(IDC_COLOR_BUTTON, chooseColor)
		COMMAND_ID_HANDLER(IDC_BLURCOLOR_BUTTON, chooseBlurColor)
		COMMAND_HANDLER(IDC_RADIO_WRAP, BN_CLICKED, wrap)
		COMMAND_HANDLER(IDC_RADIO_VERTICAL, BN_CLICKED, vertical)
		COMMAND_HANDLER(IDC_RADIO_ALTERNATE, BN_CLICKED, alternate)
		COMMAND_HANDLER(IDC_COMBO_ANTIALIAS, CBN_SELENDOK, antialias)
		COMMAND_HANDLER(IDC_CONTRAST, EN_CHANGE, contrast)
		REFLECT_NOTIFICATIONS()	// So that TreeViewCombo can handle its own DrawItem code.
	END_MSG_MAP()
};