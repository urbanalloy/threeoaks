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

// ActiveScriptImpl.cpp: implementation of the ActiveScriptImpl class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_DEBUG)
#define AFX_CDECL __cdecl


// Memory tracking allocation
void* AFX_CDECL operator new(size_t nSize, const char* lpszFileName, int nLine);
#define DEBUG_NEW new(THIS_FILE, __LINE__)
#if _MSC_VER >= 1200
void AFX_CDECL operator delete(void* p, const char* lpszFileName, int nLine);
#endif

#endif // _DEBUG


#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

#include "ActiveScriptImpl.h"

const int ActiveScriptImpl::MY_SCRIPTITEM_WEAK = 0x40000000;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static bool oleEqFunc(LPCOLESTR lhs, LPCOLESTR rhs)
{
	return wcscmp(lhs, rhs) == 0;
}

ActiveScriptImpl::ActiveScriptImpl() :
	fileName(""),
	scriptType(""),
	namedItems(oleEqFunc),
	weakItems(oleEqFunc),
	scriptPtr(0),
	scriptParsePtr(0),
	needToConnect(false)
{

}

ActiveScriptImpl::~ActiveScriptImpl()
{
	ReleaseObjects();
}

void ActiveScriptImpl::ReleaseObjects()
{
	if (scriptParsePtr != NULL)
	{
		scriptParsePtr->Release();
		scriptParsePtr = NULL;
	}

	if (scriptPtr != NULL)
	{
		const OLECHAR **pKeys = new const OLECHAR*[weakItems.count()];
		weakItems.getKeys(pKeys);
		for (int i=0; i<weakItems.count(); i++)
		{
			int *pInt = weakItems.get(pKeys[i]);	// To counteract what 'Close()' is about to do.
			for (int j=0; j < *pInt; j++)
				namedItems.get(pKeys[i])->AddRef();
		}
		delete[] pKeys;

		scriptPtr->Close();
		scriptPtr->Release();
		scriptPtr = NULL;
	}

	namedItems.clear();
	weakItems.clear();
}


BOOL ActiveScriptImpl::CreateEngine()
{
	HRESULT hr;
	CLSID clsid;

	ReleaseObjects();

	// Search for LanguageID and create instance of script engine
	hr = CLSIDFromProgID(scriptType, &clsid);
	if (!SUCCEEDED(hr))
		return false;

	hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IActiveScript, (void **)&scriptPtr);
	if (SUCCEEDED(hr))
	{
		hr = scriptPtr->SetScriptSite(this);
		if (SUCCEEDED(hr))
		{
			// Script Engine must support IActiveScriptParse for us to use it
			scriptPtr->QueryInterface(__uuidof(IActiveScriptParse), (void**)&scriptParsePtr);
			hr = scriptParsePtr->InitNew();
			if (SUCCEEDED(hr))
			{
				hr = scriptPtr->SetScriptState(SCRIPTSTATE_INITIALIZED);
				if (SUCCEEDED(hr))
					return true;
			}
		}
	}
	else
	{
		// If this happens, the scripting engine is probably not properly registered
	}

	ReleaseObjects();
	return false;
}

BOOL ActiveScriptImpl::LoadAndRun()
{
	BSTR code;

	if ((scriptPtr != NULL) && LoadScriptCode(&code))
	{
		needToConnect = false;

		if (AddScriptCode(code) && AddDefaultScriptItem())	// Necessary for us to be able to call script methods
		{
			::SysFreeString(code);
			HRESULT hr = scriptPtr->SetScriptState(SCRIPTSTATE_STARTED);

			if (!SUCCEEDED(hr))
				return false;

			if (needToConnect)
				hr = scriptPtr->SetScriptState(SCRIPTSTATE_CONNECTED);

			if (!SUCCEEDED(hr))
				return false;
		}
		return true;
	}

	return false;
}

BOOL ActiveScriptImpl::LoadScriptCode(BSTR *ScriptText)
{
  _bstr_t pth = fileName;
  
  int fp = _wopen(pth, _O_RDONLY|_O_BINARY);
  if( fp > 0 )
  {
    _lseek(fp, 0, SEEK_END);
    int siz = _tell(fp);
    _lseek(fp, 0, SEEK_SET);
    char *pSt = new char[siz+1];
    _read(fp, pSt, siz);
    _close(fp);
    pSt[siz] = 0;
    *ScriptText = _com_util::ConvertStringToBSTR(pSt);
	return true;
  }

  return false;
}

BOOL ActiveScriptImpl::AddScriptCode(BSTR scriptText)
{
	if (scriptPtr == NULL)
		return false;

	HRESULT hr;
	EXCEPINFO ei = { 0 };
	hr = scriptParsePtr->ParseScriptText(scriptText, 0, 0, 0, 0, 0, 
							SCRIPTTEXT_ISPERSISTENT|SCRIPTTEXT_ISVISIBLE,
							0, &ei);
	if (!SUCCEEDED(hr))
		return false;

	return true;
}

BOOL ActiveScriptImpl::AddScriptItem(BSTR namedItem, LPUNKNOWN lpUnknown, DWORD dwFlags)
{
	// Add Top-level Global Named Item
	if (scriptPtr == NULL || lpUnknown == NULL)
		return false;

	HRESULT hr;

	if (namedItems.contains(namedItem))
		return true;	// This item has already been added

	// Need to save the item so that GetItemInfo() will work.
	AtlTrace(L"Adding named item called %s\n", namedItem);
	namedItems.put(namedItem, lpUnknown);
	if (dwFlags & MY_SCRIPTITEM_WEAK)
	{
		int *pInt = new int[1];
		*pInt = 0;
		weakItems.put(namedItem, pInt);
	}

	hr = scriptPtr->AddNamedItem(namedItem, dwFlags & SCRIPTITEM_ALL_FLAGS);
	if (!SUCCEEDED(hr))
	{
		namedItems.remove(namedItem);
		weakItems.remove(namedItem);
		return false;
	}

	// Connected to object sink-interface.
	if (dwFlags & SCRIPTITEM_ISSOURCE)
		needToConnect = true;

	return true;
}

