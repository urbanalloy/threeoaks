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

// Sens.h : Declaration of the CSens

#ifndef __SENS_H_
#define __SENS_H_

#include <Sens.h>
#include <EventSys.h>
#include <SensAPI.h>
#include <atlwin.h>

#include "resource.h"       // main symbols
#include "hook.h"
#include "SysStatsControllerCategory.h"
#include "ControllerImpl.h"
#import "C:\WINDOWS\system32\SENS.DLL" raw_interfaces_only, raw_native_types, no_namespace, named_guids 

/////////////////////////////////////////////////////////////////////////////
// CSens
class ATL_NO_VTABLE CSens :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSens, &CLSID_SysStatsSens>,
	public ISupportErrorInfo,
	public IDispatchImpl<ControllerImpl<ISens>, &IID_ISens, &LIBID_COMCONTROLLERSLib>,
	public IDispatchImpl<ISensLogon, &IID_ISensLogon, &LIBID_SensEvents>,
	public IDispatchImpl<ISensLogon2, &IID_ISensLogon2, &LIBID_SensEvents>,
	public CWindowImpl<CSens, CWindow, CWinTraits<0, 0> >
{
public:
	CSens();
	~CSens();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();

DECLARE_REGISTRY_RESOURCEID(IDR_SENS)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CSens)
	COM_INTERFACE_ENTRY(ISens)
	COM_INTERFACE_ENTRY(IController)
//DEL 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY2(IDispatch, ControllerImpl<ISens>)
	COM_INTERFACE_ENTRY(ISensLogon)
	COM_INTERFACE_ENTRY(ISensLogon2)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CSens)
	IMPLEMENTED_CATEGORY(CATID_SysStatsControllerCategory)
END_CATEGORY_MAP()

BEGIN_MSG_MAP(CSens)
	MESSAGE_HANDLER(getSysCommandMsg(), SysCommand)
END_MSG_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// ISens
public:
	STDMETHOD(get_Present)(/*[out, retval]*/ UINT *pVal);
	STDMETHOD(put_Present)(/*[in]*/ UINT newVal);
	STDMETHOD(get_Away)(/*[out, retval]*/ UINT *pVal);
	STDMETHOD(put_Away)(/*[in]*/ UINT newVal);

// IController
	STDMETHOD(HandleMessage)(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * meters, LONG hDlg);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(put_MessageType)(/*[in]*/ short newVal) { return S_OK; }
	STDMETHOD(put_PassThrough)(/*[in]*/ VARIANT_BOOL pVal) { return S_OK; }
	STDMETHOD(put_OverlayPath)(/*[in]*/ LPSAFEARRAY newVal) { return S_OK; }
	STDMETHOD(put_Key)(/*[in]*/ UINT newVal) { return S_OK; }

// ISensLogon
	STDMETHOD(Logon)(BSTR bstrUserName);
	STDMETHOD(Logoff)(BSTR bstrUserName);
	STDMETHOD(StartShell)(BSTR bstrUserName);
	STDMETHOD(DisplayLock)(BSTR bstrUserName);
	STDMETHOD(DisplayUnlock)(BSTR bstrUserName);
	STDMETHOD(StartScreenSaver)(BSTR bstrUserName);
	STDMETHOD(StopScreenSaver)(BSTR bstrUserName);
// ISensLogon2
	STDMETHOD(Logon)(BSTR bstrUserName, ULONG dwSessionId);
	STDMETHOD(Logoff)(BSTR bstrUserName, ULONG dwSessionId);
	STDMETHOD(SessionDisconnect)(BSTR bstrUserName, ULONG dwSessionId);
	STDMETHOD(SessionReconnect)(BSTR bstrUserName, ULONG dwSessionId);
	STDMETHOD(PostShell)(BSTR bstrUserName, ULONG dwSessionId);

protected:
	bool itsUs(BSTR bstrUserName, ULONG dwSessionId);
	STDMETHOD(SysCommand)(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	UINT sendMessage;
	ULONG sessionId;
	BSTR userName;
	_bstr_t bstrLogonSubscriptionID;
	_bstr_t bstrLogon2SubscriptionID;
	IEventSystem *pEventSystem;

	UINT away;
	UINT present;

	static const UINT MONITOR_ON_WPARAM;
	static const UINT MONITOR_OFF_WPARAM;
	static const UINT MONITOR_STANDBY_WPARAM;
	static const UINT LOGON_WPARAM;
	static const UINT LOGOFF_WPARAM;
	static const UINT LOCK_WPARAM;
	static const UINT UNLOCK_WPARAM;
	static const UINT STARTSHELL_WPARAM;
	static const UINT SCREENSAVER_ON_WPARAM;
	static const UINT SCREENSAVER_OFF_WPARAM;
	static const UINT DISCONNECT_WPARAM;
	static const UINT RECONNECT_WPARAM;
	static const UINT POSTSHELL_WPARAM;
};

#endif //__SENS_H_
