///////////////////////////////////////////////////////////////////////////////////////////////
//
// DXSystemEx - Extended System Information
//
// Copyright (c) 2009-2010, Julien Templier
// All rights reserved.
//
// Based on Tutorial by J. Brown / www.catch22.net
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

class ATL_NO_VTABLE CDropTarget :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDropTarget>,
	public IDropTarget
{
public:
	CDropTarget() : m_hWnd(NULL),
					m_objID(0),
					m_fAllowDrop(FALSE)
	{}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease() 
	{

	}

	DECLARE_NOT_AGGREGATABLE(CDropTarget)

	BEGIN_COM_MAP(CDropTarget)
		COM_INTERFACE_ENTRY(IDropTarget)
	END_COM_MAP()

private:


	// Private member variables
	HWND	m_hWnd;
	DWORD   m_objID;
	bool    m_fAllowDrop;

	IDataObject *m_pDataObject;

	// internal helper function
	DWORD DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed);
	bool  QueryDataObject(IDataObject *pDataObject);

public:

	void setObjID(DWORD objID);

	void setHwnd(HWND hwnd);
	HWND getHwnd();

	//////////////////////////////////////////////////////////////////////////
	// IDropTarget
	//////////////////////////////////////////////////////////////////////////
	STDMETHOD(DragEnter)(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
	STDMETHOD(DragOver)(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
	STDMETHOD(DragLeave)();
	STDMETHOD(Drop)(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);

};
//OBJECT_ENTRY_AUTO(__uuidof(CDropTarget), CDropTarget)