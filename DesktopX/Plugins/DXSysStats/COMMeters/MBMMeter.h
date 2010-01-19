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

// MBMMeter.h : Declaration of the CMBMMeter

#ifndef __MBMMETER_H_
#define __MBMMETER_H_

#include "COMMeters.h"       // main symbols
#include "resource.h"       // main symbols
#include "SysStatsMeterCategory.h"
#include "MeterImpl.h"

namespace mbm {
	class index;
	class sensor;
};

/////////////////////////////////////////////////////////////////////////////
// CMBMMeter
class ATL_NO_VTABLE CMBMMeter : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMBMMeter, &CLSID_MBMMeter>,
	public ISupportErrorInfo,
	public IDispatchImpl<MeterImpl<IMBMMeter>, &IID_IMBMMeter, &LIBID_COMMETERSLib>
{
public:
	CMBMMeter();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_REGISTRY_RESOURCEID(IDR_MBMMETER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMBMMeter)
	COM_INTERFACE_ENTRY(IMBMMeter)
	COM_INTERFACE_ENTRY(IMeter)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CMBMMeter)
	IMPLEMENTED_CATEGORY(CATID_SysStatsMeterCategory)
END_CATEGORY_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IMBMMeter
public:
	STDMETHOD(get_Scale)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_Scale)(/*[in]*/ double newVal);
	STDMETHOD(get_Reading)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Reading)(/*[in]*/ long newVal);
	STDMETHOD(get_Index)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Index)(/*[in]*/ long newVal);

// IMeter
	STDMETHOD(Update)(/*[out, retval]*/ long *dirty);
	STDMETHOD(Configure)(IObserver * observer, LONG hWnd);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(GetAsString)(/*[in]*/ BSTR format, /*[in]*/ BSTR selector, /*[out, retval]*/ BSTR *pRet);
	STDMETHOD(GetAsDouble)(/*[in]*/ BSTR selector, /*[out, retval]*/ double *pRet);
	STDMETHOD(GetAsLong)(/*[in]*/ BSTR selector, /*[out,retval]*/ long *pRet);
	STDMETHOD(GetValue)(/*[out,retval]*/ VARIANT *pRet);

	mbm::index *getTypes();
	mbm::sensor *getSensors();

protected:
	// Sensor index
	int index;

	// Reading type
	int reading;

	double scale;
	IUnknown *pWeakTarget;
};

#endif //__MBMMETER_H_
