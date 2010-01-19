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

// ActiveScriptImpl.h: interface for the ActiveScriptImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIVESCRIPTIMPL_H__F03BF05E_8185_45D9_AE15_F5219418C3A5__INCLUDED_)
#define AFX_ACTIVESCRIPTIMPL_H__F03BF05E_8185_45D9_AE15_F5219418C3A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <activscp.h>

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

#include <comdef.h>

#include "DictionaryPtr.cpp"

#define SCRIPTITEM_NAMEDITEM		(SCRIPTITEM_ISSOURCE | \
									 SCRIPTITEM_ISVISIBLE | \
									 SCRIPTITEM_GLOBALMEMBERS)

#define LANG_ENGLISH_NEUTRAL	(MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL))
#define LOCALE_SCRIPT_DEFAULT	(MAKELCID(LANG_ENGLISH_NEUTRAL, SORT_DEFAULT))

class ActiveScriptImpl :
	public IActiveScriptSite,
	public IActiveScriptSiteWindow
{
public:
	ActiveScriptImpl();
	virtual ~ActiveScriptImpl();

public:
// IActiveScriptSite
	STDMETHOD(GetLCID)(LCID * plcid);
	STDMETHOD(GetItemInfo)(LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown * * ppiunkItem, ITypeInfo * * ppti);
	STDMETHOD(GetDocVersionString)(BSTR * pbstrVersion);
	STDMETHOD(OnScriptTerminate)(const VARIANT * pvarResult, const EXCEPINFO * pexcepinfo);
	STDMETHOD(OnStateChange)(SCRIPTSTATE ssScriptState);
	STDMETHOD(OnScriptError)(IActiveScriptError * pscripterror);
	STDMETHOD(OnEnterScript)();
	STDMETHOD(OnLeaveScript)();

public:
// IActiveScriptSiteWindow
	STDMETHOD(GetWindow)(HWND * phwnd);
	STDMETHOD(EnableModeless)(BOOL fEnable);

#ifdef ASD
public:
// IActiveScriptSiteDebug
	STDMETHOD(GetDocumentContextFromPosition)(DWORD_PTR dwSourceContext, ULONG uCharacterOffset, ULONG uNumChars, IDebugDocumentContext**  ppsc) { return E_NOTIMPL; }
	STDMETHOD(GetApplication)(IDebugApplication**  ppda) { return E_NOTIMPL; }
	STDMETHOD(GetRootApplicationNode)(IDebugApplicationNode**  ppdanRoot) { return E_NOTIMPL; }
	STDMETHOD(OnScriptErrorDebug)(IActiveScriptErrorDebug*  pErrorDebug, BOOL* pfEnterDebugger, BOOL* pfCallOnScriptErrorWhenContinuing) { return E_NOTIMPL; }
#endif

public:
// This class
	STDMETHOD(get_ScriptType)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ScriptType)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_FileName)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_FileName)(/*[in]*/ BSTR newVal);
	STDMETHOD(Trace)(/*[in]*/ BSTR msg);
	virtual BOOL CreateEngine();
	virtual BOOL LoadAndRun();
	virtual BOOL LoadScriptCode(BSTR *ScriptText);
	virtual BOOL AddScriptCode(BSTR scriptCode);
	virtual BOOL AddScriptItem(BSTR namedItem, LPUNKNOWN lpUnknown, DWORD dwFlags = SCRIPTITEM_NAMEDITEM);
	virtual BOOL AddDefaultScriptItem() = 0;
	virtual void ReleaseObjects();

	static const int MY_SCRIPTITEM_WEAK;

protected:
	IActiveScript	  *scriptPtr;	// Active Script
	IActiveScriptParse *scriptParsePtr;
	_bstr_t fileName;
	_bstr_t scriptType;

	SCRIPTSTATE state;
	BOOL inScript;
	BOOL needToConnect;

	DictionaryPtr<const OLECHAR, IUnknown> namedItems;
	DictionaryPtr<const OLECHAR, int> weakItems;
};

#endif // !defined(AFX_ACTIVESCRIPTIMPL_H__F03BF05E_8185_45D9_AE15_F5219418C3A5__INCLUDED_)
