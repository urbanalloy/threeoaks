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

// MappedController.h : Declaration of the CMappedController

#ifndef __MAPPEDCONTROLLER_H_
#define __MAPPEDCONTROLLER_H_

#include "resource.h"       // main symbols
#include "SysStatsControllerCategory.h"
#include "MessageSenderImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CMappedController
class ATL_NO_VTABLE CMappedController :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMappedController, &CLSID_MappedController>,
	public ISupportErrorInfo,
	public IDispatchImpl<MessageSenderImpl<IMappedController>, &IID_IMappedController, &LIBID_COMCONTROLLERSLib>
{
public:
	CMappedController();
	virtual ~CMappedController();

DECLARE_REGISTRY_RESOURCEID(IDR_MAPPEDCONTROLLER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMappedController)
	COM_INTERFACE_ENTRY(IMappedController)
	COM_INTERFACE_ENTRY(IMessageSender)
	COM_INTERFACE_ENTRY(IController)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CMappedController)
	IMPLEMENTED_CATEGORY(CATID_SysStatsControllerCategory)
END_CATEGORY_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IMappedController
public:

// IController
	STDMETHOD(HandleMessage)(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * meters, LONG hDlg);
	STDMETHOD(get_Type)(BSTR * pVal);

// WindowSubclassMessageHandler
	STDMETHOD(HandleSubclassMessage)(UINT msg, UINT wparam, UINT lparam, VARIANT_BOOL *bHandled);

protected:
	STDMETHOD(PostMessage)(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled);
	virtual void SetModal();

	bool subclassed;
};

#endif //__MAPPEDCONTROLLER_H_
