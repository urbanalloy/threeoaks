//Weather Docklet for ObjectDock

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <DockletSDK.h>
#include "resource.h"

#include <Gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")



BOOL SendNotepadString(char *szText)
{
	HWND hwndNotepad, hwndEdit;
	hwndNotepad = FindWindowEx(NULL,		NULL, "Notepad", NULL);
	hwndEdit	= FindWindowEx(hwndNotepad, NULL, "Edit",	 NULL); 
	if(!hwndNotepad || !hwndEdit)
		return FALSE;

	UINT i;
	for(i = 0 ; i < strlen(szText) ; i++)
	{
		SendMessage(hwndEdit, WM_CHAR, (WPARAM) szText[i], NULL);
	}

	return TRUE;
}

  ///////////////////////////////////////////////////
  //Docklet 'Data' struct. On the OnCreate I create 
  // on of these structures to store all the plugin's
  // 'personal' data in. If mutliple of my own plugins
  // added to the dock, they will each therefore automatically
  // have their own personal data stores and thus will able
  // to easily keep track of all their individual options.
typedef struct
{
	HWND hwndDocklet;
	HINSTANCE hInstanceDll;

	char szImage[MAX_PATH];

	char szZipCode[100];
	char szLocation[500];
	char szTemperature[500];
	char szCurrentConditions[500];

	char szFolderName[MAX_PATH+10];

	BOOL bMetric;

	HANDLE hUpdateThread;

} DOCKLET_DATA;
  //
  ///////////////////////////////////




  ///////////////////////////////////////
  //Local weather-plugin specific funcs
#define UPDATE_WEATHER_TIMER	123211
DWORD WINAPI UpdateWeatherThread(LPVOID lpInput);

void UpdateWeather(DOCKLET_DATA *lpData);
void GetWeather(char *szZipCode, char *szImageNum, char *szLocation, char *szConditions, char *szTemperature, BOOL bMetric = FALSE);
static SOCKET telnet(char *host, int port);

Image *CreateTemperatureOverlayImage(DOCKLET_DATA *lpData);

int CALLBACK ConfigureDocklet(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
  //
  ///////////////////////////////////////


BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}



//Return basic information about the plugin
void CALLBACK OnGetInformation(char *szName, char *szAuthor, int *iVersion, char *szNotes)
{
	strcpy(szName, "Weather Docklet");
	strcpy(szAuthor, "Jeff Bargmann");
	(*iVersion) = 110;
	strcpy(szNotes,   "Weather Plugin for ObjectDock");
}




//In the OnCreate, we will be passed the handle to the window which owns this docklet. In ObjectDock, each icon/image
// is its own window, thus the window passed is the actual window that the image is being displayed on. Thus, it is recommended
// that you not move or otherwise manually change the appearance of this window.
// However! You will need to record this window handle, as it is through that that you will identify yourself to ObjectDock in callbacks.
//
// An hInstance is passed giving this DLL's instance, as well as a location of an ini and the specific ini group to load from.
// If szIni and szIniGroup are NULL, this plugin is being created from scratch, and default options should be set/loaded. 
//   If they are not null, the plugin should load its settings from the ini given by szIni, from the ini group given by szIniGroup. See examples.
DOCKLET_DATA *CALLBACK OnCreate(HWND hwndDocklet, HINSTANCE hInstance, char *szIni, char *szIniGroup)
{
	//Create a plugin personal-data structure and return it. This data will be maintained through the life
	// of the plugin, and will be passed with every function call.
	DOCKLET_DATA *lpData = new DOCKLET_DATA;
	ZeroMemory(lpData, sizeof(DOCKLET_DATA));


	//Remember to store the docklet's window handle in its personal data so we can interact with the host!
	lpData->hwndDocklet = hwndDocklet;
	lpData->hInstanceDll = hInstance;


	/////////

	



	if(szIni && szIniGroup)
	{
		//If an Ini file and group were passed, load the plugin's data using them
		GetPrivateProfileString(szIniGroup, "ZipCode", "", lpData->szZipCode, MAX_PATH, szIni);
		GetPrivateProfileString(szIniGroup, "ImagesFolder", "icons", lpData->szFolderName, MAX_PATH, szIni);
		lpData->bMetric = GetPrivateProfileInt(szIniGroup, "UseMetric", FALSE, szIni);


		DockletSetLabel(lpData->hwndDocklet, "Retrieving Weather Information...");
		UpdateWeather(lpData);
	}
	else
	{
		//otherwise, load the default data and set the title appropriatly.
		strcpy(lpData->szZipCode, "");
		strcpy(lpData->szFolderName, "icons");
		lpData->bMetric = FALSE;


		DockletSetLabel(lpData->hwndDocklet, "Weather Docklet... click to setup!");
	}

	//////////



	//Set the docklet's image to the 'n/a' image by default in all cases.
	char szImage[MAX_PATH+10];
	strcpy(szImage, "");
	DockletGetRelativeFolder(lpData->hwndDocklet, szImage);
	strcat(szImage, lpData->szFolderName);
	strcat(szImage, "\\");
	strcat(szImage, "na");
	strcat(szImage, ".png");
	DockletSetImageFile(lpData->hwndDocklet, szImage);


	//update every 30min, per Weather.com's XML restrictions
	SetTimer(lpData->hwndDocklet, UPDATE_WEATHER_TIMER, (1000 * 60) * 15, NULL);	

	return lpData;
}




