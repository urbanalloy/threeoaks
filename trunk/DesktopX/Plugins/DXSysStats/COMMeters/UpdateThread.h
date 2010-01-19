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

// UpdateThread.h: interface for the UpdateThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UPDATETHREAD_H__A10F6F7C_F931_4CB1_BFD0_0343F2523EE9__INCLUDED_)
#define AFX_UPDATETHREAD_H__A10F6F7C_F931_4CB1_BFD0_0343F2523EE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ArrayPtr.h"

#import <msxml4.dll> no_implementation
using namespace MSXML2;

class CXMLMeter;

class UpdateThread
{
public:
	UpdateThread(CXMLMeter *meter, BSTR URL);
	~UpdateThread();

	void stop();

	STDMETHOD(GetNodeList)(/*[in]*/ BSTR selector, /*[out, retval]*/ IDispatch* *pNodeList);
	STDMETHOD(put_Value)(/*[in]*/ BSTR newVal);
	STDMETHOD(GetAsString)(/*[in]*/ BSTR format, /*[in]*/ BSTR selector, /*[out, retval]*/ BSTR *pRet);
	STDMETHOD(GetAsDouble)(/*[in]*/ BSTR selector, /*[out, retval]*/ double *pRet);
	STDMETHOD(GetAsLong)(/*[in]*/ BSTR selector, /*[out,retval]*/ long *pRet);
	STDMETHOD(GetValue)(/*[out,retval]*/ VARIANT *pRet);

	STDMETHOD(get_LoadFailed)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_LoadFailed)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_Namespaces)(/*[out, retval]*/ LPSAFEARRAY *pVal);
	STDMETHOD(put_Namespaces)(/*[in]*/ LPSAFEARRAY newVal);
	STDMETHOD(get_UseXPath)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_UseXPath)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_ProxyPassword)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ProxyPassword)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ProxyUsername)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ProxyUsername)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ProxyURL)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ProxyURL)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Password)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Password)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Username)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Username)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_URL)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_URL)(/*[in]*/ BSTR newVal);

	STDMETHOD(Update)(/*[out, retval]*/ long *dirty);

	void getData();

protected:
	void ReportError(IXMLDOMDocumentPtr pDoc);

private:
	bool finished;
	CXMLMeter *meter;

	_bstr_t URL;
	_bstr_t username;
	_bstr_t password;
	_bstr_t proxyURL;
	_bstr_t proxyUsername;
	_bstr_t proxyPassword;
	bool useXPath;
	ArrayPtr<_bstr_t> namespaces;
	VARIANT_BOOL loadFailed;

	HANDLE hXMLMutex;
	HANDLE hUpdateEvent;
	HANDLE hUpdateThread;
	IXMLDOMDocumentPtr pXMLDoc;
	MSXML2::IXMLDOMDocument *pThreadDoc;
	IStream	*docStream;

	DWORD WINAPI updateThread(/*LPVOID lpInput*/);
	static DWORD WINAPI _updateThread(LPVOID in);
};

#endif // !defined(AFX_UPDATETHREAD_H__A10F6F7C_F931_4CB1_BFD0_0343F2523EE9__INCLUDED_)
