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

// ModelPanel.cpp: implementation of the CModelPanel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include <stdio.h>
#include <ComUtil.h>
#include "SysStatsCOM.h"
#include "ModelPanel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModelPanel::CModelPanel(IClientModel *model, IObserver *observer, WORD idd) :
	CSubject(observer),
	Dialog(idd),
	model(model)
{
}

CModelPanel::~CModelPanel()
{
}

LRESULT CModelPanel::setName(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[MAX_PATH];
	*text = 0;
	GetDlgItemText(wID, text, sizeof(text));

	if (model)
	{
		model->put_Name(_bstr_t(&text[0]));
	}

	return 1;
}

LRESULT CModelPanel::setDescription(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[MAX_PATH];
	*text = 0;
	GetDlgItemText(wID, text, sizeof(text));

	if (model)
	{
		model->put_Description(_bstr_t(&text[0]));
	}

	return 1;
}

LRESULT CModelPanel::setVersion(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[MAX_PATH];
	*text = 0;
	GetDlgItemText(wID, text, sizeof(text));

	if (model)
	{
		model->put_Version(_bstr_t(&text[0]));
	}

	return 1;
}

LRESULT CModelPanel::setAuthor(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[MAX_PATH];
	*text = 0;
	GetDlgItemText(wID, text, sizeof(text));

	if (model)
	{
		model->put_Author(_bstr_t(&text[0]));
	}

	return 1;
}

LRESULT CModelPanel::hScale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	double scale = 1.0L;
	char val[64];
	GetDlgItemText(wID, val, sizeof(val));
	sscanf(val, "%lg", &scale);
	IOverlayHome *overlay = 0;
	model->get_Overlays(&overlay);
	overlay->put_HScale(scale);
	overlay->Release();
	notifyObserver();

	return 1;
}

LRESULT CModelPanel::vScale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	double scale = 1.0L;
	char val[64];
	GetDlgItemText(wID, val, sizeof(val));
	sscanf(val, "%lg", &scale);
	IOverlayHome *overlay = 0;
	model->get_Overlays(&overlay);
	overlay->put_VScale(scale);
	overlay->Release();
	notifyObserver();

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT CModelPanel::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Subclass the overlays combo box window
	link.SubclassWindow(GetDlgItem(IDC_LINK));
	link.SetHyperLink("http://sysstats.jools.net/");
//	link.SetToolTipText("SysStats");

	BSTR s;
	model->get_Name(&s);
	SetDlgItemText(IDC_MODELNAME, _bstr_t(s, false));

	model->get_Description(&s);
	SetDlgItemText(IDC_MODELDESC, _bstr_t(s, false));

	model->get_Version(&s);
	SetDlgItemText(IDC_MODELVERSION, _bstr_t(s, false));

	model->get_Author(&s);
	SetDlgItemText(IDC_MODELAUTHOR, _bstr_t(s, false));

	double scale;
	char val[64];

	IOverlayHome *overlay = 0;
	model->get_Overlays(&overlay);

	overlay->get_HScale(&scale);
	sprintf(val, "%.20g", scale);
	SetDlgItemText(IDC_XSCALE, val);

	overlay->get_VScale(&scale);
	sprintf(val, "%.20g", scale);
	SetDlgItemText(IDC_YSCALE, val);

	overlay->Release();

	bHandled = FALSE;

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being destroyed.
 */
LRESULT CModelPanel::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}
