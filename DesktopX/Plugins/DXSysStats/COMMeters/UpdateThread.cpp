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

// UpdateThread.cpp: implementation of the UpdateThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <stdio.h>
#include "SysStatsUtils.h"
#include "ArrayPtr.cpp"
#include "UpdateThread.h"
#include "XMLMeter.h"
#import <msxml4.dll> implementation_only

static _bstr_t percentS("%s");
static _bstr_t emptyStr("");

DWORD WINAPI UpdateThread::_updateThread(LPVOID in)
{
	UpdateThread *ut = (UpdateThread*)in;

	DWORD ret = ut->updateThread();

	AtlTrace("ut9\n");

	try {
		delete ut;	// Only way to guarantee that the object hangs around until
						// the thread exits without waiting on the thread.
	} catch (...) {
		AtlTrace("Caught exception while deleting update thread!\n");
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UpdateThread::UpdateThread(CXMLMeter *meter, BSTR URL) :
	pXMLDoc(NULL),
	docStream(0),
	URL(URL),
	username(""),
	password(""),
	proxyURL(""),
	proxyUsername(""),
	proxyPassword(""),
	meter(meter),
	useXPath(false),
	loadFailed(false)
{
AtlTrace("ut::ut1\n");
 
	hUpdateEvent = CreateEvent( 
        NULL,         // no security attributes
        FALSE,        // auto-reset event
        FALSE,        // initial state is non-signaled - i.e. waiters will block
        NULL		 // object name
     );

	if (hUpdateEvent == NULL)
		SysStatsUtils::checkError();

	hXMLMutex = CreateMutex( 
		NULL,		// no security attributes
		FALSE,		// initially not owned
		NULL	// object name
	);

	if (hXMLMutex == NULL)
		SysStatsUtils::checkError();

	finished = FALSE;

	AtlTrace("ut::ut2\n");

	pXMLDoc.CreateInstance("Msxml2.DOMDocument", NULL, CLSCTX_INPROC_SERVER);
	pXMLDoc->async = false;
	LPOLESTR lpolestr;
	StringFromCLSID(__uuidof(pXMLDoc), &lpolestr);
	AtlTrace(L"%s\n", lpolestr);
	/*HRESULT hr = */CoMarshalInterThreadInterfaceInStream(__uuidof(pXMLDoc), pXMLDoc, &docStream);
	DWORD threadID;
	hUpdateThread = CreateThread(NULL, 0, _updateThread, (VOID *) this, 0, &threadID);
	::CoTaskMemFree(lpolestr);
}

UpdateThread::~UpdateThread()
{
	AtlTrace("Deleting update thread\n");
	CloseHandle(hUpdateEvent);
	hUpdateEvent = NULL;
	CloseHandle(hXMLMutex);
	hXMLMutex = NULL;
	CloseHandle(hUpdateThread);
	hUpdateThread = NULL;

	namespaces.clearAndDestroy();
}

void UpdateThread::stop()
{
	finished = TRUE;
	SetEvent(hUpdateEvent);	// Wake up the update thread so that it can quit.
	::SwitchToThread();	// Give it a chance to drain before we destroy ourselves.
}

STDMETHODIMP UpdateThread::Update(long *dirty)
{
	AtlTrace("ut::u1\n");
	if (!SetEvent(hUpdateEvent))
		SysStatsUtils::checkError();	// Wake up the update thread.
	*dirty = false;

	AtlTrace("ut::u2\n");

	return S_OK;
}

STDMETHODIMP UpdateThread::get_URL(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = URL.copy();
		
	return S_OK;
}

STDMETHODIMP UpdateThread::put_URL(BSTR newVal)
{
	URL = newVal;

	return S_OK;
}

STDMETHODIMP UpdateThread::get_Username(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = username.copy();
		
	return S_OK;
}

STDMETHODIMP UpdateThread::put_Username(BSTR newVal)
{
	username = newVal;

	return S_OK;
}

STDMETHODIMP UpdateThread::get_Password(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = password.copy();
		
	return S_OK;
}

STDMETHODIMP UpdateThread::put_Password(BSTR newVal)
{
	password = newVal;

	return S_OK;
}

STDMETHODIMP UpdateThread::get_ProxyURL(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = proxyURL.copy();
		
	return S_OK;
}

STDMETHODIMP UpdateThread::put_ProxyURL(BSTR newVal)
{
	proxyURL = newVal;

	return S_OK;
}

STDMETHODIMP UpdateThread::get_ProxyUsername(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = proxyUsername.copy();
		
	return S_OK;
}

STDMETHODIMP UpdateThread::put_ProxyUsername(BSTR newVal)
{
	proxyUsername = newVal;

	return S_OK;
}

STDMETHODIMP UpdateThread::get_ProxyPassword(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = proxyPassword.copy();
		
	return S_OK;
}

STDMETHODIMP UpdateThread::put_ProxyPassword(BSTR newVal)
{
	proxyPassword = newVal;

	return S_OK;
}

STDMETHODIMP UpdateThread::get_UseXPath(VARIANT_BOOL *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = useXPath;
		
	return S_OK;
}

STDMETHODIMP UpdateThread::put_UseXPath(VARIANT_BOOL newVal)
{
	useXPath = (newVal == VARIANT_TRUE) ? true : false;

	return S_OK;
}

STDMETHODIMP UpdateThread::get_Namespaces(LPSAFEARRAY *pVal)
{
	SAFEARRAY *pArray = ::SafeArrayCreateVector(VT_BSTR, 0, namespaces.count());

	long indices[1];
	for (int i=0; i<namespaces.count(); i++)
	{
		indices[0] = i;
		::SafeArrayPutElement(pArray, indices, namespaces[i]->copy());
	}

	*pVal = pArray;

	return S_OK;
}

STDMETHODIMP UpdateThread::put_Namespaces(LPSAFEARRAY newVal)
{
	namespaces.clearAndDestroy();

	long lb = 0;
	long ub = 0;
	VARTYPE vt;
	::SafeArrayGetVartype(newVal, &vt);
	if (vt != VT_BSTR)
		return E_INVALIDARG;

	HRESULT hr = ::SafeArrayGetUBound(newVal, 1, &ub);
	hr = ::SafeArrayGetLBound(newVal, 1, &lb);

	long indices[1];
	for (int i=lb; i<=ub; i++)
	{
		indices[0] = i;
		BSTR bStr = 0;
		::SafeArrayGetElement(newVal, indices, &bStr);
		AtlTrace(L"Setting %d to %s", i, bStr);
		namespaces.add(new _bstr_t(bStr));
	}

	return S_OK;
}

STDMETHODIMP UpdateThread::put_Value(BSTR newVal)
{
	// Wait to be signalled
	if (WaitForSingleObject(hXMLMutex, INFINITE) == WAIT_OBJECT_0)
	{
		try {
			pXMLDoc->loadXML(newVal);
		} catch (...) {
			AtlTrace("Caught exception trying to load XML from string\n");
		}

		ReleaseMutex(hXMLMutex);
	}

	return S_OK;
}

STDMETHODIMP UpdateThread::GetNodeList(BSTR xpath, IDispatch **pNodeList)
{
	if (pNodeList == NULL)
		return E_POINTER;

	MSXML2::IXMLDOMNodeListPtr pNL = pXMLDoc->selectNodes(xpath);
	AtlTrace("%d\n", pNL->length);
	*pNodeList = (::MSXML2::IXMLDOMNodeList*)pNL;
	(*pNodeList)->AddRef();

	return S_OK;
}

STDMETHODIMP UpdateThread::GetAsString(BSTR format, BSTR selector, BSTR *pRet)
{
	_bstr_t format_t(format);
	_bstr_t selector_t(selector);

	AtlTrace("ut::gas %s,%s\n",(char*)format_t,(char*)selector_t);

	// Gross Hack
	if (selector_t == _bstr_t("loadFailed")) {
		wchar_t buf[MAX_PATH];
		if (loadFailed)
			_snwprintf(buf, MAX_PATH, format, L"failed");
		else
			_snwprintf(buf, MAX_PATH, format, L"succeeded");

		buf[MAX_PATH-1] = 0;
		*pRet = ::SysAllocString(buf);
		return S_OK;
	}

	// Can't wait, but we DO need this mutex
	if (WaitForSingleObject(hXMLMutex, 50) == WAIT_OBJECT_0)
	{
		if (pXMLDoc != NULL && pXMLDoc->documentElement != NULL)
		{
			if (useXPath)
			{
				MSXML2::IXMLDOMDocumentPtr xslt;
				xslt.CreateInstance("Msxml2.DOMDocument", NULL, CLSCTX_INPROC_SERVER);
				try
				{
					// Actually use XSLT...
					AtlTrace("Using XSLT\n");

					_bstr_t xsl;
					xsl += L"<?xml version=\"1.0\"?><xsl:stylesheet xmlns:xsl=\" http://www.w3.org/1999/XSL/Transform\" version=\"1.0\"";

					for (int i=0; i<namespaces.count(); i++)
					{
						xsl += L" ";
						xsl += *(namespaces[i]);
					}

					xsl += L"><xsl:output omit-xml-declaration=\"yes\"/><xsl:template match=\"/\"><xsl:value-of select=\"";
					xsl += selector;
					xsl += L"\"/></xsl:template></xsl:stylesheet>\n";

					AtlTrace((BSTR)xsl);

					xslt->async = false;
					xslt->loadXML((BSTR)xsl);
					// This is supposed to return a BSTR. It actually returns a _bstr_t
					// which promptly destructs and frees the result! So anyway, we just
					// put it into a _bstr_t.
					_bstr_t result = pXMLDoc->transformNode(xslt);
					AtlTrace(L"Result of query: %s\n", (BSTR)result);

					AtlTrace("ut::gas1\n");

					wchar_t buf[MAX_PATH];
					_snwprintf(buf, MAX_PATH, format, (BSTR)result);
					buf[MAX_PATH-1] = 0;
					*pRet = ::SysAllocString(buf);

					// Apparently need to do this or the document will hang around!
					xslt->loadXML("");
					ReleaseMutex(hXMLMutex);
					return S_OK;
				} catch (const _com_error &e)
				{
					AtlTrace("Caught Exception %s\n", e.ErrorMessage());
				} catch (...)
				{
					AtlTrace("Caught Exception\n");
					ReportError(xslt);
				}
			}
			else
			{
				try
				{
					AtlTrace("Using XSL Pattern\n");
					MSXML2::IXMLDOMNodePtr node = pXMLDoc->selectSingleNode(selector_t);
					if (node != NULL)
					{

						_bstr_t nt = node->nodeTypeString;
						_bstr_t t = node->nodeTypedValue;
						char buf[MAX_PATH];
						_snprintf(buf, sizeof(buf), _bstr_t(format), (char*)t);
						buf[MAX_PATH-1] = 0;

						*pRet = _com_util::ConvertStringToBSTR(buf);
						AtlTrace("%s\n", buf);
						ReleaseMutex(hXMLMutex);
						return S_OK;
					}
				} catch (...)
				{
					AtlTrace("Caught Exception\n");
					ReportError(pXMLDoc);
				}
			}
		}

		ReleaseMutex(hXMLMutex);
	}

	*pRet = format_t.copy();

	return S_OK;
}

void UpdateThread::ReportError(MSXML2::IXMLDOMDocumentPtr pDoc)
{
	MSXML2::IXMLDOMParseErrorPtr parseError = pDoc->GetparseError();
	BSTR bstrReason = parseError->Getreason();
	long ulCode = parseError->GeterrorCode();
	long ulLine = parseError->Getline();

	if (ulLine != 0)
	{
		// It's a parsing error
		long ulColumn = parseError->Getlinepos();
		AtlTrace(L"Error: 0x%0.8x - %s at %d:%d\n", ulCode, bstrReason, ulLine, ulColumn);
	}
	else
	{
		if ((HRESULT) ulCode == INET_E_OBJECT_NOT_FOUND)
		{
			AtlTrace("Object not found\n");
		}
		else if (ulCode == 0)
		{
			SysStatsUtils::checkError();
		}
		else
		{
			AtlTrace(L"Error: 0x%0.8x - %s\n", ulCode, bstrReason);
		}
	}
}

STDMETHODIMP UpdateThread::GetAsLong(BSTR selector, long *pRet)
{
	BSTR pVal = 0;
	GetAsString(percentS, selector, &pVal);
	sscanf(_bstr_t(pVal, false), "%ld", pRet);

	return S_OK;
}

STDMETHODIMP UpdateThread::GetAsDouble(BSTR selector, double *pRet)
{
	BSTR pVal = 0;
	GetAsString(percentS, selector, &pVal);
	sscanf(_bstr_t(pVal, false), "%lg", pRet);

	return S_OK;
}

STDMETHODIMP UpdateThread::GetValue(VARIANT *pRet)
{
	BSTR s = 0;
	GetAsString(percentS, emptyStr, &s);
	CComVariant(s).Detach(pRet);
	::SysFreeString(s);

	return S_OK;
}

/*
 * The thread that retrieves the data
 */
DWORD WINAPI UpdateThread::updateThread(/*LPVOID lpInput*/)
{

	AtlTrace("ut1\n");

	::CoInitialize(NULL);

	AtlTrace("ut2\n");

	HRESULT hr = CoGetInterfaceAndReleaseStream(docStream, __uuidof(pThreadDoc), (void**)&pThreadDoc);
	if (SUCCEEDED(hr))
	{
		pThreadDoc->AddRef();

		AtlTrace("ut3\n");

		while (!finished)
		{

			AtlTrace("ut4\n");

			// Wait to be signalled
			if (WaitForSingleObject(hUpdateEvent, INFINITE) == WAIT_OBJECT_0)
			{
				AtlTrace("ut5\n");

				// Event will now be non-signalled so waiters (e.g. us) will block

				//Retrieve the XML document
				if (!finished)
				{
					try {
						if (URL != _bstr_t(""))
							getData();
					} catch (...) {
						AtlTrace("Exception in getData()!\n");
					}
				}
				// Go back and wait to be signalled again
			}

			AtlTrace("ut6\n");
		}

		AtlTrace("ut7\n");

		pThreadDoc->Release();
	}

	//Exit the thread
	CoUninitialize();
	AtlTrace("ut8\n");

	return 0;
}

void UpdateThread::getData()
{
	AtlTrace("*** Trying to load XML document at %s\n", (char*)URL);

	bool gotMutex = false;
	loadFailed = true;

	HRESULT hr;
	try {
		IServerXMLHTTPRequest2Ptr pRequest = NULL;
		hr=pRequest.CreateInstance("Msxml2.ServerXMLHTTP.4.0", NULL, CLSCTX_INPROC_SERVER);

		hr=pRequest->open("GET", URL, false, username, password);

		AtlTrace(L"%s, %s\n", (BSTR)proxyURL, (BSTR)emptyStr);
		if (proxyURL != emptyStr)
		{
			pRequest->setProxy(SXH_PROXY_SET_PROXY, proxyURL);
			pRequest->setProxyCredentials(proxyUsername, proxyPassword);
		}

		hr=pRequest->send();
		if (SUCCEEDED(hr))
		{
			BSTR bStr = 0;
			pRequest->get_responseText(&bStr);

			// Now wait for at least 5 secs for access to pThreadDoc
			if (WaitForSingleObject(hXMLMutex, 5000) == WAIT_OBJECT_0)
			{
				gotMutex = true;
				pThreadDoc->loadXML(bStr);
				loadFailed = false;
				ReleaseMutex(hXMLMutex);
				gotMutex = false;
			}

			::SysFreeString(bStr);
			meter->put_dirty(true);
		}
	} catch (...) {

		if (gotMutex)
		{
			ReleaseMutex(hXMLMutex);
			gotMutex = false;
		}

		AtlTrace("*** MSXML4 failed, trying standard %s\n", (char*)URL);

		try {
			MSXML2::IXMLDOMDocumentPtr pRequest;
			pRequest.CreateInstance("Msxml2.DOMDocument", NULL, CLSCTX_INPROC_SERVER);
			pRequest->async = false;
			pRequest->load(URL);
			if (pRequest->documentElement != 0)
			{
				AtlTrace("Using MSXML2 worked\n");
				BSTR bStr = 0;
				pRequest->get_xml(&bStr);

				// Now wait for at least 5 secs for access to pThreadDoc
				if (WaitForSingleObject(hXMLMutex, 5000) == WAIT_OBJECT_0)
				{
					gotMutex = true;
					pThreadDoc->loadXML(bStr);
					loadFailed = false;
					ReleaseMutex(hXMLMutex);
					gotMutex = false;
				}

				::SysFreeString(bStr);
				meter->put_dirty(true);
			}

/*
			// Now wait for at least 5 secs for access to pThreadDoc
			if (WaitForSingleObject(hXMLMutex, 5000) == WAIT_OBJECT_0)
			{
				gotMutex = true;
				pThreadDoc->load(URL);
				ReleaseMutex(hXMLMutex);
				gotMutex = false;
			}
			meter->put_dirty(true);
*/
		} catch (...) {
			AtlTrace("load threw exception\n");

		}
	}

//		if (pRequest != NULL)
//			pRequest.Release();


	if (loadFailed)
		AtlTrace("*** Failed to load %s\n", (char*)URL);
	else
		AtlTrace("*** Loaded %s\n", (char*)URL);

	// Release pXMLDoc
	if (gotMutex)
		ReleaseMutex(hXMLMutex);

	AtlTrace("released mutex\n", (char*)URL);

	AtlTrace("returning\n");
}

STDMETHODIMP UpdateThread::get_LoadFailed(VARIANT_BOOL *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = loadFailed;

	return S_OK;
}

STDMETHODIMP UpdateThread::put_LoadFailed(VARIANT_BOOL newVal)
{
	loadFailed = newVal;

	return S_OK;
}
