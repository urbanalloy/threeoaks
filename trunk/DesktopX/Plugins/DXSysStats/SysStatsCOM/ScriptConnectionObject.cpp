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

// ScriptConnectionObject.cpp : Implementation of CScriptConnectionObject
#include "stdafx.h"
#include "SysStatsCOM.h"
#include "ScriptConnectionObject.h"

/////////////////////////////////////////////////////////////////////////////
// CScriptConnectionObject

STDMETHODIMP CScriptConnectionObject::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScriptConnectionObject
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CScriptConnectionObject::CScriptConnectionObject() :
	prefix(""),
	dwCookie(0),
	pConnection(0),
	pObj(0),
	typeInfo(0),
	iScript(0)
{
}

CScriptConnectionObject::~CScriptConnectionObject()
{
	AtlTrace("CScriptConnectionObject being destroyed\n");

	Disconnect();
}

/*
 * Get the typeinfo of the event source from the source IDispatch.
 */
STDMETHODIMP CScriptConnectionObject::SetSourceTypeInfo(IDispatch *pdispObj)
{
	IProvideClassInfo *pClassInfo;
	HRESULT hr;
	
	hr = pdispObj->QueryInterface(IID_IProvideClassInfo, (void**)&pClassInfo);

	if (SUCCEEDED(hr))
	{
		ITypeInfo *ptiCoClass;
		hr = pClassInfo->GetClassInfo(&ptiCoClass);

		if (SUCCEEDED(hr))
		{
			const int impltypeDefaultSource = IMPLTYPEFLAG_FDEFAULT | IMPLTYPEFLAG_FSOURCE;
			HREFTYPE hreftype;
			TYPEATTR *ptattr;

			typeInfo = NULL;
			ptiCoClass->GetTypeAttr(&ptattr);

			unsigned short cimpl = ptattr->cImplTypes;	// Number of implemented interfaces

			ptiCoClass->ReleaseTypeAttr(ptattr);

			hr = E_INVALIDARG;

			// Loop through the implemented interfaces looking for ITypeInfo of the eventsource
			for (unsigned short iimpl = 0 ; iimpl < cimpl ; iimpl++)
			{
				int impltype;
				ptiCoClass->GetImplTypeFlags(iimpl, &impltype);

				if ((impltype & impltypeDefaultSource) == impltypeDefaultSource )
				{
					ptiCoClass->GetRefTypeOfImplType(iimpl, &hreftype);
					ptiCoClass->GetRefTypeInfo(hreftype, &typeInfo);
					// Should I release typeInfo?
					hr = S_OK;
					break;
				}
			}

			ptiCoClass->Release();
		}

		pClassInfo->Release();
	}

	return hr;
}

/*
 * Disconnect from script.
 */
STDMETHODIMP CScriptConnectionObject::Disconnect()
{
	AtlTrace("CScriptConnectionObject::Disconnect()\n");

	if (pConnection)
	{
		try {
			pConnection->Unadvise(dwCookie);
			pConnection->Release();
		} catch (...)
		{
		}

		pConnection = 0;
	}

	if (typeInfo)
	{
		typeInfo->Release();
		typeInfo = 0;
	}

	return S_OK;
}



/*
 * Hook the passed IDispatch up with us. We will proxy events to our script.
 */
STDMETHODIMP CScriptConnectionObject::Connect(IDispatch *pdispObj, BSTR bstrPrefix)
{
	AtlTrace(L"Connect(%s)\n", bstrPrefix);

	// Get the type info of the source interface from the event source object
	HRESULT hr = SetSourceTypeInfo(pdispObj);

	if (FAILED(hr))
		return hr;

	IConnectionPointContainer *pContainer;

	// Get the object's connection point container
	hr = pdispObj->QueryInterface(IID_IConnectionPointContainer, (void **)&pContainer);
	if (SUCCEEDED(hr))
	{
		TYPEATTR *pTypeAttr;

		// Get the connection point from that container
		typeInfo->GetTypeAttr(&pTypeAttr);
		hr = pContainer->FindConnectionPoint(pTypeAttr->guid, &pConnection);
		if (SUCCEEDED(hr))
		{
			// Find out what interface we are supposed to be implementing!
			pConnection->GetConnectionInterface(&iid);
			// Patch up the dispatch table

			// Tell it we want calls
			IUnknown *pSinkUnk;
			QueryInterface (IID_IUnknown,(void **)&pSinkUnk);
			hr = pConnection->Advise(pSinkUnk, &dwCookie);

			if (SUCCEEDED(hr))
				pObj = pdispObj;

			pSinkUnk->Release();
		}

		// Release resources
		typeInfo->ReleaseTypeAttr(pTypeAttr);
		pContainer->Release();

		// Stash the prefix, etc., that we'll need to actually call the function,
		// in the member data for the connection sink.
		prefix = bstrPrefix;
	}

	return hr;
}

/*
 * This will be called by the object that we sink. Pass the call to the script.
 */
STDMETHODIMP CScriptConnectionObject::Invoke(
	DISPID nDispID,
	REFIID rIID,
	LCID oLCID,
	WORD wFlags,
	DISPPARAMS *pdp,
	VARIANT *pvarRes,
	EXCEPINFO *pexcepinfo,
	UINT *perr)
{

	// Get the name of the calling function. There better be just one.
	BSTR bstrName;
	unsigned int cNames = 0;
	typeInfo->GetNames(nDispID, &bstrName, 1, &cNames);
	AtlTrace(L"CScriptConnectionObject::Invoke(%s)\n", bstrName);

	// Calculate the name of the script function :)
	_bstr_t sfName = prefix + bstrName;
	BSTR names[1] = { sfName };

	// Get the DISPID of the script function
	DISPID id;

	HRESULT hr = iScript->GetIDsOfNames(IID_NULL, names, 1, oLCID, &id);

	// Call it
	if (SUCCEEDED(hr))
	{
		AtlTrace("CScriptConnectionObject: Found script function\n");

		// The invoke might cause this object to be destroyed so addref this
		AddRef();
		hr = iScript->Invoke(id, rIID, oLCID, wFlags, pdp, pvarRes, pexcepinfo, perr);
		if (SUCCEEDED(hr))
			AtlTrace(L"CScriptConnectionObject: Call to %s succeeded\n", names[0]);
		Release();
	}

	return hr;
}


STDMETHODIMP CScriptConnectionObject::get_Script(IDispatch **pVal)
{
	*pVal = iScript;

	return S_OK;
}

STDMETHODIMP CScriptConnectionObject::put_Script(IDispatch *newVal)
{
	iScript = newVal;

	return S_OK;
}

STDMETHODIMP CScriptConnectionObject::IsConnectionFor(IDispatch *pdispObj, VARIANT_BOOL *pRet)
{
	*pRet = pdispObj == pObj;

	return S_OK;
}
