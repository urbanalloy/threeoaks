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

#pragma once

#include "stdafx.h"
#include <string>

#include "CDropTarget.h"
#include "CGdiPlusBitmap.h"
using namespace std;

#define DOCKLET_SIZE_MIN 32
#define DOCKLET_SIZE_MAX 256
#define DOCKLET_DEFAULT_SIZE 128
#define DOCKLET_CONFIG_SECTION "DockletConfig"
#define OBJECTDOCK_KEY "Software\\Stardock\\Misc\\ObjectDock\\"
#define OBJECTDOCK_FREE_KEY "Software\\Stardock\\FreeStuff\\ObjectDock\\"

#pragma hdrstop
#pragma optimize("", off)

#define MAX_SIZE 2000
#define MakePtr(Type, Base, Offset) ((Type)(DWORD(Base) + (DWORD)(Offset)))

// Encapsulate a ObjectDock plugin instance
class Docklet
{
	private :
		typedef void	(__stdcall *OD_GETINFORMATION)		(char *szName, char *szAuthor, int *iVersion, char *szNotes);
		typedef void	(__stdcall *OD_GETREQUIREDVERSION)	(LONGLONG version);
		typedef void*	(__stdcall *OD_CREATE)				(HWND hwndDocklet, HINSTANCE hInstance, char *szIni, char *szIniGroup);
		typedef void	(__stdcall *OD_SAVE)				(void *lpData, char *szIni, char *szIniGroup, BOOL bIsForExport);
		typedef void	(__stdcall *OD_DESTROY)				(void *lpData, HWND hwndDocklet);
		typedef BOOL	(__stdcall *OD_EXPORTFILES)			(void *lpData, char *szFileRelativeOut, int iteration);
		typedef BOOL	(__stdcall *OD_LEFTBUTTONCLICK)		(void *lpData, POINT *ptCursor, SIZE *sizeDocklet);
		typedef BOOL	(__stdcall *OD_DOUBLECLICK)			(void *lpData, POINT *ptCursor, SIZE *sizeDocklet);
		typedef BOOL	(__stdcall *OD_LEFTBUTTONHELD)		(void *lpData, POINT *ptCursor, SIZE *sizeDocklet);
		typedef BOOL	(__stdcall *OD_RIGHTBUTTONCLICK)	(void *lpData, POINT *ptCursor, SIZE *sizeDocklet);
		typedef BOOL	(__stdcall *OD_CONFIGURE)			(void *lpData);
		typedef BOOL	(__stdcall *OD_ACCEPTDROPFILES)		(void *lpData);
		typedef void	(__stdcall *OD_DROPFILES)			(void *lpData, HDROP hDrop);
		typedef HRESULT (__stdcall *OD_DROPDATA)			(IDataObject *pDataObject, DWORD grfKeyState, DWORD *pdwEffect);
		typedef void	(__stdcall *OD_PROCESSMESSAGE)		(void *lpData, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		typedef void	(__stdcall *OD_NOTIFYSIZECHANGE)	(void *lpData, int size, BOOL hasChanged);
		typedef IDataObject* (__stdcall *OD_DRAGDROPGETSOURCEOBJECT) (DWORD *pdwDropEffect);

		// Docklet functions
		OD_GETINFORMATION OnGetInformation;
		OD_GETREQUIREDVERSION OnGetRequiredVersion;
		OD_CREATE OnCreate;
		OD_SAVE OnSave;
		OD_DESTROY OnDestroy;
		OD_EXPORTFILES OnExportFiles;
		OD_LEFTBUTTONCLICK OnLeftButtonClick;
		OD_DOUBLECLICK OnDoubleClick;
		OD_LEFTBUTTONHELD OnLeftButtonHeld;
		OD_RIGHTBUTTONCLICK OnRightButtonClick;
		OD_CONFIGURE OnConfigure;
		OD_ACCEPTDROPFILES OnAcceptDropFiles;
		OD_DROPFILES OnDropFiles;
		OD_PROCESSMESSAGE OnProcessMessage;
		OD_DROPDATA OnDropData;
		OD_NOTIFYSIZECHANGE NotifySizeChange;
		OD_DRAGDROPGETSOURCEOBJECT OnDragDropGetSourceObject;

public:

		//////////////////////////////////////////////////////////////////////////
		// Docklet Enums
		enum EDGE
		{
			EDGE_BOTTOM,
			EDGE_TOP,
			EDGE_LEFT,
			EDGE_RIGHT
		};

		enum ALIGN
		{
			ALIGN_LEFT,	// Top if dock is vertical
			ALIGN_MIDDLE,
			ALIGN_RIGHT	// Right if dock is vertical
		};

		//////////////////////////////////////////////////////////////////////////
		// Informations
		struct DOCKLET_INFO {
			char name[MAX_PATH];
			char author[MAX_PATH];
			int version;
			char notes[MAX_PATH];
			bool valid;

			DOCKLET_INFO() {
				strcpy_s(name, "");
				strcpy_s(author, "");
				version = 0;
				strcpy_s(notes, "");
				valid = false;
			}
		};

private:

		//////////////////////////////////////////////////////////////////////////
		// Configuration
		struct DOCKLET_CONFIG
		{
			char dockletFile[MAX_PATH];
			char rootFolder[MAX_PATH];
			char relativeFolder[MAX_PATH];
			char iniFile[MAX_PATH];
			int size;
			EDGE edge;
			ALIGN align;
			string label;

			DOCKLET_CONFIG()
			{
				size = DOCKLET_DEFAULT_SIZE;
				strcpy_s(dockletFile, "");
				strcpy_s(rootFolder, "");
				strcpy_s(relativeFolder, "");
				strcpy_s(iniFile, "");
				edge = EDGE_BOTTOM;
				align = ALIGN_MIDDLE;

				label = string("");
			}
		};

		//////////////////////////////////////////////////////////////////////////
		// Docklet instance
		HINSTANCE hLibrary;
		void* docklet_data;

		// State
		BOOL initialized;
		int objID;
		HWND hwnd;
		BOOL locked;

		// Drag&drop
		CComObject<CDropTarget>* pDropTarget;

		// Image & overlay
		struct DOCKLET_IMAGES {
			Bitmap* image;
			char path[MAX_PATH]; // path to image if any
			Bitmap* overlay;
			BOOL disposeImage;
			BOOL disposeOverlay;
			CGdiPlusBitmapResource* background; // Default image

			DOCKLET_IMAGES() {
				strcpy_s(path, "");
				image = NULL;
				overlay = NULL;
				disposeImage = TRUE;
				disposeOverlay = TRUE;
				background = NULL;
			}

			~DOCKLET_IMAGES() {
				SAFE_DELETE(background);
			}
		};

		DOCKLET_IMAGES images;
		DOCKLET_INFO info;
		DOCKLET_CONFIG config;

	public :

		//////////////////////////////////////////////////////////////////////////
		// Constructor/Destructor
		Docklet(int id);
		~Docklet();

		// Docklet Startup/Shutdown
		void Start(HWND hwnd);
		void Stop();

		void LoadConfig(char* ini);
		void SaveConfig(char* ini, BOOL isForExport);

		// Drawing
		void Redraw();
		void Draw(HDC hdc, HBITMAP hBitmap);

		// Accessors
		void SetSize(int size);
		int GetSize();
		void SetDockletFile(char* path);
		char* GetDockletFile();
		void GetImageFile(char* path);
		HWND GetHWND() { return hwnd; }
		BOOL IsLoaded() { return initialized; }

		// Informations
		DOCKLET_INFO GetInformation();
		static DOCKLET_INFO GetInformationFromDll(char* dll);

		// Misc
		static void GetObjectDockFolder(char* path);
		void ShowDefaultImage();

		//////////////////////////////////////////////////////////////////////////
		// Host functions
		//////////////////////////////////////////////////////////////////////////

		BOOL IsVisible();
		BOOL GetRect(RECT *rcDocklet);
		int GetLabel(char *szLabel);
		void SetLabel(char *szLabel);

		void SetImage(Image *lpImageNew, BOOL bAutomaticallyDeleteImage = TRUE);
		void SetImageFile(char *szImage);
		void SetImageOverlay(Image *imageOverlay, BOOL bAutomaticallyDeleteImage = TRUE);

		static BOOL BrowseForImage(HWND hwndParent, char *szImage, char *szAlternateRelativeRoot = NULL);

		void LockMouseEffect(BOOL bLock);
		void DoAttentionAnimation();

		void RemoveSelf(WPARAM wParam);

		void GetRootFolder(char *szFolder);
		void GetRelativeFolder(char *szFolder);

		void DefaultConfigDialog();

		int QueryDockEdge();
		int QueryDockAlign();
		BOOL SetDockEdge(int iNewEdge);
		BOOL SetDockAlign(int iNewAlign);

		static Bitmap* LoadGDIPlusImage(char *szImage);

		//////////////////////////////////////////////////////////////////////////
		// Events
		//////////////////////////////////////////////////////////////////////////
		void LeftButtonClick(POINT ptCursor);
		void DoubleClick(POINT ptCursor);
		void LeftButtonHeld(POINT ptCursor);
		BOOL RightButtonClick(POINT ptCursor);

		void Configure();
		void Save(char* ini, BOOL isForExport);

		BOOL AcceptDropFiles();
		BOOL Drop(IDataObject *pDataObject, DWORD grfKeyState, DWORD *pdwEffect);

		void ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);


	private:
		// Misc
		void ComputePaths(); // compute root and relative folders

		void ClearFunctionPointers();

		// Load Docklet
		static HINSTANCE LoadPluginInstance(char* dll, INT nCount, ...);
		static void LoadInformation(OD_GETINFORMATION funcPointer, DOCKLET_INFO* info);

		// Drawing
		void DrawBitmap(Bitmap* bitmap, RectF rect, Graphics* graphics);

		// GetModuleHandle Hook
		typedef HMODULE (WINAPI *TGetModuleHandleAFuncPtr)(LPCTSTR);
		typedef HMODULE (WINAPI *TGetModuleHandleWFuncPtr)(LPCWSTR);
		TGetModuleHandleAFuncPtr p_fnGetModuleHandleAOrg;
		TGetModuleHandleWFuncPtr p_fnGetModuleHandleWOrg;
		BOOL InterceptDllCall(HMODULE hLocalModule, LPCSTR c_szDllName, const char* c_szApiName, PVOID pApiNew, PVOID* p_pApiOrg, PVOID pApiToChange);

};
