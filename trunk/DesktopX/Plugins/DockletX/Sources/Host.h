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

BOOL DockletIsVisible(HWND hwndDocklet);
BOOL DockletGetRect(HWND hwndDocklet, RECT *rcDocklet);
int DockletGetLabel(HWND hwndDocklet, char *szLabel);
void DockletSetLabel(HWND hwndDocklet, char *szLabel);

void DockletSetImage(HWND hwndDocklet, Image *lpImageNew, BOOL bAutomaticallyDeleteImage = TRUE);
void DockletSetImageFile(HWND hwndDocklet, char *szImage);
void DockletSetImageOverlay(HWND hwndDocklet, Image *imageOverlay, BOOL bAutomaticallyDeleteImage = TRUE);

BOOL DockletBrowseForImage(HWND hwndParent, char *szImage, char *szAlternateRelativeRoot = NULL);

void DockletLockMouseEffect(HWND hwndDocklet, BOOL bLock);
void DockletDoAttentionAnimation(HWND hwndDocklet);
void DockletDoClickAnimation(HWND hwndDocklet);

void DockletGetRootFolder(HWND hwndDocklet, char *szFolder);
void DockletGetRelativeFolder(HWND hwndDocklet, char *szFolder);

void DockletDefaultConfigDialog(HWND hwndDocklet);
void DockletRemoveSelf(HWND hwndDocklet, WPARAM wParam);

int DockletQueryDockEdge(HWND hwndDocklet);
int DockletQueryDockAlign(HWND hwndDocklet);
BOOL DockletSetDockEdge(HWND hwndDocklet, int iNewEdge);
BOOL DockletSetDockAlign(HWND hwndDocklet, int iNewAlign);

Bitmap *DockletLoadGDIPlusImage(char *szImage);
int WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int iValue, LPCTSTR lpFileName);
