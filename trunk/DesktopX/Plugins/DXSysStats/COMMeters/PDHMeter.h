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

// PDHMeter.h : Declaration of the CPDHMeter

#ifndef __PDHMETER_H_
#define __PDHMETER_H_

#include "COMMeters.h"       // main symbols
#include "resource.h"       // main symbols
#include "SysStatsMeterCategory.h"
#include "CPdh.h"
#include "MeterImpl.h"
#include "MeterImpl.cpp"

/////////////////////////////////////////////////////////////////////////////
// CPDHMeter
class ATL_NO_VTABLE CPDHMeter : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPDHMeter, &CLSID_PDHMeter>,
	public IDispatchImpl<MeterImpl<IPDHMeter>, &IID_IPDHMeter, &LIBID_COMMETERSLib>
{
public:
	CPDHMeter();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_REGISTRY_RESOURCEID(IDR_PDHMETER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CPDHMeter)
	COM_INTERFACE_ENTRY(IPDHMeter)
	COM_INTERFACE_ENTRY(IMeter)
	COM_INTERFACE_ENTRY(IDispatch)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()


BEGIN_CATEGORY_MAP(CPDHMeter)
	IMPLEMENTED_CATEGORY(CATID_SysStatsMeterCategory)
END_CATEGORY_MAP()

// IPDHMeter
public:
	STDMETHOD(get_Counters)(/*[out, retval]*/ LPSAFEARRAY *pVal);
	STDMETHOD(put_Counters)(/*[in]*/ LPSAFEARRAY newVal);
	STDMETHOD(get_Scale)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_Scale)(/*[in]*/ double newVal);
// IMeter
	STDMETHOD(Update)(/*[out, retval]*/ long *dirty);
	STDMETHOD(Configure)(IObserver * observer, LONG hWnd);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(GetAsString)(/*[in]*/ BSTR format, /*[in]*/ BSTR selector, /*[out, retval]*/ BSTR *pRet);
	STDMETHOD(GetAsDouble)(/*[in]*/ BSTR selector, /*[out, retval]*/ double *pRet);
	STDMETHOD(GetAsLong)(/*[in]*/ BSTR selector, /*[out,retval]*/ long *pRet);
	STDMETHOD(GetValue)(/*[out,retval]*/ VARIANT *pRet);

	CPdh* getPDH() { return &pdh; }
	PDH_STATUS SetCounters(char *szFullCounterPath[], int count);

protected:
	double scale;

	CPdh pdh;	// Copying this by value won't work.
	IUnknown *pWeakTarget;
};

#endif //__PDHMETER_H_