//When ObjectDock needs to save this plugin's individual properties, it will give you the plugin's data
// along with an szIni (file) and szIniGroup (ini section) to save the plugin's data to.
// REMEMBER! Multiple of your same plugin can be added to one dock at once, so if you want each of your
// plugins to be able to have unique options, follow the example given below.
//
// "bIsForExport" lets you know if this save is being done to be saved in a package to distribute.
//  While not important for most docklets, if for instance you have a mail checking plugin
//   you will not want to save any passwords/usernames in the event that this flag is set to TRUE.
void CALLBACK OnSave(DOCKLET_DATA *lpData, char *szIni, char *szIniGroup, BOOL bIsForExport)
{
	WritePrivateProfileString(szIniGroup, "ZipCode", lpData->szZipCode, szIni);
	WritePrivateProfileString(szIniGroup, "ImagesFolder", lpData->szFolderName, szIni);
	WritePrivateProfileInt(szIniGroup, "UseMetric", lpData->bMetric, szIni);
}



//**ObjectDock has the capability of automatically packing up individual items as well as entire docks
// to be easily distruted and automatically installed with one click. If your plugin
// has files that it uses other than its own DLL and the image it is currently displaying, which are packed up automatically,
// you will want to tell object dock about any files/folders you want to include in the event your docklet
// was to be packed up. This includes other images your docklet might use, such as the weather images folder for this plugin.
BOOL CALLBACK OnExportFiles(DOCKLET_DATA *lpData, char *szFileRelativeOut, int iteration)
{
	switch(iteration)
	{
	case 0:
		DockletGetRelativeFolder(lpData->hwndDocklet, szFileRelativeOut);
		strcat(szFileRelativeOut, lpData->szFolderName);
		return TRUE;

	case 1:
		//if more files needed to be saved, i would continue down, 1, 2, 3, etc,
		// copying the absolute OR relative path of the file i want to include with the 
		// packaging into 'szFileRelativeOut', and returning TRUE.
		// Returning FALSE stops the iteration, signifying no more files need be exported.
		break;
	}

	return FALSE;
}



//On destruction, top all timers, kill all threads left running and
// free the memory allocated for the plugin's personal data.
void CALLBACK OnDestroy(DOCKLET_DATA *lpData, HWND hwndDocklet)
{
	if(lpData->hUpdateThread)
		TerminateThread(lpData->hUpdateThread, 0);
	CloseHandle(lpData->hUpdateThread);
	lpData->hUpdateThread = NULL;

	delete lpData;
}




