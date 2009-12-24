///////////////////////////////////////////////////////////////////////////////////////////////
//
// DockletX - Docklet support plugin for DesktopX
//
// Copyright (c) 2006-2010, Julien Templier
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

#include "stdafx.h"

#include "Docklet.h"
#include "DockletX.h"
#include "Host.h"
#include "Dialogs.h"

#include <SDPlugin.h>

extern BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD);
extern HINSTANCE dllInstance;
HMODULE WINAPI MyGetModuleHandleA(LPCTSTR lpModuleName);
HMODULE WINAPI MyGetModuleHandleW(LPCWSTR lpModuleName);

//////////////////////////////////////////////////////////////////////////
// Constructor&Destructor
//////////////////////////////////////////////////////////////////////////

Docklet::Docklet(int objID) :
	hLibrary(NULL),
	objID(objID),
	hwnd(NULL),
	pDropTarget(NULL),
	initialized(FALSE),
	locked(FALSE),
	docklet_data(NULL)
{
	ClearFunctionPointers();

	images.background = new CGdiPlusBitmapResource();
	images.background->Load(IDB_BACKGROUND, _T("PNG"), dllInstance);
}

void Docklet::ClearFunctionPointers()
{
	OnGetInformation = NULL;
	OnCreate = NULL;
	OnSave = NULL;
	OnDestroy = NULL;
	OnExportFiles = NULL;
	OnLeftButtonClick = NULL;
	OnDoubleClick = NULL;
	OnLeftButtonHeld = NULL;
	OnRightButtonClick = NULL;
	OnConfigure = NULL;
	OnAcceptDropFiles = NULL;
	OnDropFiles = NULL;
	OnProcessMessage = NULL;
	OnDropData = NULL;
	OnDragDropGetSourceObject = NULL;
	NotifySizeChange = NULL;

	// Hooks
	p_fnGetModuleHandleAOrg = NULL;
	p_fnGetModuleHandleWOrg = NULL;
}

Docklet::~Docklet()
{
}

void Docklet::Start(HWND hwnd)
{
	if (strcmp(config.dockletFile, "") == 0) {
		ShowDefaultImage();
		SDHostMessage(SD_SET_TOOLTIP_TEXT, objID, (DWORD)"CTRL+Click to configure docklet");
		return;
	}

	hLibrary = LoadPluginInstance((char*)config.dockletFile,
		 					      17,
								  &OnGetInformation,		"OnGetInformation",
								  &OnGetRequiredVersion,	"OnGetRequiredVersion",
								  &OnCreate,				"OnCreate",
								  &OnSave,					"OnSave",
								  &OnDestroy,				"OnDestroy",
								  &OnExportFiles,			"OnExportFiles",
								  &OnLeftButtonClick,		"OnLeftButtonClick",
								  &OnDoubleClick,			"OnDoubleClick",
								  &OnLeftButtonHeld,		"OnLeftButtonHeld",
								  &OnRightButtonClick,		"OnRightButtonClick",
								  &OnConfigure,				"OnConfigure",
								  &OnAcceptDropFiles,		"OnAcceptDropFiles",
								  &OnDropFiles,				"OnDropFiles",
								  &OnDropData,				"OnDropData",
								  &OnProcessMessage,		"OnProcessMessage",
								  &NotifySizeChange,		"NotifySizeChange",
								  &OnDragDropGetSourceObject, "OnDragDropGetSourceObject");

	this->hwnd = hwnd;

	// Check for mandatory exported functions
	if (OnGetInformation == NULL || OnCreate == NULL || OnDestroy == NULL)
		return;

	initialized = true;

	//////////////////////////////////////////////////////////////////////////
	// Register for drag&drop
	CComObject<CDropTarget>* pTarget;
	CComObject<CDropTarget>::CreateInstance(&pTarget);
	pTarget->SetDocklet(this);

	RegisterDropWindow(hwnd, pTarget);

	this->pDropTarget = pTarget;

	//////////////////////////////////////////////////////////////////////////
	// Get plugin information on load
	LoadInformation(OnGetInformation, &info);

	// Hook GetModuleHandle(NULL)
	InterceptDllCall(hLibrary, "kernel32.dll", "GetModuleHandleA", (PVOID)&MyGetModuleHandleA, (PVOID*)&p_fnGetModuleHandleAOrg, NULL);
	InterceptDllCall(hLibrary, "kernel32.dll", "GetModuleHandleW", (PVOID)&MyGetModuleHandleW, (PVOID*)&p_fnGetModuleHandleWOrg, NULL);

	//////////////////////////////////////////////////////////////////////////
	// Start the docklet
	// Docklets that do not have a OnSave export might not handle the second OnCreate correctly, so bypass that
	if (strcmp(config.iniFile, "") == 0 || OnSave == NULL)
		docklet_data = OnCreate(hwnd, hLibrary, NULL, NULL);
	else
		docklet_data = OnCreate(hwnd, hLibrary, config.iniFile, DOCKLET_CONFIG_SECTION);
}

