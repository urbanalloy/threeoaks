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

// ScriptController.h : Declaration of the CScriptController

#ifndef __SCRIPTCONTROLLER_H_
#define __SCRIPTCONTROLLER_H_

#include "COMControllers.h"       // main symbols
#include "resource.h"       // main symbols
#include "SysStatsControllerCategory.h"
#include "ControllerImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CScriptController
class ATL_NO_VTABLE CScriptController :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CScriptController, &CLSID_ScriptController>,
	public IDispatchImpl<ControllerImpl<IScriptController>, &IID_IScriptController, &LIBID_COMCONTROLLERSLib>
{
public:
	CScriptController();
	virtual ~CScriptController();

DECLARE_REGISTRY_RESOURCEID(IDR_SCRIPTCONTROLLER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CScriptController)
	COM_INTERFACE_ENTRY(IScriptController)
	COM_INTERFACE_ENTRY(IController)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CScriptController)
	IMPLEMENTED_CATEGORY(CATID_SysStatsControllerCategory)
END_CATEGORY_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IScriptController
public:
	STDMETHOD(ShowConsole)(VARIANT_BOOL show) { return pImpl->ShowConsole(show); }
	STDMETHOD(get_ScriptType)(/*[out, retval]*/ BSTR *pVal) { return pImpl->get_ScriptType(pVal); }
	STDMETHOD(put_ScriptType)(/*[in]*/ BSTR newVal) { return pImpl->put_ScriptType(newVal); }
	STDMETHOD(get_FileName)(/*[out, retval]*/ BSTR *pVal) { return pImpl->get_FileName(pVal); }
	STDMETHOD(put_FileName)(/*[in]*/ BSTR newVal) { return pImpl->put_FileName(newVal); }

// IController
	STDMETHOD(HandleMessage)(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * meters, LONG hDlg);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(get_Model)(/*[out, retval]*/ IClientModel* *pVal);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel* newVal);

protected:
	IScriptControllerImpl *pImpl;
};

#endif //__SCRIPTCONTROLLER_H_
