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

#include "CDropTarget.h"
#include "Docklet.h"

void RegisterDropWindow(HWND hwnd, IDropTarget* pDropTarget)
{
	// acquire a strong lock
	CoLockObjectExternal(pDropTarget, TRUE, FALSE);

	// tell OLE that the window is a drop target
	RegisterDragDrop(hwnd, pDropTarget);
}

void UnregisterDropWindow(HWND hwnd, IDropTarget *pDropTarget)
{
	// remove drag+drop
	RevokeDragDrop(hwnd);

	// remove the strong lock
	CoLockObjectExternal(pDropTarget, FALSE, TRUE);
}

void CDropTarget::SetDocklet(Docklet* pDocklet)
{
	m_pDocklet = pDocklet;
}

//
//	QueryDataObject private helper routine
//
bool CDropTarget::QueryDataObject(IDataObject *pDataObject)
{
	// do not accept anything if the docklet does not handle it
	if (!m_pDocklet->AcceptDropFiles())
		return false;

	// does the data object support CF_TEXT using a HGLOBAL?
	FORMATETC fmttext = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	if (pDataObject->QueryGetData(&fmttext) == S_OK)
		return true;

	// does the data object support CF_HDROP using a HGLOBAL?
	FORMATETC fmthdrop = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	if (pDataObject->QueryGetData(&fmthdrop) == S_OK)
		return true;

	return false;
}

//
//	DropEffect private helper routine
//
#pragma warning(push)
#pragma warning(disable: 4100)
DWORD CDropTarget::DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
	DWORD dwEffect = 0;

	// 1. check "pt" -> do we allow a drop at the specified coordinates?

	// 2. work out that the drop-effect should be based on grfKeyState
	/*if(grfKeyState & MK_CONTROL) {
	dwEffect = dwAllowed & DROPEFFECT_COPY;
	} else if(grfKeyState & MK_SHIFT) {
	dwEffect = dwAllowed & DROPEFFECT_COPY;
	}*/

	// 3. no key-modifiers were specified (or drop effect not allowed), so
	//    base the effect on those allowed by the dropsource
	if(dwEffect == 0) {
		if(dwAllowed & DROPEFFECT_COPY) dwEffect = DROPEFFECT_COPY;
		if(dwAllowed & DROPEFFECT_MOVE) dwEffect = DROPEFFECT_COPY;
	}

	return dwEffect;
}
#pragma warning(pop)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	IDropTarget Interface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT __stdcall CDropTarget::DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	// does the dataobject contain data we want?
	m_fAllowDrop = QueryDataObject(pDataObject);

	if(m_fAllowDrop) {
		// get the dropeffect based on keyboard state
		*pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
	} else {
		*pdwEffect = DROPEFFECT_NONE;
	}

	return S_OK;
}


HRESULT __stdcall CDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	if(m_fAllowDrop) {
		*pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);		
	} else {
		*pdwEffect = DROPEFFECT_NONE;
	}

	return S_OK;
}


HRESULT __stdcall CDropTarget::DragLeave(void)
{
	return S_OK;
}


HRESULT __stdcall CDropTarget::Drop(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	if(m_fAllowDrop) {		
		BOOL handleEffect = m_pDocklet->Drop(pDataObject, grfKeyState, pdwEffect);

		if (!handleEffect)
			*pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
	} else {
		*pdwEffect = DROPEFFECT_NONE;
	}

	return S_OK;
}