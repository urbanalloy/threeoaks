//Sample plugin for ObjectDock

#include <stdio.h>
#include <windows.h>
#include <shlwapi.h>
#include <DockletSDK.h>
#include "resource.h"

typedef struct
{
	HWND hwndDocklet;
	HINSTANCE hInstanceDll;

	int iTestNumber;
	char szTestString[500];

} DOCKLET_DATA;


/////////////////////////////////////////////////
//Local functions
void strcatNumber(char *szString, int num);
/////////////////////////////////////////////////


BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}



void CALLBACK OnGetInformation(char *szName, char *szAuthor, int *iVersion, char *szNotes)
{
	strcpy(szName, "Sample Docklet");
	strcpy(szAuthor, "Jeff Bargmann");
	(*iVersion) = 100;
	strcpy(szNotes,   "Sample plugin for developers of ObjectDock docklets");
}


DOCKLET_DATA *CALLBACK OnCreate(HWND hwndDocklet, HINSTANCE hInstance, char *szIni, char *szIniGroup)
{
	DOCKLET_DATA *lpData = new DOCKLET_DATA;
	ZeroMemory(lpData, sizeof(DOCKLET_DATA));

	lpData->hwndDocklet = hwndDocklet;
	lpData->hInstanceDll = hInstance;



	if(szIni && szIniGroup)
	{
		//Load options from INI
		GetPrivateProfileString(szIniGroup, "TestString", "", lpData->szTestString, MAX_PATH, szIni);
		lpData->iTestNumber = GetPrivateProfileInt(szIniGroup, "TestNumber", 0, szIni);
	}
	else
	{
		//Set default options
		DockletSetLabel(lpData->hwndDocklet, "Sample Dummy Plugin");
		DockletSetImageFile(lpData->hwndDocklet, "default.png");

		srand(GetTickCount());

		lpData->iTestNumber = (rand() % 999);
		strcpy(lpData->szTestString, "");

		if( (lpData->iTestNumber / 100) )
			strcatNumber(lpData->szTestString, (lpData->iTestNumber / 100));

		if( ((lpData->iTestNumber % 100) / 10) )
			strcatNumber(lpData->szTestString, ((lpData->iTestNumber % 100) / 10));

		strcatNumber(lpData->szTestString, (lpData->iTestNumber % 10));
	}


	//Return handle to our plugin's personal data
	return lpData;
}



void CALLBACK OnSave(DOCKLET_DATA *lpData, char *szIni, char *szIniGroup, BOOL bIsForExport)
{
	WritePrivateProfileString(szIniGroup, "TestString", lpData->szTestString, szIni);
	WritePrivateProfileInt(szIniGroup, "TestNumber", lpData->iTestNumber, szIni);
}

void CALLBACK OnDestroy(DOCKLET_DATA *lpData, HWND hwndDocklet)
{	
	delete lpData;
}


BOOL CALLBACK OnLeftButtonClick(DOCKLET_DATA *lpData, POINT *ptCursor, SIZE *sizeDocklet)
{
	char szMessage[1000];
	sprintf(szMessage, "This plugin's data:\n\nnumber:%i\nstring:%s", lpData->iTestNumber, lpData->szTestString);
	MessageBox(NULL, szMessage, "Left Click - Sample Docklet", MB_OK | MB_ICONINFORMATION);
	return TRUE;
}


BOOL CALLBACK OnLeftButtonHeld(DOCKLET_DATA *lpData, POINT *ptCursor, SIZE *sizeDocklet)
{
	char szMessage[1000];
	sprintf(szMessage, "This plugin's data:\n\nnumber:%i\nstring:%s", lpData->iTestNumber, lpData->szTestString);
	MessageBox(NULL, szMessage, "Left Held - Sample Docklet", MB_OK | MB_ICONINFORMATION);
	return TRUE;
}

