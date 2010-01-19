// Dummy.h : Declaration of the CDummy

#ifndef __DUMMY_H_
#define __DUMMY_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDummy
class ATL_NO_VTABLE CDummy : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDummy, &CLSID_Dummy>,
	public IDispatchImpl<IDummy, &IID_IDummy, &LIBID_SYSSTATSODCOMLib>
{
public:
	CDummy()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_DUMMY)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CDummy)
	COM_INTERFACE_ENTRY(IDummy)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IDummy
public:
};

#endif //__DUMMY_H_