//Each icon in the dock is its own window. The OnProcessMessage function recieves every message that
// gets sent to that window (the HWND passed on OnCreate), including any timer messages, which we will
// make use of here
void CALLBACK OnProcessMessage(DOCKLET_DATA *lpData, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_TIMER:
		if(wParam == UPDATE_WEATHER_TIMER)
			UpdateWeather(lpData);
		break;
	}
	return;
}



//Little helper function I wrote to easily draw nice-looking text with a shadow. Kindof specific to the purpose
// however so might need to be reworked a little to be reusable.
void DrawNiceText(Graphics *graphics, char *szText, WCHAR *fontName, int style, REAL size, Rect &rect)
{
	OLECHAR text[800];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szText, -1, text, sizeof(text));

	FontFamily  fontFamily(fontName);
	Font        font(&fontFamily, 18, FontStyleBold, UnitPoint);
	SolidBrush  black(Color(160, 0, 0, 0));
	SolidBrush	white(Color(255, 255, 255, 255));
	SolidBrush  blue(Color(255, 0, 0, 255));
	SolidBrush  transparent(Color(0, 255, 255, 255));
	Pen			blackOutline(Color(255, 0, 0, 0), 1);



	GraphicsPath path(FillModeAlternate);
	StringFormat format(0, LANG_NEUTRAL);
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentFar);
	path.AddString(text, -1, &fontFamily, style, size, rect, &format);

	SIZE sizeMovement = {2, 2};
	GraphicsPath pathShaddow(FillModeAlternate);

	Rect shaddowRect(rect.GetLeft()+sizeMovement.cx, rect.GetTop()+sizeMovement.cy, (rect.GetRight()-rect.GetLeft()), (rect.GetBottom()-rect.GetTop()));

	pathShaddow.AddString(text, -1, &fontFamily, style, size, shaddowRect, &format);
	graphics->FillPath(&black, &pathShaddow);


	graphics->FillPath(&white, &path);
	graphics->DrawPath(&blackOutline, &path);
}




//Upon recieving the weather information, we will first set a File image to the plugin,
// and then create an overlay image with the temperature to place on top of that image.
// This function creates that temperature image overlay.
Image *CreateTemperatureOverlayImage(DOCKLET_DATA *lpData)
{
	if(strlen(lpData->szTemperature) <= 0)
		return NULL;

	Bitmap *imageReturn = new Bitmap(128, 128, PixelFormat32bppARGB);

	Graphics graphics(imageReturn);
	graphics.SetInterpolationMode(InterpolationModeHighQuality);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);

	
	DrawNiceText(&graphics, lpData->szTemperature, L"Arial", FontStyleBold, 42, Rect(0, 112, 128, (128-112)));
	return imageReturn;
}



BOOL HittestControl(HWND hDlg, UINT uID, POINT *ptCursor)
{
	HWND hwndControl = GetDlgItem(hDlg, uID);

	RECT rcControl;
	GetWindowRect(hwndControl, &rcControl);

	return PtInRect(&rcControl, (*ptCursor));
}

void FillImagesFolderList(char *szFolder, HWND hwndCombo, char *szSelected)
{
	int index = 0;
	SendMessage(hwndCombo, CB_RESETCONTENT, (WPARAM) NULL, (LPARAM) NULL);


	char szFullPathSearch[MAX_PATH+15];
	strcpy(szFullPathSearch, szFolder);
	strcat(szFullPathSearch, "*.*");

	WIN32_FIND_DATA	lpFindData;
	HANDLE hSearch = FindFirstFile(szFullPathSearch, &lpFindData);


	if(hSearch != INVALID_HANDLE_VALUE && hSearch)
	{
		while(TRUE)
		{
			if((stricmp(lpFindData.cFileName, ".") != 0) & (stricmp(lpFindData.cFileName, "..") != 0))
			{
				if(lpFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					SendMessage(hwndCombo, CB_INSERTSTRING, index, (LPARAM) lpFindData.cFileName);

					if(szSelected && (stricmp(lpFindData.cFileName, szSelected) == 0))
						SendMessage(hwndCombo, CB_SETCURSEL, index, (LPARAM) NULL);

					index++;
				}
			}

			if(!FindNextFile(hSearch, &lpFindData))
			{
				if(GetLastError() == ERROR_NO_MORE_FILES)
				{
					FindClose(hSearch);
					break;
				}
			}
		}
	}

	return;
}



