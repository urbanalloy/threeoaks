//Simple animation plugin for ObjectDock

#include <windows.h>
#include <stdio.h>
#include <DockletSDK.h>
#include "CAnimation.h"
#include "resource.h"


int CALLBACK ConfigureDocklet(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);


BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}



void CALLBACK OnGetInformation(char *szName, char *szAuthor, int *iVersion, char *szNotes)
{
	strcpy(szName, "Simple Animation Docklet");
	strcpy(szAuthor, "Jeff Bargmann");
	(*iVersion) = 100;
	strcpy(szNotes,   "A simple animation docklet for ObjectDock");
}

Animation *CALLBACK OnCreate(HWND hwndDocklet, HINSTANCE hInstance, char *szIni, char *szIniGroup)
{
	Animation *lpAnimation = new Animation(hwndDocklet, hInstance);

	/////////

	char szImageToLoad[MAX_PATH+10];
	strcpy(szImageToLoad, "");
	
	if(szIni && szIniGroup)
	{
		//load from ini
		GetPrivateProfileString(szIniGroup, "AnimationImage", "", szImageToLoad, MAX_PATH, szIni);
	}
	else
	{
		//Load default options
		DockletSetLabel(lpAnimation->hwndDocklet, "Animation Docklet!");

		DockletGetRelativeFolder(lpAnimation->hwndDocklet, szImageToLoad);
		strcat(szImageToLoad, "globe.png");
	}
	

	lpAnimation->SetImage(szImageToLoad);


	return lpAnimation;
}

void CALLBACK OnSave(Animation *lpAnimation, char *szIni, char *szIniGroup, BOOL bIsForExport)
{
	char szImage[MAX_PATH+100];
	lpAnimation->GetImage(szImage);
	WritePrivateProfileString(szIniGroup, "AnimationImage", szImage, szIni);
}

void CALLBACK OnDestroy(Animation *lpAnimation, HWND hwndDocklet)
{
	delete lpAnimation;
	lpAnimation = NULL;
}


BOOL CALLBACK OnLeftButtonClick(Animation *lpAnimation, POINT *ptCursor, SIZE *sizeDocklet)
{
	DockletDoAttentionAnimation(lpAnimation->hwndDocklet);
	return TRUE;
}


int CALLBACK ConfigureDocklet(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	Animation *lpAnimation = (Animation *) GetProp(hDlg, "lpAnimation");

	switch(iMsg)
	{
	case WM_INITDIALOG:
		SetProp(hDlg, "lpAnimation", (HANDLE) (char*) lParam);
		lpAnimation = (Animation *) GetProp(hDlg, "lpAnimation");
		if(!lpAnimation)
			return TRUE;

		break;

	case WM_COMMAND:
		char szThis[2000];
		strcpy(szThis, "");
		switch(LOWORD(wParam))
		{
		case IDOK:
			if(!lpAnimation)
				break;

			
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;

	case WM_DESTROY:
		RemoveProp(hDlg, "lpAnimation");
		break;
	}

	return FALSE;
}

void CALLBACK OnConfigure(Animation *lpAnimation)
{
	//Get the animation folder's root, we want to limit image selection to this folder
	char szBrowseRoot[MAX_PATH+10];
	DockletGetRelativeFolder(lpAnimation->hwndDocklet, szBrowseRoot);
	
	//Show ChooseImage dialog to pick a new image
	char szCurrentImage[MAX_PATH+10];
	lpAnimation->GetImage(szCurrentImage);
	if(DockletBrowseForImage(NULL, szCurrentImage, szBrowseRoot))
		lpAnimation->SetImage(szCurrentImage);
}

BOOL CALLBACK OnDoubleClick(Animation *lpAnimation, POINT *ptCursor, SIZE *sizeDocklet)
{
	return FALSE;
}

BOOL CALLBACK OnAcceptDropFiles(Animation *lpAnimation)
{
	return FALSE;
}

void CALLBACK OnDropFiles(Animation *lpAnimation, HDROP hDrop)
{
	return;
}
