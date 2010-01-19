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

// MetaData.h : Declaration of the CMetaData

#ifndef __METADATA_H_
#define __METADATA_H_

#include "resource.h"       // main symbols
#include <ComUtil.h>

/////////////////////////////////////////////////////////////////////////////
// CMetaData
class ATL_NO_VTABLE CMetaData : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMetaData, &CLSID_MetaData>,
	public ISupportErrorInfo,
	public IDispatchImpl<IMetaData, &IID_IMetaData, &LIBID_SYSSTATSCOMLib>
{
public:
	CMetaData() : resourceName("")
	{
	}

	CMetaData(LPCTSTR resourceName) : resourceName(resourceName)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_METADATA)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMetaData)
	COM_INTERFACE_ENTRY(IMetaData)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IMetaData
public:
	STDMETHOD(PutResourceInfo)(/*[in]*/ long hModule, /*[in]*/ BSTR name);
	STDMETHOD(GetMetaData)(/*[out, retval]*/ BSTR *retVal);

protected:
	HMODULE hModule;
	_bstr_t resourceName;
};

#endif //__METADATA_H_
