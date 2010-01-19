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

// AnimationDialog.h : Declaration of the AnimationDialog

#ifndef __ANIMATIONDIALOG_H_
#define __ANIMATIONDIALOG_H_

#include "Dialog.h"
#include "PickList.h"

/////////////////////////////////////////////////////////////////////////////
// AnimationDialog

class CAnimation;

class AnimationDialog : public Dialog
{
public:
	AnimationDialog(CAnimation *pMeter, WORD idd=IDD_ANIMATION);
	virtual ~AnimationDialog();

protected:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT framerate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT framecount(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT loop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT autoreverse(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT singlestep(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT autostart(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT setMessage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT lparam(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	CAnimation *pMeter;
	PickList<UINT> messageList;

public:
	BEGIN_MSG_MAP(AnimationDialog)
		CHAIN_MSG_MAP(Dialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_FRAMERATE, EN_CHANGE, framerate)
		COMMAND_HANDLER(IDC_FRAMECOUNT, EN_CHANGE, framecount)
		COMMAND_HANDLER(IDC_LOOP, BN_CLICKED, loop)
		COMMAND_HANDLER(IDC_AUTOREVERSE, BN_CLICKED, autoreverse)
		COMMAND_HANDLER(IDC_SINGLESTEP, BN_CLICKED, singlestep)
		COMMAND_HANDLER(IDC_AUTOSTART, BN_CLICKED, autostart)
		COMMAND_ID_HANDLER(IDC_COMBO_MESSAGE, setMessage)
		COMMAND_HANDLER(IDC_STATELPARAM, EN_CHANGE, lparam)
	END_MSG_MAP()
};
#endif // __ANIMATIONDIALOG_H_
