#pragma once

#include "stdafx.h"
#include "SysStatsCOM.h"
#include <stdio.h>
#include <comdef.h>

#include "SysStatsUtils.h"
#include "CategoryFactoryT.h"
#include "PluginManager.h"

#include "DictionaryPtr.h"
#include "DictionaryPtr.cpp"

template <class I, const GUID* G, const CATID* C>
const GUID *CategoryFactoryT<I,G,C>::interfaceGUID = G;

template <class I, const GUID* G, const CATID* C>
const CATID *CategoryFactoryT<I,G,C>::categoryID = C;

template <class I, const GUID* G, const CATID* C>
SysStatsUtils::CriticalSection CategoryFactoryT<I,G,C>::createCS;

static bool streq(const char* lhs, const char* rhs)
{
	return strcmp(lhs, rhs) == 0;
}

template <class I, const GUID* G, const CATID* C>
AutoDictionaryPtr<char, CLSID>* CategoryFactoryT<I,G,C>::getRegistry()
{
	static AutoDictionaryPtr<char, CLSID> registry(&streq);
	AtlTrace("Registry address=%x\n", registry);

	if (PluginManager::getManager()->load() || registry.count() == 0)
	{
		populateRegistry(&registry);
	}

	return &registry;
}

/*
 * Create an instance of the specified type.
 */
template <class I, const GUID* G, const CATID* C>
I* CategoryFactoryT<I,G,C>::createInstance(const char* type, IClientModel *model)
{
	AtlTrace("Trying to create an instance of %s\n", type);

	SysStatsUtils::Lock lock(createCS);

	const CLSID *pCLSID = getRegistry()->get(type);

	I *pInstance = NULL;

	if (pCLSID)
	{
		AtlTrace("Trying to create an instance\n");
		HRESULT hr = CoCreateInstance(*pCLSID, NULL, CLSCTX_INPROC_SERVER, *interfaceGUID, (void**)&pInstance);
		if (!SUCCEEDED(hr))
			AtlTrace("Failed - %x\n", hr);
		else
			pInstance->put_Model(model);
	}

	return pInstance;
}

template <class I, const GUID* G, const CATID* C>
int CategoryFactoryT<I,G,C>::typeCount()
{
	AtlTrace("Count=%d\n", getRegistry()->count());
	return getRegistry()->count();
}

template <class I, const GUID* G, const CATID* C>
void CategoryFactoryT<I,G,C>::listTypes(char** outArray)
{
	AtlTrace("Listing types\n");
	getRegistry()->getKeys(outArray);
}

template <class I, const GUID* G, const CATID* C>
void CategoryFactoryT<I,G,C>::populateRegistry(AutoDictionaryPtr<char, CLSID>* pRegistry)
{
	pRegistry->clearAndDestroy();

	CComPtr<ICatInformation> pInfo;
    if (FAILED(CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL,
                 CLSCTX_INPROC_SERVER, IID_ICatInformation, (void**)&pInfo)))
        return;

    int cIDs = 1;
    CATID IDs[1];
    IDs[0] = *categoryID;

	CComPtr<IEnumCLSID> pEnumCLSID = NULL;
    if (FAILED(pInfo->EnumClassesOfCategories(cIDs, IDs, 0, NULL,
              &pEnumCLSID)))
        return;

    char szFriendlyName[128];
    CLSID clsid;

    while (pEnumCLSID->Next(1, &clsid, NULL) == S_OK)
    {
		AtlTrace("Found an interface");
		*szFriendlyName=0;
        getFriendlyName(clsid, szFriendlyName, sizeof(szFriendlyName));
		if (*szFriendlyName)
		{
			AtlTrace(" called %s", szFriendlyName);
			DWORD version = getVersion(clsid);
			AtlTrace(" version=%d",  version);
			const CLSID *pCLSID = pRegistry->get(szFriendlyName);
			if (!pCLSID)
			{
				CLSID* pclsid = new CLSID;
				*pclsid = clsid;
				pRegistry->put(strdup(szFriendlyName), pclsid);
			}
			else if (version >= getVersion(*pCLSID))
			{
				AtlTrace(", replacing version %d", getVersion(*pCLSID));
				CLSID* pclsid = new CLSID;
				*pclsid = clsid;
				pRegistry->remove(szFriendlyName);
				pRegistry->put(strdup(szFriendlyName), pclsid);
			}
			else
			{
				AtlTrace(" new version (%d) already loaded", getVersion(*pCLSID));
			}
		}
		AtlTrace("\n");
    }
}

template <class I, const GUID* G, const CATID* C>
DWORD CategoryFactoryT<I,G,C>::getVersion(const CLSID &clsid)
{
	HKEY hKey;
	char szKeyBuf[1024];
	BSTR bCLSID;

	// Convert the clsid to a string.
	StringFromCLSID(clsid, &bCLSID);

	// Make the key.
	sprintf(szKeyBuf, "CLSID\\%s\\Version", (char*)_bstr_t(bCLSID));

	// Create and open key and subkey.
	long lResult = RegOpenKeyEx(	HKEY_CLASSES_ROOT,
									szKeyBuf, 
									0, 
									KEY_READ, 
									&hKey);
	if (lResult != ERROR_SUCCESS)
	{
		return 0;
	}

	// Set the Value.
	DWORD val = 0;
	DWORD dwSize = sizeof(val);
	lResult = RegQueryValueEx( hKey, NULL, NULL, NULL, (BYTE *)&val, &dwSize);

	RegCloseKey(hKey);

	::CoTaskMemFree(bCLSID);

	return val;
}

template <class I, const GUID* G, const CATID* C>
void CategoryFactoryT<I,G,C>::getFriendlyName(const CLSID& clsid, LPCTSTR szFriendlyName, int iLength)
{
	SysStatsUtils::CLSIDToName(clsid, szFriendlyName, iLength);
}

template <class I, const GUID* G, const CATID* C>
void CategoryFactoryT<I,G,C>::getDisplayName(const CLSID& clsid, LPCTSTR szDisplayName, int iLength)
{
	HKEY hKey;
	char szKeyBuf[1024];
	BSTR bCLSID;

	// Convert the clsid to a string.
	StringFromCLSID(clsid, &bCLSID);

	// Make the key.
	sprintf(szKeyBuf, "CLSID\\%s\\DisplayName", (char*)_bstr_t(bCLSID));

	// Create and open key and subkey.
	long lResult = RegOpenKeyEx(	HKEY_CLASSES_ROOT,
									szKeyBuf, 
									0, 
									KEY_READ, 
									&hKey);
	if (lResult != ERROR_SUCCESS)
	{
		getFriendlyName(clsid, szDisplayName, iLength);
		return;
	}

	// Set the Value.
	DWORD dwSize = iLength ;
	lResult = RegQueryValueEx( hKey, NULL, NULL, NULL, (BYTE *)szDisplayName, &dwSize);

	if (lResult != ERROR_SUCCESS)
	{
	}
	
	RegCloseKey(hKey);

	::CoTaskMemFree(bCLSID);

	return;
}