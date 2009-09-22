///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXSystemEx - Extended System Information
//
// Copyright (c) 2009, Julien Templier
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer. 
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution. 
//  3. The name of the author may not be used to endorse or promote products derived from this
//     software without specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include <comsvcs.h>
#include <comutil.h>

#include "COMError.h"
#include "DXSystemEx.h"
#include "resource.h"

#include <vector>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CZipUtility
class ATL_NO_VTABLE CZipUtility : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CZipUtility, &CLSID_ZipUtility>,	
	public IDispatchImpl<IZipUtility, &IID_IZipUtility, &LIBID_DXSystemExLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public ISupportErrorInfo
{
	public:
	CZipUtility() {}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{		
		return S_OK;
	}

	void FinalRelease() 
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ZIPUTILITY)

DECLARE_NOT_AGGREGATABLE(CZipUtility)

BEGIN_COM_MAP(CZipUtility)
	COM_INTERFACE_ENTRY(IZipUtility)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

private:
		struct ZipFileInfo
		{
			_bstr_t bstrFileName;
			_bstr_t bstrNewName;
			_bstr_t bstrPath;
			long lCompSize;
		};

		typedef vector<ZipFileInfo> ZipFileInfoVtr;

		_bstr_t m_bstrFileName;
		_bstr_t m_bstrOutputPath;
		_bstr_t m_bstrInputPath;
		ZipFileInfoVtr m_vtrFiles;

		_bstr_t CalcFullFileName();

	public:
		//////////////////////////////////////////////////////////////////////////
		// ISupportErrorInfo
		//////////////////////////////////////////////////////////////////////////
		STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

		//////////////////////////////////////////////////////////////////////////
		// IZipUtility
		//////////////////////////////////////////////////////////////////////////
		STDMETHOD(ExistsFile)(BSTR strFileName, BOOL* pVal);
		STDMETHOD(Open)();
		STDMETHOD(get_FullFileName)(BSTR *pVal);
		STDMETHOD(Unzip)();
		STDMETHOD(Zip)();
		STDMETHOD(get_Count)(long *pVal);
		STDMETHOD(AddFile)(BSTR strFileName, BSTR strNewName);
		STDMETHOD(get_FileName)(BSTR *pVal);
		STDMETHOD(put_FileName)(BSTR newVal);
		STDMETHOD(get_OutputPath)(BSTR *pVal);
		STDMETHOD(put_OutputPath)(BSTR newVal);
		STDMETHOD(get_InputPath)(BSTR *pVal);
		STDMETHOD(put_InputPath)(BSTR newVal);
};

OBJECT_ENTRY_AUTO(__uuidof(ZipUtility), CZipUtility)
