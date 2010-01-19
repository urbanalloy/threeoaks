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

// WeakTarget.h : Declaration of the CWeakTarget

#ifndef __WEAKTARGET_H_
#define __WEAKTARGET_H_

#include "resource.h"       // main symbols
#include "ArrayPtr.cpp"

/////////////////////////////////////////////////////////////////////////////
// CWeakTarget
//
// The target of a WeakReference.
//
class CWeakTarget : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CWeakTarget, &CLSID_WeakTarget>,
	public ISupportErrorInfo,
	public IDispatchImpl<IWeakTarget, &IID_IWeakTarget, &LIBID_SYSSTATSCOMLib>
{
public:
	CWeakTarget();
	virtual ~CWeakTarget();
	void FinalRelease();

DECLARE_REGISTRY_RESOURCEID(IDR_WEAKTARGET)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CWeakTarget)
	COM_INTERFACE_ENTRY(IWeakTarget)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IWeakTarget
public:
	STDMETHOD(Detach)(/*[in]*/ IWeakReference *pVal);
	STDMETHOD(Attach)(/*[in]*/IWeakReference *pVal);

protected:
	ArrayPtr<IWeakReference> referers;
};

#endif //__WEAKTARGET_H_
