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

/////////////////////////////////////////////////////////////////////////////
// CArchiveItem
class ATL_NO_VTABLE CArchiveItem :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CArchiveItem, &CLSID_ArchiveItem>,
	public IDispatchImpl<IArchiveItem, &IID_IArchiveItem, &LIBID_DXSystemExLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public ISupportErrorInfo
{
public:
	CArchiveItem() {}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ARCHIVEITEM)

DECLARE_NOT_AGGREGATABLE(CArchiveItem)

BEGIN_COM_MAP(CArchiveItem)
	COM_INTERFACE_ENTRY(IArchiveItem)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

	struct ArchiveItem
	{
		_bstr_t filename;
		bool isDirectory;
		long compressedSize;

		ArchiveItem& operator=(const ArchiveItem &rhs) {
			// Check for self-assignment
		    if (this == &rhs)
		   		return *this; 

			filename = rhs.filename.copy();
			isDirectory = rhs.isDirectory;
			compressedSize = rhs.compressedSize;

			return *this;
		}
	}; 

	private:
		ArchiveItem item;

	public:
		void Init(ArchiveItem file);

		//////////////////////////////////////////////////////////////////////////
		// ISupportErrorInfo
		//////////////////////////////////////////////////////////////////////////
		STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

		//////////////////////////////////////////////////////////////////////////
		// ISystemEx
		//////////////////////////////////////////////////////////////////////////
		STDMETHOD(get_Name)(BSTR* name);
		STDMETHOD(get_IsDirectory)(VARIANT_BOOL *isDirectory);
		STDMETHOD(get_CompressedSize)(long* compressedSize);
};

OBJECT_ENTRY_AUTO(__uuidof(ArchiveItem), CArchiveItem)