void Docklet::Stop()
{
	if (!initialized)
		return;

	///////////////////////////////////////////
	// Unregister drag&drop
	UnregisterDropWindow(hwnd, pDropTarget);

	// Destroy docklet
	if (docklet_data != NULL) {
		OnDestroy(docklet_data, hwnd);
		docklet_data = NULL;
	}

	///////////////////////////////////////////
	// Free images
	if (images.disposeImage)
		SAFE_DELETE(images.image);

	if (images.disposeOverlay)
		SAFE_DELETE(images.overlay);

	// Reset structure
	strcpy_s(images.path, "");
	images.image = NULL;
	images.overlay = NULL;
	images.disposeImage = TRUE;
	images.disposeOverlay = TRUE;

	///////////////////////////////////////////
	// Free plugin instance
	if (hLibrary) {
		FreeLibrary(hLibrary);
		hLibrary = NULL;
	}

	ClearFunctionPointers();

	///////////////////////////////////////////
	//TODO clear all config
	//objID = 0;
	strcpy_s(config.dockletFile, "");
	config.size = DOCKLET_DEFAULT_SIZE;
	config.label = string("");

	locked = FALSE;
	initialized = FALSE;
}

// Compute relative and root paths from docklet file
void Docklet::ComputePaths()
{
	// Root
	Docklet::GetObjectDockFolder(config.rootFolder);

	// Relative
	CPathA fullPath(config.dockletFile);
	fullPath.RemoveFileSpec();
	fullPath.AddBackslash();

	CPathA relative;
	relative.RelativePathTo(config.rootFolder, FILE_ATTRIBUTE_DIRECTORY, fullPath.m_strPath.GetString(), FILE_ATTRIBUTE_DIRECTORY);

	strcpy_s(config.relativeFolder, relative.m_strPath.GetString());
}

// ObjectDock Folder
void Docklet::GetObjectDockFolder(char* path)
{
	strcpy_s(path, MAX_PATH*sizeof(char), "");

	HKEY key;
	DWORD size;
	DWORD type = REG_SZ;
	char buffer[MAX_PATH];

	// Open parent key
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, OBJECTDOCK_KEY, NULL, KEY_READ, &key) != ERROR_SUCCESS) {
		// Try to open the free version
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, OBJECTDOCK_FREE_KEY, NULL, KEY_READ, &key) != ERROR_SUCCESS)
			return;
	}

	size = sizeof(buffer);
	strcpy_s((char*)&buffer, size, "");
	if (RegQueryValueExA(key, "Path", NULL, &type, (LPBYTE)&buffer, &size) == ERROR_SUCCESS)
		strcpy_s((char *)path, size, (const char *)&buffer);

	RegCloseKey(key);
}

//////////////////////////////////////////////////////////////////////////
// Host functions
//////////////////////////////////////////////////////////////////////////

BOOL Docklet::IsVisible()
{
	// No way to get that information from a plugin ?
	return TRUE;
}

BOOL Docklet::GetRect(RECT *rcDocklet)
{
	if (!initialized || objID == 0)
		return FALSE;

	SDHostMessage(SD_GET_ABSOLUTE_RECT, objID, (DWORD)rcDocklet);

	return TRUE;
}

