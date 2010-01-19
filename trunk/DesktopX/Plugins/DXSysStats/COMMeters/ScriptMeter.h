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

// ScriptMeter.h : Declaration of the CScriptMeter

#ifndef __SCRIPTMETER_H_
#define __SCRIPTMETER_H_

#include "COMMeters.h"       // main symbols
#include "resource.h"       // main symbols
#include "SysStatsMeterCategory.h"
#include "MeterImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CScriptMeter
class ATL_NO_VTABLE CScriptMeter :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CScriptMeter, &CLSID_ScriptMeter>,
	public IDispatchImpl<MeterImpl<IScriptMeter>, &IID_IScriptMeter, &LIBID_COMMETERSLib>
{
public:
	CScriptMeter();
	virtual ~CScriptMeter();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_REGISTRY_RESOURCEID(IDR_SCRIPTMETER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScriptMeter)
	COM_INTERFACE_ENTRY(IScriptMeter)
	COM_INTERFACE_ENTRY(IMeter)
	COM_INTERFACE_ENTRY(ITimerClient)
	COM_INTERFACE_ENTRY(IDispatch)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CScriptMeter)
	IMPLEMENTED_CATEGORY(CATID_SysStatsMeterCategory)
END_CATEGORY_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScriptMeter
public:
	STDMETHOD(ShowConsole)(/*[in]*/ VARIANT_BOOL show) { return pImpl->ShowConsole(show); }
	STDMETHOD(put_Value)(/*[in]*/ BSTR newVal) { return pImpl->put_Value(newVal); }
	STDMETHOD(get_ScriptType)(/*[out, retval]*/ BSTR *pVal) { return pImpl->get_ScriptType(pVal); }
	STDMETHOD(put_ScriptType)(/*[in]*/ BSTR newVal) { return pImpl->put_ScriptType(newVal); }
	STDMETHOD(get_FileName)(/*[out, retval]*/ BSTR *pVal) { return pImpl->get_FileName(pVal); }
	STDMETHOD(put_FileName)(/*[in]*/ BSTR newVal) { return pImpl->put_FileName(newVal); }

// IMeter
	STDMETHOD(Update)(/*[out, retval]*/ long *dirty);
	STDMETHOD(Configure)(IObserver * observer, LONG hWnd);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(GetAsString)(/*[in]*/ BSTR format, /*[in]*/ BSTR selector, /*[out, retval]*/ BSTR *pRet);
	STDMETHOD(GetAsDouble)(/*[in]*/ BSTR selector, /*[out, retval]*/ double *pRet);
	STDMETHOD(GetAsLong)(/*[in]*/ BSTR selector, /*[out,retval]*/ long *pRet);
	STDMETHOD(GetValue)(/*[out,retval]*/ VARIANT *pRet);
	STDMETHOD(get_Model)(/*[out, retval]*/ IClientModel* *pVal);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel* newVal);

protected:
	IScriptMeterImpl *pImpl;
	IUnknown *pWeakTarget;
};


#endif //__SCRIPTMETER_H_
