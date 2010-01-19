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

// ScriptUtils.h : Declaration of the CScriptUtils

#ifndef __SCRIPTUTILS_H_
#define __SCRIPTUTILS_H_

#include "resource.h"       // main symbols
#include "ArrayPtr.h"

template <class I>
class ArrayPtrCOM : public ArrayPtr<I>
{
public:
	virtual ~ArrayPtrCOM()
	{
		for (int c=0; c < count(); c++)
		{
			(*this)[c]->Release();
		}
	}
};

struct IScriptConnectionObject;

/////////////////////////////////////////////////////////////////////////////
// CScriptUtils
class ATL_NO_VTABLE CScriptUtils : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CScriptUtils, &CLSID_ScriptUtils>,
	public ISupportErrorInfo,
	public IDispatchImpl<IScriptUtils, &IID_IScriptUtils, &LIBID_SYSSTATSCOMLib>
{
public:
	CScriptUtils();
	virtual ~CScriptUtils();

DECLARE_REGISTRY_RESOURCEID(IDR_SCRIPTUTILS)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScriptUtils)
	COM_INTERFACE_ENTRY(IScriptUtils)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScriptUtils
public:
	STDMETHOD(get_ScriptHost)(/*[out, retval]*/ IDispatch* *pVal);
	STDMETHOD(put_ScriptHost)(/*[in]*/ IDispatch* newVal);
	STDMETHOD(Echo)(/*[in]*/ BSTR s);
	STDMETHOD(DisconnectAll)();
	STDMETHOD(Disconnect)(/*[in]*/ IDispatch *pdispObj);
	STDMETHOD(get_Script)(/*[out, retval]*/ IDispatch* *pVal);
	STDMETHOD(put_Script)(/*[in]*/ IDispatch* newVal);
	STDMETHOD(ConnectObject)(/*[in]*/ IDispatch*, /*[in]*/ BSTR);

protected:
	ArrayPtrCOM<IScriptConnectionObject> *connections;
	IDispatch *iScript;
	IDispatch *iScriptHost;
};

#endif //__SCRIPTUTILS_H_
