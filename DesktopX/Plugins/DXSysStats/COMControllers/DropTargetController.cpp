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

// DropTargetController.cpp : Implementation of CDropTargetController
#include "stdafx.h"
#include "MessageSenderImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#include "SysStatsUtils.h"
#include "COMControllers.h"
#include "CDropTarget.h"
#include "DropTargetController.h"
#include "DropTargetControllerDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CDropTargetController

STDMETHODIMP CDropTargetController::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IDropTargetController
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/*
 * Constructor
 */
CDropTargetController::CDropTargetController() :
	pTargetImpl(NULL),
	effect(0),
	aveClipFormat(::RegisterClipboardFormat("AveDeskDesklet"))
{
}

CDropTargetController::~CDropTargetController()
{
	AtlTrace("DropTargetController destroyed\n");

	if (pTargetImpl)
	{
//		if (FAILED(pTargetImpl->RevokeDragDrop()))
		 pTargetImpl->Release();
	}
}

/*
 * This method is called when a windows message of the correct type has been posted.
 */
STDMETHODIMP CDropTargetController::HandleMessage(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled)
{
	*handled = !passThrough;

	// Provide implementation.
	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the controller should
 * be displayed so that the user can configure the controller to meet their
 * requirements.
 */
STDMETHODIMP CDropTargetController::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	DropTargetControllerDialog *d = new DropTargetControllerDialog(this, observer, meters);
	d->DoModal((HWND)hDlg);
	delete d;

	return S_OK;
}

/*
 * Return a string representation of the controller’s type.
 */
STDMETHODIMP CDropTargetController::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_DropTargetController, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CDropTargetController::put_Model(IClientModel *newVal)
{
	ControllerImpl<IDropTargetController>::put_Model(newVal);

	long hWnd;
	model->get_hWnd(&hWnd);
	if (pTargetImpl)
	{
		pTargetImpl->Release();
//		pTargetImpl->RevokeDragDrop();	// Should release CDragDrop
	}

	pTargetImpl = new CDropTarget(this);
	pTargetImpl->AddRef();
	pTargetImpl->SetWindow((HWND)hWnd);
	pTargetImpl->RegisterDragDrop();

	return S_OK;
}

static byte bitCount[] =
{
	0, 1, 1, 2, 1, 2, 2, 3
};

DWORD CDropTargetController::getEffect(DWORD dwKeys, DWORD *pEffect)
{
	DWORD selEffect = effect & *pEffect & 0x7;

	DWORD pEff = DROPEFFECT_NONE;
	byte bc = bitCount[selEffect];
	AtlTrace("selEffect=0x%x, bc=0x%x\n", selEffect, bc);
	
	if (bc == 0)
		pEff = DROPEFFECT_NONE;
	else if (bc == 1)
		pEff = selEffect;
	else
	{
		pEff = DROPEFFECT_MOVE;

		if ((dwKeys & 0x4) && (selEffect & DROPEFFECT_MOVE))
			pEff = DROPEFFECT_MOVE;

		if ((dwKeys & 0x8) && (selEffect & DROPEFFECT_COPY))
			pEff = DROPEFFECT_COPY;

		if ((dwKeys & 0x20) && (selEffect & DROPEFFECT_LINK))
			pEff = DROPEFFECT_LINK;
	}

	return pEff;
}

STDMETHODIMP CDropTargetController::DragEffect(DWORD dwKeys, POINTL ptl, DWORD * pEffect)
{
	// The mouse should be over a particular overlay for the controller to respond.
	DWORD selEffect = getEffect(dwKeys, pEffect);

	if (selEffect == DROPEFFECT_NONE)
	{
		*pEffect = DROPEFFECT_NONE;
		return S_OK;
	}

	IOverlay *pOverlay = 0;
	get_Overlay(&pOverlay);
	DWORD pEff = DROPEFFECT_NONE;
	POINT local = {ptl.x, ptl.y};
	long hWnd;
	model->get_hWnd(&hWnd);
	::ScreenToClient((HWND)hWnd, &local);

	if (pOverlay != NULL)
	{
		ISimpleOverlay2 *pOverlay2 = 0;
		pOverlay->QueryInterface(__uuidof(ISimpleOverlay2), (void**)&pOverlay2);
		if (pOverlay2)
		{
			VARIANT_BOOL d = false;

			pOverlay2->HitTest((short)local.x, (short)local.y, &d);
			if (d)
			{
				pEff = selEffect;
			}

			pOverlay2->Release();
		}
	}

	*pEffect = pEff;

	return S_OK;
}

// IDropTarget
STDMETHODIMP CDropTargetController::DragEnter(IDataObject* obj, DWORD dwKeys, POINTL ptl, DWORD* pEffect)
{
	return DragEffect(dwKeys, ptl, pEffect);
}

STDMETHODIMP CDropTargetController::DragOver(DWORD dwKeys, POINTL ptl, DWORD * pEffect)
{
	return DragEffect(dwKeys, ptl, pEffect);
}

STDMETHODIMP CDropTargetController::DragLeave()
{
	return S_OK;
}

