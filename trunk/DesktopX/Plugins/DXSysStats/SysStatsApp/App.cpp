#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include "ArrayPtr.h"
#include "ArrayPtr.cpp"
#include "Meter.h"
#include "App.h"

const char* App::NAME = "Name";
const char* App::METER_INDEX = "MeterIndex";
const char* App::SELECTOR = "Selector";
const char* App::ARGS = "Args";
const char* App::START_IN = "StartIn";

App::App(HWND hwnd)
{
	init();
	hwndOwner = hwnd;
}

void App::setHWND(HWND hwnd)
{
	hwndOwner = hwnd;
}

char *App::getAppName()
{
	return app;
}

void App::setAppName(char *appName)
{
	strncpy(app, appName, sizeof(app));
}

char *App::getAppSelector()
{
	return selector;
}

void App::setAppSelector(char *appSelector)
{
	strncpy(selector, appSelector, sizeof(selector));
}

int App::getAppMeterIndex()
{
	return meterIndex;
}

void App::setAppMeterIndex(int index)
{
	meterIndex = index;
}

char *App::getCommandLine()
{
	return cmdLine;
}

void App::setCommandLine(char *cmd)
{
	strncpy(cmdLine, cmd, sizeof(cmdLine));
}

char *App::getStartDirectory()
{
	return directory;
}

void App::setStartDirectory(char *dir)
{
	strncpy(directory, dir, sizeof(directory));
}

char *App::getHomeDirectory()
{
	return homeDirectory;
}

void App::setHomeDirectory(char *dir)
{
	strncpy(homeDirectory, dir, sizeof(homeDirectory));
}

void App::init()
{
    memset((OPENFILENAME*)this,0,sizeof(OPENFILENAME));   
    lStructSize=sizeof(OPENFILENAME);
	*app = 0;
	*selector = 0;
	meterIndex = -1;
	*cmdLine = 0;
	*directory = 0;
	*homeDirectory = 0;
	lpstrFile = app;
	nMaxFile = sizeof(app);
	lpstrTitle = "Choose application to run";
	Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
}

BOOL App::execute(ArrayPtr<Meter> *meters)
{
	if (*getAppName() != 0) 
	{
		Meter *pMeter = (*meters)[getAppMeterIndex()];
		char buf[MAX_PATH];
		*buf=0;

		if (pMeter)
			pMeter->getAsString(getAppName(), buf, sizeof(buf), getAppSelector());
		else
			strncpy(buf, getAppName(), sizeof(buf));

		return (int)ShellExecute(hwndOwner,
			"open",
			buf,
			getCommandLine(),
			getStartDirectory(),
			SW_SHOWNORMAL) > 32;
	}

	return FALSE;
}

void App::load(const char *szIni, const char *szIniGroup, char *prefix)
{
	char buf[32];
	int offset = strlen(prefix);
	strcpy(buf, prefix);

	strcpy(buf+offset, NAME);
	GetPrivateProfileString(szIniGroup, buf, "", app, sizeof(app), szIni);

	strcpy(buf+offset, METER_INDEX);
	setAppMeterIndex(GetPrivateProfileInt(szIniGroup, buf, getAppMeterIndex(), szIni));

	strcpy(buf+offset, SELECTOR);
	GetPrivateProfileString(szIniGroup, buf, "", selector, sizeof(selector), szIni);

	strcpy(buf+offset, ARGS);
	GetPrivateProfileString(szIniGroup, buf, "", cmdLine, sizeof(cmdLine), szIni);

	strcpy(buf+offset, START_IN);
	GetPrivateProfileString(szIniGroup, buf, "", directory, sizeof(directory), szIni);
}

/*
 * Called when we need to persist our state.
 */
void App::save(const char *szIni, const char *szIniGroup, BOOL bIsForExport, char *prefix)
{
	char buf[32];
	char iBuf[32];
	int offset = strlen(prefix);
	strcpy(buf, prefix);

	strcpy(buf+offset, NAME);
	WritePrivateProfileString(szIniGroup, buf, getAppName(), szIni);

	strcpy(buf+offset, METER_INDEX);
	WritePrivateProfileString(szIniGroup, buf, itoa(getAppMeterIndex(), iBuf, 10), szIni);

	strcpy(buf+offset, SELECTOR);
	WritePrivateProfileString(szIniGroup, buf, getAppSelector(), szIni);

	strcpy(buf+offset, ARGS);
	WritePrivateProfileString(szIniGroup, buf, getCommandLine(), szIni);

	strcpy(buf+offset, START_IN);
	WritePrivateProfileString(szIniGroup, buf, getStartDirectory(), szIni);
}
