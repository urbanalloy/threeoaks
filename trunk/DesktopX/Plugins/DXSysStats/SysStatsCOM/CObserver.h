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

// CObserver.h : Declaration of the CObserver

#ifndef __CObserver_H_
#define __CObserver_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CObserver
class ATL_NO_VTABLE CObserver : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CObserver, &CLSID_Observer>,
	public ISupportErrorInfo,
	public IDispatchImpl<IObserver, &IID_IObserver, &LIBID_SYSSTATSCOMLib>
{
public:
	CObserver()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_OBSERVER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CObserver)
	COM_INTERFACE_ENTRY(IObserver)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IObserver
public:
	STDMETHOD(HandleEvent)();
};

#endif //__CObserver_H_
