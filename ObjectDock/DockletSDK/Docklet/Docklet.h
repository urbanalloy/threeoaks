///////////////////////////////////////////////////////////////////////////////////////////////
//
// DockletSDK : Docklet Class
//
// (c) 2004 - Julien Templier
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Utils.h"

#include <windows.h>
#include <string>
using namespace std;

#include <Gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

class Docklet
{
	private:
		struct DOCKLET_STATIC_DATA
		{
			HWND hwndDocklet;
			HINSTANCE hInstance;
			string rootFolder;		// ObjectDock Root Folder
			string relativeFolder;	// ObjectDock relative path
		};

		DOCKLET_STATIC_DATA* data;
		void PopulateStaticData(HWND hwndDocklet,HINSTANCE hInstance);
		void _GetRootFolder(char *szRootFolder);
		void _GetRelativeFolder(char *szFolder);

	public:
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

		Docklet(HWND hwndDocklet,HINSTANCE hInstance);
		~Docklet();

		HWND GetHWND();
		HINSTANCE GetInstance();

		bool IsVisible();
		bool GetRect(RECT* rect);
		
		void SetLabel(string label);
		string GetLabel();

		Bitmap* LoadGDIPlusImage(string imagePath);
		void SetImage(Image *image, bool automaticallyDeleteImage = TRUE);
		void SetImageFile(string imagePath);
		void SetImageOverlay(Image *imageOverlay, bool automaticallyDeleteImage = TRUE);
		bool BrowseForImage(HWND hwndParent, char* image, string alternativeRelativeRoot = string(""));

		void LockMouseEffect(bool lock);
		void DoAttentionAnimation();
		void DoClickAnimation();
		
		BOOL RemoveSelf(WPARAM wParam);

		string GetRootFolder();
		string GetRelativeFolder();

		void DefaultConfigDialog();

		EDGE QueryDockEdge();
		ALIGN QueryDockAlign();
		bool SetDockEdge(EDGE newEdge);
		bool SetDockAlign(ALIGN newAlign);
};

///////////////////////////////////////////////////////////////////////////////////////////////
///								HELPER FUNCTIONS										    ///
///////////////////////////////////////////////////////////////////////////////////////////////
int WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int iValue, LPCTSTR lpFileName);