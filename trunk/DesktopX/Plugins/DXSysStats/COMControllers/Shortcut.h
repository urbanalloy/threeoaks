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

// Shortcut.h : Declaration of the CShortcut

#ifndef __SHORTCUT_H_
#define __SHORTCUT_H_

#include "COMControllers.h"       // main symbols
#include "resource.h"       // main symbols
#include "SysStatsControllerCategory.h"
#include "ControllerImpl.cpp"

/////////////////////////////////////////////////////////////////////////////
// CShortcut
class ATL_NO_VTABLE CShortcut : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CShortcut, &CLSID_Shortcut>,
	public IDispatchImpl<ControllerImpl<IShortcut>, &IID_IShortcut, &LIBID_COMCONTROLLERSLib>
{
public:
	CShortcut();

DECLARE_REGISTRY_RESOURCEID(IDR_SHORTCUT)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CShortcut)
	COM_INTERFACE_ENTRY(IShortcut)
	COM_INTERFACE_ENTRY(IController)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CShortcut)
	IMPLEMENTED_CATEGORY(CATID_SysStatsControllerCategory)
END_CATEGORY_MAP()

// IShortcut

// IController
	STDMETHOD(HandleMessage)(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * meters, LONG hDlg);
	STDMETHOD(get_Type)(BSTR * pVal);
public:
	STDMETHOD(get_Shortcut)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Shortcut)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Selector)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Selector)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_StartIn)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_StartIn)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Args)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Args)(/*[in]*/ BSTR newVal);

protected:
	_bstr_t args;
	_bstr_t startIn;
	_bstr_t selector;
	_bstr_t shortcut;
};

#endif //__SHORTCUT_H_
