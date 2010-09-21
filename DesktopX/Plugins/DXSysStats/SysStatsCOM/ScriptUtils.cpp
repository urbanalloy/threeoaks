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

// ScriptUtils.cpp : Implementation of CScriptUtils
#include "stdafx.h"
#include "SysStatsCOM.h"
#include "ScriptUtils.h"
#include "ArrayPtr.cpp"
#include "ScriptConnectionObject.h"

/////////////////////////////////////////////////////////////////////////////
// CScriptUtils

STDMETHODIMP CScriptUtils::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScriptUtils
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CScriptUtils::CScriptUtils() : connections(new ArrayPtrCOM<IScriptConnectionObject>()), iScript(0), iScriptHost(0)
{
}

CScriptUtils::~CScriptUtils()
{
	AtlTrace("CScriptUtils::~CScriptUtils\n");
	DisconnectAll();
	delete connections;
#ifdef notdef
	if (iScript)
		iScript->Release();
#endif
}

STDMETHODIMP CScriptUtils::ConnectObject(IDispatch *pdispObj, BSTR bstrPrefix)
{
	AtlTrace("Connecting Object\n");

	IScriptConnectionObject *sco;

	HRESULT hr = ::CoCreateInstance(CLSID_ScriptConnectionObject, NULL, CLSCTX_INPROC_SERVER, IID_IScriptConnectionObject, (void**)&sco);

	if (SUCCEEDED(hr))
	{
		sco->put_Script(iScript);
		hr = sco->Connect(pdispObj, bstrPrefix);
		if (SUCCEEDED(hr))
		{
			connections->add(sco);
		}
		else
			sco->Release();
	}

	return hr;
}

STDMETHODIMP CScriptUtils::get_Script(IDispatch **pVal)
{
	*pVal = iScript;
	return S_OK;
}

STDMETHODIMP CScriptUtils::put_Script(IDispatch *newVal)
{
	// Destroy any old connections
	DisconnectAll();

	iScript = newVal;

	return S_OK;
}

STDMETHODIMP CScriptUtils::Disconnect(IDispatch *pdispObj)
{
	AtlTrace("Disconnecting Object\n");

	for (int i=connections->count()-1; i >= 0; i--)
	{
		IScriptConnectionObject *sco = (*connections)[i];
		VARIANT_BOOL icf = false;
		sco->IsConnectionFor(pdispObj, &icf);

		if (icf)
		{
			sco->Disconnect();
			sco->Release();
			connections->remove(i);
			AtlTrace("Disconnected object\n");
		}
	}

	return S_OK;
}

STDMETHODIMP CScriptUtils::DisconnectAll()
{
	AtlTrace("Disconnecting ALL Objects\n");

	for (int i=connections->count()-1; i >= 0; i--)
	{
		IScriptConnectionObject *sco = (*connections)[i];
		sco->Disconnect();
		sco->Release();
		connections->remove(i);
	}

	return S_OK;
}

STDMETHODIMP CScriptUtils::Echo(BSTR s)
{
	LPOLESTR name = L"Trace";   
	DISPID id;
	HRESULT hr = iScriptHost->GetIDsOfNames(IID_NULL, 
                           &name, 
                           1, 
                           LOCALE_USER_DEFAULT, 
                           &id);

	if (SUCCEEDED(hr))
	{
		AtlTrace("CScriptUtils: Found Trace method\n");
		CComVariant varResult;
		CComVariant* pvars = new CComVariant[1];

		VariantClear(&varResult);
		_bstr_t _s(s);
		_s += "\r\n";
		pvars[0] = (BSTR)_s;
		DISPPARAMS disp = { pvars, NULL, 1, 0 };
		hr = iScriptHost->Invoke(id, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, &varResult, NULL, NULL);
		delete[] pvars;
	}

	if (FAILED(hr))
		AtlTrace(L"Failed to echo: %s\n", s);

	return S_OK;
}

STDMETHODIMP CScriptUtils::get_ScriptHost(IDispatch **pVal)
{
	*pVal = iScriptHost;

	return S_OK;
}

STDMETHODIMP CScriptUtils::put_ScriptHost(IDispatch *newVal)
{
	iScriptHost = newVal;

	return S_OK;
}
