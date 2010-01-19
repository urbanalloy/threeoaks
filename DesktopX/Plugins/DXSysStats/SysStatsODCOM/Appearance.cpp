#include "stdafx.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
// #include <shlwapi.h>
// #include <shlobj.h>

#include "SysStatsUtils.h"
#include "Appearance.h"
#include "AppearanceDialog.h"
#include "ArrayPtr.h"
#include "ArrayPtr.cpp"
// #include "Meter.h"

#include "DockletSDK.h"

const char *Appearance::LABEL_FORMAT = "LabelFormat";
const char *Appearance::LABEL_SELECTOR = "LabelSelector";
const char *Appearance::LABEL_METER = "LabelMeter";
const char *Appearance::LABEL_WIDTH = "ImageWidth";
const char *Appearance::LABEL_HEIGHT = "ImageHeight";

Appearance::Appearance()
{
	init();
}

void Appearance::init()
{
	setLabelFormat("SysStats");
	*labelSelector = 0;
	labelMeterIndex = -1;
}

void Appearance::setLabelFormat(char *_labelFormat)
{
	strncpy(labelFormat, _labelFormat, sizeof(labelFormat));
}

char *Appearance::getLabelFormat()
{
	return labelFormat;
}

void Appearance::setLabelSelector(char *_labelSelector)
{
	strncpy(labelSelector, _labelSelector, sizeof(labelSelector));
}

char *Appearance::getLabelSelector()
{
	return labelSelector;
}

/*
 * Called from the main docklet class.
 */
void Appearance::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	AppearanceDialog *pDialog = new AppearanceDialog(this, observer, meters);
	pDialog->DoModal((HWND)hDlg);
	delete pDialog;
}

void Appearance::setHWND(HWND hwnd)
{
	hwndDocklet = hwnd;
}

HWND Appearance::getHWND()
{
	return hwndDocklet;
}

static void convertFormat(char *dest, int size)
{
	// Handle legacy configs
	char *buf = new char[size];
	*buf = 0;
	char *start = dest;
	char *pPercent;
	while (pPercent = strstr(start, "%ld"))
	{
		strncat(buf, start, (pPercent-start));
		strcat(buf, "%.0f");
		start = pPercent + 3;
	}
	strcat(buf, start);
	strncpy(dest, buf, size);
	delete []buf;
}

void Appearance::load(char *szIni, char *szIniGroup, char *prefix)
{
	GetPrivateProfileString(szIniGroup, LABEL_FORMAT, "", labelFormat, sizeof(labelFormat), szIni);
	convertFormat(labelFormat, sizeof(labelFormat));

	GetPrivateProfileString(szIniGroup, LABEL_SELECTOR, "", labelSelector, sizeof(labelSelector), szIni);

	setLabelMeterIndex(GetPrivateProfileInt(szIniGroup, LABEL_METER, -1, szIni));
}

void Appearance::setLabelMeterIndex(int index)
{
	this->labelMeterIndex = index;
}

int Appearance::getLabelMeterIndex()
{
	return labelMeterIndex;
}

const char* Appearance::getLabel(char *out, int size, IMeterHome *meters)
{
	IMeter *pMeter = 0;
	meters->Item(CComVariant(labelMeterIndex), &pMeter);

	if (pMeter)
	{
		BSTR buf = 0;

		pMeter->GetAsString(_bstr_t(getLabelFormat()), _bstr_t(getLabelSelector()), &buf);
		strncpy(out, _bstr_t(buf, false), size);
	}
	else
		strncpy(out, getLabelFormat(), size);

	return out;
}

/*
 * Called when we need to persist our state.
 */
void Appearance::save(char *szIni, char *szIniGroup, BOOL bIsForExport, char *prefix)
{
	char iBuf[32];

	AtlTrace("Writing label to %s\n", szIni);

	WritePrivateProfileString(szIniGroup, LABEL_FORMAT, labelFormat, szIni);
	WritePrivateProfileString(szIniGroup, LABEL_SELECTOR, labelSelector, szIni);
	WritePrivateProfileString(szIniGroup, LABEL_METER, itoa(getLabelMeterIndex(), iBuf, 10), szIni);
}
