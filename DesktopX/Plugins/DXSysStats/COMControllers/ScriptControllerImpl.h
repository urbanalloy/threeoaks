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

// ScriptControllerImpl.h : Declaration of the CScriptControllerImpl

#ifndef __SCRIPTCONTROLLERIMPL_H_
#define __SCRIPTCONTROLLERIMPL_H_

#include <activscp.h>
#include "resource.h"       // main symbols
#include "ActiveScriptImpl.h"
#include "SysStatsControllerCategory.h"
#include "ControllerImpl.h"
#include "COMControllersCP.h"

class IScriptConsole;

/////////////////////////////////////////////////////////////////////////////
// CScriptControllerImpl
class ATL_NO_VTABLE CScriptControllerImpl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CScriptControllerImpl, &CLSID_ScriptControllerImpl>,
	public ISupportErrorInfo,
	public ActiveScriptImpl,
	public IDispatchImpl<ControllerImpl<IScriptControllerImpl>, &IID_IScriptControllerImpl, &LIBID_COMCONTROLLERSLib>,
	public CProxy_IScriptControllerEvents< CScriptControllerImpl >,
	public IProvideClassInfo2Impl<&CLSID_ScriptControllerImpl,&DIID__IScriptControllerEvents, &LIBID_COMCONTROLLERSLib, 1, 0>,
	public IConnectionPointContainerImpl<CScriptControllerImpl>
{
public:
	CScriptControllerImpl();
	virtual ~CScriptControllerImpl();

DECLARE_REGISTRY_RESOURCEID(IDR_SCRIPTCONTROLLERIMPL)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScriptControllerImpl)
	COM_INTERFACE_ENTRY(IScriptControllerImpl)
	COM_INTERFACE_ENTRY(IController)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IActiveScriptSite)
	COM_INTERFACE_ENTRY(IActiveScriptSiteWindow)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CScriptControllerImpl)
	CONNECTION_POINT_ENTRY(DIID__IScriptControllerEvents)
END_CONNECTION_POINT_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScriptControllerImpl
public:
	STDMETHOD(ShowConsole)(VARIANT_BOOL show);
	virtual BOOL AddDefaultScriptItem();
	STDMETHOD(get_ScriptType)(/*[out, retval]*/ BSTR *pVal) { return ActiveScriptImpl::get_ScriptType(pVal); }
	STDMETHOD(put_ScriptType)(/*[in]*/ BSTR newVal) { return ActiveScriptImpl::put_ScriptType(newVal); }
	STDMETHOD(get_FileName)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_FileName)(/*[in]*/ BSTR newVal);
	STDMETHOD(Trace)(/*[in]*/ BSTR msg);

// IController
	STDMETHOD(HandleMessage)(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * meters, LONG hDlg);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(put_Model)(IClientModel *model);

protected:
	IScriptConsole *pConsole;
	IScriptUtils *pUtils;

	static const _bstr_t UTILS_NAME;
	static const _bstr_t ACTIVESCRIPT_HOST_NAME;
	static const _bstr_t MODEL_NAME;
	static const _bstr_t HANDLE_NAME;
};

#endif //__SCRIPTCONTROLLERIMPL_H_
