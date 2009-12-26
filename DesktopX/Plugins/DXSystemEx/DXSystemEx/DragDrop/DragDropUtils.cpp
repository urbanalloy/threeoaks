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

#include "DragDropUtils.h"
#include "CDropTarget.h"

#include <SDPlugin.h>
#include <comdef.h>
#include <string>

using namespace std;

extern BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD);

void RegisterDropWindow(IDropTarget* pDropTarget)
{
	if (pDropTarget == NULL)
		return;

	// acquire a strong lock
	CoLockObjectExternal(pDropTarget, TRUE, FALSE);

	// tell OLE that the window is a drop target
	RegisterDragDrop(((CDropTarget*)pDropTarget)->getHwnd(), pDropTarget);
}

void UnregisterDropWindow(IDropTarget *pDropTarget)
{
	if (pDropTarget == NULL)
		return;

	// remove drag+drop
	RevokeDragDrop(((CDropTarget*)pDropTarget)->getHwnd());

	// remove the strong lock - will release all pointers to the object if this is the last reference
	CoLockObjectExternal(pDropTarget, FALSE, TRUE);
}

void DropData(DWORD objID, IDataObject *pDataObject)
{
	// construct a FORMATETC object
	FORMATETC fmttext = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgmed;

	// See if the DataObject contains any TEXT stored as a HGLOBAL
	if(pDataObject->QueryGetData(&fmttext) == S_OK) {

		// Yippie! the data is there, so go get it!
		if(pDataObject->GetData(&fmttext, &stgmed) == S_OK) {

			// we asked for the data as a HGLOBAL, so access it appropriately
			PVOID data = GlobalLock(stgmed.hGlobal);

			// Send data to DesktopX
			SD_SCRIPTABLE_EVENT se;
			se.cbSize = sizeof(SD_SCRIPTABLE_EVENT);
			lstrcpy(se.szEventName, PLUGIN_PREFIX "OnDropText");
			se.flags=0;

			memset(&se.dp, 0, sizeof(DISPPARAMS));

			USES_CONVERSION;	

			se.dp.cArgs = 1;
			VARIANT* lpvt = (VARIANT*)malloc(sizeof(VARIANT)*1);
			VariantInit(&lpvt[0]);							
			lpvt[0].vt = VT_BSTR;
			lpvt[0].bstrVal = SysAllocString((OLECHAR*) T2OLE((char*)data));

			se.dp.rgvarg = lpvt;

			SDHostMessage(SD_SCRIPTABLE_PLUGIN_EVENT, (DWORD) objID, (DWORD) &se);

			free(se.dp.rgvarg);

			GlobalUnlock(stgmed.hGlobal);

			// release the data using the COM API
			ReleaseStgMedium(&stgmed);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// HDROP (files...)
	FORMATETC fmthdrop = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	if(pDataObject->QueryGetData(&fmthdrop) == S_OK) {
		if(pDataObject->GetData(&fmthdrop, &stgmed) == S_OK) {

			// we asked for the data as a HGLOBAL, so access it appropriately
			HDROP hdrop = (HDROP)GlobalLock(stgmed.hGlobal);

			// Get the # of files being dropped.
			UINT uNumFiles = DragQueryFile(hdrop, (UINT)-1, NULL, (UINT)0);

			string data = "";
			TCHAR szNextFile[MAX_PATH];

			// Create SafeArray of VARIANT BSTRs
			SAFEARRAY *pSA;
			SAFEARRAYBOUND aDim[1];
			variant_t vOut;

			aDim[0].lLbound = 0;
			aDim[0].cElements = uNumFiles;

			pSA = SafeArrayCreate(VT_VARIANT, 1, aDim);

			if (pSA == NULL)
				goto cleanup;			
			
			for (UINT uFile = 0; uFile < uNumFiles; uFile++) {

				// Get the next filename from the HDROP info.
				if ( DragQueryFile(hdrop, uFile, szNextFile, MAX_PATH) > 0 ) {	

					if (uFile > 0)
						data.append("|");

					data.append(szNextFile);


					// Add element to array
					vOut = szNextFile;

					HRESULT hr = SafeArrayPutElement(pSA, (LONG*)&uFile, &vOut);
					if (FAILED(hr)) {
						SafeArrayDestroy(pSA); // does a deep destroy
						goto cleanup;
					}
				}				
			}

			// We implement both the default Object_OnDropFiles callback and the DXSystemEx-specific version			

			// Default DX callback
			{			
			SD_SCRIPTABLE_EVENT se;
			se.cbSize = sizeof(SD_SCRIPTABLE_EVENT);
			lstrcpy(se.szEventName, "Object_OnDropFiles");
			se.flags=0;

			memset(&se.dp, 0, sizeof(DISPPARAMS));

			USES_CONVERSION;
			se.dp.cArgs = 1;
			VARIANT* lpvt = (VARIANT*)malloc(sizeof(VARIANT)*1);
			VariantInit(&lpvt[0]);							
			lpvt[0].vt = VT_BSTR;
			lpvt[0].bstrVal = SysAllocString((OLECHAR*) T2OLE(data.c_str()));

			se.dp.rgvarg = lpvt;

			SDHostMessage(SD_SCRIPTABLE_PLUGIN_EVENT, (DWORD) objID, (DWORD) &se);
			free(se.dp.rgvarg);
			}

			// DXSystemEx callback
			{			
			SD_SCRIPTABLE_EVENT se;
			se.cbSize = sizeof(SD_SCRIPTABLE_EVENT);
			lstrcpy(se.szEventName, PLUGIN_PREFIX "OnDropFiles");
			se.flags=0;

			memset(&se.dp, 0, sizeof(DISPPARAMS));

			USES_CONVERSION;
			se.dp.cArgs = 1;
			VARIANT* lpvt = (VARIANT*)malloc(sizeof(VARIANT)*1);
			VariantInit(&lpvt[0]);	
			V_VT(&lpvt[0]) = VT_ARRAY | VT_VARIANT;
			V_ARRAY(&lpvt[0])= pSA;

			se.dp.rgvarg = lpvt;

			SDHostMessage(SD_SCRIPTABLE_PLUGIN_EVENT, (DWORD) objID, (DWORD) &se);
			free(se.dp.rgvarg);
			}

cleanup:		
			GlobalUnlock(stgmed.hGlobal);

			// release the data using the COM API
			ReleaseStgMedium(&stgmed);
		}
	}
}