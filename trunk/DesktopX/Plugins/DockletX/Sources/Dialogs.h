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

#include "DockletX.h"

void showImage(HWND hDlg, char* image, Docklet* docklet);
void showInfo(HWND hDlg, PLUGIN_DATA *data, int selected);

int CALLBACK ConfigurePlugin(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
int CALLBACK ConfigureDisplay(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);