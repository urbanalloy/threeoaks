//////////////////////////////////////////////////////////////////////////////////////////////
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

#include "Utils.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// Send the text to the opened notepad window
void SendNotepadString(char *szText)
{
	HWND hwndNotepad, hwndEdit;
	hwndNotepad = FindWindowEx(NULL,		NULL, "Notepad", NULL);
	hwndEdit	= FindWindowEx(hwndNotepad, NULL, "Edit",	 NULL); 
	if(!hwndNotepad || !hwndEdit)
		return;

	UINT i;
	for(i = 0 ; i < strlen(szText) ; i++)
	{
		SendMessage(hwndEdit, WM_CHAR, (WPARAM) szText[i], NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
void SendNotepadString(const char *szText)
{
	SendNotepadString((char*)szText);
}

///////////////////////////////////////////////////////////////////////////////////////////////
void SendNotepadString(int number)
{
	char buffer[25];
	_itoa(number, buffer, 10);

	SendNotepadString(buffer);
}


///////////////////////////////////////////////////////////////////////////////////////////////
void ErrorDescription(HRESULT hr) {
	if(FACILITY_WINDOWS == HRESULT_FACILITY(hr))
		hr = HRESULT_CODE(hr);
	char* szErrMsg;
	if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPTSTR)&szErrMsg,
		0,
		NULL) != 0) 
	{
		LOG(szErrMsg);
		LocalFree(szErrMsg);
	} else {
		LOG("[Could not find a description for error # ");
		LOG(hr);
	}
}