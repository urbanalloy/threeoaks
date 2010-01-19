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

// ScriptMeterImplImpl.h : Declaration of the CScriptMeterImplImpl

#ifndef __ScriptMeterImplIMPL_H_
#define __ScriptMeterImplIMPL_H_

#include <activscp.h>
#include "COMMeters.h"       // main symbols
#include "ActiveScriptImpl.h"
#include "resource.h"       // main symbols
#include "MeterImpl.h"       // main symbols
#include "MeterImpl.cpp"       // main symbols
#include "COMMetersCP.h"

#define SCRIPTITEM_NAMEDITEM		(SCRIPTITEM_ISSOURCE | \
									 SCRIPTITEM_ISVISIBLE | \
									 SCRIPTITEM_GLOBALMEMBERS)

#define LANG_ENGLISH_NEUTRAL	(MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL))
#define LOCALE_SCRIPT_DEFAULT	(MAKELCID(LANG_ENGLISH_NEUTRAL, SORT_DEFAULT))

class IScriptConsole;

/////////////////////////////////////////////////////////////////////////////
// CScriptMeterImpl
class ATL_NO_VTABLE CScriptMeterImpl : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CScriptMeterImpl, &CLSID_ScriptMeterImpl>,
	public ISupportErrorInfo,
	public IProvideClassInfo2Impl<&CLSID_ScriptMeterImpl,&DIID__IScriptMeterEvents, &LIBID_COMMETERSLib, 1, 0>,
	public IConnectionPointContainerImpl<CScriptMeterImpl>,
	public ActiveScriptImpl,
	public IDispatchImpl<MeterImpl<IScriptMeterImpl>, &IID_IScriptMeterImpl, &LIBID_COMMETERSLib>,
	public CProxy_IScriptMeterEvents< CScriptMeterImpl >
{
public:
	CScriptMeterImpl();
	virtual ~CScriptMeterImpl();

DECLARE_REGISTRY_RESOURCEID(IDR_SCRIPTMETERIMPL)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScriptMeterImpl)
	COM_INTERFACE_ENTRY(IScriptMeterImpl)
	COM_INTERFACE_ENTRY(IMeter)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IActiveScriptSite)
	COM_INTERFACE_ENTRY(IActiveScriptSiteWindow)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CScriptMeterImpl)
	CONNECTION_POINT_ENTRY(DIID__IScriptMeterEvents)
END_CONNECTION_POINT_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

public:
// IMeter
	STDMETHOD(Update)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Model)(IClientModel *model);
	STDMETHOD(Configure)(IObserver * observer, LONG hWnd);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(GetAsLong)(BSTR selector, LONG * pRet);
	STDMETHOD(GetAsDouble)(BSTR selector, DOUBLE * pRet);
	STDMETHOD(GetAsString)(BSTR format, BSTR selector, BSTR * pRet);
	STDMETHOD(GetValue)(VARIANT * pRet);

// IScriptMeterImpl
public:
	STDMETHOD(ShowConsole)(/*[in]*/ VARIANT_BOOL show);
	virtual BOOL AddDefaultScriptItem();
	STDMETHOD(get_ScriptType)(/*[out, retval]*/ BSTR *pVal) { return ActiveScriptImpl::get_ScriptType(pVal); }
	STDMETHOD(put_ScriptType)(/*[in]*/ BSTR newVal) { return ActiveScriptImpl::put_ScriptType(newVal); }
	STDMETHOD(get_FileName)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_FileName)(/*[in]*/ BSTR newVal);
	STDMETHOD(put_Value)(/*[in]*/ BSTR newVal);

	STDMETHOD(Trace)(/*[in]*/ BSTR msg);

protected:
	HRESULT GetValue(BSTR selector, VARIANT *pRet);

	IScriptConsole *pConsole;
	IScriptUtils *pUtils;

	static const _bstr_t ACTIVESCRIPT_HOST_NAME;
	static const _bstr_t MODEL_NAME;
	static const _bstr_t UTILS_NAME;
	static const _bstr_t UPDATE_NAME;
	static const _bstr_t GETVALUE_NAME;
	static const _bstr_t PUTVALUE_NAME;
};

#endif //__ScriptMeterImplIMPL_H_
