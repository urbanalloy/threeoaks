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
#include "SysStatsCOM.h"

#include <windows.h>
#include <io.h>
#include <direct.h>
#include <comdef.h>
#include "Shlwapi.h"

#include "PluginManager.h"
#include "AppConfig.h"
#include "SysStatsUtils.h"

typedef HRESULT (*DLLFunc)();

SysStatsUtils::CriticalSection PluginManager::cs;
SysStatsUtils::CriticalSection PluginManager::loadCS;
PluginManager* PluginManager::manager;

PluginManager *PluginManager::getManager()
{
	SysStatsUtils::Lock lock(cs);

	if (!manager)
		manager = new PluginManager;

	return manager;
}

void PluginManager::destroyManager()
{	
	SysStatsUtils::Lock lock(cs);

	if (manager)
	{
		delete manager;
		manager = NULL;
	}
}

PluginManager::PluginManager() : appConfig(0)
{
	AtlTrace("PluginManager::PluginManager\n");
	strcpy(pluginDir, "plugins");
	strcpy(searchPattern, pluginDir);
	strcat(searchPattern, "\\*.spl");
}

PluginManager::~PluginManager()
{
	AtlTrace("PluginManager::~PluginManager\n");

	if (appConfig)
		appConfig->Release();

	files.clearAndDestroy();
}

bool PluginManager::load(const char *file)
{
	SysStatsUtils::Lock lock(loadCS);

	AtlTrace("PluginManager::load - %s\n", file);
	if (GetModuleHandle(file) == NULL)
	{
		HINSTANCE hDLL = LoadLibrary(file);
		if (hDLL)
		{
			DLLFunc pFunc = (DLLFunc)GetProcAddress(hDLL, "DllRegisterServer");
			if (pFunc)
			{
				// call the function
				AtlTrace("Registering server for %s\n", file);
				HRESULT hr = pFunc();
				if (!SUCCEEDED(hr))
				{
					AtlTrace("RegisterServer failed=%x\n", hr);
					SysStatsUtils::checkError();
				}
				else
					return true;
			}
		}
		else
		{
			SysStatsUtils::checkError();
		}
	}
	else
	{
		AtlTrace("\tAlready loaded\n");
	}

	return false;
}

bool PluginManager::load()
{
	AtlTrace(L"PluginManager::load\n");
	return findFiles();
}

STDMETHODIMP PluginManager::put_AppConfig(IAppConfig *newVal)
{
	newVal->AddRef();

	if (appConfig)
		appConfig->Release();

	appConfig = newVal;

	return S_OK;
}

bool PluginManager::findFiles()
{
	bool newLoad = false;
	WIN32_FIND_DATA fileData;
	BSTR nPattern;
	appConfig->CanonicalPath(_bstr_t(searchPattern), &nPattern);
	char absPath[MAX_PATH];
	strcpy(absPath, _bstr_t(nPattern, false));
	HANDLE searchHandle = FindFirstFile(absPath, &fileData);
	::PathRemoveFileSpec(absPath);
	if (searchHandle != INVALID_HANDLE_VALUE)
	{
		char buf[MAX_PATH];
		BOOL notDone = FALSE;
		do
		{
			AtlTrace("PluginManager::findFiles - %s\n", fileData.cFileName);
			strcpy(buf, absPath);
			strcat(buf, "\\");
			strcat(buf, fileData.cFileName);
			newLoad = newLoad || load(buf);
			if (load(buf))
			{
				files.add(strdup(buf));
				newLoad = true;
			}
			notDone = FindNextFile(searchHandle, &fileData);
		} while (notDone);

		FindClose(searchHandle);
	}

	return newLoad;
}