static HCURSOR hcurHand		= NULL;
static HCURSOR hcurArrow	= LoadCursor(NULL, IDC_ARROW);

//Config dialog's proc
int CALLBACK ConfigureDocklet(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	DOCKLET_DATA *lpData = (DOCKLET_DATA *) GetProp(hDlg, "lpData");
	POINT ptCursor;
	switch(iMsg)
	{
	case WM_INITDIALOG:
		SetProp(hDlg, "lpData", (HANDLE) (char*) lParam);
		lpData = (DOCKLET_DATA *) GetProp(hDlg, "lpData");
		if(!lpData)
			return TRUE;

		if(!hcurHand)
			hcurHand = LoadCursor(lpData->hInstanceDll, MAKEINTRESOURCE(IDC_MYHAND));

		SetWindowText(GetDlgItem(hDlg, IDC_ZIPCODE), lpData->szZipCode);
		if(lpData->bMetric)
			Button_SetCheck(GetDlgItem(hDlg, IDC_METRIC), TRUE);
		else
			Button_SetCheck(GetDlgItem(hDlg, IDC_ENGLISH), TRUE);


		char szFullDockletFolderPath[MAX_PATH+10];
		char szRelativeFolder[MAX_PATH+10];
		DockletGetRootFolder(lpData->hwndDocklet, szFullDockletFolderPath);
		DockletGetRelativeFolder(lpData->hwndDocklet, szRelativeFolder);
		strcat(szFullDockletFolderPath, szRelativeFolder);
		FillImagesFolderList(szFullDockletFolderPath, GetDlgItem(hDlg, IDC_IMAGESET), lpData->szFolderName);
		break;

	case WM_PAINT:
	case WM_MOUSEMOVE:
		GetCursorPos(&ptCursor);
		if(HittestControl(hDlg, IDC_WEATHER1, &ptCursor) || HittestControl(hDlg, IDC_WEATHER2, &ptCursor))
		{
			if(GetCursor() != hcurHand)
				SetClassLong(hDlg, GCL_HCURSOR, (DWORD)hcurHand);
		}
		else if(hcurHand)
		{
			if(GetCursor() != hcurArrow)
				SetClassLong(hDlg, GCL_HCURSOR, (DWORD)hcurArrow);
		}
		break;

	case WM_LBUTTONDOWN:
		GetCursorPos(&ptCursor);
		if(HittestControl(hDlg, IDC_WEATHER1, &ptCursor) || HittestControl(hDlg, IDC_WEATHER2, &ptCursor))
			ShellExecute(lpData->hwndDocklet, NULL, "http://www.weather.com/?prod=xoap&par=1003670712", NULL, NULL, SW_SHOW);
		break;

	case WM_COMMAND:
		char szThis[2000];
		strcpy(szThis, "");
		switch(LOWORD(wParam))
		{
		case IDOK:
			if(!lpData)
				break;

			GetWindowText(GetDlgItem(hDlg, IDC_ZIPCODE), lpData->szZipCode, sizeof(lpData->szZipCode));
			lpData->bMetric = Button_GetCheck(GetDlgItem(hDlg, IDC_METRIC));


			char szImageSetNew[MAX_PATH+10];
			strcpy(szImageSetNew, "");
			SendMessage(GetDlgItem(hDlg, IDC_IMAGESET), CB_GETLBTEXT, SendMessage(GetDlgItem(hDlg, IDC_IMAGESET), CB_GETCURSEL, (WPARAM) 0, 0), (LPARAM) szImageSetNew);
			if(strlen(szImageSetNew) > 0)
				strcpy(lpData->szFolderName, szImageSetNew);


			char szImage[MAX_PATH+10];
			strcpy(szImage, "");
			DockletGetRelativeFolder(lpData->hwndDocklet, szImage);
			strcat(szImage, lpData->szFolderName);
			strcat(szImage, "\\");
			strcat(szImage, "na");
			strcat(szImage, ".png");

			//Since we're doing a full update, set the plugin's image/text indicate of an update (the n/a image and "updating weather" text)
			DockletSetImageFile(lpData->hwndDocklet, szImage);
			DockletSetLabel(lpData->hwndDocklet, "Retrieving Weather Information...");
			DockletSetImageOverlay(lpData->hwndDocklet, NULL);

			//Start UpdateWeather thread to update weather
			UpdateWeather(lpData);
			

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




//Called the dock item is called to be configured by ObjectDock, such as would happen if our plugin did not impliment
// OnRightMouseClick which would result in ObjectDock displaying a menu on right click with a menu option to configure.
void CALLBACK OnConfigure(DOCKLET_DATA *lpData)
{
	//Create dialog box 
	DialogBoxParam(lpData->hInstanceDll, MAKEINTRESOURCE(IDD_CONFIG), lpData->hwndDocklet, ConfigureDocklet, (LPARAM) lpData);
}




//Process a left mouse button click. Returning TRUE reports to ObjectDock that the event was acted upon.
// In the case of the weather plugin, if no zipcode has yet been set on a left click, open the Configure dialog.
//  otherwise, open that weather page.
BOOL CALLBACK OnLeftButtonClick(DOCKLET_DATA *lpData, POINT *ptCursor, SIZE *sizeDocklet)
{
	if( strlen(lpData->szZipCode) <= 0 )
	{
		OnConfigure(lpData);
		return TRUE;
	}

	char szWebpage[500];
	sprintf(szWebpage, "http://www.weather.com/weather/local/%s", lpData->szZipCode);
	ShellExecute(lpData->hwndDocklet, NULL, szWebpage, NULL, NULL, SW_SHOW);
	DockletDoAttentionAnimation(lpData->hwndDocklet);
	return TRUE;
}




//Process whent he left mouse button has been held. Nothing here yet...
//Returning TRUE reports to ObjectDock that the event was acted upon.
BOOL CALLBACK OnLeftButtonHeld(DOCKLET_DATA *lpData, POINT *ptCursor, SIZE *sizeDocklet)
{
	return FALSE;
}




//By implimenting the OnRightButtonClick function, ObjectDock will not automatically show a right
// click menu for the item, and thus we must create one. We want one with an Update and a Change location (configure)
BOOL CALLBACK OnRightButtonClick(DOCKLET_DATA *lpData, POINT *ptCursor, SIZE *sizeDocklet)
{
	HMENU hMenu = CreatePopupMenu();
	
	AppendMenu(hMenu, MF_STRING | MF_ENABLED, 1, "Update");
	AppendMenu(hMenu, MF_STRING | MF_ENABLED, 2, "Weather Docklet Settings...");


	POINT ptMenu;
	GetCursorPos(&ptMenu);
	DockletLockMouseEffect(lpData->hwndDocklet, TRUE);	//Lock the dock's zooming while our menu is in use.
	int iCommand = TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, ptMenu.x, ptMenu.y, 0, lpData->hwndDocklet, NULL);
	DockletLockMouseEffect(lpData->hwndDocklet, FALSE);
	DestroyMenu(hMenu);
	if(iCommand <= 0)
		return TRUE;


	switch(iCommand)
	{
	case 1:
		UpdateWeather(lpData);
		break;

	case 2:
		OnConfigure(lpData);
		break;
	}
	
	return TRUE;
}







/////////////////////////////////////////////////////////////////




void UpdateWeather(DOCKLET_DATA *lpData)
{
	if(lpData->hUpdateThread)
		return;

	//Create a thread to update the weather as to not lock up the program during any slow updates.
	DWORD dwNewThreadId;
	lpData->hUpdateThread = CreateThread(NULL, 0, UpdateWeatherThread, (VOID *) lpData, 0, &dwNewThreadId);
}



DWORD WINAPI UpdateWeatherThread(LPVOID lpInput)
{
	DOCKLET_DATA *lpData = (DOCKLET_DATA *) lpInput;
	if(!lpData)
		return 0;


	//Retrieve the weather information
	char szImageNum[100];
	strcpy(szImageNum, "");
	GetWeather(lpData->szZipCode, szImageNum, lpData->szLocation, lpData->szCurrentConditions, lpData->szTemperature, lpData->bMetric);


	//Ensure the szImageNum is two digits
	if(strlen(szImageNum) == 1)
	{
		szImageNum[0] = '0';
		szImageNum[1] = szImageNum[0];
		szImageNum[2] = 0;
	}



	//Determine what to set as the label, and set it
	char szLabel[500];
	if(strlen(lpData->szZipCode) <= 0)
		strcpy(szLabel, "No ZipCode Entered");
	else if(  (strlen(lpData->szLocation) <= 0) && (strlen(lpData->szTemperature) <= 0) )
		strcpy(szLabel, "Weather Information Unavailable");
	else
		sprintf(szLabel, "%s - %s", lpData->szLocation, lpData->szTemperature);
	DockletSetLabel(lpData->hwndDocklet, szLabel);


	//Determine the new file image to use
	char szImage[MAX_PATH+10];
	strcpy(szImage, "");
	DockletGetRelativeFolder(lpData->hwndDocklet, szImage);
	strcat(szImage, lpData->szFolderName);
	strcat(szImage, "\\");
	strcat(szImage, szImageNum);
	strcat(szImage, ".png");

	//Set the appropriate file image
	DockletSetImageFile(lpData->hwndDocklet, szImage);


	//Create a temperature overlay image and set it on top of our loaded file image.
	DockletSetImageOverlay(lpData->hwndDocklet, CreateTemperatureOverlayImage(lpData));



	//Exit the thread
	CloseHandle(lpData->hUpdateThread);
	lpData->hUpdateThread = NULL;

	ExitThread(0);

	return 0;
}



char *ReadXMLTag(char *szSource, char *szXMLTag, char *szOut = NULL, BOOL bFreeSourceWhenDone = FALSE);

void GetWeather(char *szZipCode, char *szImageNum, char *szLocation, char *szConditions, char *szTemperature, BOOL bMetric)
{
	if(szImageNum)
		strcpy(szImageNum, "na");
	if(szLocation)
		strcpy(szLocation, "");
	if(szConditions)
		strcpy(szConditions, "");
	if(szTemperature)
		strcpy(szTemperature, "");
	if(!szZipCode || (strlen(szZipCode) != 5))
		return;

	SOCKET sock = telnet("xoap.weather.com", 80);
	if(!sock)
		return;
	if(sock == INVALID_SOCKET)
		return;


   //Weather.com XML Licence number:
   //**FOR USE IN STARDOCK OBJECTDOCK ONLY - OTHER USE IS PROHIBITED**
   //Partner ID: 1003670712
   //License Key: 502259b72781a9e8
   //http://xoap.weather.com/weather/local/14612?cc=*&prod=xoap&par=1003670712&key=502259b72781a9e8

	char units[100];
	strcpy(units, "s");

	if(bMetric)
		strcpy(units, "m");

	char szData[100000];
	sprintf(szData,
			"GET /weather/local/%s?cc=*&unit=%s&prod=xoap&par=1003670712&key=502259b72781a9e8 HTTP/1.1\n"
			"Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/msword, application/vnd.ms-powerpoint, application/vnd.ms-excel, application/x-shockwave-flash, */*\n"
			"Accept-Language: en-us\n"
			"Accept-Encoding: gzip, deflate\n"
			"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; .NET CLR 1.0.3705)\n"
			"Host: xoap.weather.com\n"
			"Connection: Keep-Alive\n"
			"Cookie: CFMAGIC=2068896%3A63948208; useFlash=1; CFID=2068896; CFTOKEN=63948208\n"
			"\n", szZipCode, units);

	send(sock, szData, strlen(szData), 0);

	int nTotal = 0;
	int n;

	while( ((sizeof(szData)-nTotal-1) > 0) &&  ((n = recv(sock, (szData+nTotal), sizeof(szData)-nTotal-1, 0))) )
	{
		nTotal += n;
		szData[nTotal] = 0;
	}

//	SendNotepadString(szData);

	char *szXML;

	if(szLocation && (szXML = ReadXMLTag(szData, "dnam")))
	{
		strcpy(szLocation, szXML);
		delete [] szXML;
		szXML = NULL;
	}

	if(szTemperature && (szXML = ReadXMLTag(szData, "tmp")))
	{
		strcpy(szTemperature, szXML);
		delete [] szXML;
		szXML = NULL;

		if(szXML = ReadXMLTag(szData, "ut"))
		{
			strcat(szTemperature, "°");
			strcat(szTemperature, szXML);
			delete [] szXML;
			szXML = NULL;
		}
	}

	if(szImageNum && (szXML = ReadXMLTag(szData, "icon")))
	{
		strcpy(szImageNum, szXML);
		delete [] szXML;
		szXML = NULL;
	}


	closesocket(sock);
}

char *AdvanceToTag(char *szSource, char *szXMLTag)
{
	if(!szSource || !szXMLTag)
		return NULL;

	char *szReturn = NULL;

	char *szFullTag = new char[strlen(szXMLTag) + 5];
	strcpy(szFullTag, "<");
	strcat(szFullTag, szXMLTag);
	strcat(szFullTag, ">");
	if(strstr(szSource, szFullTag))


	delete [] szFullTag;
	return szReturn;
}

char *ReadXMLTag(char *szSource, char *szXMLTag, char *szOut, BOOL bFreeSourceWhenDone)
{
	if(!szSource || !szXMLTag)
		return NULL;


	char *szFullTag = new char[strlen(szXMLTag) + 5];
	strcpy(szFullTag, "<");
	strcat(szFullTag, szXMLTag);
	strcat(szFullTag, ">");
	char *szFullTagEnd = new char[strlen(szXMLTag) + 5];
	strcpy(szFullTagEnd, "</");
	strcat(szFullTagEnd, szXMLTag);
	strcat(szFullTagEnd, ">");

	if(strstr(szSource, szFullTag) && strstr(strstr(szSource, szFullTag), szFullTagEnd))
	{
		char *szStart = strstr(szSource, szFullTag) + strlen(szFullTag);
		char *szEnd = strstr(strstr(szSource, szFullTag), szFullTagEnd);

		if(!szOut)
			szOut = new char[(szEnd-szStart)+4];

		strncpy(szOut, szStart, (szEnd-szStart));
		szOut[(szEnd-szStart)] = 0;
	}
	else
		szOut = NULL;


	delete [] szFullTag;
	delete [] szFullTagEnd;

	if(bFreeSourceWhenDone)
		delete [] szSource;

	return szOut;
}

static SOCKET telnet(char *host, int port)
{
	SOCKET s = INVALID_SOCKET;
	struct hostent *he;
	struct sockaddr_in sin;
	int ret;

	static int initWinSock = 0;
	if (!initWinSock)
	{
		WORD version = MAKEWORD(1,1);
		WSADATA wsaData;

		ret = WSAStartup(version, &wsaData);
		if (ret != 0)
			return 0;
	
		initWinSock = 1;
	}

	he = gethostbyname(host);
	if (he)
	{
		s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s != INVALID_SOCKET)
		{
			sin.sin_family = AF_INET;
			sin.sin_addr = *(struct in_addr *) he->h_addr;
			sin.sin_port = htons((u_short) port);
			ret = connect (s, (const struct sockaddr *) &sin, sizeof(struct sockaddr));

			if (ret == SOCKET_ERROR)
			{
				closesocket(s);
				s = INVALID_SOCKET;
			}
		}
	}
	return s;
}