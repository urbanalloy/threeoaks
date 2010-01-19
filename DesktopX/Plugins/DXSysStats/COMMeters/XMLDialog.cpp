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

#include <stdafx.h>
#include "resource.h"
#include "ArrayPtr.cpp"

#include "XMLMeter.h"
#include "XMLDialog.h"
#import <msxml4.dll>

XMLDialog::XMLDialog(CXMLMeter *pMeter, WORD idd) :
	pMeter(pMeter),
	Dialog(idd)
{
}

XMLDialog::~XMLDialog()
{
}

LRESULT XMLDialog::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    EndDialog(wID);
	return 1;
}

LRESULT XMLDialog::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char buf[MAX_PATH];

	*buf = 0;
	GetDlgItemText(IDC_METER_NAME, buf, sizeof(buf));
	pMeter->put_URL(_bstr_t(buf));

	LRESULT state = SendDlgItemMessage(IDC_CHECK_XPATH, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	pMeter->put_UseXPath(state);

	*buf = 0;
	GetDlgItemText(IDC_EDIT_NAMESPACES, buf, sizeof(buf));
	ArrayPtr<_bstr_t> namespaces;
	char *token = strtok(buf, "\r\n");
	while(token != NULL)
	{
		namespaces.add(new _bstr_t(token));
		/* Get next token: */
		token = strtok(NULL, "\r\n");
	}

	SAFEARRAY *pArray = ::SafeArrayCreateVector(VT_BSTR, 0, namespaces.count());

	long indices[1];
	for (int i=0; i<namespaces.count(); i++)
	{
		indices[0] = i;
		::SafeArrayPutElement(pArray, indices, namespaces[i]->copy());
		delete namespaces[i];
	}

	pMeter->put_Namespaces(pArray);
	::SafeArrayDestroy(pArray);

	*buf = 0;
	GetDlgItemText(IDC_METER_USERNAME, buf, sizeof(buf));
	pMeter->put_Username(_bstr_t(buf));

	*buf = 0;
	GetDlgItemText(IDC_METER_PASSWORD, buf, sizeof(buf));
	pMeter->put_Password(_bstr_t(buf));

	*buf = 0;
	GetDlgItemText(IDC_METER_PROXY, buf, sizeof(buf));
	pMeter->put_ProxyURL(_bstr_t(buf));

	*buf = 0;
	GetDlgItemText(IDC_METER_PROXY_USERNAME, buf, sizeof(buf));
	pMeter->put_ProxyUsername(_bstr_t(buf));

	*buf = 0;
	GetDlgItemText(IDC_METER_PROXY_PASSWORD, buf, sizeof(buf));
	pMeter->put_ProxyPassword(_bstr_t(buf));

    EndDialog(wID);

	return S_OK;
}

LRESULT XMLDialog::xpath(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT state = SendDlgItemMessage(IDC_CHECK_XPATH, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if (state)
		::EnableWindow(GetDlgItem(IDC_EDIT_NAMESPACES), TRUE);
	else
		::EnableWindow(GetDlgItem(IDC_EDIT_NAMESPACES), FALSE);

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT XMLDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BSTR bstr = 0;

	pMeter->get_URL(&bstr);
	SetDlgItemText(IDC_METER_NAME, _bstr_t(bstr, false));

	VARIANT_BOOL b;
	pMeter->get_UseXPath(&b);
	SendDlgItemMessage(IDC_CHECK_XPATH, BM_SETCHECK, b ? BST_CHECKED : BST_UNCHECKED, 0);
	if (b)
		::EnableWindow(GetDlgItem(IDC_EDIT_NAMESPACES), TRUE);

	LPSAFEARRAY pArray;
	pMeter->get_Namespaces(&pArray);

	long lb = 0;
	long ub = 0;
	VARTYPE vt;
	::SafeArrayGetVartype(pArray, &vt);
	if (vt == VT_BSTR)
	{
		HRESULT hr = ::SafeArrayGetUBound(pArray, 1, &ub);
		hr = ::SafeArrayGetLBound(pArray, 1, &lb);
		_bstr_t buf;

		long indices[1];
		for (int i=lb; i<=ub; i++)
		{
			indices[0] = i;
			BSTR bStr = 0;
			::SafeArrayGetElement(pArray, indices, &bStr);
			buf += bStr;
			buf += "\r\n";
		}
		::SetDlgItemTextW(m_hWnd, IDC_EDIT_NAMESPACES, buf);
		::SendDlgItemMessage(m_hWnd, IDC_EDIT_NAMESPACES, WM_VSCROLL, (WPARAM)SB_BOTTOM, 0);
	}

	::SafeArrayDestroy(pArray);


	pMeter->get_Username(&bstr);
	SetDlgItemText(IDC_METER_USERNAME, _bstr_t(bstr, false));

	pMeter->get_Password(&bstr);
	SetDlgItemText(IDC_METER_PASSWORD, _bstr_t(bstr, false));

	pMeter->get_ProxyURL(&bstr);
	SetDlgItemText(IDC_METER_PROXY, _bstr_t(bstr, false));

	pMeter->get_ProxyUsername(&bstr);
	SetDlgItemText(IDC_METER_PROXY_USERNAME, _bstr_t(bstr, false));

	pMeter->get_ProxyPassword(&bstr);
	SetDlgItemText(IDC_METER_PROXY_PASSWORD, _bstr_t(bstr, false));

	::MSXML2::IServerXMLHTTPRequest2Ptr pRequest = NULL;
	HRESULT hr;
	hr=pRequest.CreateInstance("Msxml2.ServerXMLHTTP.4.0", NULL, CLSCTX_INPROC_SERVER);
	if (SUCCEEDED(hr))
	{
		pRequest.Release();
		::EnableWindow(GetDlgItem(IDC_METER_USERNAME), TRUE);
		::EnableWindow(GetDlgItem(IDC_METER_PASSWORD), TRUE);
		::EnableWindow(GetDlgItem(IDC_METER_PROXY), TRUE);
		::EnableWindow(GetDlgItem(IDC_METER_PROXY_USERNAME), TRUE);
		::EnableWindow(GetDlgItem(IDC_METER_PROXY_PASSWORD), TRUE);
	}

	bHandled = FALSE;	// Make CWindow go through all the message maps.

	return 1;
}
