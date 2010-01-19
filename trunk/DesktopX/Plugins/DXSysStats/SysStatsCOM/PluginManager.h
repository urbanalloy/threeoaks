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

#pragma once
#include "ArrayPtr.h"
#include "ArrayPtr.cpp"
#include "SysStatsUtils.h"

struct IAppConfig;

class PluginManager
{
public:
	virtual ~PluginManager();

	static PluginManager *getManager();
	static void destroyManager();

	bool load();

	STDMETHOD(put_AppConfig)(IAppConfig *appConfig);
	char *getSearchPattern() { return searchPattern; }
	void setSearchPatterm(char *pattern) { strcpy(searchPattern, pattern); }

protected:
	bool load(const char *);
	bool findFiles();

private:
	PluginManager();

	static PluginManager *manager;

	char searchPattern[MAX_PATH];
	char pluginDir[MAX_PATH];

	IAppConfig *appConfig;
	ArrayPtr<char> files;

	static SysStatsUtils::CriticalSection cs;
	static SysStatsUtils::CriticalSection loadCS;
};