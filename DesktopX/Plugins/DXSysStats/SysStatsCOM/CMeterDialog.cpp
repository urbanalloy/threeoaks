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

#include <stdio.h>
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <comdef.h>
#include "resource.h"
#include "SysStatsCOM.h"

#include "CMeterDialog.h"
#include "ArrayPtr.h"
#include "CMeterFactory.h"
#include "CCompositeOverlay.h"
#include "CRenameDialog.h"

#include "ArrayPtr.cpp"

CMeterDialog::CMeterDialog(IClientModel *model, IObserver *observer, WORD idd) :
	CSubject(observer),
	Dialog(idd),
	model(model)
{
	model->get_Meters(&meters);
	model->get_Overlays(&overlayRoot);
}

CMeterDialog::~CMeterDialog()
{
	if (meters)
		meters->Release();

	if (overlayRoot)
		overlayRoot->Release();
}

LRESULT CMeterDialog::add(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[256];
	*text = 0;
	GetDlgItemText(IDC_METER_TYPE, text, sizeof(text));
	IMeter *m = CMeterFactory::createInstance(text, model);
	m->put_Name(_bstr_t("New Meter"));
	short i = 0;
	meters->Count(&i);
	meters->Add(m);
	m->Release();

	resetList(i);
	enable(i);

	notifyObserver();

	return 1;
}

void CMeterDialog::remove(ICompositeOverlay *root, IMeter *pMeter)
{
	// Remove any overlays viewing it.
	short i;
	for (root->Count(&i); i>=0; i--)
	{
		IOverlay *pOverlay;
		root->Item(CComVariant(i), &pOverlay);
		if (pOverlay)
		{
			BSTR overlayType;
			pOverlay->get_Type(&overlayType);	// We own the BSTR now
			_bstr_t ot(overlayType, false);		// Adopt it.
			_bstr_t cmp("Composite");
			if (ot == cmp)
			{
				remove((ICompositeOverlay*)pOverlay, pMeter);
			}

			IMeter *pMet;
			pOverlay->get_meter(&pMet);
			if (pMet == pMeter)
			{
				root->Remove(CComVariant(i));
			}
			pOverlay->Release();
		}
	}
}

LRESULT CMeterDialog::remove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT mList = SendDlgItemMessage(IDC_METER_LIST, LB_GETCURSEL, 0, 0);

	if (mList != CB_ERR)
	{
		CComVariant index((int)mList);
		IMeter *pMeter;
		meters->Item(index, &pMeter);
		meters->Remove(index);

		if (pMeter)
		{
//			remove(overlayRoot, pMeter);
//			overlayPanel.resetList();
			resetList((int)mList);
			notifyObserver();
			pMeter->Release();
		}
	}

	return 1;
}

LRESULT CMeterDialog::rename(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT mList = SendDlgItemMessage(IDC_METER_LIST, LB_GETCURSEL, 0, 0);

	if (mList != CB_ERR)
	{
		char buf[64];
		SendDlgItemMessage(IDC_METER_LIST, LB_GETTEXT, (WPARAM)mList, (LPARAM)buf);
		CRenameDialog *d = new CRenameDialog(buf);
		d->DoModal(m_hWnd);
		delete d;

		if (*buf != 0)	// Can't have empty strings
		{
			IMeter *meter;
			meters->Item(CComVariant((int)mList), &meter);
			meter->put_Name(_bstr_t(buf));
			resetList((int)mList);
			meter->Release();
		}
	}

	return 1;
}

LRESULT CMeterDialog::configure(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT mList = SendDlgItemMessage(IDC_METER_LIST, LB_GETCURSEL, 0, 0);

	if (mList != CB_ERR)
	{
		IMeter *meter;
		meters->Item(CComVariant((int)mList), &meter);
		meter->Configure(observer, (long)m_hWnd);
		notifyObserver();
		meter->Release();
	}

	return 1;
}

