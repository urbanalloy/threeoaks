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

// AppConfig.cpp : Implementation of CAppConfig
#include "stdafx.h"
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include "SysStatsCOM.h"
#include "AppConfig.h"

const _bstr_t CAppConfig::APPLICATION_KEY = "\\Software\\SysStatsApp\\2.0";
const _bstr_t CAppConfig::INSTALL_DIR_KEY = "\\InstallDir";
const _bstr_t CAppConfig::CWD_KEY = "\\CWD";	// Relative to HKCU

/////////////////////////////////////////////////////////////////////////////
// CAppConfig

CAppConfig::CAppConfig() :
	applicationKey(APPLICATION_KEY),
	cwd(""),
	altRoot("")
{
	char _cwd[MAX_PATH+1]; 
	cwd = _getcwd(_cwd, MAX_PATH);
}

CAppConfig::~CAppConfig()
{
}

STDMETHODIMP CAppConfig::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IAppConfig
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/*
 * If the passed path is relative, convert it to an absolute path
 * starting with our root directory.
 */
STDMETHODIMP CAppConfig::CanonicalPath(BSTR path, BSTR *pRet)
{
	AtlTrace(L"CP %s\n", path);

	// Is path absolute?
	if (wcscspn(path, L"\\") == 0 || wcscspn(path, L":") == 1 || wcsstr(path, L"http://") == path)
	{
		*pRet = ::SysAllocString(path);
		AtlTrace(L"CPath0=%s\n", path);
		return S_OK;
	}

	// Path is relative - check we have an absolute path defined somewhere
	BSTR rootPath;
	if (!SUCCEEDED(get_RootPath(&rootPath)) && !SUCCEEDED(get_CWD(&rootPath)))
	{
		*pRet = ::SysAllocString(path);
		AtlTrace(L"CPath1=%s\n", path);
		return S_OK;
	}

	// Prepend the root path (which *has* to be absolute).
	size_t len = wcslen(rootPath);
	wchar_t tmp[MAX_PATH];
	wcscpy(tmp, rootPath);

	if (rootPath[len-1] != L'\\')
		wcscat(tmp, L"\\");
	wcscat(tmp, path);

	::SysFreeString(rootPath);

	AtlTrace(L"CPath2=%s\n", tmp);
	*pRet = ::SysAllocString(tmp);

	return S_OK;
}

/*
 * If the passed path is contained within the root directory, return
 * a relative path, otherwise return an absolute path.
 */
STDMETHODIMP CAppConfig::NormalizePath(BSTR path, BSTR *pRet)
{
	_bstr_t path_t = path;
	AtlTrace("NP %s\n", (char*)path_t);
	BSTR rootPath;
	if (!SUCCEEDED(get_RootPath(&rootPath)) && !SUCCEEDED(get_CWD(&rootPath)))
	{
		*pRet = ::SysAllocString(path);
		AtlTrace("Path1=%s\n", (char*)path_t);
		return S_OK;
	}

	AtlTrace(L"rootPath = %s\n", rootPath);

	size_t len = wcslen(rootPath);
	
	if (rootPath[len-1] == L'\\')
	{
		rootPath[len-1] = 0;
		len = len-1;
	}

	if (wcsnicmp(path, rootPath, len) == 0)
	{
		AtlTrace("Path2a\n");
		*pRet = ::SysAllocString(path+len+1);
		AtlTrace("Path3=%s\n", (char*)_bstr_t(*pRet));
	}
	else
	{
		AtlTrace("Path3\n");
		*pRet = ::SysAllocString(path);
	}

	::SysFreeString(rootPath);

	AtlTrace("Path4\n");
	return S_OK;
}

/*
 * Root path is a derived value depending on what keys are in the
 * registry. If InstallDir is defined then it is used, otherwise
 * CWD is used.
 */
STDMETHODIMP CAppConfig::get_RootPath(BSTR *pVal)
{
	if (!installDir)
		return ReadString(HKEY_LOCAL_MACHINE, applicationKey, INSTALL_DIR_KEY, pVal);
	else
		return get_InstallDir(pVal);
}

STDMETHODIMP CAppConfig::get_CWD(BSTR *pVal)
{
	*pVal = cwd.copy();

	return S_OK;
}

STDMETHODIMP CAppConfig::put_CWD(BSTR newVal)
{
	cwd = newVal;

	return S_OK;
}

STDMETHODIMP CAppConfig::ReadString(HKEY base, BSTR path, BSTR key, BSTR *pRet)
{
	HKEY keyHandle;
	_bstr_t path_t = path;
	HRESULT ret = E_FAIL;

	if (RegOpenKeyEx(base, // e.g. HKCU
			path_t,	// e.g. APPLICATION_KEY
			0, 
			KEY_READ, 
			&keyHandle)==ERROR_SUCCESS)
	{
		DWORD type;
		DWORD size = 0;
		_bstr_t key_t = key;

		RegQueryValueEx(keyHandle,
			key_t,	// e.g. CWD
			NULL, 
			&type,	// out value, e.g.: REG_SZ
			NULL, 
			(LPDWORD) &size);

		TCHAR *pStr = new TCHAR[size];
		if (RegQueryValueEx(keyHandle, key_t, NULL, &type, (BYTE*) pStr,(LPDWORD) &size)==ERROR_SUCCESS)
		{
			_bstr_t tmp = pStr;
			*pRet = tmp.copy();
			delete[] pStr;
			ret = S_OK;
		}

		delete[] pStr;
		RegCloseKey(keyHandle);
	}

	return ret;
}

STDMETHODIMP CAppConfig::get_ApplicationKey(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = applicationKey.copy();

	return S_OK;
}

STDMETHODIMP CAppConfig::put_ApplicationKey(BSTR newVal)
{
	applicationKey = newVal;

	return S_OK;
}

STDMETHODIMP CAppConfig::get_InstallDir(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = installDir.copy();

	return S_OK;
}

STDMETHODIMP CAppConfig::put_InstallDir(BSTR newVal)
{
	installDir = newVal;

	return S_OK;
}

STDMETHODIMP CAppConfig::get_AltRoot(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = altRoot.copy();

	return S_OK;
}

STDMETHODIMP CAppConfig::put_AltRoot(BSTR newVal)
{
	altRoot = newVal;

	return S_OK;
}

STDMETHODIMP CAppConfig::AltCanonicalPath(BSTR path, BSTR *pRet)
{
	_bstr_t path_t = path;

	if (altRoot == _bstr_t(""))
	{
		*pRet = ::SysAllocString(path);
		AtlTrace("AltCPath1=%s\n", (char*)path_t);
	}
	else
	{
		if (wcscspn(path, L"\\") == 0 || wcscspn(path, L":") == 1)
		{
			*pRet = ::SysAllocString(path);
			AtlTrace("AltCPath0=%s\n", (char*)path_t);
			return S_OK;
		}

		// Prepend the root path (which *has* to be absolute).
		BSTR _altRoot = altRoot;
		size_t len = wcslen(_altRoot);
		_bstr_t tmp = _altRoot;
		if (_altRoot[len-1] != L'\\')
			tmp = tmp + _bstr_t(L"\\");
		tmp = tmp + path_t;

		AtlTrace("AltCPath2=%s\n", (char*)tmp);
		*pRet = tmp.copy();
	}

	return S_OK;
}
