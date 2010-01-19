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

// XMLMeter.h : Declaration of the CXMLMeter

#ifndef __XMLMETER_H_
#define __XMLMETER_H_

#include "COMMeters.h"       // main symbols
#include "resource.h"       // main symbols
#include "SysStatsMeterCategory.h"
#include "MeterImpl.h"

class UpdateThread;

/////////////////////////////////////////////////////////////////////////////
// CXMLMeter
class ATL_NO_VTABLE CXMLMeter : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CXMLMeter, &CLSID_XMLMeter>,
	public ISupportErrorInfo,
	public IDispatchImpl<MeterImpl<IXMLMeter>, &IID_IXMLMeter, &LIBID_COMMETERSLib>
{
public:
	CXMLMeter();
	virtual ~CXMLMeter();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_REGISTRY_RESOURCEID(IDR_XMLMETER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CXMLMeter)
	COM_INTERFACE_ENTRY(IXMLMeter)
	COM_INTERFACE_ENTRY(IMeter)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CXMLMeter)
	IMPLEMENTED_CATEGORY(CATID_SysStatsMeterCategory)
END_CATEGORY_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IXMLMeter
public:
	STDMETHOD(get_LoadFailed)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_LoadFailed)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(GetNodeList)(/*[in]*/ BSTR xpath, /*[out, retval]*/ IDispatch* *pNodelList);
	STDMETHOD(put_Value)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Namespaces)(/*[out, retval]*/ LPSAFEARRAY *pVal);
	STDMETHOD(put_Namespaces)(/*[in]*/ LPSAFEARRAY newVal);
	STDMETHOD(get_UseXPath)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_UseXPath)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_ProxyPassword)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ProxyPassword)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ProxyUsername)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ProxyUsername)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ProxyURL)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ProxyURL)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Password)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Password)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Username)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Username)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_URL)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_URL)(/*[in]*/ BSTR newVal);

// IMeter
	STDMETHOD(Update)(/*[out, retval]*/ long *dirty);
	STDMETHOD(Configure)(IObserver * observer, LONG hWnd);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(GetAsString)(/*[in]*/ BSTR format, /*[in]*/ BSTR selector, /*[out, retval]*/ BSTR *pRet);
	STDMETHOD(GetAsDouble)(/*[in]*/ BSTR selector, /*[out, retval]*/ double *pRet);
	STDMETHOD(GetAsLong)(/*[in]*/ BSTR selector, /*[out,retval]*/ long *pRet);
	STDMETHOD(GetValue)(/*[out,retval]*/ VARIANT *pRet);

	void put_dirty (bool dirty);

protected:
	UpdateThread *reader;
	IUnknown *pWeakTarget;
};

#endif //__XMLMETER_H_
