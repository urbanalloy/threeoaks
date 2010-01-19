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

// ASyncScriptController.h : Declaration of the CASyncScriptController

#ifndef __ASYNCSCRIPTCONTROLLER_H_
#define __ASYNCSCRIPTCONTROLLER_H_

#include "resource.h"       // main symbols
#include "SysStatsControllerCategory.h"
#include "ControllerImpl.h"

class ScriptThread;

/////////////////////////////////////////////////////////////////////////////
// CASyncScriptController
class ATL_NO_VTABLE CASyncScriptController :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CASyncScriptController, &CLSID_ASyncScriptController>,
	public ISupportErrorInfo,
	public IDispatchImpl<ControllerImpl<IASyncScriptController>, &IID_IASyncScriptController, &LIBID_COMCONTROLLERSLib>
{
public:
	CASyncScriptController();
	virtual ~CASyncScriptController();

DECLARE_REGISTRY_RESOURCEID(IDR_ASYNCSCRIPTCONTROLLER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CASyncScriptController)
	COM_INTERFACE_ENTRY(IASyncScriptController)
	COM_INTERFACE_ENTRY(IController)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CASyncScriptController)
	IMPLEMENTED_CATEGORY(CATID_SysStatsControllerCategory)
END_CATEGORY_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IASyncScriptController
public:
	STDMETHOD(ShowConsole)(VARIANT_BOOL show);
	STDMETHOD(get_ScriptType)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ScriptType)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_FileName)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_FileName)(/*[in]*/ BSTR newVal);

// IController
	STDMETHOD(HandleMessage)(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * meters, LONG hDlg);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel* newVal);

protected:
	ScriptThread *pThread;
};

#endif //__ASYNCSCRIPTCONTROLLER_H_
