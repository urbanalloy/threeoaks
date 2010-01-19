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

// CDropTarget.cpp: implementation of the CDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "COMControllers.h"
#include "DropTargetController.h"
#include "CDropTarget.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDropTarget::CDropTarget(CDropTargetController *pController) :
	pController(pController),
	ref(0)
{

}

CDropTarget::~CDropTarget()
{
	AtlTrace("CDropTarget destroyed\n");
}

// IDropTarget
STDMETHODIMP CDropTarget::DragEnter(IDataObject* obj, DWORD dwKeys, POINTL ptl, DWORD* pEffect)
{
	pController->DragEnter(obj, dwKeys, ptl, pEffect);

	IDropTargetImpl<CDropTarget>::DragEnter(obj, dwKeys, ptl, pEffect);

	return S_OK;
}

STDMETHODIMP CDropTarget::DragOver(DWORD dwKeys, POINTL ptl, DWORD * pEffect)
{
	pController->DragOver(dwKeys, ptl, pEffect);

	IDropTargetImpl<CDropTarget>::DragOver(dwKeys, ptl, pEffect);

	return S_OK;
}

STDMETHODIMP CDropTarget::DragLeave()
{
	pController->DragLeave();

	IDropTargetImpl<CDropTarget>::DragLeave();

	return S_OK;
}

STDMETHODIMP CDropTarget::Drop(IDataObject * obj, DWORD dwKeys, POINTL ptl, DWORD *pEffect)
{
	pController->Drop(obj, dwKeys, ptl, pEffect);

	IDropTargetImpl<CDropTarget>::Drop(obj, dwKeys, ptl, pEffect);

	return S_OK;
}

