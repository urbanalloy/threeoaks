// Meter.cpp : Implementation of CSysStatsCOMApp and DLL registration.

#include "stdafx.h"
#include "SysStatsCOM.h"
#include "CMeter.h"

/////////////////////////////////////////////////////////////////////////////
//

STDMETHODIMP CMeter::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IMeter,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

