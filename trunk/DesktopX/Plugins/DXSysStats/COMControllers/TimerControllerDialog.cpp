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

// TimerController.cpp : Implementation of TimerControllerDialog.cpp
#include "stdafx.h"
#include <stdio.h>
#include "COMControllers.h"
#include "TimerController.h"
#include "TimerControllerDialog.h"

TimerControllerDialog::TimerControllerDialog(ITimerController *pController, IObserver *observer, IMeterHome *meters, WORD idd) :
	MessageSenderDialog(pController, observer, meters, idd)
{
}

TimerControllerDialog::~TimerControllerDialog()
{
}

LRESULT TimerControllerDialog::setInterval(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = FALSE;
	double interval = 1.0;
	char val[64];
	GetDlgItemText(wID, val, sizeof(val));
	sscanf(val, "%lg", &interval);
	ITimerClient *pTimerClient = 0;
	HRESULT hr = pController->QueryInterface(__uuidof(ITimerClient), (void**)&pTimerClient);
	if (SUCCEEDED(hr))
	{
		pTimerClient->put_TimerInterval(interval);
		pTimerClient->Release();
	}

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 * Should initialize all of the controls using values from the meter.
 */
LRESULT TimerControllerDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ITimerClient *pTimerClient = 0;
	HRESULT hr = pController->QueryInterface(__uuidof(ITimerClient), (void**)&pTimerClient);
	if (SUCCEEDED(hr))
	{
		double timer;

		pTimerClient->get_TimerInterval(&timer);
		char buf[40];
		_snprintf(buf, sizeof(buf), "%.20g", timer);
		SetDlgItemText(IDC_EDIT_INTERVAL, buf);
		pTimerClient->Release();
	}

	bHandled = FALSE;	// Make CWindow go through all the message maps.

	return 1;
}
