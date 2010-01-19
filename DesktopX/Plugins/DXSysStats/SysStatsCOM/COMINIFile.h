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

// COMINIFile.h : Declaration of the CCOMINIFile

#ifndef __COMINIFILE_H_
#define __COMINIFILE_H_

#include "resource.h"       // main symbols
#include "IniFile.h"

/////////////////////////////////////////////////////////////////////////////
// CCOMINIFile
class ATL_NO_VTABLE CCOMINIFile : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMINIFile, &CLSID_COMINIFile>,
	public ISupportErrorInfo,
	public IDispatchImpl<ICOMINIFile, &IID_ICOMINIFile, &LIBID_SYSSTATSCOMLib>
{
public:
	CCOMINIFile()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_COMINIFILE)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCOMINIFile)
	COM_INTERFACE_ENTRY(ICOMINIFile)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// ICOMINIFile
public:
	STDMETHOD(PutLong)(/*[in]*/ BSTR sectionName, /*[in]*/ BSTR key, /*[in]*/ long value);
	STDMETHOD(GetLong)(/*[in]*/ BSTR sectionName, /*[in]*/ BSTR key, /*[in]*/ long defValue, /*[out,retval]*/ long *pVal);
	STDMETHOD(GetDouble)(/*[in]*/ BSTR sectionName, /*[in]*/ BSTR key, /*[in]*/ double defValue, /*[out,retval]*/ double *pVal);
	STDMETHOD(PutDouble)(/*[in]*/ BSTR sectionName, /*[in]*/ BSTR key, /*[in]*/ double value);
	STDMETHOD(Read)(/*[in]*/ BSTR filename);
	STDMETHOD(Save)(/*[in]*/ BSTR filename);
	STDMETHOD(GetString)(/*[in]*/ BSTR sectionName, /*[in]*/ BSTR key, /*[in]*/ BSTR defValue, /*[out,retval]*/ BSTR *pVal);
	STDMETHOD(GetInt)(/*[in]*/ BSTR sectionName, /*[in]*/ BSTR key, /*[in]*/ int defValue, /*[out,retval]*/ int *pVal);
	STDMETHOD(PutString)(/*[in]*/ BSTR sectionName, /*[in]*/ BSTR key, /*[in]*/ BSTR value);
	STDMETHOD(PutInt)(/*[in]*/ BSTR sectionName, /*[in]*/ BSTR key, /*[in]*/ int value);

protected:
	CIniFile iniFile;
};

#endif //__COMINIFILE_H_