int Docklet::GetLabel(char *szLabel)
{
	if (szLabel != NULL && sizeof(szLabel) != 0)
		strcpy_s(szLabel, config.label.size()*sizeof(char), config.label.c_str());

	return config.label.size();
}

void Docklet::SetLabel(char *szLabel)
{
	if (!initialized)
		return;

	SDHostMessage(SD_SET_TOOLTIP_TEXT, objID, (DWORD)szLabel);

	// Store label for later
	config.label = string(szLabel);
}

void Docklet::SetImage(Image *lpImageNew, BOOL bAutomaticallyDeleteImage)
{
	// Remove previous image
	if (images.disposeImage)
		SAFE_DELETE(images.image);

	strcpy_s(images.path, "");

	images.image = (Bitmap*)lpImageNew;
	images.disposeImage = bAutomaticallyDeleteImage;

	Redraw();
}

void Docklet::SetImageFile(char *szImage)
{
	SetImage(LoadGDIPlusImage(szImage), TRUE);

	// Save image path
	strcpy_s(images.path, szImage);
}

void Docklet::GetImageFile(char* path)
{
	// We assume the path char is big enough
	strcpy_s(path, MAX_PATH*sizeof(char), images.path);
}

void Docklet::SetImageOverlay(Image *imageOverlay, BOOL bAutomaticallyDeleteImage)
{
	if (!initialized)
		return;

	// Remove previous image
	if (images.disposeOverlay)
		SAFE_DELETE(images.overlay);

	images.overlay = (Bitmap*)imageOverlay;
	images.disposeOverlay = bAutomaticallyDeleteImage;

	Redraw();
}

BOOL Docklet::BrowseForImage(HWND hwndParent, char *szImage, char *szAlternateRelativeRoot)
{
	OPENFILENAMEA ofn;
	char szFileName[MAX_PATH] = "";
	strcpy_s(szFileName, szImage);

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwndParent;
	ofn.lpstrFilter = "Image Files (*.png, *.jpg, *.bmp)\0*.png;*.jpg;*.bmp\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.lpstrInitialDir = szAlternateRelativeRoot;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "png";

	if(GetOpenFileNameA(&ofn) == 0)
		return FALSE;

	// Copy the selected file path to szImage
	strcpy_s(szImage, MAX_PATH*sizeof(char), szFileName);

	return TRUE;
}

void Docklet::LockMouseEffect(BOOL bLock)
{
	locked = bLock;
}

void Docklet::DoAttentionAnimation()
{
	//NOT_IMPLEMENTED
}

void Docklet::GetRootFolder(char *szFolder)
{
	strcpy_s(szFolder, MAX_PATH, config.rootFolder);
}

void Docklet::GetRelativeFolder(char *szFolder)
{
	strcpy_s(szFolder, MAX_PATH, config.relativeFolder);
}

void Docklet::DefaultConfigDialog()
{
	DialogBoxParam(dllInstance, MAKEINTRESOURCE(IDD_DISPLAY), (HWND)hwnd, ConfigureDisplay, (LPARAM)this);
}

int Docklet::QueryDockEdge()
{
	return config.edge;
}

int Docklet::QueryDockAlign()
{
	return config.align;
}

#pragma warning(push)
#pragma warning(disable: 4100)

void Docklet::RemoveSelf(WPARAM wParam)
{
	SDHostMessage(SD_DELETE_OBJECT, objID, FALSE);
}

BOOL Docklet::SetDockEdge(int iNewEdge)
{
	// No sense on a desktop docklet
	return FALSE;
}

BOOL Docklet::SetDockAlign(int iNewAlign)
{
	// No sense on a desktop docklet
	return FALSE;
}
#pragma warning(pop)

