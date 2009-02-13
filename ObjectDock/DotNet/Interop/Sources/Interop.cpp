///////////////////////////////////////////////////////////////////////////////////////////////
//
// DotNetDocklets : bringing .NET to ObjectDock
//
// Copyright (c) 2004-2009, Julien Templier
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


#include "Interop.h"
#include "Utils.h"
#include "Register.h"
#include "Dialogs.h"
#include "resource.h"

// Build numbers
#include "version.h"

///////////////////////////////////////////////////////////////////////////////////////////////

const int InteropFileNamesSize = 5;
const char* InteropFileNames[] = {"Interop.dll",
								  "Interop.png",
								  "ObjectDockSDK.dll",
								  "RegisterHelper.dll"};

IDockletInterface *lastDockletCPI = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//Return basic information about the plugin
void CALLBACK InteropOnGetInformation(char *szName, char *szAuthor, int *iVersion, char *szNotes)
{
	// Set the default docklet
	strcpy_s(szName, _countof(DOCKLET_NAME), DOCKLET_NAME);
	strcpy_s(szAuthor, _countof(DOCKLET_AUTHOR), DOCKLET_AUTHOR);
	(*iVersion) = 100 * VERSION_MAJOR + VERSION_MINOR;
	strcpy_s(szNotes, _countof(DOCKLET_NOTES), DOCKLET_NOTES);
}


///////////////////////////////////////////////////////////////////////////////////////////////
DOCKLET_DATA *CALLBACK InteropOnCreate(HWND hwndDocklet, HINSTANCE hInstance, char *szIni, char *szIniGroup)
{
	// Create Plugin personal-data structure
	DOCKLET_DATA *data = new DOCKLET_DATA;
	//ZeroMemory(data, sizeof(DOCKLET_DATA));

	data->hwndDocklet = hwndDocklet;
	data->hInstanceDll = hInstance;
	data->autoload = false;

	// Populate DOCKLET_STATIC_DATA
	data->static_data.hwndDocklet = (long)hwndDocklet;
	data->static_data.hInstance = (long)hInstance;
	PopulateStaticData(data);

	// Load setting from the global ini file
	if(szIni && szIniGroup)
	{
		LOG("Loading settings...\n");
		DOCKLET_LIST dlist;

		// We are loading an existing docklet, so set it to autoload
		data->autoload = true;

		// Path
		GetPrivateProfileString(szIniGroup, DOCKLET_INI_DOCKLET, "", dlist.path, FOLDER_SIZE ,szIni);
		LOG("  Path: "); LOG(dlist.path); LOG("\n");
		// Guid
		char guid[GUID_SIZE];
		strcpy_s(guid, _countof(""), "");
		GetPrivateProfileString(szIniGroup, DOCKLET_INI_GUID, "", (LPSTR)&guid, GUID_SIZE ,szIni);
		LOG("  Guid: "); LOG(guid); LOG("\n"); 

		if (UuidFromString((unsigned char *)guid, &dlist.CLSID_Docklet) == RPC_S_INVALID_STRING_UUID) {
			data->autoload = false;
			LOG("  -> Invalid Guid: do not autoload docklet.\n");
		}

		// Save the data
		data->docklets.push_back(dlist);
		data->index = 0;	// index of the currently selected docklet

		LOG("Finished loading settings.\n\n");
	} 

	if (data->autoload) {
		// Finish populating DOCKLET_STATIC_DATA
		PopulateStaticData_Relative(data);

		// Initialize COM and create an instance of the DockletImplementation class
		LOG("Creating an Instance of DockletImplementation...\n");
		CoInitialize(NULL);
		HRESULT hr = CREATE_INSTANCE(data->docklets[data->index].CLSID_Docklet, IID_IDockletInterface, &data->cpi);

		if (FAILED(hr)) {
			// Try to register the docklet
			char docklet_ini[FOLDER_SIZE];
			sprintf(docklet_ini, "%s%s%s", data->static_data.rootFolder, data->static_data.relativeFolder, DOCKLET_DLL_INI);

			char docklet_dll[FOLDER_SIZE];
			strcpy_s(docklet_dll, _countof(""), "");
			GetPrivateProfileString(DOCKLET_INI_DLL_SECTION, DOCKLET_INI_DLL, "", (char*)&docklet_dll, FOLDER_SIZE , (char*)&docklet_ini);

            char dll[FOLDER_SIZE];
			sprintf(dll, "%s%s%s", data->static_data.rootFolder, data->static_data.relativeFolder, docklet_dll);

			// reuse docklet_ini -> interop_path
			sprintf(docklet_ini, "%s%s", data->static_data.rootFolder, data->static_data.interopFolder);
			if (RegisterDocklet(dll, docklet_ini)) {
				// Try creating instance of the docklet
				hr = CREATE_INSTANCE(data->docklets[data->index].CLSID_Docklet, IID_IDockletInterface, &data->cpi);
			}
		}

		if (SUCCEEDED(hr)) {
			if (szIni && szIniGroup)
				data->cpi->OnCreate(data->static_data, szIni, szIniGroup);
			else
				data->cpi->OnCreate(data->static_data, "", "");
				
			return data;
		}

        // Oops, if we got here, it means there is a problem loading this docklet
		// clean up a little
		CoUninitialize();	
		data->cpi = NULL;
		data->autoload = false;
	}

	//Set the docklet's image to the default image.
	char icon[FOLDER_SIZE];
	sprintf(icon, "%s%s", data->static_data.interopFolder, DOCKLET_ICON);
	DockletSetImageFile(data->hwndDocklet, icon);

	// Set the default title
	DockletSetLabel(data->hwndDocklet, DOCKLET_TITLE);
	
	// Populate the list of docklets
	PopulateDockletList(data);

	return data;
}


