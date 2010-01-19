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

// COMINIFile.cpp : Implementation of CCOMINIFile
#include "stdafx.h"
#include "SysStatsCOM.h"
#include "COMINIFile.h"

/////////////////////////////////////////////////////////////////////////////
// CCOMINIFile

STDMETHODIMP CCOMINIFile::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ICOMINIFile
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CCOMINIFile::PutString(BSTR sectionName, BSTR key, BSTR value)
{
	iniFile.SetValue(sectionName, key, value);

	return S_OK;
}

STDMETHODIMP CCOMINIFile::GetInt(BSTR sectionName, BSTR key, int defValue, int *pVal)
{
	*pVal = iniFile.GetInt(sectionName, key, defValue);

	return S_OK;
}

STDMETHODIMP CCOMINIFile::PutInt(BSTR sectionName, BSTR key, int value)
{
	iniFile.SetValue(sectionName, key, value);

	return S_OK;
}

STDMETHODIMP CCOMINIFile::GetString(BSTR sectionName, BSTR key, BSTR defValue, BSTR *pVal)
{
	*pVal = iniFile.GetBSTR(sectionName, key, defValue);

	return S_OK;
}

STDMETHODIMP CCOMINIFile::Save(BSTR filename)
{
	iniFile.Save(filename);

	return S_OK;
}

STDMETHODIMP CCOMINIFile::Read(BSTR filename)
{
	iniFile.Read(filename);

	return S_OK;
}

STDMETHODIMP CCOMINIFile::PutDouble(BSTR sectionName, BSTR key, double value)
{
	iniFile.SetValue(sectionName, key, value);

	return S_OK;
}

STDMETHODIMP CCOMINIFile::GetDouble(BSTR sectionName, BSTR key, double defValue, double *pVal)
{
	*pVal = iniFile.GetDouble(sectionName, key, defValue);

	return S_OK;
}

STDMETHODIMP CCOMINIFile::GetLong(BSTR sectionName, BSTR key, long defValue, long *pVal)
{
	*pVal = iniFile.GetLong(sectionName, key, defValue);

	return S_OK;
}

STDMETHODIMP CCOMINIFile::PutLong(BSTR sectionName, BSTR key, long value)
{
	iniFile.SetValue(sectionName, key, value);

	return S_OK;
}
