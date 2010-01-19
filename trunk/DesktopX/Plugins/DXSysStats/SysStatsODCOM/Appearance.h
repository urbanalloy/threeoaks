#pragma once

#import "SysStatsCOM.tlb" no_namespace raw_interfaces_only

#include <commdlg.h>

#define APPEARANCE_LABEL_SIZE 64

template <class I> class ArrayPtr;
class Meter;

class Appearance
{
public:
	Appearance();

	char *getLabelFormat();
	void setLabelFormat(char *labelFormat);

	void setLabelMeterIndex(int index);
	int getLabelMeterIndex();

	void setLabelSelector(char *selector);
	char *getLabelSelector();

	const char* getLabel(char *out, int size, IMeterHome *meters);

	void setHWND(HWND hwnd);
	HWND getHWND();

	virtual void Configure(IObserver * observer, IMeterHome * meters, LONG hDlg);
	virtual void load(char *szIni, char *szIniGroup, char *prefix);
	virtual void save(char *szIni, char *szIniGroup, BOOL bIsForExport, char *prefix);

private:
	void init();

	char labelFormat[APPEARANCE_LABEL_SIZE];
	char labelSelector[MAX_PATH];

	int labelMeterIndex;

	HWND hwndDocklet;

	static const char *LABEL_FORMAT;
	static const char *LABEL_SELECTOR;
	static const char *LABEL_METER;
	static const char *LABEL_WIDTH;
	static const char *LABEL_HEIGHT;

	static const char *CONFIG_GENERAL;
};