///////////////////////////////////////////////////////////////////////////////////////////////
void CALLBACK InteropOnSave(DOCKLET_DATA *data, char *szIni, char *szIniGroup, BOOL bIsForExport)
{
	if (!data->autoload)
		return;

	// Save the selected .net docklet for future autoload
	char guid[GUID_SIZE];
	strcpy_s(guid, _countof(""), "");
	char *pguid = (char *)&guid;
	UuidToString(&data->docklets[data->index].CLSID_Docklet, (unsigned char **)&pguid);

	WritePrivateProfileString(szIniGroup, DOCKLET_INI_GUID, pguid, szIni);
	WritePrivateProfileString(szIniGroup, DOCKLET_INI_DOCKLET, data->docklets[data->index].path, szIni);
	LOG("  Saving selected .Net Docklet GUID\n");

	data->cpi->OnSave(szIni, szIniGroup, (VARIANT_BOOL)bIsForExport);
}


///////////////////////////////////////////////////////////////////////////////////////////////
void CALLBACK InteropOnDestroy(DOCKLET_DATA *data, HWND hwndDocklet)
{
	if (data->autoload) {
		data->cpi->OnDestroy();

		if (lastDockletCPI == data->cpi)
			lastDockletCPI = NULL;

		data->cpi->Release();
		data->cpi = NULL;

		// clean up COM:
		CoUninitialize();
		CoFreeUnusedLibraries();
		LOG("Releasing the Instance\n\n");
	}

	// allocated from PopulateStaticData
	free(data->static_data.interopFolder);
	free(data->static_data.relativeFolder);
	free(data->static_data.rootFolder);

	delete data;
}