LRESULT CMeterDialog::setInterval(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT mList = SendDlgItemMessage(IDC_METER_LIST, LB_GETCURSEL, 0, 0);

	if (mList != CB_ERR)
	{
		BOOL iRet = FALSE;
		double interval = 1.0;
		char val[64];
		GetDlgItemText(wID, val, sizeof(val));
		sscanf(val, "%lg", &interval);
		IMeter *meter;
		meters->Item(CComVariant((int)mList), &meter);
		meter->put_TimerInterval(interval);
		notifyObserver();
		meter->Release();
	}

	return 1;
}

LRESULT CMeterDialog::OnMeterType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendDlgItemMessage(IDC_METER_LIST, LB_SETCURSEL, (WPARAM)-1, 0);

	return 1;
}

LRESULT CMeterDialog::listSelChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT mList = SendDlgItemMessage(wID, LB_GETCURSEL, 0, 0);

	if (mList != CB_ERR)
		enable((int)mList);
	else
		disable();

	return 1;
}

void CMeterDialog::enable(int item)
{
	short max;
	meters->Count(&max);
	if (item < max)
	{
		IMeter *meter;
		meters->Item(CComVariant(item), &meter);

		BSTR type;
		meter->get_Type(&type);	// We own the BSTR now

		SendDlgItemMessage(IDC_METER_TYPE,CB_SELECTSTRING,0,(LPARAM)((char*)_bstr_t(type, false)));
		::EnableWindow(GetDlgItem(IDC_METER_DELETE), TRUE);
		::EnableWindow(GetDlgItem(IDC_METER_RENAME), TRUE);
		::EnableWindow(GetDlgItem(IDC_METER_CONFIGURE), TRUE);

		double timer;

		meter->get_TimerInterval(&timer);
		char buf[40];
		_snprintf(buf, sizeof(buf), "%.10g", timer);
		SetDlgItemText(IDC_METER_INTERVAL, buf);
		::EnableWindow(GetDlgItem(IDC_METER_INTERVAL), TRUE);
		meter->Release();
	}
}

void CMeterDialog::disable()
{
	::EnableWindow(GetDlgItem(IDC_METER_DELETE), FALSE);
	::EnableWindow(GetDlgItem(IDC_METER_RENAME), FALSE);
	::EnableWindow(GetDlgItem(IDC_METER_CONFIGURE), FALSE);
	SetDlgItemText(IDC_METER_INTERVAL, "");
	::EnableWindow(GetDlgItem(IDC_METER_INTERVAL), FALSE);
}

void CMeterDialog::resetList(int item)
{
	SendDlgItemMessage(IDC_METER_LIST,LB_RESETCONTENT,0,0);
	short max;
	meters->Count(&max);
	for (int i=0; i<max; i++)
	{
		BSTR name;
		IMeter *meter;
		meters->Item(CComVariant(i), &meter);

		meter->get_Name(&name);
		SendDlgItemMessage(IDC_METER_LIST,LB_ADDSTRING,0,(LPARAM)((char*)_bstr_t(name, false)));
		meter->Release();
	}

	if (item < 0)
		return;
	else if (item < max)
		SendDlgItemMessage(IDC_METER_LIST, LB_SETCURSEL, (WPARAM)item, 0);
	else if (max != 0)
		SendDlgItemMessage(IDC_METER_LIST, LB_SETCURSEL, (WPARAM)(max-1), 0);
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT CMeterDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Sort out the meter dropdown list
	char** keys;
	keys = (char**) malloc(sizeof(const char*) * CMeterFactory::typeCount());
	CMeterFactory::listTypes(keys);

	for (int i=0; i<CMeterFactory::typeCount(); i++)
	{
		SendDlgItemMessage(IDC_METER_TYPE,CB_ADDSTRING,0,(LPARAM)keys[i]);
	}

	SendDlgItemMessage(IDC_METER_TYPE,CB_SETCURSEL,0,0);

	free(keys);

	// Draw the list of meters.
	resetList(-1);

	bHandled = FALSE;

	return 1;
}
