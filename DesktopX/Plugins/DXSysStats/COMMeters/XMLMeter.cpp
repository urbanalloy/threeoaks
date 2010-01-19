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

// XMLMeter.cpp : Implementation of CXMLMeter
#include "stdafx.h"
#include "MeterImpl.cpp"
#include "SysStatsUtils.h"
#include "COMMeters.h"
#include "UpdateThread.h"
#include "XMLDialog.h"
#include "XMLMeter.h"

static _bstr_t percentS("%s");
static _bstr_t emptyStr("");

/////////////////////////////////////////////////////////////////////////////
// CXMLMeter

STDMETHODIMP CXMLMeter::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IXMLMeter
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CXMLMeter::CXMLMeter() : reader(new UpdateThread(this, emptyStr)), pWeakTarget(0)
{
	interval = 3600;
}

CXMLMeter::~CXMLMeter()
{
	reader->stop();	// It will delete itself when its finished!
}

HRESULT CXMLMeter::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	return ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);
}

void CXMLMeter::FinalRelease()
{
	pWeakTarget->Release();
}

void CXMLMeter::put_dirty(bool dirty)
{
	if (model)
		model->put_dirty(dirty);
}

STDMETHODIMP CXMLMeter::get_URL(BSTR *pVal)
{
	return reader->get_URL(pVal);
}

STDMETHODIMP CXMLMeter::put_URL(BSTR newVal)
{
	return reader->put_URL(newVal);
}

STDMETHODIMP CXMLMeter::get_Username(BSTR *pVal)
{
	return reader->get_Username(pVal);
}

STDMETHODIMP CXMLMeter::put_Username(BSTR newVal)
{
	return reader->put_Username(newVal);
}

STDMETHODIMP CXMLMeter::get_Password(BSTR *pVal)
{
	return reader->get_Password(pVal);
}

STDMETHODIMP CXMLMeter::put_Password(BSTR newVal)
{
	return reader->put_Password(newVal);
}

STDMETHODIMP CXMLMeter::get_ProxyURL(BSTR *pVal)
{
	return reader->get_ProxyURL(pVal);
}

STDMETHODIMP CXMLMeter::put_ProxyURL(BSTR newVal)
{
	return reader->put_ProxyURL(newVal);
}

STDMETHODIMP CXMLMeter::get_ProxyUsername(BSTR *pVal)
{
	return reader->get_ProxyUsername(pVal);
}

STDMETHODIMP CXMLMeter::put_ProxyUsername(BSTR newVal)
{
	return reader->put_ProxyUsername(newVal);
}

STDMETHODIMP CXMLMeter::get_ProxyPassword(BSTR *pVal)
{
	return reader->get_ProxyPassword(pVal);
}

STDMETHODIMP CXMLMeter::put_ProxyPassword(BSTR newVal)
{
	return reader->put_ProxyPassword(newVal);
}

STDMETHODIMP CXMLMeter::Update(long *dirty)
{
	return reader->Update(dirty);
}

STDMETHODIMP CXMLMeter::Configure(IObserver * observer, LONG hWnd)
{
	XMLDialog *d = new XMLDialog(this);
	d->DoModal((HWND)hWnd);
	delete d;

	return S_OK;
}

STDMETHODIMP CXMLMeter::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_XMLMeter, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CXMLMeter::GetAsLong(BSTR sel, long *pRet)
{
	return reader->GetAsLong(sel, pRet);
}

STDMETHODIMP CXMLMeter::GetAsDouble(BSTR sel, double *pRet)
{
	return reader->GetAsDouble(sel, pRet);
}

STDMETHODIMP CXMLMeter::GetAsString(BSTR format, BSTR selector, BSTR *pRet)
{
	return reader->GetAsString(format, selector, pRet);
}

STDMETHODIMP CXMLMeter::GetValue(VARIANT *pRet)
{
	return reader->GetValue(pRet);
}

STDMETHODIMP CXMLMeter::get_UseXPath(VARIANT_BOOL *pVal)
{
	return reader->get_UseXPath(pVal);
}

STDMETHODIMP CXMLMeter::put_UseXPath(VARIANT_BOOL newVal)
{
	return reader->put_UseXPath(newVal);
}

STDMETHODIMP CXMLMeter::get_Namespaces(LPSAFEARRAY *pVal)
{
	return reader->get_Namespaces(pVal);
}

STDMETHODIMP CXMLMeter::put_Namespaces(LPSAFEARRAY newVal)
{
	return reader->put_Namespaces(newVal);
}

STDMETHODIMP CXMLMeter::put_Value(BSTR newVal)
{
	return reader->put_Value(newVal);
}

STDMETHODIMP CXMLMeter::GetNodeList(BSTR xpath, IDispatch **pNodeList)
{
	return reader->GetNodeList(xpath, pNodeList);
}

STDMETHODIMP CXMLMeter::get_LoadFailed(VARIANT_BOOL *pVal)
{
	return reader->get_LoadFailed(pVal);
}

STDMETHODIMP CXMLMeter::put_LoadFailed(VARIANT_BOOL newVal)
{
	return reader->put_LoadFailed(newVal);
}
