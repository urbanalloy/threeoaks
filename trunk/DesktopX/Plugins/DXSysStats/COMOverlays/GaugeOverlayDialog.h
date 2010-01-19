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

#include "MeterOverlayDialog.h"

class Meter;
struct IGaugeOverlay;

class GaugeOverlayDialog : public MeterOverlayDialog
{
public:
	GaugeOverlayDialog(IGaugeOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd=IDD_GAUGEOVERLAY);
	virtual ~GaugeOverlayDialog();

protected:
	LRESULT span(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(GaugeOverlayDialog)
		COMMAND_HANDLER(IDC_SPAN, EN_CHANGE, span)
		CHAIN_MSG_MAP(MeterOverlayDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()
};