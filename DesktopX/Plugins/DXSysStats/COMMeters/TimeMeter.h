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

// TimeMeter.h : Declaration of the CTimeMeter

#ifndef __TIMEMETER_H_
#define __TIMEMETER_H_

#include "COMMeters.h"       // main symbols
#include "resource.h"       // main symbols
#include "SysStatsMeterCategory.h"
#include "MeterImpl.h"

struct tm;

struct TZI
{
	 LONG       Bias;
	 LONG       StandardBias;
	 LONG       DaylightBias;
	 SYSTEMTIME StandardDate;
	 SYSTEMTIME DaylightDate;
};

/////////////////////////////////////////////////////////////////////////////
// CTimeMeter
class ATL_NO_VTABLE CTimeMeter : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimeMeter, &CLSID_TimeMeter>,
	public ISupportErrorInfo,
	public IDispatchImpl<MeterImpl<ITimeMeter>, &IID_ITimeMeter, &LIBID_COMMETERSLib>
{
public:
	CTimeMeter();
	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_REGISTRY_RESOURCEID(IDR_TIMEMETER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CTimeMeter)
	COM_INTERFACE_ENTRY(ITimeMeter)
	COM_INTERFACE_ENTRY(IMeter)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_AGGREGATE(__uuidof(IMetaData), pMetaData)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CTimeMeter)
	IMPLEMENTED_CATEGORY(CATID_SysStatsMeterCategory)
END_CATEGORY_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// ITimeMeter
public:
	STDMETHOD(get_TZDescription)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_TZDescription)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ForceDST)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_ForceDST)(/*[in]*/ short newVal);
	STDMETHOD(get_TimeZone)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_TimeZone)(/*[in]*/ BSTR newVal);

// IMeter
	STDMETHOD(Update)(/*[out, retval]*/ long *dirty);
	STDMETHOD(Configure)(IObserver * observer, LONG hWnd);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(GetAsString)(/*[in]*/ BSTR format, /*[in]*/ BSTR selector, /*[out, retval]*/ BSTR *pRet);
	STDMETHOD(GetAsDouble)(/*[in]*/ BSTR selector, /*[out, retval]*/ double *pRet);
	STDMETHOD(GetAsLong)(/*[in]*/ BSTR selector, /*[out,retval]*/ long *pRet);
	STDMETHOD(GetValue)(/*[out,retval]*/ VARIANT *pRet);

protected:
	_bstr_t BuildTimeZoneDoc();
	bool GetTimeZoneInfo();
	void SystemTime2tm(SYSTEMTIME *sysTime, struct tm *pLT);

	SYSTEMTIME tzTime;
	TIME_ZONE_INFORMATION tzInfo;

	_bstr_t tzDescription;
	_bstr_t tz;
	short forceDST;	// Not used. Too hard!
	DWORD isDST;

	static const int TIME_SEL;
	static const int TZDESC_SEL;
	static const int ISDST_SEL;
	static const int GMTOFFSET_SEL;

	IUnknown *pWeakTarget;
	IUnknown *pMetaData;
};

#endif //__TIMEMETER_H_
