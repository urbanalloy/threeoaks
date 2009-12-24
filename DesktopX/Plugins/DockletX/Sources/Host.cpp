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


//////////////////////////////////////////////////////////////////////////
//	Implementation of the Host Docklet API
//////////////////////////////////////////////////////////////////////////

#include "Host.h"
#include "Docklet.h"
#include "DockletX.h"
#include "SDPlugin.h"

extern BOOL (__stdcall *SDHostMessage)(UINT, DWORD, DWORD);
extern map<HWND, Docklet*> dockletMap;

Docklet* FromHWND(HWND hwnd)
{
	map<HWND, Docklet*>::iterator iter = dockletMap.find(hwnd);
	if (iter != dockletMap.end())
		return iter->second;

	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// Query DesktopX visibility
BOOL DockletIsVisible(HWND hwndDocklet)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (!pDocklet)
		return FALSE;
	
	return pDocklet->IsVisible();		
}


//////////////////////////////////////////////////////////////////////////
BOOL DockletGetRect(HWND hwndDocklet, RECT *rcDocklet)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (!pDocklet)
		return FALSE;
	
	return pDocklet->GetRect(rcDocklet);
}


//////////////////////////////////////////////////////////////////////////
// Query DesktopX label
int DockletGetLabel(HWND hwndDocklet, char *szLabel)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (!pDocklet)
		return 0;
	
	return pDocklet->GetLabel(szLabel);
}


//////////////////////////////////////////////////////////////////////////
// Set DesktopX Label
void DockletSetLabel(HWND hwndDocklet, char *szLabel)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (pDocklet)
		pDocklet->SetLabel(szLabel);
}


//////////////////////////////////////////////////////////////////////////
// Draw image into plugin surface, redrawing overlay
void DockletSetImage(HWND hwndDocklet, Image *lpImageNew, BOOL bAutomaticallyDeleteImage)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (pDocklet)
		pDocklet->SetImage(lpImageNew, bAutomaticallyDeleteImage);	
}


//////////////////////////////////////////////////////////////////////////
// Load image and draw it into plugin surface
void DockletSetImageFile(HWND hwndDocklet, char *szImage)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (pDocklet)
		pDocklet->SetImageFile(szImage);
}


//////////////////////////////////////////////////////////////////////////
// Draw image overlay into plugin surface, redrawing the whole thing...
void DockletSetImageOverlay(HWND hwndDocklet, Image *imageOverlay, BOOL bAutomaticallyDeleteImage)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (pDocklet)
		pDocklet->SetImageOverlay(imageOverlay, bAutomaticallyDeleteImage);
}


//////////////////////////////////////////////////////////////////////////
// Browse for an image
BOOL DockletBrowseForImage(HWND hwndParent, char *szImage, char *szAlternateRelativeRoot)
{
	return Docklet::BrowseForImage(hwndParent, szImage, szAlternateRelativeRoot);
}


//////////////////////////////////////////////////////////////////////////
// Lock any mouse effect (zoom) - N/A for now
void DockletLockMouseEffect(HWND hwndDocklet, BOOL bLock)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (pDocklet)
		pDocklet->LockMouseEffect(bLock);
}


//////////////////////////////////////////////////////////////////////////
// Do an attention animation
void DockletDoAttentionAnimation(HWND hwndDocklet)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (pDocklet)
		pDocklet->DoAttentionAnimation();
}

void DockletDoClickAnimation(HWND hwndDocklet)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (pDocklet)
		pDocklet->DoAttentionAnimation();
}

//////////////////////////////////////////////////////////////////////////
// Remove the docklet
void DockletRemoveSelf(HWND hwndDocklet, WPARAM wParam)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (pDocklet)
		pDocklet->RemoveSelf(wParam);
}

//////////////////////////////////////////////////////////////////////////
// Get the computed root folder
void DockletGetRootFolder(HWND hwndDocklet, char *szFolder)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (pDocklet)
		pDocklet->GetRootFolder(szFolder);
}


//////////////////////////////////////////////////////////////////////////
// Get the relative folder
void DockletGetRelativeFolder(HWND hwndDocklet, char *szFolder)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (pDocklet)
		pDocklet->GetRelativeFolder(szFolder);
}


//////////////////////////////////////////////////////////////////////////
// Show default configuration dialog
void DockletDefaultConfigDialog(HWND hwndDocklet)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (pDocklet)
		pDocklet->DefaultConfigDialog();
}


//////////////////////////////////////////////////////////////////////////
// Return the dock edge, depends of Object position on screen
int DockletQueryDockEdge(HWND hwndDocklet)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (!pDocklet)
		return Docklet::EDGE_BOTTOM;

	return pDocklet->QueryDockEdge();	
}

//////////////////////////////////////////////////////////////////////////
// Return the dock align, depend of the position on screen
int DockletQueryDockAlign(HWND hwndDocklet)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (!pDocklet)
		return Docklet::ALIGN_MIDDLE;
	
	return pDocklet->QueryDockAlign();
}

//////////////////////////////////////////////////////////////////////////
// Do nothing
BOOL DockletSetDockEdge(HWND hwndDocklet, int iNewEdge)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (!pDocklet)
		return FALSE;

	return pDocklet->SetDockEdge(iNewEdge);
}

//////////////////////////////////////////////////////////////////////////
// Do nothing
BOOL DockletSetDockAlign(HWND hwndDocklet, int iNewAlign)
{
	Docklet* pDocklet = FromHWND(hwndDocklet);
	if (!pDocklet)
		return FALSE;

	return pDocklet->SetDockAlign(iNewAlign);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Bitmap* DockletLoadGDIPlusImage(char *szImage)
{	
	return Docklet::LoadGDIPlusImage(szImage);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registry
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int iValue, LPCTSTR lpFileName)
{
	//////////////////////////////////////////////////////////////////////////
	///Helper function included to quickly & easily save integers to an Ini
	char szNumber[100];
	strcpy_s(szNumber, 100, "");
	_itoa_s(iValue, szNumber, 100, 10);
	return WritePrivateProfileStringA((LPCSTR)lpAppName, (LPCSTR)lpKeyName, szNumber, (LPCSTR)lpFileName);
}