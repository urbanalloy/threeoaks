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

// ScrollerOverlayDialog.h : Declaration of the ScrollerOverlayDialog

#ifndef __SCROLLEROVERLAYDIALOG_H_
#define __SCROLLEROVERLAYDIALOG_H_

#include "OverlayDialog.h"

/////////////////////////////////////////////////////////////////////////////
// ScrollerOverlayDialog

class CScrollerOverlay;
struct IMeterHome;

class ScrollerOverlayDialog : public OverlayDialog
{
public:
	ScrollerOverlayDialog(CScrollerOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd=IDD_SCROLLEROVERLAY);
	virtual ~ScrollerOverlayDialog();

protected:
	LRESULT alpha(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT dx(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT dy(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT setMeter2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT loopLast(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	PickList<long> meter2List;

public:
	BEGIN_MSG_MAP(ScrollerOverlayDialog)
		CHAIN_MSG_MAP(OverlayDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_ALPHA, EN_CHANGE, alpha)
		COMMAND_HANDLER(IDC_DX, EN_CHANGE, dx)
		COMMAND_HANDLER(IDC_DY, EN_CHANGE, dy)
		COMMAND_HANDLER(IDC_COMBO_METER2, CBN_SELENDOK, setMeter2)
		COMMAND_HANDLER(IDC_LOOPLAST, BN_CLICKED, loopLast)
		REFLECT_NOTIFICATIONS()	// So that TreeViewCombo can handle its own DrawItem code.
	END_MSG_MAP()
};
#endif // __SCROLLEROVERLAYDIALOG_H_
