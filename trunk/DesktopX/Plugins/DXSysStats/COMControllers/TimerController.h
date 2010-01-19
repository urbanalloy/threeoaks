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

// TimerController.h : Declaration of the CTimerController

#ifndef __TIMERCONTROLLER_H_
#define __TIMERCONTROLLER_H_

#include "resource.h"       // main symbols
#include "SysStatsControllerCategory.h"
#include "MessageSenderImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CTimerController
class ATL_NO_VTABLE CTimerController :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimerController, &CLSID_TimerController>,
	public ISupportErrorInfo,
	public IDispatchImpl<MessageSenderImpl<ITimerController>, &IID_ITimerController, &LIBID_COMCONTROLLERSLib>,
	public IDispatchImpl<ITimerClient, &__uuidof(ITimerClient), &LIBID_COMCONTROLLERSLib>
{
public:
	CTimerController();
	virtual ~CTimerController();

DECLARE_REGISTRY_RESOURCEID(IDR_TIMERCONTROLLER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CTimerController)
	COM_INTERFACE_ENTRY(ITimerController)
	COM_INTERFACE_ENTRY(IMessageSender)
	COM_INTERFACE_ENTRY(IController)
//DEL 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY2(IDispatch, ControllerImpl<ITimerController>)
	COM_INTERFACE_ENTRY(ITimerClient)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CTimerController)
	IMPLEMENTED_CATEGORY(CATID_SysStatsControllerCategory)
END_CATEGORY_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// ITimerController
public:
	STDMETHOD(get_Interval)(/*[out, retval]*/ DOUBLE *pVal);
	STDMETHOD(put_Interval)(/*[in]*/ DOUBLE newVal);
	STDMETHOD(Stop)();
	STDMETHOD(Start)();

// IMessageSender
	
// IController
	STDMETHOD(HandleMessage)(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * meters, LONG hDlg);
	STDMETHOD(get_Type)(BSTR * pVal);

// ITimerClient
	STDMETHOD(get_TimerInterval)(DOUBLE * pVal)
	{
		return get_Interval(pVal);
	}
	STDMETHOD(put_TimerInterval)(DOUBLE pVal)
	{
		return put_Interval(pVal);
	}
	STDMETHOD(TimerFired)()
	{
		if (model)
			model->PostMessage(sendMessage, wParam, 0);

		return S_OK;
	}

protected:
	double interval;
};

#endif //__TIMERCONTROLLER_H_
