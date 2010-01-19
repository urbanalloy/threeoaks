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
#include <stdio.h>
#include <windows.h>
#include <Pdh.h>
#include <shlwapi.h>
#include <shlobj.h>
#include "resource.h"
#include "PickList.cpp"

#include "DiskMeter.h"
#include "DiskDialog.h"

extern void ToValL(long *out, char *s);
extern void ToStrL(char *buf, int maxlen, long val);
extern void ToValD(double *out, char *s);
extern void ToStrD(char *buf, int maxlen, double val);

template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

DiskDialog::DiskDialog(CDiskMeter *pMeter, WORD idd) :
	pMeter(pMeter),
	selList(Identity<long>, ToStrL, ToValL, true),
	scaleList(Identity<double>, ToStrD, ToValD, false),
	Dialog(idd)
{
}

DiskDialog::~DiskDialog()
{
}

LRESULT DiskDialog::path(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[MAX_PATH];
	*text = 0;
	GetDlgItemText(wID, text, sizeof(text));
	pMeter->put_Path(_bstr_t(text));

	return 1;
}

LRESULT DiskDialog::chooseDir(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BROWSEINFO bi = { 0 };
    bi.lpszTitle = "Directory to Monitor...";
	bi.hwndOwner = m_hWnd;
    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
    if ( pidl != 0 )
    {
        // get the name of the folder
        TCHAR path[MAX_PATH];
        SHGetPathFromIDList ( pidl, path );
		SetDlgItemText(IDC_DIRECTORY, path);
		pMeter->put_Path(_bstr_t(path));

        // free memory used
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free ( pidl );
            imalloc->Release ( );
        }

		return S_OK;
	}

	return S_OK;
}

LRESULT DiskDialog::metric(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val = 0;
	selList.getData(&val, 0 /* default value */);
	pMeter->put_Selector(val);

	return S_OK;
}

LRESULT DiskDialog::scale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	double val = 1.0L;
	scaleList.getData(&val, 1.0L /* default value */);
	pMeter->put_Scale(val);

	return S_OK;
}

static const char *scaleLabels[] =
{
	"1 Thousand",
	"1 Million",
	"1 Billion",
	"1K",
	"1M",
	"1G",
	NULL
};

static double scaleData[] =
{
	1000,
	1000000,
	1000000000,
	1024,
	1024*1024,
	1024*1024*1024
};

static const char *metricLabels[] =
{
	"% used",
	"% free",
	"Used",
	"Free",
	"Total",
	NULL
};

static long metricData[] =
{
	0, 1, 2, 3, 4
};

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT DiskDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BSTR f = 0;
	pMeter->get_Path(&f);
	SetDlgItemText(IDC_DIRECTORY, _bstr_t(f, false));

	long sel;
	pMeter->get_Selector(&sel);
	selList.init(m_hWnd, IDC_COMBO_SELECTOR, metricLabels, metricData, sel);

	double scale;
	pMeter->get_Scale(&scale);
	scaleList.init(m_hWnd, IDC_COMBO_SCALE, scaleLabels, scaleData, scale);

	bHandled = FALSE;	// Make CWindow go through all the message maps.

	return 1;
}