// Oups, no way to determine the docklet...
// Assume the relative path is using ObjectDock folder...
Bitmap* Docklet::LoadGDIPlusImage(char *szImage)
{
	CPathA filePath(szImage);

	if (!filePath.IsRelative())
		return Bitmap::FromFile(CA2W(filePath.m_strPath.GetString()));

	// Get ObjectDock path from registry
	char rootFolder[MAX_PATH];
	GetObjectDockFolder(rootFolder);

	CPathA path(rootFolder);
	path.AddBackslash();
	path.Append(szImage);

	return Bitmap::FromFile(CA2W(path.m_strPath.GetString()));
}

//////////////////////////////////////////////////////////////////////////
// Events
//////////////////////////////////////////////////////////////////////////

void Docklet::LeftButtonClick(POINT ptCursor)
{
	if (OnLeftButtonClick == NULL || locked)
		return;

	SIZE dockletSize = {config.size, config.size};

	// No default action if nothing is done
	OnLeftButtonClick(docklet_data, &ptCursor, &dockletSize);
}

void Docklet::DoubleClick(POINT ptCursor)
{
	if (OnDoubleClick == NULL || locked)
		return;

	SIZE dockletSize = {config.size, config.size};

	// No default action if nothing is done
	OnDoubleClick(docklet_data, &ptCursor, &dockletSize);
}

void Docklet::LeftButtonHeld(POINT ptCursor)
{
	if (OnLeftButtonHeld == NULL || locked)
		return;

	SIZE dockletSize = {config.size, config.size};

	// No default action if nothing is done
	OnLeftButtonHeld(docklet_data, &ptCursor, &dockletSize);
}

BOOL Docklet::RightButtonClick(POINT ptCursor)
{
	if (locked)
		return TRUE;

	SIZE dockletSize = {config.size, config.size};

	if (OnRightButtonClick != NULL)
		return OnRightButtonClick(docklet_data, &ptCursor, &dockletSize);FALSE;

	return FALSE;
}

void Docklet::Configure()
{
	BOOL isHandled = FALSE;

	if (OnConfigure != NULL)
		isHandled = OnConfigure(docklet_data);

	// Show config dialog
	// Crappy docklets do not return a valid value, so never show the default config dialog :(
	//if (!isHandled)
	//	DefaultConfigDialog();
}

void Docklet::Save(char* ini, BOOL isForExport)
{
	if (OnSave == NULL)
		return;

	OnSave(docklet_data, ini, DOCKLET_CONFIG_SECTION, isForExport);
}

BOOL Docklet::AcceptDropFiles()
{
	if (OnAcceptDropFiles == NULL || locked)
		return FALSE;

	return OnAcceptDropFiles(docklet_data);
}

