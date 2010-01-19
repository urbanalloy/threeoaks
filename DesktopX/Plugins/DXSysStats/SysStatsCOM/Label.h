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

// Label.h : Declaration of the CLabel

#ifndef __LABEL_H_
#define __LABEL_H_

#include "resource.h"       // main symbols
#include <comutil.h>

/////////////////////////////////////////////////////////////////////////////
// CLabel
class ATL_NO_VTABLE CLabel : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLabel, &CLSID_Label>,
	public ISupportErrorInfo,
	public IDispatchImpl<ILabel, &IID_ILabel, &LIBID_SYSSTATSCOMLib>
{
public:
	CLabel();
	virtual ~CLabel();

DECLARE_REGISTRY_RESOURCEID(IDR_LABEL)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CLabel)
	COM_INTERFACE_ENTRY(ILabel)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// ILabel
public:
	STDMETHOD(GetLabel)(/*[out, retval]*/ BSTR *retVal);
	STDMETHOD(get_selector)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_selector)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Format)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Format)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Model)(/*[out, retval]*/ IClientModel* *pVal);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel* newVal);
	STDMETHOD(get_MeterIndex)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_MeterIndex)(/*[in]*/ long newVal);
	STDMETHOD(get_meter)(/*[out, retval]*/ IMeter* *pVal);
	STDMETHOD(put_meter)(/*[in]*/ IMeter* newVal);

protected:
	_bstr_t selector;
	_bstr_t format;
	IWeakReference *meter;
	IClientModel *model;
};

#endif //__LABEL_H_
