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


#include "stdafx.h"
#include "COMOverlays.h"

#include <stdio.h>
#include <shlwapi.h>
#include <shlobj.h>
#include "resource.h"

#include "GaugeOverlay.h"
#include "GaugeOverlayDialog.h"

GaugeOverlayDialog::GaugeOverlayDialog(IGaugeOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd) :
	MeterOverlayDialog(overlay, observer, meters, idd)
{
}

GaugeOverlayDialog::~GaugeOverlayDialog()
{
}

LRESULT GaugeOverlayDialog::span(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CGaugeOverlay*)overlay)->put_Span((long)i);
		notifyObserver();
	}

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT GaugeOverlayDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	long l;
	((CGaugeOverlay*)overlay)->get_Span(&l);
	SetDlgItemInt(IDC_SPAN, (UINT)l, TRUE);

	bHandled = FALSE;

	return TRUE;
}
