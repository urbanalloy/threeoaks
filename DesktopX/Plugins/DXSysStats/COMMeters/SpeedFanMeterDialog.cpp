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

// SpeedFanMeter.cpp : Implementation of SpeedFanMeterDialog.cpp
#include "stdafx.h"
#include "COMMeters.h"
#include "SpeedFanMeter.h"
#include "SpeedFanMeterDialog.h"
#include "PickList.cpp"

extern void ToValL(long *out, char *s);
extern void ToStrL(char *buf, int maxlen, long val);
extern void ToValD(double *out, char *s);
extern void ToStrD(char *buf, int maxlen, double val);

template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

SpeedFanMeterDialog::SpeedFanMeterDialog(CSpeedFanMeter *pMeter, WORD idd) :
	pMeter(pMeter),
	indexList(Identity<long>, ToStrL, ToValL, /* fixed */ true),
	readingList(Identity<long>, ToStrL, ToValL, /* fixed */ true),
	scaleList(Identity<double>, ToStrD, ToValD, /* fixed */ false),
	Dialog(idd)
{
}

SpeedFanMeterDialog::~SpeedFanMeterDialog()
{
}

LRESULT SpeedFanMeterDialog::setReading(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val=-1;
	readingList.getData(&val, -1);
	if (val >= 0)
		pMeter->put_Reading(val);

	resetIndexList();

	return S_OK;
}

LRESULT SpeedFanMeterDialog::setIndex(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val=-1;
	indexList.getData(&val, -1);
	if (val >= 0)
		pMeter->put_Index(val);

	return S_OK;
}

LRESULT SpeedFanMeterDialog::scale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	double val = 1.0L;
	scaleList.getData(&val, 1.0L /* default value */);
	pMeter->put_Scale(val);

	return S_OK;
}

LRESULT SpeedFanMeterDialog::resetIndexList()
{

	long curReading = -1;
	readingList.getData(&curReading, -1 /* default value */);

	if (curReading != -1)
	{
		long curIndex = 0;
		pMeter->get_Index(&curIndex);
		long count = 0;
		pMeter->GetCount(curReading, &count);
		const char **indexNames = (const char**)malloc(sizeof(char*)*(count+1));
		long *indexData = (long*)malloc(sizeof(long)*(count));
		indexNames[0] = NULL;
		for (long i=0; i<count; i++)
		{
			indexData[i] = i;
			char buf[20];
			::ltoa(i, buf, 10);
			indexNames[i] = ::strdup(buf);
		}
		indexNames[i] = NULL;

		SendDlgItemMessage(IDC_COMBO_INDEX,CB_RESETCONTENT,0,0);
		indexList.init(m_hWnd, IDC_COMBO_INDEX, indexNames, indexData, curIndex);
		for (i=0; indexNames[i]; i++)
		{
			free((void*)(indexNames[i]));
		}
		free(indexNames);
		free(indexData);
	}

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

static const char* readingNames[] =
{
	"Temperature",
	"Fan",
	"Volts",
	NULL
};

static long readingData[] =
{
	0,
	1,
	2
};

/*
 * Called from the dialog proc when the dialog is being initialized.
 * Should initialize all of the controls using values from the meter.
 */
LRESULT SpeedFanMeterDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Build the reading pick list
	long reading;
	pMeter->get_Reading(&reading);
	readingList.init(m_hWnd, IDC_COMBO_READING, readingNames, readingData, reading);

	// Build the scale pick list
	double scale;
	pMeter->get_Scale(&scale);
	scaleList.init(m_hWnd, IDC_COMBO_SCALE, scaleLabels, scaleData, scale);

	resetIndexList();

	bHandled = FALSE;	// Make CWindow go through all the message maps.

	return 1;
}
