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

// FixedMeter.h : Declaration of the CFixedMeter

#ifndef __FIXEDMETER_H_
#define __FIXEDMETER_H_

#include "COMMeters.h"       // main symbols
#include "resource.h"       // main symbols
#include "SysStatsMeterCategory.h"
#include "MeterImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CFixedMeter
class ATL_NO_VTABLE CFixedMeter : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFixedMeter, &CLSID_FixedMeter>,
	public ISupportErrorInfo,
	public IDispatchImpl<MeterImpl<IFixedMeter>, &IID_IFixedMeter, &LIBID_COMMETERSLib>
{
public:
	CFixedMeter();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_REGISTRY_RESOURCEID(IDR_FIXEDMETER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CFixedMeter)
	COM_INTERFACE_ENTRY(IFixedMeter)
	COM_INTERFACE_ENTRY(IMeter)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CFixedMeter)
	IMPLEMENTED_CATEGORY(CATID_SysStatsMeterCategory)
END_CATEGORY_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IFixedMeter
public:
	STDMETHOD(get_Value)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Value)(/*[in]*/ BSTR newVal);

// IMeter
	STDMETHOD(Update)(/*[out, retval]*/ long *dirty);
	STDMETHOD(Configure)(IObserver * observer, LONG hWnd);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(GetAsString)(/*[in]*/ BSTR format, /*[in]*/ BSTR selector, /*[out, retval]*/ BSTR *pRet);
	STDMETHOD(GetAsDouble)(/*[in]*/ BSTR selector, /*[out, retval]*/ double *pRet);
	STDMETHOD(GetAsLong)(/*[in]*/ BSTR selector, /*[out,retval]*/ long *pRet);
	STDMETHOD(GetValue)(/*[out,retval]*/ VARIANT *pRet);

protected:
	_bstr_t value;
	IUnknown *pWeakTarget;
};

#endif //__FIXEDMETER_H_
