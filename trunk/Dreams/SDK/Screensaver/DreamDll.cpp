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
#include "shlwapi.h"

DreamDll::DreamDll(LPCSTR dll) : 	
	hLibrary(NULL),
	dllName(dll),
	initialized(false),
	settings(NULL),
	signatureID(0),
	signatureData(0),
	deltaTime(1000.0f/DEFAULT_SCREENUPDATEFREQUENCY)
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

	// Load signed dll
	if (this->NotificationProcedureSecure)
		if (!LoadSignedDll())
			this->initialized = false;
}


DreamDll::~DreamDll()
{
	// Free instance
	if (this->hLibrary)
		FreeLibrary(this->hLibrary);

	if (this->hDeskscape)
		FreeLibrary(this->hDeskscape);

	if (this->settings) {
		delete(this->settings);
	}
}

void DreamDll::QuerySettings(D3DFORMAT format) {

	if (!initialized)
		return;

	settings->d3dFormat = QueryDefaultSettings(QUERY_D3DFORMAT, format);
	settings->videoTexture = QueryDefaultSettings(QUERY_VIDEOTEXTURE, 0);
	settings->screenUpdateFrequency = QueryDefaultSettings(QUERY_SCREENUPDATEFREQUENCY, DEFAULT_SCREENUPDATEFREQUENCY);
	settings->query_4 = QueryDefaultSettings(QUERY_4, 0);
	settings->query_5 = QueryDefaultSettings(QUERY_5, 0);

	// Check config
	if (settings->screenUpdateFrequency == 0)
		settings->screenUpdateFrequency = DEFAULT_SCREENUPDATEFREQUENCY;
	
	// Update helper values
	deltaTime = (float)(1000.0f/settings->screenUpdateFrequency);
}

BOOL DreamDll::Render(BOOL isRenderingDisabled, LPDIRECT3DDEVICE9 pd3dDevice, RECT rect, RECT screen, HWND hDeskscapes)
{
	if (NotificationProcedureSecure)
	{
		if (signatureData == 0 || signatureID == 0)
			return FALSE;

		return this->RenderSceneOnScreen(isRenderingDisabled, (LPDIRECT3DDEVICE9)(pd3dDevice + (signatureID + 2027 + signatureData)), rect, screen, hDeskscapes);
	}

	return this->RenderSceneOnScreen(isRenderingDisabled, pd3dDevice, rect, screen, hDeskscapes);
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
void DreamDll::LoadSignatureID() 
{
	if (NotificationProcedureSecure == NULL)
		return;

	signatureID = signatureData = 0;

	LoadSignatureData();

	// Get the signature ID
	NotificationProcedureSecure(NOTIFY_SIGNATURE, GET_SIGNATURE_ID, &signatureID);

	if (signatureID > 0 && signatureID != 0xffffffff) {
		PWSTR programData = NULL;
		SHGetKnownFolderPath(FOLDERID_ProgramData , KF_FLAG_CREATE, NULL, &programData);
	
		char filename[MAX_PATH];
		wsprintf((char*)&filename, "%ls\\Stardock\\DeskScapes\\ContentLicenses\\sig%d.bin", programData, signatureID);

		CoTaskMemFree(programData);

		//////////////////////////////////////////////////////////////////////////
		// Check that the signature file exists
		if (!PathFileExists(filename))
		{
			signatureID = signatureData = 0;
			return;
		}
	}
}

BOOL DreamDll::LoadSignedDll()
{
	// Try loading from default path
	this->hDeskscape = LoadLibraryEx("deskscape.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);

	if (this->hDeskscape)
		return TRUE;

	// Copy dream dll to temp folder, rename to deskscape.dll and load
	char tempFolder[MAX_PATH];
	GetTempPath(MAX_PATH, (char*)&tempFolder);

	char filename[MAX_PATH];
	wsprintf((char*)&filename, "%s\\deskscape.dll", tempFolder);

	CopyFile(this->dllName, filename, FALSE);

	this->hDeskscape = LoadLibraryEx(filename, NULL, DONT_RESOLVE_DLL_REFERENCES);

	if (this->hDeskscape)
		return TRUE;

	return FALSE;
}

void DreamDll::LoadSignatureData()
{
	// We do not need to actually check the signature validity, so simply compute the numbers from the SID
	char stringID[256];

	if (ComputeSID(GetUserSID(), stringID, 256))
		for (int i = 0; i < (signed)strlen((char*)stringID); i++)
			signatureData += toupper(stringID[i]);	
}

BOOL DreamDll::ComputeSID(PSID pSid, LPSTR lpString, int dSize)
{
	if (!IsValidSid(pSid))
		return FALSE;
	
	PSID_IDENTIFIER_AUTHORITY auth_id = GetSidIdentifierAuthority(pSid);
	int count = *GetSidSubAuthorityCount(pSid);

	// Check that we have enough space available
	int space = 12 * count + 28;
	if (dSize < space)
	{
		dSize = space;
		return FALSE;
	}

	int res2 = 0;
	int res1 = wsprintfA(lpString, "S-%lu-", 1);

	int length = lstrlen(lpString);
	if (auth_id->Value[0] || auth_id->Value[1] != auth_id->Value[0])
	{
		res2 = wsprintf(&lpString[length], "0x%02hx%02hx%02hx%02hx%02hx%02hx", auth_id->Value[0], auth_id->Value[1], auth_id->Value[2], auth_id->Value[3], auth_id->Value[4], auth_id->Value[5]);
	}
	else
	{
		res2 = wsprintf(&lpString[length], "%lu", auth_id->Value[5] + ((auth_id->Value[4] + ((auth_id->Value[3] + (auth_id->Value[2] << 8)) << 8)) << 8));
	}

	int res3 = res1 + res2;
	
	for (int i = 0; i < count; i++)
	{
		PDWORD sub_auth = GetSidSubAuthority(pSid, i);
		res3 += wsprintf(&lpString[res3], "-%lu", *sub_auth);
	}
	
	return TRUE;
}

PSID DreamDll::GetUserSID() {
	PSID         Sid;
	DWORD        cbReferencedDomainName, cbSid;
	LPTSTR       ReferencedDomainName;
	SID_NAME_USE eUse;

	LPCTSTR lpszSystemName = 0;
	char lpszAccountName[200];
	DWORD nSize = 200;
	if (!GetComputerName((char*)&lpszAccountName, &nSize))
		return 0;

	cbReferencedDomainName = cbSid = 0;
	if (LookupAccountName(lpszSystemName, lpszAccountName, 0, &cbSid, 
		0, &cbReferencedDomainName, &eUse)) {
			SetLastError(ERROR_NONE_MAPPED);
			return 0;
	}
	if (GetLastError(  ) != ERROR_INSUFFICIENT_BUFFER) return 0;

	Sid = (PSID)LocalAlloc(LMEM_FIXED, cbSid);
	if (!Sid) 
		return 0;

	ReferencedDomainName = (LPTSTR)LocalAlloc(LMEM_FIXED, cbReferencedDomainName);
	if (!ReferencedDomainName) {
		LocalFree(Sid);
		return 0;
	}

	if (!LookupAccountName(lpszSystemName, lpszAccountName, Sid, &cbSid,
		ReferencedDomainName, &cbReferencedDomainName, &eUse)) {
			LocalFree(ReferencedDomainName);
			LocalFree(Sid);
			return 0;
	}

	LocalFree(ReferencedDomainName);
	return Sid;
}
