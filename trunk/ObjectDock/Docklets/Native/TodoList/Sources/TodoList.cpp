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


#include "TodoList.h"

#include <Docklet/Utils.h>
#include "Dialogs.h"
#include "resource.h"

#pragma warning (disable: 4100)

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//Return basic information about the plugin
void CALLBACK OnGetInformation(char *szName, char *szAuthor, int *iVersion, char *szNotes)
{
	strcpy(szName, "TodoList Docklet");
	strcpy(szAuthor, "Julien Templier");
	(*iVersion) = 100 * VERSION_MAJOR + VERSION_MINOR;
	strcpy(szNotes,   "Todo List for ObjectDock");
}


///////////////////////////////////////////////////////////////////////////////////////////////
DOCKLET_DATA *CALLBACK OnCreate(HWND hwndDocklet,
								HINSTANCE hInstance,
								char *szIni,
								char *szIniGroup)
{
	// plugin personal-data structure
	DOCKLET_DATA *lpData = new DOCKLET_DATA;
	ZeroMemory(lpData, sizeof(DOCKLET_DATA));

	// Create Console
	lpData->console = new ODConsole();
	lpData->console->Create("TodoList Console", true);

	lpData->docklet = new Docklet(hwndDocklet, hInstance);
	lpData->todo = new Todo(lpData->docklet, lpData->console);
	lpData->configuring = false;

	if(szIni && szIniGroup)
		lpData->todo->LoadSettings(szIni, szIniGroup);
	else
		lpData->todo->LoadDefaultSettings();

	return lpData;
}


///////////////////////////////////////////////////////////////////////////////////////////////
void CALLBACK OnSave(DOCKLET_DATA *lpData, char *szIni, char *szIniGroup, BOOL bIsForExport)
{
	lpData->todo->SaveSettings(szIni, szIniGroup, bIsForExport != 0);	
}


///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK OnExportFiles(DOCKLET_DATA *lpData, char *szFileRelativeOut, int iteration)
{
	//switch(iteration)
	//{
	//case 0:
	//	DockletGetRelativeFolder(lpData->hwndDocklet, szFileRelativeOut);
	//	strcat(szFileRelativeOut, lpData->szFolderName);
	//	return TRUE;

	//case 1:
	//	// FIXME: add icon files
	//	break;
	//}

	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////////////////////
void CALLBACK OnDestroy(DOCKLET_DATA *lpData, HWND hwndDocklet)
{
	delete lpData->docklet;
	delete lpData->todo;

#ifdef _DEBUG
	lpData->console->Close();
	delete lpData->console;
#endif

	delete lpData;
}


///////////////////////////////////////////////////////////////////////////////////////////////
void CALLBACK OnConfigure(DOCKLET_DATA *lpData)
{
	//Create dialog box 
	lpData->configuring = true;
	DialogBoxParam(lpData->docklet->GetInstance(), MAKEINTRESOURCE(IDD_CONFIG), lpData->docklet->GetHWND(), ConfigureDocklet, (LPARAM) lpData);
	lpData->configuring = false;
}


///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK OnLeftButtonClick(DOCKLET_DATA *lpData, POINT *ptCursor, SIZE *sizeDocklet)
{
	OnConfigure(lpData);	
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK OnRightButtonClick(DOCKLET_DATA *lpData, POINT *ptCursor, SIZE *sizeDocklet)
{
	if (lpData->configuring)
		return TRUE;

	HMENU hMenu = CreatePopupMenu();
	
	AppendMenu(hMenu, MF_STRING | MF_ENABLED, 1, "Edit Task");

	POINT ptMenu;
	GetCursorPos(&ptMenu);
	//Lock the dock's zooming while our menu is in use.
	lpData->docklet->LockMouseEffect(TRUE);

	int iCommand = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, ptMenu.x, ptMenu.y, 0, lpData->docklet->GetHWND(), NULL);
	lpData->docklet->LockMouseEffect(FALSE);
	DestroyMenu(hMenu);

	if(iCommand <= 0)
		return TRUE;

	switch(iCommand)
	{
		case 1:
			OnConfigure(lpData);
			break;
	}
	
	return TRUE;
}