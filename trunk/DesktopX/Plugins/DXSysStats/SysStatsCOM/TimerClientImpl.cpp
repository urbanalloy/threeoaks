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

// TimerClientImpl.cpp: implementation of the TimerClientImpl class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "TimerClientImpl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template <class B>
TimerClientImpl<B>::TimerClientImpl() : interval(1.0)
{

}

template <class B>
TimerClientImpl<B>::~TimerClientImpl()
{

}

template <class B>
STDMETHODIMP TimerClientImpl<B>::put_TimerInterval(double val)
{
	interval = val;

	return S_OK;
}

template <class B>
STDMETHODIMP TimerClientImpl<B>::get_TimerInterval(double * pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = interval;

	return S_OK;
}

template <class B>
STDMETHODIMP TimerClientImpl<B>::TimerFired()
{
	// Its OK to do nothing I guess...
	return S_OK;
}

