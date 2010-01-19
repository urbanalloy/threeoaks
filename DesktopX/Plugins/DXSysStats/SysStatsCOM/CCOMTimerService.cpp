// COMTimerService.cpp : Implementation of CCOMTimerService
#include "stdafx.h"
#include "SysStatsCOM.h"
#include "CCOMTimerService.h"

/////////////////////////////////////////////////////////////////////////////
// CCOMTimerService

STDMETHODIMP CCOMTimerService::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ICOMTimerService
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}
