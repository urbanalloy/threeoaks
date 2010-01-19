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

// MappedOverlayDialog.h : Declaration of the MappedOverlayDialog

#ifndef __MAPPEDOVERLAYDIALOG_H_
#define __MAPPEDOVERLAYDIALOG_H_

#include "OverlayDialog.h"

/////////////////////////////////////////////////////////////////////////////
// MappedOverlayDialog

class CMappedOverlay;

class MappedOverlayDialog : public OverlayDialog
{
public:
	MappedOverlayDialog(CMappedOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd=IDD_MAPPEDOVERLAY);
	virtual ~MappedOverlayDialog();

protected:
	LRESULT alpha(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT setMin(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT setMax(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT imagePath(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT mapPath(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT chooseImage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT chooseMap(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(MappedOverlayDialog)
		CHAIN_MSG_MAP(OverlayDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_ALPHA, EN_CHANGE, alpha)
		COMMAND_HANDLER(IDC_FORMAT, EN_CHANGE, imagePath)
		COMMAND_HANDLER(IDC_FORMAT_MAP, EN_CHANGE, mapPath)
		COMMAND_ID_HANDLER(IDC_BROWSE, chooseImage)
		COMMAND_ID_HANDLER(IDC_BROWSE_MAP, chooseMap)
		COMMAND_HANDLER(IDC_MIN, EN_CHANGE, setMin)
		COMMAND_HANDLER(IDC_MAX, EN_CHANGE, setMax)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		REFLECT_NOTIFICATIONS()	// So that TreeViewCombo can handle its own DrawItem code.
	END_MSG_MAP()
};
#endif // __MAPPEDOVERLAYDIALOG_H_
