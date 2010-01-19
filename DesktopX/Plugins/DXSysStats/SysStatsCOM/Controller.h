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

// Controller.h : Declaration of the CController

#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CController
class ATL_NO_VTABLE CController : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CController, &CLSID_Controller>,
	public ISupportErrorInfo,
	public IDispatchImpl<IController, &IID_IController, &LIBID_SYSSTATSCOMLib>
{
public:
	CController()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CONTROLLER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CController)
	COM_INTERFACE_ENTRY(IController)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IController
public:
	STDMETHOD(HandleMessage)(/*[out, retval]*/ VARIANT_BOOL *retVal);
	STDMETHOD(get_Model)(/*[out, retval]*/ IClientModel* *pVal);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel* newVal);
	STDMETHOD(get_Type)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(Configure)(/*[in]*/ IObserver *observer, /*[in]*/ long hWnd);
};

#endif //__CONTROLLER_H_
