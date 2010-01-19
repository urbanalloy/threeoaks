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

// ConfigureDialog.cpp: implementation of the CConfigureDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "SysStatsCOM.h"
#include "ConfigureDialog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigureDialog::CConfigureDialog(IClientModel *model, IObserver *observer, WORD idd) :
	CSubject(observer),
	Dialog(idd),
	modelPanel(model, observer),
	meterPanel(model, observer),
	overlayPanel(model, observer),
	model(model),
	controllerPanel(model, observer)
{
	numTabs = 4;
	dialogs = new Dialog*[numTabs];
	dialogs[0] = &modelPanel;
	dialogs[1] = &overlayPanel;
	dialogs[2] = &meterPanel;
	dialogs[3] = &controllerPanel;
}

CConfigureDialog::~CConfigureDialog()
{
	delete[] dialogs;
}

void CConfigureDialog::ShowChild(int number)
{
	AtlTrace("Showing tab %d\n", number);

	// Validate the parameter
	if ((number >= 0) && (number < numTabs))
	{
		// Hide every window except for the chosen one
		for (int count = 0; count < numTabs; count++)
		{
			if (count != number)
			{
				dialogs[count]->ShowWindow(SW_HIDE);
			}
			else if (count == number)
			{
#ifdef notdef
				// Show the chosen window and set it's location
				dialogs[count]->SetWindowPos(&wndTop, m_rSettingsRect.left,
					m_rSettingsRect.top, m_rSettingsRect.right,
					m_rSettingsRect.bottom, SWP_SHOWWINDOW);
#endif
				// Show the chosen window and set it's location
				dialogs[count]->ShowWindow(SW_SHOW);


				// Select tab zero
				SendDlgItemMessage(IDC_TAB, TCM_SETCURSEL, (WPARAM)number, 0);
			}
		}
	}
}

LRESULT CConfigureDialog::OnSelchangeTab(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	int tabNo = (int) SendDlgItemMessage(idCtrl, TCM_GETCURFOCUS, 0, 0);

	ShowChild(tabNo);

	return 1;
}

LRESULT CConfigureDialog::OnHelp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	_bstr_t helpFile_t("system\\help.html");
	BSTR helpFile;
	IAppConfig *appConfig;
	model->get_AppConfig(&appConfig);
	appConfig->CanonicalPath(helpFile_t, &helpFile);
	appConfig->Release();

	ShellExecute(0,
			"open",
			_bstr_t(helpFile, false),
			"",
			"",
			SW_SHOWNORMAL);

	return 1;
}

#define NONMODAL

LRESULT CConfigureDialog::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	model->Save(true);
#ifdef NONMODAL
	DestroyWindow();

	return 1;
#else
	return Dialog::OnOK(wNotifyCode, wID, hWndCtl, bHandled);
#endif
}

LRESULT CConfigureDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	model->Save(true);
#ifdef NONMODAL
	DestroyWindow();

	return 1;
#else
	return Dialog::OnClose(uMsg, wParam, lParam, bHandled);
#endif
}

LRESULT CConfigureDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Create the child windows for the main window class
	HWND tabWnd = GetDlgItem(IDC_TAB);
	RECT tabRect;

	::GetClientRect(tabWnd, &tabRect);
	RECT tabContentRect;

	// Set the size and location of the child windows based on the tab control
	tabContentRect.left = tabRect.left + 5;
	tabContentRect.top = tabRect.top + 25;
	tabContentRect.right = tabRect.right - 5;
	tabContentRect.bottom = tabRect.bottom - 5;

	// Create the child windows for the main window class
	HWND panelWnd = meterPanel.Create(tabWnd);
	meterPanel.MoveWindow(&tabContentRect);

	panelWnd = overlayPanel.Create(tabWnd);
	::MoveWindow(panelWnd, tabContentRect.left, tabContentRect.top, tabContentRect.right - tabContentRect.left, tabContentRect.bottom - tabContentRect.top, true);

	panelWnd = controllerPanel.Create(tabWnd);
	::MoveWindow(panelWnd, tabContentRect.left, tabContentRect.top, tabContentRect.right - tabContentRect.left, tabContentRect.bottom - tabContentRect.top, true);

	panelWnd = modelPanel.Create(tabWnd);
	::MoveWindow(modelPanel, tabContentRect.left, tabContentRect.top, tabContentRect.right - tabContentRect.left, tabContentRect.bottom - tabContentRect.top, true);

	// Set the titles for each tab
	TCITEM tabItem;
	tabItem.mask = TCIF_TEXT;

	tabItem.pszText = "  Docklet   ";
	SendDlgItemMessage(IDC_TAB, TCM_INSERTITEM , (WPARAM)0, (LPARAM)&tabItem);

	tabItem.pszText = "  Overlays    ";
	SendDlgItemMessage(IDC_TAB, TCM_INSERTITEM , (WPARAM)1, (LPARAM)&tabItem);

	tabItem.pszText = "  Meters   ";
	SendDlgItemMessage(IDC_TAB, TCM_INSERTITEM , (WPARAM)2, (LPARAM)&tabItem);

	tabItem.pszText = "  Controllers  ";
	SendDlgItemMessage(IDC_TAB, TCM_INSERTITEM , (WPARAM)3, (LPARAM)&tabItem);

	// Select tab zero
	ShowChild(0);

	bHandled = false;

	return TRUE;  // return TRUE  unless you set the focus to a control
}
