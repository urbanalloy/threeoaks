// Meter.h: Definition of the CMeter class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_METER_H__DA5141D4_49E8_4009_AA83_22A42567D3C7__INCLUDED_)
#define AFX_METER_H__DA5141D4_49E8_4009_AA83_22A42567D3C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols
#include "MeterImpl.h"
#include "MeterImpl.cpp"


// CMeter.h : Declaration of the CMeter


/////////////////////////////////////////////////////////////////////////////
// CMeter
class ATL_NO_VTABLE CMeter : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMeter, &CLSID_Meter>,
	public ISupportErrorInfo,
	public IDispatchImpl<MeterImpl<IMeter>, &IID_IMeter, &LIBID_SYSSTATSCOMLib>
{
public:
	CMeter()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_Meter)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMeter)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IMeter)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IMeter
public:
};

#endif // !defined(AFX_METER_H__DA5141D4_49E8_4009_AA83_22A42567D3C7__INCLUDED_)
