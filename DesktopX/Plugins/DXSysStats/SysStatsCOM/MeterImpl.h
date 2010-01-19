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

// MeterImpl.h: interface for the Meter template.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MeterImpl_H__EDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_)
#define AFX_MeterImpl_H__EDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <comutil.h>
#include "TimerClientImpl.h"
#include "TimerClientImpl.cpp"

struct IObserver;
struct IClientModel;

template <class B>
class MeterImpl : public TimerClientImpl<B>
{
public:
	MeterImpl();
	virtual ~MeterImpl();

public:
	STDMETHOD(Configure)(/*[in]*/IObserver *observer, /*[in]*/HWND hWnd);
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);
	STDMETHOD(Update)(/*[out, retval]*/ long *dirty);
	STDMETHOD(get_Model)(/*[out, retval]*/ IClientModel* *pVal);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel* newVal);

// ITimerClient
	STDMETHOD(TimerFired)();

protected:
	_bstr_t name;

	IClientModel *model;
};

#endif // !defined(AFX_MeterImpl_H__EDEDBF0F_2018_4187_A2AE_EDEB63FC1A8E__INCLUDED_)
