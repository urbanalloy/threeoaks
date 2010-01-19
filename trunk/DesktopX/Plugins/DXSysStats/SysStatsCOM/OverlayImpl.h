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

// OverlayImpl.h: interface for the Overlay template.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OverlayImpl_H__FDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_)
#define AFX_OverlayImpl_H__FDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <comutil.h>

struct IObserver;
struct IMeterHome;
struct IOverlay;
struct IClientModel;
struct IWeakReference;

template <class B>
class OverlayImpl : public B
{
public:
	OverlayImpl();
	virtual ~OverlayImpl();

public:
	STDMETHOD(get_Type)(/*[out, retval]*/ BSTR *retVal) = 0;
	STDMETHOD(clone)(/*[out, retval]*/ IOverlay **pRet);
	STDMETHOD(Configure)(/*[in]*/ IObserver *observer, /*[in]*/ IMeterHome *meters, /*[in]*/ long hWnd);
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_meter)(/*[out, retval]*/ IMeter **pRet);
	STDMETHOD(put_meter)(/*[in]*/ IMeter *meter);
	STDMETHOD(get_Model)(/*[out, retval]*/ IClientModel **pRet);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel *model);
	STDMETHOD(get_Mode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Mode)(/*[in]*/ long newVal);
	STDMETHOD(get_MeterIndex)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_MeterIndex)(/*[in]*/ long newVal);
	STDMETHOD(get_Visible)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_Visible)(/*[in]*/ VARIANT_BOOL newVal);

protected:
	virtual IOverlay *createInstance() = 0;

protected:
	_bstr_t name;
	IClientModel *model;
	long mode;
	VARIANT_BOOL visible;

private:
	IWeakReference *meter;
};

#endif // !defined(AFX_OverlayImpl_H__FDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_)
