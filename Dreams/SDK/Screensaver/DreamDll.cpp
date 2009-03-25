///////////////////////////////////////////////////////////////////////////////////////////////
//
// Dream Dll Loader
//
// (c) 2008, Three Oaks Crossing
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy $
///////////////////////////////////////////////////////////////////////////////////////////////

#include "DreamDll.h"
#include "DreamSDK.h"
#include "shlobj.h"

DreamDll::DreamDll(LPCSTR dll) : 	
	hLibrary(NULL),
	dllName(dll),
	initialized(false),
	settings(NULL),
	signatureID(NULL),
	signatureData(NULL)
{
	LoadPluginInstance(7, &this->QueryDefaultSettings,		"QueryDefaultSettings",
						  &this->NotificationProcedure,		"NotificationProcedure",
						  &this->SetupPlugin,				"SetupPlugin",
						  &this->UpdateScene,				"UpdateScene",
						  &this->RenderSceneOnScreen,		"RenderSceneOnScreen",
						  &this->FreePlugin,				"FreePlugin",
						  &this->ConfigPlugin,				"ConfigPlugin");

	this->NotificationProcedureSecure = (DREAM_NOTIFICATIONPROCEDURE_SEC) GetProcAddress(this->hLibrary, (LPCSTR)LOWORD(21));

	settings = new DREAM_SETTINGS;
}


DreamDll::~DreamDll()
{
	// Free instance
	if (this->hLibrary)
		FreeLibrary(this->hLibrary);

	if (this->settings) {
		delete(this->settings);
	}
}

void DreamDll::QuerySettings(D3DPRESENT_PARAMETERS d3dpp) {

	if (!initialized)
		return;

	settings->query_1 = QueryDefaultSettings(QUERY_D3DFORMAT, d3dpp.BackBufferFormat);
	settings->videoTexture = QueryDefaultSettings(QUERY_VIDEOTEXTURE, 0);
	settings->screenUpdateFrequency = QueryDefaultSettings(QUERY_SCREENUPDATEFREQUENCY, DEFAULT_SCREENUPDATEFREQUENCY);
	settings->query_4 = QueryDefaultSettings(QUERY_4, 0);
	settings->query_5 = QueryDefaultSettings(QUERY_5, 0);
}

void DreamDll::LoadSignature() 
{
	if (!NotificationProcedureSecure)
		return;

	// Get the signature ID
	NotificationProcedureSecure(NOTIFY_SIGNATURE, GET_SIGNATURE_ID, &signatureID);

	// Load the data from the signature bin file
	PWSTR programData = NULL;
	SHGetKnownFolderPath(FOLDERID_ProgramData , KF_FLAG_CREATE, NULL, &programData);

	



	CoTaskMemFree(programData);
}

void DreamDll::Notify(int type, int value) {

	if (!initialized)
		return;

	if (NotificationProcedureSecure == NULL)
		NotificationProcedure(type, value);
	else
		NotificationProcedureSecure(type, value, &signatureID);
}


//LoadPluginInstance
//Argument3: nCount - Number of functions to load
//[Arguments Format]
//Argument4: Function Address - Function address we want to store
//Argument5: Function Name -  Name of the function we want
//[Repeat Format]
void DreamDll::LoadPluginInstance(INT nCount, ...)
{
    va_list va;
    va_start(va, nCount);

	if ((this->hLibrary = LoadLibraryA(this->dllName)) != NULL) {

        FARPROC * lpfProcFunction = NULL;
        LPSTR lpszFuncName = NULL;
        INT nIdxCount = 0;
        while (nIdxCount < nCount) {

            lpfProcFunction = va_arg(va, FARPROC*);
            lpszFuncName = va_arg(va, LPSTR);

            if ((*lpfProcFunction = GetProcAddress(this->hLibrary, lpszFuncName)) == NULL) {				
                lpfProcFunction = NULL;
            }

            nIdxCount++;
        }
    } else {
        va_end(va);
        this->initialized = false;
		return;
    }

    va_end(va);
	this->initialized = true;
    return;
}