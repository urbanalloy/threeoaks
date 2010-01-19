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

// DropTargetController.h : Declaration of the CDropTargetController

#ifndef __DROPTARGETCONTROLLER_H_
#define __DROPTARGETCONTROLLER_H_

#include "resource.h"       // main symbols
#include "SysStatsControllerCategory.h"
#include "MessageSenderImpl.h"

class CDropTarget;

/////////////////////////////////////////////////////////////////////////////
// CDropTargetController
class ATL_NO_VTABLE CDropTargetController :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDropTargetController, &CLSID_DropTargetController>,
	public ISupportErrorInfo,
	public IDispatchImpl<MessageSenderImpl<IDropTargetController>, &IID_IDropTargetController, &LIBID_COMCONTROLLERSLib>
{
public:
	CDropTargetController();
	virtual ~CDropTargetController();

DECLARE_REGISTRY_RESOURCEID(IDR_DROPTARGETCONTROLLER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CDropTargetController)
	COM_INTERFACE_ENTRY(IDropTargetController)
	COM_INTERFACE_ENTRY(IMessageSender)
	COM_INTERFACE_ENTRY(IController)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CDropTargetController)
	IMPLEMENTED_CATEGORY(CATID_SysStatsControllerCategory)
END_CATEGORY_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IDropTargetController
public:
	STDMETHOD(get_Effect)(/*[out, retval]*/ DWORD *pVal);
	STDMETHOD(put_Effect)(/*[in]*/ DWORD newVal);

// IController
	STDMETHOD(HandleMessage)(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * meters, LONG hDlg);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel* newVal);

// IDropTarget
	STDMETHOD(DragEnter)(IDataObject* obj, DWORD dwKeys, POINTL ptl, DWORD* pEffect);
    STDMETHOD(DragOver)(DWORD dwKeys, POINTL ptl, DWORD * pEffect);
    STDMETHOD(DragLeave)();
    STDMETHOD(Drop)(IDataObject * obj, DWORD dwKeys, POINTL ptl, DWORD *pEffect);

protected:
	STDMETHOD(DragEffect)(DWORD dwKeys, POINTL ptl, DWORD * pEffect);
	DWORD getEffect(DWORD dwKeys, DWORD *pEffect);
	bool PutValue(TCHAR *szFileName);
	bool ReadHdropData(IDataObject* pDataObject);
	bool ReadAveDropData (IDataObject* pDataObject, HWND *hwnd);

	DWORD effect;
	CDropTarget *pTargetImpl;
	WORD aveClipFormat;
};

#endif //__DROPTARGETCONTROLLER_H_
