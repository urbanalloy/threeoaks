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

// Cursor.h : Declaration of the CCursor

#ifndef __CURSORCONTROLLER_H_
#define __CURSORCONTROLLER_H_

#include "resource.h"       // main symbols
#include "SysStatsControllerCategory.h"
#include "ControllerImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CCursor
class ATL_NO_VTABLE CCursorController :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCursorController, &CLSID_CursorController>,
	public ISupportErrorInfo,
	public IDispatchImpl<ControllerImpl<ICursorController>, &IID_ICursorController, &LIBID_COMCONTROLLERSLib>
{
public:
	CCursorController();

DECLARE_REGISTRY_RESOURCEID(IDR_CURSORCONTROLLER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCursorController)
	COM_INTERFACE_ENTRY(ICursorController)
	COM_INTERFACE_ENTRY(IController)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CCursor)
	IMPLEMENTED_CATEGORY(CATID_SysStatsControllerCategory)
END_CATEGORY_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// ICursorController
public:
	STDMETHOD(get_CursorID)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_CursorID)(/*[in]*/ long newVal);

// IController
	STDMETHOD(HandleMessage)(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * meters, LONG hDlg);
	STDMETHOD(get_Type)(BSTR * pVal);

protected:
	long cursorID;
};

#endif //__CURSORCONTROLLER_H_
