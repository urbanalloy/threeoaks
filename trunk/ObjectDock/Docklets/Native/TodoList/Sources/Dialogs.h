///////////////////////////////////////////////////////////////////////////////////////////////
//
// TodoList : a ToDo Docklet for ObjectDock
//
// (c) 2004 - Julien Templier
// Parts from Weather Docklet by Jeff Bargmann
//
// This work is licensed under the Creative Commons
// Attribution-ShareAlike License. To view a copy of this license, visit
// http://creativecommons.org/licenses/by-sa/2.0/ or send a letter to
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DIALOGS_H
#define _DIALOGS_H

#include <windows.h>

void showPreviewIcon(HWND hDlg, DOCKLET_DATA *lpData);
void showOverdue(HWND hDlg, DOCKLET_DATA *lpData);
void showIcon(HWND hDlg, int ID_DIALOG, DOCKLET_DATA *lpData, char* name, int size);

int CALLBACK ConfigureDocklet(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
int CALLBACK ReminderOptions(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);

#endif