STDMETHODIMP CDropTargetController::Drop(IDataObject * obj, DWORD dwKeys, POINTL ptl, DWORD *pEffect)
{
	HWND hwnd = 0;

	if (ReadHdropData(obj))
	{
		VARIANT_BOOL handled;
		MessageSenderImpl<IDropTargetController>::HandleMessage(0, getEffect(dwKeys, pEffect), 0, 0, &handled);
	} else if (ReadAveDropData(obj, &hwnd))
	{
		VARIANT_BOOL handled;
		MessageSenderImpl<IDropTargetController>::HandleMessage((UINT)hwnd, getEffect(dwKeys, pEffect), 0, 0, &handled);
	}

	return S_OK;
}

// ReadHdropData() reads CF_HDROP data from the passed-in data object, and 
// puts all dropped files/folders into the specified XML meter.
bool CDropTargetController::ReadHdropData (IDataObject* pDataObject)
{
	FORMATETC	formatEtc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM	stgMedium;
	HDROP       hdrop;
	UINT        uNumFiles;
	TCHAR       szNextFile [MAX_PATH];
	_bstr_t		fileset;
	bool		ret = false;

    // Get the HDROP data from the data object.

    if (SUCCEEDED(pDataObject->GetData(&formatEtc, &stgMedium)) && stgMedium.tymed == TYMED_HGLOBAL)
	{
		hdrop = (HDROP) ::GlobalLock(stgMedium.hGlobal);

		if (hdrop != NULL)
		{
			// Get the # of files being dropped.
			uNumFiles = ::DragQueryFile (hdrop, -1, NULL, 0);
			fileset = "<?xml version=\"1.0\"?>\n<fileset>\n";

			for (UINT uFile = 0; uFile < uNumFiles; uFile++)
			{
				// Get the next filename from the HDROP info.

				if (::DragQueryFile ( hdrop, uFile, szNextFile, MAX_PATH ) > 0)
				{
					// Insert it into the list!
					fileset += "<file><path>";
					SysStatsUtils::EncodeXMLString(szNextFile, &fileset);
					fileset += "</path></file>\n";
				}
			}   // end for

			fileset += "</fileset>";

#ifdef _DEBUG
	FILE *file = fopen("droptarget.xml", "w");
	fprintf(file, "%s\n", (char*)fileset);
	::fflush(file);
	fclose(file);
#endif

			PutValue(fileset);

			ret = true;
		}


		::GlobalUnlock(stgMedium.hGlobal);
	}
    
    return ret;
}

// ReadAveDropData() reads the AveDeskDesklet data from the passed-in data object, and 
// puts all dropped HWNDs into the specfied XML meter.
bool CDropTargetController::ReadAveDropData (IDataObject* pDataObject, HWND *hwnd)
{
	FORMATETC	formatEtc = { aveClipFormat, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM	stgMedium;
	bool		ret = false;

    if (SUCCEEDED(pDataObject->GetData(&formatEtc, &stgMedium)) && stgMedium.tymed == TYMED_HGLOBAL)
	{
		HWND* deskletsHwnd= reinterpret_cast<HWND*>(::GlobalLock(stgMedium.hGlobal));

		if (deskletsHwnd != NULL)
		{
			*hwnd = *deskletsHwnd;
			ret = true;
		}


		::GlobalUnlock(stgMedium.hGlobal);
	}
    
    return ret;
}

bool CDropTargetController::PutValue(TCHAR *szFileName)
{
	IMeter *iMeter = 0;
	get_meter(&iMeter);
	if (iMeter)
	{
		DISPID id;
		HRESULT hr;
		LPOLESTR name;
#ifdef notdef
		name = L"PutValue";   
		hr = iMeter->GetIDsOfNames(IID_NULL, 
                               &name, 
                               1, 
                               LOCALE_USER_DEFAULT, 
                               &id);

		if (SUCCEEDED(hr))
		{
			AtlTrace("DropTargetController: Found PutValue method\n");
			CComVariant varResult;
			CComVariant* pvars = new CComVariant[2];

			VariantClear(&varResult);
			pvars[0] = (BSTR)value;
			pvars[1] = (BSTR)selector;
			DISPPARAMS disp = { pvars, NULL, 2, 0 };
			hr = iMeter->Invoke(id, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, &varResult, NULL, NULL);
			delete[] pvars;

			if (SUCCEEDED(hr))
			{
				AtlTrace("DropTargetController: Set Value via PutValue\n");
				return true;
			}
		}
#endif
		name = L"Value";   
		hr = iMeter->GetIDsOfNames(IID_NULL, 
                               &name, 
                               1, 
                               MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), 
                               &id);

		if (SUCCEEDED(hr))
		{
			AtlTrace("ValueSetter: Found Value Property\n");
			CComVariant varResult;
			CComVariant* pvars = new CComVariant[1];

			VariantClear(&varResult);
			DISPID dispidNamed = DISPID_PROPERTYPUT;
			DISPPARAMS disp = { pvars, &dispidNamed, 1, 1 };
			_bstr_t value(szFileName);
			pvars[0] = (BSTR)value;
			hr = iMeter->Invoke(id, 
									IID_NULL, 
									MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), 
									DISPATCH_PROPERTYPUT, 
									&disp, 
									&varResult, 
									NULL, 
									NULL);
			delete[] pvars;

			if (SUCCEEDED(hr))
			{
				AtlTrace("CDropTargetController: Set Value Property\n");
				return true;
			}
		}
	}

	return false;
}

STDMETHODIMP CDropTargetController::get_Effect(DWORD *pVal)
{
	*pVal = effect;

	return S_OK;
}

STDMETHODIMP CDropTargetController::put_Effect(DWORD newVal)
{
	effect = newVal;

	return S_OK;
}
