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
