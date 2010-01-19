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

// WeakReference.h : Declaration of the CWeakReference

#ifndef __WEAKREFERENCE_H_
#define __WEAKREFERENCE_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CWeakReference
class ATL_NO_VTABLE CWeakReference : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CWeakReference, &CLSID_WeakReference>,
	public ISupportErrorInfo,
	public IDispatchImpl<IWeakReference, &IID_IWeakReference, &LIBID_SYSSTATSCOMLib>
{
public:
	CWeakReference();
	virtual ~CWeakReference();

DECLARE_REGISTRY_RESOURCEID(IDR_WEAKREFERENCE)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CWeakReference)
	COM_INTERFACE_ENTRY(IWeakReference)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IWeakReference
public:
	STDMETHOD(AssignReference)(/*[in]*/ IUnknown *pVal);
	STDMETHOD(SetReference)(/*[in]*/ IUnknown *pVal);
	STDMETHOD(DeReference)(/*[out, retval]*/ IUnknown **pVal);
	STDMETHOD(ClearReference)();

protected:
	IUnknown *ref;
};

#endif //__WEAKREFERENCE_H_
