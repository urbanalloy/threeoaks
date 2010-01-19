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

// ScriptConnectionObject.h : Declaration of the CScriptConnectionObject

#ifndef __SCRIPTCONNECTIONOBJECT_H_
#define __SCRIPTCONNECTIONOBJECT_H_

#include "resource.h"       // main symbols
#include <comutil.h>

/////////////////////////////////////////////////////////////////////////////
// CScriptConnectionObject
//
// Implements some WScript-like functionality. specifically ConnectObject. The
// key here is that it overrides IDispatch::Invoke!
//
class ATL_NO_VTABLE CScriptConnectionObject : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CScriptConnectionObject, &CLSID_ScriptConnectionObject>,
	public ISupportErrorInfo,
	public IDispatchImpl<IScriptConnectionObject, &IID_IScriptConnectionObject, &LIBID_SYSSTATSCOMLib>
{
public:
	CScriptConnectionObject();
	~CScriptConnectionObject();

DECLARE_REGISTRY_RESOURCEID(IDR_SCRIPTCONNECTIONOBJECT)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScriptConnectionObject)
	COM_INTERFACE_ENTRY(IScriptConnectionObject)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_FUNC_BLIND(0, Delegator)
END_COM_MAP()

    static HRESULT WINAPI Delegator(void* pv, REFIID riid, LPVOID* ppv, DWORD dw)
    {
		CScriptConnectionObject *pObj = (CScriptConnectionObject*)pv;
		if (pObj->iid == riid)
		{
			pObj->QueryInterface(IID_IDispatch, ppv);

//			((IDispatch*)*ppv)->Release();
			return S_OK;
		}
		else
			return S_FALSE;
    }

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScriptConnectionObject
public:

// IDispatch
public:
	STDMETHOD(Invoke)(DISPID nDispID, REFIID rIID, LCID oLCID,WORD wFlags, DISPPARAMS * pdp, VARIANT * pvarRes,
								  EXCEPINFO * pexcepinfo, UINT * perr);

// IUnknown
public:
//	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject);

// Non-COM methods
public:
	STDMETHOD(IsConnectionFor)(/*[in]*/ IDispatch* pObj, /*[out]*/ VARIANT_BOOL *pRet);
	STDMETHOD(get_Script)(/*[out, retval]*/ IDispatch* *pVal);
	STDMETHOD(put_Script)(/*[in]*/ IDispatch* newVal);
	STDMETHOD(Connect)(IDispatch *pdispObj, BSTR bstrPrefix);
	STDMETHOD(Disconnect)();

protected:
	STDMETHOD(SetSourceTypeInfo)(IDispatch *pdispObj);

	DWORD dwCookie;					// Connection ID
	_bstr_t prefix;					// Prefix of functions in script
	IConnectionPoint *pConnection;	// Yer actual event source
	IDispatch *pObj;				// Cache the original IDispatch
	ITypeInfo *typeInfo;			// TypeInfo of event source
	IDispatch *iScript;				// The script's dispatch interface
	IID iid;						// IID of the event sink interface we are supposed to be implementing
};

#endif //__SCRIPTCONNECTIONOBJECT_H_
