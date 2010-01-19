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

// ASyncScriptMeter.h : Declaration of the CASyncScriptMeter

#ifndef __ASyncScriptMETER_H_
#define __ASyncScriptMETER_H_

#include "COMMeters.h"       // main symbols
#include "resource.h"       // main symbols
#include "SysStatsMeterCategory.h"
#include "MeterImpl.h"

class ScriptThread;

/////////////////////////////////////////////////////////////////////////////
// CASyncScriptMeter
class ATL_NO_VTABLE CASyncScriptMeter :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CASyncScriptMeter, &CLSID_ASyncScriptMeter>,
	public IDispatchImpl<MeterImpl<IASyncScriptMeter>, &IID_IASyncScriptMeter, &LIBID_COMMETERSLib>
{
public:
	CASyncScriptMeter();
	virtual ~CASyncScriptMeter();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_REGISTRY_RESOURCEID(IDR_ASYNCSCRIPTMETER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CASyncScriptMeter)
	COM_INTERFACE_ENTRY(IASyncScriptMeter)
	COM_INTERFACE_ENTRY(IMeter)
	COM_INTERFACE_ENTRY(ITimerClient)
	COM_INTERFACE_ENTRY(IDispatch)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CASyncScriptMeter)
	IMPLEMENTED_CATEGORY(CATID_SysStatsMeterCategory)
END_CATEGORY_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IASyncScriptMeter
public:
	STDMETHOD(put_Value)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ScriptType)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ScriptType)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_FileName)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_FileName)(/*[in]*/ BSTR newVal);
	STDMETHOD(ShowConsole)(/*[in]*/ VARIANT_BOOL show);

// IMeter
	STDMETHOD(Update)(/*[out, retval]*/ long *dirty);
	STDMETHOD(Configure)(IObserver * observer, LONG hWnd);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(GetAsString)(/*[in]*/ BSTR format, /*[in]*/ BSTR selector, /*[out, retval]*/ BSTR *pRet);
	STDMETHOD(GetAsDouble)(/*[in]*/ BSTR selector, /*[out, retval]*/ double *pRet);
	STDMETHOD(GetAsLong)(/*[in]*/ BSTR selector, /*[out,retval]*/ long *pRet);
	STDMETHOD(GetValue)(/*[out,retval]*/ VARIANT *pRet);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel* newVal);

protected:
	ScriptThread *pThread;
	IUnknown *pWeakTarget;
};


#endif //__ASyncScriptMETER_H_
