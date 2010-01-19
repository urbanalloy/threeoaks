#pragma once

#include <windows.h>
#include <commdlg.h>
#include <GdiplusH.h>

template <class I> class ArrayPtr;
class Meter;

class App : public OPENFILENAME
{
public:
	App(HWND hOwner);

	char *getAppName();
	void setAppName(char *appName);
	int getAppMeterIndex();
	void setAppMeterIndex(int index);
	char *getAppSelector();
	void setAppSelector(char*);

	char *getCommandLine();
	void setCommandLine(char *cmd);

	char *getStartDirectory();
	void setStartDirectory(char *dir);

	char *getHomeDirectory();
	void setHomeDirectory(char *dir);

	void setHWND(HWND hwnd);
	BOOL execute(ArrayPtr<Meter> *meters);

	virtual void load(const char *szIni, const char *szIniGroup, char *prefix);
	virtual void save(const char *szIni, const char *szIniGroup, BOOL bIsForExport, char *prefix);

private:
	void init();

	char app[MAX_PATH];
	int meterIndex;
	char selector[MAX_PATH];

	char cmdLine[MAX_PATH];
	char directory[MAX_PATH];
	char homeDirectory[MAX_PATH];

	static const char* NAME;
	static const char* METER_INDEX;
	static const char* SELECTOR;
	static const char* ARGS;
	static const char* START_IN;
};