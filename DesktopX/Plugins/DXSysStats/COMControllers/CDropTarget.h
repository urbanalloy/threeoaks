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

// CDropTarget.h: interface for the CDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CDROPTARGET_H__F10CAD38_30D2_4E39_9154_A3989E2CB30A__INCLUDED_)
#define AFX_CDROPTARGET_H__F10CAD38_30D2_4E39_9154_A3989E2CB30A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DropTargetImpl.hpp"

class CDropTargetController;

// Really just exists to remove a circular reference. Proxies everything
// to the controller.
class CDropTarget : public IDropTargetImpl<CDropTarget>  
{
public:
	CDropTarget(CDropTargetController *pController);
	virtual ~CDropTarget();

	virtual HRESULT __stdcall QueryInterface(REFIID riid, void **ppv) { return S_OK; }
	virtual ULONG   __stdcall AddRef() { return ++ref; }
	virtual ULONG   __stdcall Release() { ref--; if (!ref) delete this; return ref; }

// IDropTarget
	STDMETHOD(DragEnter)(IDataObject* obj, DWORD dwKeys, POINTL ptl, DWORD* pEffect);
    STDMETHOD(DragOver)(DWORD dwKeys, POINTL ptl, DWORD * pEffect);
    STDMETHOD(DragLeave)();
    STDMETHOD(Drop)(IDataObject * obj, DWORD dwKeys, POINTL ptl, DWORD *pEffect);

protected:
	CDropTargetController *pController;

	ULONG ref;
};

#endif // !defined(AFX_CDROPTARGET_H__F10CAD38_30D2_4E39_9154_A3989E2CB30A__INCLUDED_)
