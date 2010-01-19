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

// ControllerImpl.h: interface for the Meter template.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ControllerImpl_H__EDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_)
#define AFX_ControllerImpl_H__EDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <comutil.h>

struct IObserver;
struct IClientModel;
struct IMeterHome;
struct IMeter;
struct IWeakReference;
struct IOverlay;

template <class B>
class ControllerImpl : public B
{
public:
	ControllerImpl();
	virtual ~ControllerImpl();

public:
	STDMETHOD(Configure)(/*[in]*/ IObserver * observer, /*[in]*/ IMeterHome * meters, /*[in]*/ LONG hDlg);
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);
	STDMETHOD(HandleMessage)(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled);
	STDMETHOD(HandleSubclassMessage)(/*[in]*/ UINT msg, /*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[out, retval]*/ VARIANT_BOOL *handled);
	STDMETHOD(get_Model)(/*[out, retval]*/ IClientModel* *pVal);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel* newVal);
	STDMETHOD(get_meter)(/*[out, retval]*/ IMeter **pRet);
	STDMETHOD(put_meter)(/*[in]*/ IMeter *meter);
	STDMETHOD(get_MeterIndex)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_MeterIndex)(/*[in]*/ long newVal);
	STDMETHOD(get_MessageType)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_MessageType)(/*[in]*/ short newVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ UINT *pVal);
	STDMETHOD(put_Key)(/*[in]*/ UINT newVal);
	STDMETHOD(get_OverlayPath)(/*[out, retval]*/ LPSAFEARRAY *pVal);
	STDMETHOD(put_OverlayPath)(/*[in]*/ LPSAFEARRAY newVal);
	STDMETHOD(get_Overlay)(/*[out, retval]*/ IOverlay **pVal);
	STDMETHOD(put_Overlay)(/*[in]*/ IOverlay *pVal);
	STDMETHOD(get_PassThrough)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_PassThrough)(/*[in]*/ VARIANT_BOOL pVal);

protected:
	_bstr_t name;
	short messageType;
	WPARAM key;
	VARIANT_BOOL passThrough;

	IClientModel *model;

private:
	IWeakReference *meter;
	IWeakReference *overlay;
};

#endif // !defined(AFX_ControllerImpl_H__EDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_)