/*
int CALLBACK ConfigureDocklet(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	DOCKLET_DATA *lpData = (DOCKLET_DATA *) GetProp(hDlg, "lpData");

	switch(iMsg)
	{
	case WM_INITDIALOG:
		SetProp(hDlg, "lpData", (HANDLE) (char*) lParam);
		lpData = (DOCKLET_DATA *) GetProp(hDlg, "lpData");
		if(!lpData)
			return TRUE;

		break;

	case WM_COMMAND:
		char szThis[2000];
		strcpy(szThis, "");
		switch(LOWORD(wParam))
		{
		case IDOK:
			if(!lpData)
				break;

			
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;

	case WM_DESTROY:
		RemoveProp(hDlg, "lpData");
		break;
	}

	return FALSE;
}


void CALLBACK OnConfigure(DOCKLET_DATA *lpData)
{
	//Create dialog box 
	DialogBoxParam(lpData->hInstanceDll, MAKEINTRESOURCE(IDD_CONFIG), lpData->hwndDocklet, ConfigureDocklet, (LPARAM) lpData);
}
*/

BOOL CALLBACK OnDoubleClick(DOCKLET_DATA *lpData, POINT *ptCursor, SIZE *sizeDocklet)
{
	
	char szMessage[1000];
	sprintf(szMessage, "This plugin's data:\n\nnumber:%i\nstring:%s", lpData->iTestNumber, lpData->szTestString);
	MessageBox(NULL, szMessage, "Double Click - Sample Docklet", MB_OK | MB_ICONINFORMATION);
	return TRUE;
}



void strcatNumber(char *szString, int num)
{
	if(num == 0)
		strcat(szString, "zero");
	else if(num == 1)
		strcat(szString, "one");
	else if(num == 2)
		strcat(szString, "two");
	else if(num == 3)
		strcat(szString, "three");
	else if(num == 4)
		strcat(szString, "four");
	else if(num == 5)
		strcat(szString, "five");
	else if(num == 6)
		strcat(szString, "six");
	else if(num == 7)
		strcat(szString, "seven");
	else if(num == 8)
		strcat(szString, "eight");
	else if(num == 9)
		strcat(szString, "nine");

	strcat(szString, " ");
}

BOOL CALLBACK OnAcceptDropFiles(DOCKLET_DATA *data)
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// never called, DOCUMENTATION IS WRONG!
BOOL CALLBACK OnDropFiles(DOCKLET_DATA *data, HDROP hDrop) 
{
	MessageBox(NULL, "File Dropped", "Sample Docklet", MB_OK | MB_ICONINFORMATION);

	return FALSE;
}

HRESULT CALLBACK OnDropData(IDataObject *pDataObject, DWORD grfKeyState, DWORD *pdwEffect)
{
	FORMATETC fmtetc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgmed;

	// ask the IDataObject for some CF_TEXT data, stored as a HGLOBAL
	if(pDataObject->GetData(&fmtetc, &stgmed) == S_OK)
	{
		// We need to lock the HGLOBAL handle because we can't
		// be sure if this is GMEM_FIXED (i.e. normal heap) data or not
		HDROP hdrop = (HDROP)GlobalLock(stgmed.hGlobal);

		UINT  uNumFiles;
		TCHAR szNextFile [MAX_PATH];

		// Get the # of files being dropped.
		uNumFiles = DragQueryFile ( hdrop, -1, NULL, 0 );

		for ( UINT uFile = 0; uFile < uNumFiles; uFile++ )
		{
			// Get the next filename from the HDROP info.
			if ( DragQueryFile ( hdrop, uFile, szNextFile, MAX_PATH ) > 0 )
			{
				MessageBox(NULL, szNextFile, "Sample Docklet", MB_OK | MB_ICONINFORMATION);
			}
		}

		// cleanup
		GlobalUnlock(stgmed.hGlobal);
		ReleaseStgMedium(&stgmed);
	}

	*pdwEffect = DROPEFFECT_LINK;

	return S_OK;
}

IDropSource* CALLBACK OnDragDropGetSourceObject(DWORD dwOKEffect) // always 4?
{
	// to investigate... :D
	return NULL;
}


