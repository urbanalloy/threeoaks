///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXSystemEx - Extended System Information
//
// Copyright (c) 2009-2010, Julien Templier
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

#include "Archive/ArchiveItem.h"

#if ENABLE_ZIP_FALLBACK
#include "Archive/XZip.h"
#include "Archive/XUnzip.h"
#pragma comment(lib, "zlib.lib")
#else
DECLARE_HANDLE(HZIP);
#endif

#include <vector>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CArchive
class ATL_NO_VTABLE CArchive :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CArchive, &CLSID_Archive>,
	public IDispatchImpl<IArchive, &IID_IArchive, &LIBID_DXSystemExLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public ISupportErrorInfo
{
	public:
	CArchive() {		
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_filename = "";
		m_path = "";
		m_inputFolder = "";
		m_password = "";
		m_type = kArchiveNone;

		m_hZip = NULL;

		// 7zip
		m_h7zip = NULL;
		//load7zip();

		return S_OK;
	}

	void FinalRelease()
	{
		clear();

		if (m_h7zip)
			FreeLibrary(m_h7zip);
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ARCHIVE)

DECLARE_NOT_AGGREGATABLE(CArchive)

BEGIN_COM_MAP(CArchive)
	COM_INTERFACE_ENTRY(IArchive)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

	private:
		enum ArchiveType {
			kArchiveNone,
			kArchiveZip
		};

		typedef vector<CArchiveItem::ArchiveItem> ArchiveItemList;

		_bstr_t m_filename;
		_bstr_t m_path;
		_bstr_t m_inputFolder;
		_bstr_t m_password;
		ArchiveItemList m_files;

		ArchiveType m_type;

		HZIP m_hZip;
		HMODULE m_h7zip;

		void load7zip();

		void clear();
		void parseInputFilename(BSTR file);

	public:
		//////////////////////////////////////////////////////////////////////////
		// ISupportErrorInfo
		//////////////////////////////////////////////////////////////////////////
		STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

		//////////////////////////////////////////////////////////////////////////
		// IArchive
		//////////////////////////////////////////////////////////////////////////

		// Properties
		STDMETHOD(get_InputFolder)(BSTR *inputFolder);
		STDMETHOD(put_InputFolder)(BSTR inputFolder);
		STDMETHOD(get_Path)(BSTR *path);
		STDMETHOD(get_FileName)(BSTR *filename);
		STDMETHOD(get_Password)(BSTR *password);
		STDMETHOD(put_Password)(BSTR password);
		STDMETHOD(get_Count)(long *count);
		STDMETHOD(get_Items)(VARIANT* items);
		STDMETHOD(get_Type)(int* type);
		STDMETHOD(get_SupportedExtensions)(VARIANT* extensions);		

		// Methods
		STDMETHOD(Create)(BSTR filePath, int type, VARIANT_BOOL *status);
		STDMETHOD(Open)(BSTR filePath, VARIANT_BOOL *status);
		STDMETHOD(ExistsFile)(BSTR filename, VARIANT_BOOL* status);
		STDMETHOD(AddFile)(BSTR filename, VARIANT_BOOL* status);	
		STDMETHOD(ExtractFile)(BSTR filename, BSTR outputPath, VARIANT_BOOL* status);
		STDMETHOD(Extract)(BSTR outputDirectory, VARIANT_BOOL *status);
		STDMETHOD(Close)();
				
		STDMETHOD(IsArchive)(BSTR filename, VARIANT_BOOL *status);		
};

OBJECT_ENTRY_AUTO(__uuidof(Archive), CArchive)
