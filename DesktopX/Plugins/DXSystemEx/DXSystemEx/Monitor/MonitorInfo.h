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

#include "COMError.h"
#include "DXSystemEx.h"

#include <string>
#include <vector>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CMonitorInfo
class ATL_NO_VTABLE CMonitorInfo :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMonitorInfo, &CLSID_MonitorInfo>,
	public IDispatchImpl<IMonitorInfo, &IID_IMonitorInfo, &LIBID_DXSystemExLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public ISupportErrorInfo
{
public:
	CMonitorInfo() : m_primary(false) {
		(void) SetRectEmpty(&m_rect);
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_MONITORINFO)

DECLARE_NOT_AGGREGATABLE(CMonitorInfo)

BEGIN_COM_MAP(CMonitorInfo)
	COM_INTERFACE_ENTRY(IMonitorInfo)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

	private:
		RECT m_rect;
		bool m_primary;

	public:
		void Init(const pair<RECT, bool> &info);

		//////////////////////////////////////////////////////////////////////////
		// ISupportErrorInfo
		//////////////////////////////////////////////////////////////////////////
		STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

		//////////////////////////////////////////////////////////////////////////
		// ISystemEx
		//////////////////////////////////////////////////////////////////////////
		STDMETHOD(get_IsPrimary)(VARIANT_BOOL* isPrimary);
		STDMETHOD(get_Left)(int* left);
		STDMETHOD(get_Top)(int* top);
		STDMETHOD(get_Bottom)(int* bottom);
		STDMETHOD(get_Right)(int* right);
		STDMETHOD(get_Width)(int* width);
		STDMETHOD(get_Height)(int* height);
};

OBJECT_ENTRY_AUTO(__uuidof(MonitorInfo), CMonitorInfo)
