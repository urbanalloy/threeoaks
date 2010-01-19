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

// AnimatorOverlayDialog.h : Declaration of the AnimatorOverlayDialog

#ifndef __AnimatorOverlayDIALOG_H_
#define __AnimatorOverlayDIALOG_H_

#include "OverlayDialog.h"

/////////////////////////////////////////////////////////////////////////////
// AnimatorOverlayDialog

class CAnimatorOverlay;

struct IMeterHome;

class AnimatorOverlayDialog : public OverlayDialog
{
public:
	AnimatorOverlayDialog(CAnimatorOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd=IDD_DRAWEROVERLAY);
	virtual ~AnimatorOverlayDialog();

protected:
	LRESULT alpha(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT alphaEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT xEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT yEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT xRotEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT yRotEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT rotationEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT hScaleEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT vScaleEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(AnimatorOverlayDialog)
		CHAIN_MSG_MAP(OverlayDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_ALPHA_START, EN_CHANGE, alpha)
		COMMAND_HANDLER(IDC_ALPHA_END, EN_CHANGE, alphaEnd)
		COMMAND_HANDLER(IDC_XPOS_END, EN_CHANGE, xEnd)
		COMMAND_HANDLER(IDC_YPOS_END, EN_CHANGE, yEnd)
		COMMAND_HANDLER(IDC_CXPOS_END, EN_CHANGE, xRotEnd)
		COMMAND_HANDLER(IDC_CYPOS_END, EN_CHANGE, yRotEnd)
		COMMAND_HANDLER(IDC_ROT_END, EN_CHANGE, rotationEnd)
		COMMAND_HANDLER(IDC_XSCALE_END, EN_CHANGE, hScaleEnd)
		COMMAND_HANDLER(IDC_YSCALE_END, EN_CHANGE, vScaleEnd)
		REFLECT_NOTIFICATIONS()	// So that TreeViewCombo can handle its own DrawItem code.
	END_MSG_MAP()
};
#endif // __AnimatorOverlayDIALOG_H_
