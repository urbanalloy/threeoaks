// COMTimerService.h : Declaration of the CCOMTimerService

#ifndef __COMTimerService_H_
#define __COMTimerService_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCOMTimerService
class ATL_NO_VTABLE CCOMTimerService : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCOMTimerService, &CLSID_COMTimerService>,
	public ISupportErrorInfo,
	public IDispatchImpl<ICOMTimerService, &IID_ICOMTimerService, &LIBID_SYSSTATSCOMLib>
{
public:
	CCOMTimerService()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_COMTIMERSERVICE)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCOMTimerService)
	COM_INTERFACE_ENTRY(ICOMTimerService)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// ICOMTimerService
public:
	STDMETHOD(ClearAllTimers)();
	STDMETHOD(SetTimerClient)(ITimerClient*);
	STDMETHOD(RemoveTimerClient)(ITimerClient*);
};

#endif //__COMTimerService_H_