BOOL Docklet::Drop(IDataObject *pDataObject, DWORD grfKeyState, DWORD *pdwEffect)
{
	if (locked)
		return FALSE;

	BOOL handleEffect = FALSE;

	//////////////////////////////////////////////////////////////////////////
	// OnDropData
	if (OnDropData != NULL) {
		OnDropData(pDataObject, grfKeyState, pdwEffect);
		handleEffect = TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	// HDROP (files...)

	if (OnDropFiles == NULL)
		return handleEffect;

	FORMATETC fmthdrop = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgmed;

	if(pDataObject->QueryGetData(&fmthdrop) == S_OK) {
		if(pDataObject->GetData(&fmthdrop, &stgmed) == S_OK) {

			// we asked for the data as a HGLOBAL, so access it appropriately
			HDROP hDrop = (HDROP)GlobalLock(stgmed.hGlobal);

			OnDropFiles(docklet_data, hDrop);

			GlobalUnlock(stgmed.hGlobal);

			// release the data using the COM API
			ReleaseStgMedium(&stgmed);
		}
	}

	return handleEffect;
}

void Docklet::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (OnProcessMessage == NULL)
		return;

	OnProcessMessage(docklet_data, hwnd, uMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
// Drawing
//////////////////////////////////////////////////////////////////////////

void Docklet::ShowDefaultImage()
{
	// Clear image & overlay
	SAFE_DELETE(images.image);
	SAFE_DELETE(images.overlay);

	Redraw();
}

void Docklet::Redraw()
{
	// Ask for redrawing
	SDHostMessage(SD_REDRAW, objID, NULL);
}

#pragma warning(push)
#pragma warning(disable: 4100)
void Docklet::Draw(HDC hdc, HBITMAP hBitmap)
{
	Graphics graphics(hdc);
	graphics.SetInterpolationMode(InterpolationModeHighQuality);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);

	graphics.Clear(Color::Transparent);

	RectF rect(0.0f, 0.0f, (REAL)config.size, (REAL)config.size);

	if (images.image == NULL && images.overlay == NULL)
		DrawBitmap(images.background->m_pBitmap, rect, &graphics);

	if (images.image != NULL)
		DrawBitmap(images.image, rect, &graphics);

	if (images.overlay != NULL)
		DrawBitmap(images.overlay, rect, &graphics);
}
#pragma warning(pop)

void Docklet::DrawBitmap(Bitmap* bitmap, RectF rect, Graphics* graphics)
{
	graphics->DrawImage(bitmap, rect, 0.0f, 0.0f, (REAL)bitmap->GetWidth(), (REAL)bitmap->GetHeight(), UnitPixel);
}

//////////////////////////////////////////////////////////////////////////.
// Configuration
//////////////////////////////////////////////////////////////////////////

void Docklet::LoadConfig(char* ini)
{
	char dockletFile[MAX_PATH];
	GetPrivateProfileStringA("Config", "Docklet", "", dockletFile, MAX_PATH, ini);
	int size = GetPrivateProfileIntA("Config", "Size", DOCKLET_DEFAULT_SIZE, ini);

	SetSize(size);
	SetDockletFile(dockletFile);
	strcpy_s(config.iniFile, ini);
}

void Docklet::SaveConfig(char* ini, BOOL isForExport)
{
	WritePrivateProfileStringA("Config", "Docklet", config.dockletFile, ini);
	WritePrivateProfileInt((LPCTSTR)"Config", (LPCTSTR)"Size", config.size, (LPCTSTR)ini);

	// Save label&image, as docklets might not save it themselves
	WritePrivateProfileStringA(DOCKLET_CONFIG_SECTION, "Image", images.path, ini);
	WritePrivateProfileStringA(DOCKLET_CONFIG_SECTION, "Title", config.label.c_str(), ini);

	// Save docklet configuration
	Save(ini, isForExport);
}

//////////////////////////////////////////////////////////////////////////
// Information
//////////////////////////////////////////////////////////////////////////

Docklet::DOCKLET_INFO Docklet::GetInformationFromDll(char* dll)
{
	DOCKLET_INFO info;
	OD_GETINFORMATION funcPointer = NULL;

	HINSTANCE hInst = LoadPluginInstance(dll, 1, &funcPointer, "OnGetInformation");

	if (hInst == NULL || funcPointer == NULL)
		return info;

	LoadInformation(funcPointer, &info);

	FreeLibrary(hInst);

	return info;
}

void Docklet::LoadInformation(OD_GETINFORMATION funcPointer, DOCKLET_INFO* info)
{
	funcPointer((char*)info->name,
				(char*)info->author,
				(int*)&info->version,
				(char*)info->notes);

	info->valid = true;
}

Docklet::DOCKLET_INFO Docklet::GetInformation()
{
	return info;
}


//////////////////////////////////////////////////////////////////////////
// Accessors
//////////////////////////////////////////////////////////////////////////

void Docklet::SetSize(int size)
{
	config.size = size;

	if (objID == 0)
		return;

	SD_SOP_INFO info;
	info.width = config.size;
	info.height = config.size;
	info.flags = SD_SOP_SIZE;
	SDHostMessage(SD_SET_OBJECT_POS, objID, (DWORD) &info);
}

int Docklet::GetSize()
{
	return config.size;
}

void Docklet::SetDockletFile(char* path)
{
	strcpy_s(config.dockletFile, path);

	ComputePaths();
}

char* Docklet::GetDockletFile()
{
	return (char *)&config.dockletFile;
}


//////////////////////////////////////////////////////////////////////////
//LoadPluginInstance
//Argument3: nCount - Number of functions to load
//[Arguments Format]
//Argument4: Function Address - Function address we want to store
//Argument5: Function Name -  Name of the function we want
//[Repeat Format]
HINSTANCE Docklet::LoadPluginInstance(char* dll, INT nCount, ...)
{
    va_list va;
    va_start(va, nCount);

	HINSTANCE hLibrary = NULL;
	if ((hLibrary = LoadLibraryA(dll)) != NULL) {

        FARPROC * lpfProcFunction = NULL;
        LPSTR lpszFuncName = NULL;
        INT nIdxCount = 0;
        while (nIdxCount < nCount) {

            lpfProcFunction = va_arg(va, FARPROC*);
            lpszFuncName = va_arg(va, LPSTR);

            if ((*lpfProcFunction = GetProcAddress(hLibrary, lpszFuncName)) == NULL) {
                lpfProcFunction = NULL;
            }

            nIdxCount++;
        }
    } else {
        va_end(va);
		return NULL;
    }

    va_end(va);

    return hLibrary;
}

//////////////////////////////////////////////////////////////////////////
HMODULE WINAPI MyGetModuleHandleA(LPCTSTR lpModuleName)
{
	if (lpModuleName == NULL)
		return dllInstance;
	else
		return GetModuleHandleA((LPCSTR)lpModuleName);
}

HMODULE WINAPI MyGetModuleHandleW(LPCWSTR lpModuleName)
{
	if (lpModuleName == NULL)
		return dllInstance;
	else
		return GetModuleHandleW(lpModuleName);
}

//////////////////////////////////////////////////////////////////////////
BOOL Docklet::InterceptDllCall(HMODULE hLocalModule,
					  LPCSTR c_szDllName,
					  const char* c_szApiName,
					  PVOID pApiNew,
					  PVOID* p_pApiOrg,
					  PVOID pApiToChange)
{
	PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hLocalModule;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	DWORD dwProtect;
	BOOL bSuccess = FALSE;

	DWORD dwAddressToIntercept;

	if (pApiToChange)
		dwAddressToIntercept = (DWORD)pApiToChange;
	else
		dwAddressToIntercept = (DWORD)GetProcAddress(GetModuleHandleA((LPCSTR)c_szDllName), (char*)c_szApiName);

	if (IsBadReadPtr(hLocalModule, sizeof(PIMAGE_NT_HEADERS)))
		return FALSE;

	if (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return FALSE;

	pNTHeader = MakePtr(PIMAGE_NT_HEADERS, pDOSHeader, pDOSHeader->e_lfanew);
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		return FALSE;

	pImportDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR, hLocalModule, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	if (pImportDesc == (PIMAGE_IMPORT_DESCRIPTOR)pNTHeader)

		return FALSE;

	while (pImportDesc->Name)
	{
		PIMAGE_THUNK_DATA pThunk;

		pThunk = MakePtr(PIMAGE_THUNK_DATA, hLocalModule, pImportDesc->FirstThunk);

		while (pThunk->u1.Function)
		{
			if ((DWORD)pThunk->u1.Function == dwAddressToIntercept)
			{
				if (!IsBadWritePtr((PVOID)&pThunk->u1.Function, sizeof(PVOID)))
				{
					if (p_pApiOrg)
						*p_pApiOrg = (PVOID)pThunk->u1.Function;

					pThunk->u1.Function = (DWORD)pApiNew;
					bSuccess = TRUE;
				}
				else
				{
					if (VirtualProtect((PVOID)&pThunk->u1.Function, sizeof(PVOID), PAGE_EXECUTE_READWRITE, &dwProtect))
					{
						DWORD dwNewProtect;

						if (p_pApiOrg)
							*p_pApiOrg = (PVOID)pThunk->u1.Function;

						pThunk->u1.Function = (DWORD)pApiNew;
						bSuccess = TRUE;

						dwNewProtect = dwProtect;

						VirtualProtect((PVOID)&pThunk->u1.Function, sizeof(PVOID), dwNewProtect, &dwProtect);
					}
				}
			}
			pThunk++;
		}
		pImportDesc++;
	}

	return bSuccess;
}