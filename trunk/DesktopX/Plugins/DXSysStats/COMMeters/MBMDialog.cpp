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

#include "mbm.hpp"
#include "MBMMeter.h"
#include "MBMDialog.h"

extern void ToValL(long *out, char *s);
extern void ToStrL(char *buf, int maxlen, long val);
extern void ToValD(double *out, char *s);
extern void ToStrD(char *buf, int maxlen, double val);

template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

MBMDialog::MBMDialog(CMBMMeter *pMeter, WORD idd) :
	pMeter(pMeter),
	typeList(Identity<long>, ToStrL, ToValL, /* fixed */ true),
	nameList(Identity<long>, ToStrL, ToValL, /* fixed */ true),
	readingList(Identity<long>, ToStrL, ToValL, /* fixed */ true),
	scaleList(Identity<double>, ToStrD, ToValD, /* fixed */ false),
	Dialog(idd)
{
}

MBMDialog::~MBMDialog()
{
}

LRESULT MBMDialog::setType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return resetInstanceList();
}

LRESULT MBMDialog::setName(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val=-1;
	nameList.getData(&val, -1);
	if (val >= 0)
		pMeter->put_Index(val);

	return S_OK;
}

LRESULT MBMDialog::setReading(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val=-1;
	readingList.getData(&val, -1);
	if (val >= 0)
		pMeter->put_Reading(val);

	return S_OK;
}

LRESULT MBMDialog::scale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	double val = 1.0L;
	scaleList.getData(&val, 1.0L /* default value */);
	pMeter->put_Scale(val);

	return S_OK;
}

LRESULT MBMDialog::resetInstanceList()
{

	long curType = -1;
	typeList.getData(&curType, -1 /* default value */);

	if ((curType != -1) && (offsets[curType].end >= offsets[curType].start))
	{
		long curIndex = 0;
		pMeter->get_Index(&curIndex);
		mbm::sensor *mbmSensor = pMeter->getSensors();
		if (mbmSensor == NULL)
			return E_INVALIDARG;

		int numSensors = offsets[curType].end - offsets[curType].start + 1;
		const char **sensorNames = (const char**)malloc(sizeof(char*)*(numSensors+1));
		long *sensorData = (long*)malloc(sizeof(long)*(numSensors));
		sensorNames[0] = NULL;
		int sI=0;
		std::string s;
		for (int i=offsets[curType].start; i<=offsets[curType].end; i++)
		{
			sensorData[sI] = i;
			s = mbmSensor[i].name();
			const char *s1 = s.c_str();
			sensorNames[sI++] = ::strdup(s1);
		}
		sensorNames[sI] = NULL;

		SendDlgItemMessage(IDC_COMBO_INSTANCE,CB_RESETCONTENT,0,0);
		nameList.init(m_hWnd, IDC_COMBO_INSTANCE, sensorNames, sensorData, curIndex);
		for (i=0; sensorNames[i]; i++)
		{
			free((void*)(sensorNames[i]));
		}
		free(sensorNames);
		free(sensorData);
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

static const char* typeNames[] =
{
	"Unknown",
	"Temperature",
	"Voltage",
	"Fan",
	"MHz",
	"Percentage",
	NULL
};

static const char* readingNames[] =
{
	"Current",
	"Low",
	"High",
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
 */
LRESULT MBMDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Build the types pick list
	mbm::index *mbmIndex = pMeter->getTypes();
	if (mbmIndex == NULL)
		return E_INVALIDARG;

	int numSensors = 0;
	int mbmType = 0;
	long curIndex = 0;
	pMeter->get_Index(&curIndex);
	long curTypeSel = 0;

	const char *orderedTypeNames[7];
	long typeData[6];
	for (int i=0; i<6; i++)
	{
		mbmType = mbmIndex[i].type();
		orderedTypeNames[i] = typeNames[mbmType];
		offsets[mbmType].start = numSensors;
		numSensors += mbmIndex[i].count();
		offsets[mbmType].end = numSensors - 1;
		typeData[i] = mbmType;
		if (curIndex >= offsets[mbmType].start && curIndex <= offsets[mbmType].end)
			curTypeSel = mbmType;
	}

	orderedTypeNames[6] = NULL;
	typeList.init(m_hWnd, IDC_COMBO_TYPE, orderedTypeNames, typeData, curTypeSel);

	// Build the instance pick list for the selected type
	resetInstanceList();

	// Build the reading pick list
	long reading;
	pMeter->get_Reading(&reading);
	readingList.init(m_hWnd, IDC_COMBO_READING, readingNames, readingData, reading);

	// Build the scale pick list
	double scale;
	pMeter->get_Scale(&scale);
	scaleList.init(m_hWnd, IDC_COMBO_SCALE, scaleLabels, scaleData, scale);

	bHandled = FALSE;	// Make CWindow go through all the message maps.

	return S_OK;
}
