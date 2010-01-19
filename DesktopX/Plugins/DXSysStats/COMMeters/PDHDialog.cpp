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

#include "PDHMeter.h"
#include "PDHDialog.h"
#include "CPdh.h"

extern void ToValD(double *out, char *s);
extern void ToStrD(char *buf, int maxlen, double val);

template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

PDHDialog::PDHDialog(CPDHMeter *pMeter, WORD idd) :
	pMeter(pMeter),
	scaleList(Identity<double>, ToStrD, ToValD, false),
	Dialog(idd)
{
}

PDHDialog::~PDHDialog()
{
}

LRESULT PDHDialog::showPDHConfig(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CHAR            szPathBuffer[MAX_PATH];
	*szPathBuffer = 0;

	PDH_STATUS      pdhStatus;
	PDH_BROWSE_DLG_CONFIG  BrowseDlgData;

	// Zero the contents of the structure.
	memset (&BrowseDlgData, 0, sizeof(PDH_BROWSE_DLG_CONFIG));
	pMeter->getPDH()->getCounterName(0, szPathBuffer);
#ifdef notdef
	if (strncmp(szPathBuffer, "\\\\", 2) != 0)	// No hostname, add localhost
	{
//		pMeter->getPDH()->getHostName(0, szPathBuffer);
//		strcpy(szPathBuffer, "\\\\127.0.0.1");
		strcpy(szPathBuffer, "\\\\");
		pMeter->getPDH()->getCounterName(0, szPathBuffer + strlen(szPathBuffer));
	}
#endif
	// Initialize the browser dialog window settings.
	BrowseDlgData.bIncludeInstanceIndex = FALSE;   
	BrowseDlgData.bSingleCounterPerAdd = TRUE;
	BrowseDlgData.bSingleCounterPerDialog = TRUE;  
	BrowseDlgData.bLocalCountersOnly = TRUE;      
	BrowseDlgData.bWildCardInstances = TRUE;
	BrowseDlgData.bHideDetailBox = TRUE;
	BrowseDlgData.bInitializePath = FALSE; // Fucks up if this is set to true.    
	BrowseDlgData.bDisableMachineSelection = TRUE;
	BrowseDlgData.bIncludeCostlyObjects = FALSE;
	BrowseDlgData.bReserved = TRUE;
	BrowseDlgData.hWndOwner = m_hWnd;   
	BrowseDlgData.bReserved = FALSE;           
	BrowseDlgData.szReturnPathBuffer = szPathBuffer;
	BrowseDlgData.cchReturnPathLength = MAX_PATH-1;
	BrowseDlgData.pCallBack = NULL;   
	BrowseDlgData.dwCallBackArg = 0;
	BrowseDlgData.szDataSource = 0;
	BrowseDlgData.CallBackStatus = ERROR_SUCCESS;
	BrowseDlgData.dwDefaultDetailLevel = PERF_DETAIL_WIZARD;
	BrowseDlgData.szDialogBoxCaption = "Select a counter to monitor";

	// Display the counter browser window.
	pdhStatus = PdhBrowseCounters (&BrowseDlgData);

	char *counters[1];
	counters[0] = szPathBuffer;

	pMeter->SetCounters(counters, 1);

	return TRUE;
}

LRESULT PDHDialog::setScale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	double val = 1.0L;
	scaleList.getData(&val, 1.0L /* default value */);
	pMeter->put_Scale(val);

	return 1;
}

static const char *scaleLabels[] =
{
	"1",
	"10",
	"100",
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
	1,
	10,
	100,
	1000,
	1000000,
	1000000000,
	1024,
	1024*1024,
	1024*1024*1024
};

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT PDHDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	double scale;
	pMeter->get_Scale(&scale);
	scaleList.init(m_hWnd, IDC_COMBO_SCALE, scaleLabels, scaleData, scale);

	bHandled = FALSE;	// Make CWindow go through all the message maps.

	return 1;
}
