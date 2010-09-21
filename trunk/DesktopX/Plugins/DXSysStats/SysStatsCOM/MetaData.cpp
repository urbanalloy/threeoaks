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

// MetaData.cpp : Implementation of CMetaData
#include "stdafx.h"
#include "SysStatsCOM.h"
#include "MetaData.h"
#include "SysStatsUtils.h"

/////////////////////////////////////////////////////////////////////////////
// CMetaData

STDMETHODIMP CMetaData::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IMetaData
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CMetaData::GetMetaData(BSTR *retVal)
{
	HRESULT res = E_NOTIMPL;

	HRSRC hrSrc = ::FindResource(hModule, resourceName, "XML");
	if (hrSrc)
	{
		HGLOBAL hGlobal = ::LoadResource(hModule, hrSrc);
		if (hGlobal)
		{
			void *prSrc = ::LockResource(hGlobal);
			DWORD size = ::SizeofResource(hModule, hrSrc);
			char *pbuf = (char*)malloc(size+1);
			::memcpy(pbuf, prSrc, size);
			pbuf[size] = 0;
			*retVal = _bstr_t(pbuf).copy();
			free(pbuf);
			::FreeResource(hGlobal);
			res = S_OK;
		}
	}
	else
	{
		*retVal = resourceName.copy();
		res = S_OK;
	}


	if (!SUCCEEDED(res))
		SysStatsUtils::checkError();

	return S_OK;
}

STDMETHODIMP CMetaData::PutResourceInfo(long hModule, BSTR name)
{
	resourceName = name;
	this->hModule = (HMODULE)hModule;

	return S_OK;
}
