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

// TimerClientImpl.h: interface for the TimerClientImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMERCLIENTIMPL_H__12B897D4_B3AE_45BC_92DB_D24A62C1FB5D__INCLUDED_)
#define AFX_TIMERCLIENTIMPL_H__12B897D4_B3AE_45BC_92DB_D24A62C1FB5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template <class B>
class TimerClientImpl  : public B
{
public:
	TimerClientImpl();
	virtual ~TimerClientImpl();

// ITimerClient
	STDMETHOD(get_TimerInterval)(double * pVal);
	STDMETHOD(put_TimerInterval)(double pVal);
	STDMETHOD(TimerFired)();

protected:
	double interval;
};

#endif // !defined(AFX_TIMERCLIENTIMPL_H__12B897D4_B3AE_45BC_92DB_D24A62C1FB5D__INCLUDED_)
