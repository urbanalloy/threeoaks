///////////////////////////////////////////////////////////////////////////////////////////////
//
// DockletX - Docklet support plugin for DesktopX
//
// Copyright (c) 2006-2009, Three Oaks Crossing
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"

class Docklet;

void RegisterDropWindow(HWND hwnd, IDropTarget* pDropTarget);
void UnregisterDropWindow(HWND hwnd, IDropTarget* pDropTarget);

class ATL_NO_VTABLE CDropTarget :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDropTarget>,
	public IDropTarget
{
public:
	CDropTarget() : m_pDocklet(NULL),
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
	Docklet* m_pDocklet;
	bool    m_fAllowDrop;

	IDataObject *m_pDataObject;

	// internal helper function
	DWORD DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed);
	bool  QueryDataObject(IDataObject *pDataObject);

public:

	void SetDocklet(Docklet* pDocklet);

	//////////////////////////////////////////////////////////////////////////
	// IDropTarget
	//////////////////////////////////////////////////////////////////////////
	STDMETHOD(DragEnter)(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
	STDMETHOD(DragOver)(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
	STDMETHOD(DragLeave)();
	STDMETHOD(Drop)(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);

};
//OBJECT_ENTRY_AUTO(__uuidof(CDropTarget), CDropTarget)