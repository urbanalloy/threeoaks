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

// ValueSetter.h : Declaration of the CValueSetter

#ifndef __VALUESETTER_H_
#define __VALUESETTER_H_

#include "COMControllers.h"       // main symbols
#include "resource.h"       // main symbols
#include "SysStatsControllerCategory.h"
#include "ControllerImpl.h"

struct IWeakReference;

/////////////////////////////////////////////////////////////////////////////
// CValueSetter
class ATL_NO_VTABLE CValueSetter :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CValueSetter, &CLSID_ValueSetter>,
	public ISupportErrorInfo,
	public IDispatchImpl<ControllerImpl<IValueSetter>, &IID_IValueSetter, &LIBID_COMCONTROLLERSLib>
{
public:
	CValueSetter();
	virtual ~CValueSetter();

DECLARE_REGISTRY_RESOURCEID(IDR_VALUESETTER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CValueSetter)
	COM_INTERFACE_ENTRY(IValueSetter)
	COM_INTERFACE_ENTRY(IController)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CValueSetter)
	IMPLEMENTED_CATEGORY(CATID_SysStatsControllerCategory)
END_CATEGORY_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IValueSetter
public:
	STDMETHOD(get_Selector)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Selector)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Value)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Value)(/*[in]*/ BSTR newVal);

// IController
	STDMETHOD(HandleMessage)(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * meters, LONG hDlg);
	STDMETHOD(get_Type)(BSTR * pVal);

private:
	_bstr_t value;
	_bstr_t selector;
};

#endif //__VALUESETTER_H_