///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK InteropOnExportFiles(DOCKLET_DATA *data, char *szFileRelativeOut, int iteration)
{
	LOG("Exporting files...\n");
	if (!data->autoload)
		return false;

	////////////////////////////////////////////////////////////////////////////
	//int iteration = 0;
	//char szFileRelativeOut[MAX_PATH];
	//strcpy(szFileRelativeOut, "");
	//BOOL ret = TRUE;

	//LOG("Exporting Files\n");

	//while (ret) {

	//	if (iteration == 0) {
	//		data->files = data->cpi->OnExportFiles();
	//		data->filesSize = data->files->cbElements + InteropFileNamesSize;
	//	}

	//	// Export the .Net Docklet files
	//	if (iteration < (signed)data->files->cbElements)
	//	{
	//		// get the file name
	//		wchar_t file;
	//		HRESULT hresult = SafeArrayGetElement(data->files, (long*)&iteration, &file);
	//		if(FAILED(hresult)) {
	//			LOG("Failed!!!\n");
	//			ret = FALSE;	
	//		}

	//		sprintf(szFileRelativeOut, "%s%S", data->static_data.relativeFolder, &file);

	//		LOG(szFileRelativeOut); LOG("\n");
	//		//ret = TRUE;
	//	}

	//	// Export the interop docklet files
	//	if (iteration > (signed)data->files->cbElements && iteration <= data->filesSize)
	//	{
	//		sprintf(szFileRelativeOut, "%s%s", data->static_data.interopFolder, InteropFileNames[iteration-(signed)data->files->cbElements-1]);
	//		LOG(szFileRelativeOut); LOG("\n");
	//		ret = TRUE;
	//	}

	//	// Finish Export
	//	if (iteration > data->filesSize) {
	//		SafeArrayDestroy(data->files);
	//		ret = FALSE;
	//	}

	//	iteration++;


	//}
	////////////////////////////////////////////////////////////////////////////

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK InteropOnConfigure(DOCKLET_DATA *data)
{
	if (!data->autoload)
	{
		DialogBoxParam(data->hInstanceDll, MAKEINTRESOURCE(IDD_CONFIG), data->hwndDocklet, ConfigureDocklet, (LPARAM) data);

		if (data->autoload)
		{
			// Finish populating DOCKLET_STATIC_DATA
			PopulateStaticData_Relative(data);

			LOG("Creating instance of selected Docklet...\n");
			CoInitialize(NULL);
			HRESULT hr = CREATE_INSTANCE(data->docklets[data->index].CLSID_Docklet, IID_IDockletInterface, &data->cpi);
			
			if (FAILED(hr)) {
				data->cpi = NULL;
				data->autoload = false;
				CoUninitialize();
				return true;
			}

			data->cpi->OnCreate(data->static_data, "", "");
		}
		return true;
	}
	
	return (data->cpi->OnConfigure() == VARIANT_TRUE) ? TRUE : FALSE;
}


///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK InteropOnLeftButtonClick(DOCKLET_DATA *data, POINT *ptCursor, SIZE *sizeDocklet)
{
	if (!data->autoload) {
		InteropOnConfigure(data);
		return true;
	}

	return (data->cpi->OnLeftButtonClick((System_Drawing::Point *)ptCursor,
										 (System_Drawing::Size *)sizeDocklet) == VARIANT_TRUE) ? TRUE : FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK InteropOnDoubleClick(DOCKLET_DATA *data, POINT *ptCursor, SIZE *sizeDocklet)
{
	if (!data->autoload)
		return false;

	return (data->cpi->OnDoubleClick((System_Drawing::Point *)ptCursor,
									 (System_Drawing::Size *)sizeDocklet) == VARIANT_TRUE) ? TRUE : FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK InteropOnLeftButtonHeld(DOCKLET_DATA *data, POINT *ptCursor, SIZE *sizeDocklet)
{
	if (!data->autoload)
		return false;

	return (data->cpi->OnLeftButtonHeld((System_Drawing::Point *)ptCursor,
										(System_Drawing::Size *)sizeDocklet) == VARIANT_TRUE) ? TRUE : FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK InteropOnRightButtonClick(DOCKLET_DATA *data, POINT *ptCursor, SIZE *sizeDocklet)
{
	if (!data->autoload) {
		// Docklet list
		HMENU hDocklets = CreatePopupMenu();
		if (data->docklets.empty()) {
			AppendMenu(hDocklets, MF_STRING | MF_GRAYED, 2, "No Docklets available");
		} else {
			for (int i = 0; i < (signed)data->docklets.size(); i++) {
				if (data->docklets[i].invalid)
					AppendMenu(hDocklets, MF_STRING | MF_GRAYED, i+3, (LPCSTR)&data->docklets[i].name);
				else
					AppendMenu(hDocklets, MF_STRING | MF_ENABLED, i+3, (LPCSTR)&data->docklets[i].name);
			}
		}

		HMENU hMenu = CreatePopupMenu();
		AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hDocklets, "Docklets");
		AppendMenu(hMenu, MF_SEPARATOR, 0, "");
		AppendMenu(hMenu, MF_STRING | MF_ENABLED, 1, "Configure");

		POINT ptMenu;
		GetCursorPos(&ptMenu);

		//Lock the dock's zooming while our menu is in use.
		DockletLockMouseEffect(data->hwndDocklet, TRUE);
		int iCommand = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, ptMenu.x, ptMenu.y, 0, data->hwndDocklet, NULL);
		DockletLockMouseEffect(data->hwndDocklet, FALSE);

		DestroyMenu(hMenu);

		if(iCommand <= 0)
			return true;

		switch(iCommand)
		{
			case 1:
				InteropOnConfigure(data);
				break;
			case 2: // "No docklets"
				break;
			default:
				data->index = iCommand-3;

				// FIXME Code duplication

				// Finish populating DOCKLET_STATIC_DATA
				PopulateStaticData_Relative(data);

				// Load the selected docklet
				LOG("Creating instance of "); LOG((char *)data->docklets[data->index].name); LOG("\n");
				CoInitialize(NULL);
				HRESULT hr = CREATE_INSTANCE(data->docklets[data->index].CLSID_Docklet, IID_IDockletInterface, &data->cpi);
				
				if (SUCCEEDED(hr)) {
					data->autoload = true;
					data->cpi->OnCreate(data->static_data, "", "");
					return true;
				}
				// Show an error dialog
				MessageBox(data->hwndDocklet, DOCKLET_INVALID_MESSAGE, DOCKLET_INVALID_TITLE, MB_ICONERROR);
				data->docklets[data->index].invalid = true;
				CoUninitialize();
				break;
		}

		return true;
	}

	return (data->cpi->OnRightButtonClick((System_Drawing::Point *)ptCursor,
										   (System_Drawing::Size *)sizeDocklet) == VARIANT_TRUE) ? TRUE : FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK InteropOnAcceptDropFiles(DOCKLET_DATA *data)
{
	if (!data->autoload)
		return false;

	BOOL acceptFiles = (data->cpi->OnAcceptDropFiles() == VARIANT_TRUE) ? TRUE : FALSE;

	// ObjectDock does not cache the results, so store the last called docklet cpi pointer to be used in OnDropData
	lastDockletCPI = (acceptFiles == TRUE) ? data->cpi : NULL;

	return acceptFiles;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void CALLBACK InteropOnDropFiles(DOCKLET_DATA *data, HDROP hDrop)
{
	if (!data->autoload)
		return;

	data->cpi->OnDropFiles((long)hDrop);

	// free the hDrop structure
	DragFinish(hDrop);
}

///////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CALLBACK InteropOnDropData(IDataObject *pDataObject, DWORD grfKeyState, DWORD *pdwEffect)
{
	if (lastDockletCPI == NULL)
	{
#ifdef DEBUG				
		MessageBox(NULL, "OnDropData was called before OnAcceptDropFiles!", "Interop Docklet", MB_OK | MB_ICONERROR);
#endif
		// Nothing the we can do here as we don't know on which docklet the data was dropped
		return S_OK;
	}

	FORMATETC fmtetc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgmed;

	// ask the IDataObject for some CF_TEXT data, stored as a HGLOBAL
	if(pDataObject->GetData(&fmtetc, &stgmed) == S_OK)
	{
		// We need to lock the HGLOBAL handle because we can't
		// be sure if this is GMEM_FIXED (i.e. normal heap) data or not
		HDROP hDrop = (HDROP)GlobalLock(stgmed.hGlobal);

		lastDockletCPI->OnDropFiles((long)hDrop);

		// cleanup
		GlobalUnlock(stgmed.hGlobal);
		ReleaseStgMedium(&stgmed);
	}

	*pdwEffect = DROPEFFECT_LINK;

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void CALLBACK InteropOnProcessMessage(DOCKLET_DATA *data, 
									  HWND hwnd, 
									  UINT uMsg, 
									  WPARAM wParam, 
									  LPARAM lParam)
{
	// AveDesk is calling it before the docklet is fully initialized
	if (data == NULL)
		return;

	if (!data->autoload)
		return;

	if (data->cpi == NULL)
		return;

	data->cpi->OnProcessMessage((long)hwnd, uMsg, wParam, lParam);
}

