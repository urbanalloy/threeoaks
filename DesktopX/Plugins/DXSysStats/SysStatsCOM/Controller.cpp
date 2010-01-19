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

// Controller.cpp : Implementation of CController
#include "stdafx.h"
#include "SysStatsCOM.h"
#include "Controller.h"

/////////////////////////////////////////////////////////////////////////////
// CController

STDMETHODIMP CController::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IController
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CController::Configure(IObserver *observer, long hWnd)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CController::get_Type(BSTR *pVal)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CController::get_Model(IClientModel **pVal)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CController::put_Model(IClientModel *newVal)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CController::HandleMessage(VARIANT_BOOL *retVal)
{
	// TODO: Add your implementation code here

	return S_OK;
}
