///////////////////////////////////////////////////////////////////////////////////////////////
//
// DotNetDocklets : bringing .NET to ObjectDock
//
// Copyright (c) 2004-2008, Julien Templier
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

#include <stdio.h>
#include <windows.h>
#include <DockletSDK.h>
#include "InteropSDK/IRegisterInterface.h"

#include "Interop.h"
#include "Register.h"

#define REGISTER_HELPER_DLL "RegisterHelper.dll"
#define DOCKLET_HELPER_DLL "ObjectDockSDK.dll"

///////////////////////////////////////////////////////////////////////////////////////////////
#define OPENKEY(key)\
	err = RegCreateKeyEx(HKEY_CURRENT_USER, key, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);\
	if (err != ERROR_SUCCESS)\
		return false;

#define ADDVALUE(name, value)\
	err = RegSetValueEx(hKey, name, NULL,	REG_SZ, (const BYTE*)value, sizeof(value));\
	if (err != ERROR_SUCCESS) { \
		CLOSEKEY(); \
		return false; }

#define CLOSEKEY()\
	RegCloseKey(hKey);
///////////////////////////////////////////////////////////////////////////////////////////////


BOOL RegisterHelperDll(char* interop_path)
{
	// Write Settings to registry
	// For the original reg file, see the SDK Source (Interop/Data/NetDockletHelper.reg)	
	LOG("Registering Helper Dll\n");

	// Set Codebase
	char codebase[MAX_PATH];
	sprintf_s(codebase, MAX_PATH, "file:///%s%s", interop_path, REGISTER_HELPER_DLL);	

	LONG err = 0;
	HKEY hKey = NULL;

	//////////////////////////////////////////////////////////////////////////
	// Classes
	OPENKEY("Software\\Classes\\ObjectDockSDK.Registration.Register");
	ADDVALUE(NULL, "ObjectDockSDK.Registration.Register");
	CLOSEKEY();

	OPENKEY("Software\\Classes\\ObjectDockSDK.Registration.Register\\CLSID");
	ADDVALUE(NULL, "{89CF607D-F586-416f-8C93-BC7DE95FA36F}");
	CLOSEKEY();

	//////////////////////////////////////////////////////////////////////////
	// CLSID
	OPENKEY("Software\\Classes\\CLSID\\{89CF607D-F586-416f-8C93-BC7DE95FA36F}");
	ADDVALUE(NULL, "ObjectDockSDK.Registration.Register");
	CLOSEKEY();

	OPENKEY("Software\\Classes\\CLSID\\{89CF607D-F586-416f-8C93-BC7DE95FA36F}\\Implemented Categories\\{62C8FE65-4EBB-45E7-B440-6E39B2CDBF29}");
	CLOSEKEY();

	OPENKEY("Software\\Classes\\CLSID\\{89CF607D-F586-416f-8C93-BC7DE95FA36F}\\InprocServer32");
	ADDVALUE(NULL, "mscoree.dll");
	ADDVALUE("ThreadingModel", "Both");
	ADDVALUE("Class", "ObjectDockSDK.Registration.Register");
	ADDVALUE("Assembly", "RegisterHelper, Version=2.0.0.0, Culture=neutral, PublicKeyToken=a0e191e97ea06dad");
	ADDVALUE("RuntimeVersion", "v2.0.50727");
	ADDVALUE("CodeBase", codebase);
	CLOSEKEY();

	OPENKEY("Software\\Classes\\CLSID\\{89CF607D-F586-416f-8C93-BC7DE95FA36F}\\InprocServer32\\2.0.0.0");
	ADDVALUE("Class", "ObjectDockSDK.Registration.Register");
	ADDVALUE("Assembly", "RegisterHelper, Version=2.0.0.0, Culture=neutral, PublicKeyToken=a0e191e97ea06dad");
	ADDVALUE("RuntimeVersion", "v2.0.50727");
	ADDVALUE("CodeBase", codebase);
	CLOSEKEY();

	OPENKEY("Software\\Classes\\CLSID\\{89CF607D-F586-416f-8C93-BC7DE95FA36F}\\ProgId");
	ADDVALUE(NULL, "ObjectDockSDK.Registration.Register");
	CLOSEKEY();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//	Register the docklet dll
BOOL RegisterDocklet(char* path, char* interop_path) {

	IRegisterInterface *cpi = NULL;
	HRESULT hr;
	
	// Try creating a Register instance
	CoInitialize(NULL);
	hr = CREATE_INSTANCE(CLSID_Register, IID_IRegisterInterface, &cpi);

	if (FAILED(hr)) {
		// Try Registering the helper dll
		if (RegisterHelperDll(interop_path))
			hr = CREATE_INSTANCE(CLSID_Register, IID_IRegisterInterface, &cpi);
	}

	if (SUCCEEDED(hr)) {
		// FIXME: only do that once!
		// Register the ObjectDockSDK dll
		char codebase[MAX_PATH];
		sprintf_s(codebase, MAX_PATH, "%s%s", interop_path, DOCKLET_HELPER_DLL);
		cpi->RegisterDll(codebase);

		// Register the current docklet dll
		BOOL ret = cpi->RegisterDll(path);

		cpi->Release();
		cpi = NULL;

		// clean up COM
		CoUninitialize();

		return ret;
	}

	// TODO log info about failure!

	CoUninitialize();
	return false;
}