STDMETHODIMP ActiveScriptImpl::get_FileName(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = fileName.copy();

	return S_OK;
}

STDMETHODIMP ActiveScriptImpl::put_FileName(BSTR newVal)
{
	fileName = newVal;
	CreateEngine();

	if (LoadAndRun())
		return S_OK;
	else
		return E_INVALIDARG;
}

STDMETHODIMP ActiveScriptImpl::get_ScriptType(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = scriptType.copy();

	return S_OK;
}

STDMETHODIMP ActiveScriptImpl::put_ScriptType(BSTR newVal)
{
	scriptType = newVal;
	CreateEngine();

	if (LoadAndRun())
		return S_OK;
	else
		return E_INVALIDARG;
}

// IActiveScript methods
STDMETHODIMP ActiveScriptImpl::GetLCID(LCID * plcid)
{
	if (plcid == NULL)
		return E_POINTER;
		
	return E_NOTIMPL;
}

STDMETHODIMP ActiveScriptImpl::GetItemInfo(
			/* [in] */ LPCOLESTR pstrName,
			/* [in] */ DWORD dwReturnMask,
			/* [out] */LPUNKNOWN* ppiunkItem,
			/* [out] */LPTYPEINFO* ppti)
{
	if (dwReturnMask & SCRIPTINFO_ITYPEINFO)
	{
		if (!ppti)
			return E_INVALIDARG;
		*ppti = NULL;
	}

	if (dwReturnMask & SCRIPTINFO_IUNKNOWN)
	{
		if (!ppiunkItem)
			return E_INVALIDARG;
		*ppiunkItem = NULL;
	}

	LPUNKNOWN lpUnknown = namedItems.get(pstrName);
	if (NULL == lpUnknown)
	{
		ATLTRACE("No IUnknown for item!\n");
		// This shouldn't ever happen, since we're providing the items
		return TYPE_E_ELEMENTNOTFOUND;
	}

	if (dwReturnMask & SCRIPTINFO_ITYPEINFO)
	{
		// Use IProvideClassInfo to get ITypeInfo of coclass!
		IProvideClassInfo *pci = NULL;
		HRESULT hr = lpUnknown->QueryInterface(IID_IProvideClassInfo, (void**)&pci);
		if (SUCCEEDED(hr) && pci)
		{
			hr = pci->GetClassInfo(ppti);
		}

		// Release interface
		if (pci)
			pci->Release();

		if (FAILED(hr))
			return E_FAIL;
	}

	if (dwReturnMask & SCRIPTINFO_IUNKNOWN)
	{
		*ppiunkItem = lpUnknown;
		int *pInt = weakItems.get(pstrName);
		if (!pInt)
			(*ppiunkItem)->AddRef();    // because returning
		else
			*pInt = (*pInt) + 1;
	}

	return S_OK;
}

STDMETHODIMP ActiveScriptImpl::GetDocVersionString(BSTR * pbstrVersion)
{
	if (pbstrVersion == NULL)
		return E_POINTER;
		
	if(fileName.length())
	{
		get_FileName(pbstrVersion);
		return S_OK;
	}
	return E_FAIL;
}

STDMETHODIMP ActiveScriptImpl::OnScriptTerminate(const VARIANT * pvarResult, const EXCEPINFO * pexcepinfo)
{
	return E_NOTIMPL;
}

STDMETHODIMP ActiveScriptImpl::OnStateChange(SCRIPTSTATE ssScriptState)
{
	state = ssScriptState;

	return S_OK;
}

STDMETHODIMP ActiveScriptImpl::Trace(BSTR msg)
{
	AtlTrace(L"%s", msg);

	return S_OK;
}

STDMETHODIMP ActiveScriptImpl::OnScriptError(IActiveScriptError * pscripterror)
{
	EXCEPINFO ep = {0};
	::memset(&ep, 0, sizeof ep);

	CComBSTR errLin;
	DWORD Context; ULONG Line; LONG CharPos;
	CComPtr<IActiveScriptError> pErr = pscripterror;
	if(pErr)
	{
		pErr->GetExceptionInfo(&ep);
		pErr->GetSourceLineText(&errLin);
		pErr->GetSourcePosition(&Context, &Line, &CharPos);
		wchar_t buf[MAX_PATH];
		_snwprintf(buf, MAX_PATH, L"%s at line %ld, char %ld: %s\r\n", ep.bstrDescription, Line, CharPos, errLin);
		Trace(buf);
		if (ep.bstrSource) ::SysFreeString(ep.bstrSource); // Source of the exception.
		if (ep.bstrDescription) ::SysFreeString(ep.bstrDescription); // Source of the exception.
		if (ep.bstrHelpFile) ::SysFreeString(ep.bstrHelpFile); // Source of the exception.
	}

	return S_OK;
}

STDMETHODIMP ActiveScriptImpl::OnEnterScript()
{
	inScript = TRUE;
	return S_OK;
}

STDMETHODIMP ActiveScriptImpl::OnLeaveScript()
{
	inScript = FALSE;
	return S_OK;
}

// IActiveScriptSiteWindow
STDMETHODIMP ActiveScriptImpl::GetWindow(HWND * phwnd)
{
	if (phwnd == NULL)
		return E_POINTER;

	*phwnd = GetDesktopWindow();
	return S_OK;
}

STDMETHODIMP ActiveScriptImpl::EnableModeless(BOOL fEnable)
{
	return S_OK;
}

