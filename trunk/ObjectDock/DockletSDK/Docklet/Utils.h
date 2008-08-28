///////////////////////////////////////////////////////////////////////////////////////////////
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

#pragma once

#include <Windows.h>

#ifdef _DEBUG
	#define LOG(str) SendNotepadString(str)
	// Do not use directly, use LOG(str) instead
	void SendNotepadString(char *szText);
	void SendNotepadString(const char *szText);
	void SendNotepadString(int number);
	void ErrorDescription(HRESULT hr);
#else
	#define LOG(str)
#endif