// Macros and helper functions for DesktopX scripted plugins

#define DECLARE_DXPLUGIN_READTYPEINFO(functionName, guid) \
ITypeInfo* functionName(HMODULE hm) \
{ \
	CHAR szFile[MAX_PATH] = {0}; \
	GetModuleFileName(hm, szFile, MAX_PATH); \
	CComPtr<ITypeLib> pTL; \
	ITypeInfo* pTI = NULL; \
	USES_CONVERSION; \
	if(S_OK == LoadTypeLib(A2W(szFile), &pTL)) \
	{ \
		pTL->GetTypeInfoOfGuid(guid, &pTI); \
	} \
	return pTI; \
}


#define DECLARE_DXPLUGIN_DLLFUNCTIONS(libId, dllName, clsId, progId, typeLib, indProgId, version, typeLibName) \
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) \
{ \
    if (dwReason == DLL_PROCESS_ATTACH) \
    { \
		_Module.Init(ObjectMap, hInstance, &libId); \
		DllRegisterServer(); \
    } \
    else if (dwReason == DLL_PROCESS_DETACH) \
        _Module.Term(); \
    return TRUE; \
} \
 \
STDAPI DllCanUnloadNow(void) \
{ \
	return (_Module.GetLockCount()==0) ? S_OK : S_FALSE; \
} \
 \
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) \
{ \
    return _Module.GetClassObject(rclsid, riid, ppv); \
} \
 \
STDAPI DllRegisterServer(void) \
{ \
	TCHAR szModFileName[MAX_PATH] = ""; \
	GetModuleFileName(GetModuleHandle(dllName), szModFileName, MAX_PATH); \
	{ \
		TCHAR szRegKey[] = "SOFTWARE\\Classes"; \
	     \
		HKEY hkey0 = NULL; \
		RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey, 0, KEY_ALL_ACCESS, &hkey0); \
		 \
		if (hkey0) \
		{ \
			HKEY hkey = NULL; \
			RegCreateKey(hkey0, "CLSID", &hkey); \
			 \
			if (hkey != NULL) \
			{ \
				HKEY hkey2 = NULL; \
				RegCreateKey(hkey, clsId, &hkey2); \
				if (hkey2 != NULL) \
				{ \
					HKEY hkey3 = NULL; \
					 \
					RegCreateKey(hkey2, "InprocServer32", &hkey3); \
					RegSetValueEx(hkey3, NULL, 0, REG_SZ, (BYTE*) szModFileName, (lstrlen(szModFileName)+1) * sizeof(TCHAR)); \
					RegSetValueEx(hkey3, "ThreadingModel", 0, REG_SZ, (BYTE*) "Apartment", (lstrlen("Apartment")+1) * sizeof(TCHAR)); \
					RegCloseKey(hkey3); \
					 \
					RegCreateKey(hkey2, "ProgID", &hkey3); \
					RegSetValueEx(hkey3, NULL, 0, REG_SZ, (BYTE*) progId, (lstrlen(progId)+1) * sizeof(TCHAR)); \
					RegCloseKey(hkey3); \
					 \
					RegCreateKey(hkey2, "Programmable", &hkey3); \
					RegCloseKey(hkey3); \
					 \
					RegCreateKey(hkey2, "TypeLib", &hkey3); \
					RegSetValueEx(hkey3, NULL, 0, REG_SZ, (BYTE*) typeLib, (lstrlen(typeLib)+1) * sizeof(TCHAR)); \
					RegCloseKey(hkey3); \
					 \
					RegCreateKey(hkey2, "VersionIndependentProgID", &hkey3); \
					RegSetValueEx(hkey3, NULL, 0, REG_SZ, (BYTE*) indProgId, (lstrlen(indProgId)+1) * sizeof(TCHAR)); \
					RegCloseKey(hkey3);	 \
					 \
					RegCloseKey(hkey2); \
				} \
				 \
				RegCloseKey(hkey); \
			} \
			 \
			RegCloseKey(hkey0); \
		} \
	} \
	{ \
		TCHAR szRegKey[] = "SOFTWARE\\Classes"; \
		 \
		HKEY hkey00 = NULL; \
		RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey, 0, KEY_ALL_ACCESS, &hkey00); \
		 \
		if (hkey00) \
		{ \
			HKEY hkey0 = NULL; \
			RegCreateKey(hkey00, "TypeLib", &hkey0); \
			 \
			if (hkey0 != NULL) \
			{ \
				HKEY hkey = NULL; \
				RegCreateKey(hkey0, typeLib, &hkey); \
				 \
				if (hkey != NULL) \
				{ \
					HKEY hkey2 = NULL; \
					RegCreateKey(hkey, version, &hkey2); \
					if (hkey2 != NULL) \
					{ \
						RegSetValueEx(hkey2, NULL, 0, REG_SZ, (BYTE*) typeLibName, (lstrlen(typeLibName)+1) * sizeof(TCHAR)); \
						 \
						HKEY hkey3 = NULL, hkey4 = NULL; \
						 \
						RegCreateKey(hkey2, "0", &hkey3); \
						RegCreateKey(hkey3, "Win32", &hkey4); \
						RegSetValueEx(hkey4, NULL, 0, REG_SZ, (BYTE*) szModFileName, (lstrlen(szModFileName)+1) * sizeof(TCHAR)); \
						RegCloseKey(hkey4); \
						RegCloseKey(hkey3); \
						 \
						RegCreateKey(hkey2, "FLAGS", &hkey3); \
						RegSetValueEx(hkey3, NULL, 0, REG_SZ, (BYTE*) "0", (lstrlen("0")+1) * sizeof(TCHAR)); \
						RegCloseKey(hkey3); \
						 \
						RegCreateKey(hkey2, "HELPDIR", &hkey3); \
						RegCloseKey(hkey3); \
						 \
						RegCloseKey(hkey2); \
					} \
					 \
					RegCloseKey(hkey); \
				} \
				RegCloseKey(hkey0); \
			} \
			RegCloseKey(hkey00); \
		} \
	} \
	 \
	return 1; \
} \
 \
STDAPI DllUnregisterServer(void) \
{ \
	return _Module.UnregisterServer(TRUE); \
}