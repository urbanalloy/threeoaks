/*
 * SysStats Widget Framework
 * Copyright (C) 2002-2006 Paul Andrews
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#if defined(_DEBUG)
#define AFX_CDECL __cdecl


// Memory tracking allocation
void* AFX_CDECL operator new(size_t nSize, const char* lpszFileName, int nLine);
#define DEBUG_NEW new(THIS_FILE, __LINE__)
#if _MSC_VER >= 1200
void AFX_CDECL operator delete(void* p, const char* lpszFileName, int nLine);
#endif

#endif // _DEBUG


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <comdef.h>
#include <Winerror.h>
#include <atlbase.h>

#include "SysStatsUtils.h"

#ifdef _DEBUG
//    FUNCTION: EnumLangsFunc(HANDLE, LPSTR, LPSTR, WORD, LONG) 
// 
//    PURPOSE:  Resource language callback 
static BOOL EnumLangsFunc( 
    HINSTANCE hModule,  // module handle 
    LPCTSTR lpType,  // address of resource type 
    LPCTSTR lpName,  // address of resource name 
    WORD wLang,      // resource language 
    LONG lParam)     // extra parameter, could be 
                     // used for error checking 
{ 
    HRSRC hResInfo; 
 
    hResInfo = FindResourceEx(hModule, lpType, lpName, wLang); 
    // Write the resource language to the resource information file.
	AtlTrace("\t\tLanguage: %u\n", (USHORT) wLang);
    AtlTrace("\t\thResInfo == %lx,  Size == %lu\n\n", 
        hResInfo, 
        SizeofResource(hModule, hResInfo)); 

    return TRUE; 
}

//    FUNCTION: EnumNamesFunc(HANDLE, LPSTR, LPSTR, LONG) 
// 
//    PURPOSE:  Resource name callback 
static BOOL EnumNamesFunc( 
    HINSTANCE hModule,   // module handle 
    LPCTSTR lpType,   // address of resource type 
    LPTSTR lpName,    // address of resource name 
    LONG lParam)      // extra parameter, could be 
                      // used for error checking 
{ 
     // Write the resource name to a resource information file. 
     // The name may be a string or an unsigned decimal 
     // integer, so test before printing. 
    if ((ULONG)lpName & 0xFFFF0000) 
    {
		AtlTrace("\tName: %s\n", lpName);
    } 
    else 
    {
		AtlTrace("\tName: %u\n", (USHORT)lpName);
    }

    EnumResourceLanguages(hModule, 
        lpType, 
        lpName, 
        (ENUMRESLANGPROC)EnumLangsFunc, 
        0); 
 
    return TRUE; 
} 
 
//    FUNCTION: EnumTypesFunc(HANDLE, LPSTR, LONG) 
// 
//    PURPOSE:  Resource type callback 
static BOOL EnumTypesFunc( 
    HINSTANCE hModule,   // module handle 
    LPTSTR lpType,    // address of resource type 
    LONG lParam)      // extra parameter, could be 
                      // used for error checking 
{ 
    // Write the resource type to a resource information file. 
    // The type may be a string or an unsigned decimal 
    // integer, so test before printing. 
    if ((ULONG)lpType & 0xFFFF0000) 
    { 
		AtlTrace("Type: %s\n", lpType);
    } 
    else 
    {
		AtlTrace("Type: %u\n", (USHORT)lpType);
    } 

    EnumResourceNames(hModule, 
        lpType, 
        (ENUMRESNAMEPROC)EnumNamesFunc, 
        0); 
 
    return TRUE; 
} 
#endif
 
void SysStatsUtils::DumpResources(HINSTANCE hModule)
{
#ifdef _DEBUG
	EnumResourceTypes(hModule,              // module handle 
		(ENUMRESTYPEPROC)EnumTypesFunc,  // callback function 
		0);                             // extra parameter
#endif
}

void SysStatsUtils::SetLFHeaps()
{
	int numHeaps = ::GetProcessHeaps(0, NULL);
	HANDLE *pHandles = (HANDLE*)::malloc(numHeaps * sizeof(HANDLE));
	int newNumHeaps = ::GetProcessHeaps(numHeaps, pHandles);
	ULONG  HeapFragValue = 2;

	if (newNumHeaps == numHeaps)
	{
		for (int i=0; i < numHeaps; i++)
		{
			if (::HeapSetInformation(pHandles[i], HeapCompatibilityInformation, &HeapFragValue, sizeof(HeapFragValue)) == 0)
				SysStatsUtils::checkError();
		}
	}
	else
	{
		AtlTrace("Failed to compact heaps\n");
	}

	::free(pHandles);
}

void SysStatsUtils::CompactHeaps()
{
	int numHeaps = ::GetProcessHeaps(0, NULL);
	HANDLE *pHandles = (HANDLE*)::malloc(numHeaps * sizeof(HANDLE));
	int newNumHeaps = ::GetProcessHeaps(numHeaps, pHandles);

	if (newNumHeaps == numHeaps)
	{
		for (int i=0; i < numHeaps; i++)
		{
			if (::HeapCompact(pHandles[i], 0) == 0)
				SysStatsUtils::checkError();
		}
	}
	else
	{
		AtlTrace("Failed to compact heaps\n");
	}

	::free(pHandles);
}

void SysStatsUtils::checkError()
{
#ifdef _DEBUG
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();
	if (!FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL ))
	{
	   // Handle the error.
	   return;
	}

	// Process any inserts in lpMsgBuf.
	// ...

	// Display the string.
	AtlTrace("%s\n", lpMsgBuf);
//	MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

	// Free the buffer.
	LocalFree( lpMsgBuf );
#endif
}

HRESULT SysStatsUtils::getUniqueFilename(const char *root, const char *postfix, _bstr_t *pRet)
{
	char out[MAX_PATH];
	strncpy(out, root, MAX_PATH);
	int offset = strlen(out);
	struct _stat buf;
	int res;
	int i = 0;
	// Create a unique name
	do
	{
		sprintf(out+offset, "%d%s", i++, postfix);
		res = _stat(out, &buf);
	} while (res != -1);

	*pRet = out;

	return S_OK;
}

bool SysStatsUtils::createFilePath(const char *pathToCreate)
{
	char path[MAX_PATH];
	char *pos;

	// Copy path because we have to modify it
	strcpy(path, pathToCreate);

	// From the first slash to the last check and create path
	// Exception : Jump initial duble slashed UNC host name and include first directory
	if (path[0]=='\\' && path[1]=='\\')
	{
		// Find slash ending the host name
		pos = strchr(path+2,'\\');

		// Find slash after the first directory (after the host name)
		if (pos)
			pos = strchr(pos+1,'\\');
	}
	else
		pos = strchr(path,'\\');

	bool ok = true;
	while (pos && ok)
	{
		// Cut the string
		*pos = 0;

		// Check if exist this piece of path
		if (GetFileAttributes(path)==-1)
			if (GetLastError()==ERROR_FILE_NOT_FOUND)
				ok = CreateDirectory(path,NULL)==TRUE;
			else
				ok = false;

		// Recover the character deleted
		*pos = '\\';

		// Find next slash
		pos = strchr(pos+1,'\\');
	}

	// Return true if finally the path was created or exist
	// (if not was created see LastError)
	return ok;
}

long SysStatsUtils::VariantToInteger(VARIANT *v)
{
	long ret = -1;
	VariantToInteger(v, &ret);

	return ret;
}

HRESULT SysStatsUtils::VariantToInteger(VARIANT *v, long *pRet)
{
	switch (V_VT(v))
	{
	case VT_BOOL : *pRet = V_BOOL(v); return S_OK;
	case VT_I1 : *pRet = V_I1(v); return S_OK;
	case VT_I2 : *pRet = V_I2(v); return S_OK;
	case VT_I4 : *pRet = V_I4(v); return S_OK;
	case VT_UI1 : *pRet = V_UI1(v); return S_OK;
	case VT_UI2 : *pRet = V_UI2(v); return S_OK;
	case VT_UI4 : *pRet = V_UI4(v); return S_OK;
	case VT_INT : *pRet = V_INT(v); return S_OK;
	case VT_UINT: *pRet = V_UINT(v); return S_OK;
	case VT_VARIANT : return VariantToInteger(v->pvarVal, pRet);
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT SysStatsUtils::VariantToDouble(VARIANT *v, double *pRet)
{
	switch (V_VT(v))
	{
	case VT_R4 : *pRet = V_R4(v); return S_OK;
	case VT_R8 : *pRet = V_R8(v); return S_OK;
	case VT_VARIANT : return VariantToDouble(v->pvarVal, pRet);
	}

	return E_INVALIDARG;
}

HRESULT SysStatsUtils::VariantToBSTR(VARIANT *v, BSTR *pRet)
{
	switch (V_VT(v))
	{
	case 	VT_BSTR: *pRet = V_BSTR(v); return S_OK;
	case VT_VARIANT : return VariantToBSTR(v->pvarVal, pRet);
	}

	return E_INVALIDARG;
}

HRESULT SysStatsUtils::VariantToString(VARIANT *v, char *out, int len)
{
	long lVal;
	HRESULT hr = VariantToInteger(v, &lVal);
	if (SUCCEEDED(hr))
	{
		_snprintf(out, len, "%ld", lVal);
		return S_OK;
	}

	double d;
	hr = VariantToDouble(v, &d);
	if (SUCCEEDED(hr))
	{
		_snprintf(out, len, "%.20g", d);
		return S_OK;
	}

	BSTR bStr;
	hr = VariantToBSTR(v, &bStr);
	if (SUCCEEDED(hr))
	{
		_snprintf(out, len, "%s", (char*)_bstr_t(bStr));
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT SysStatsUtils::StringToVariant(char *in, VARTYPE vt, VARIANT *pRet)
{
	::VariantInit(pRet);
	pRet->vt = vt;
	char *pEnd = in;
	long l = ::strtol(in, &pEnd, 10);

	if (strstr(in, "0x") == in)
		l = ::strtol(in, &pEnd, 16);

	double d = atof(in);

	switch (vt)
	{
	case VT_BOOL: pRet->boolVal = l; return S_OK;
	case VT_I1  : pRet->cVal = l; return S_OK;
	case VT_I2  : pRet->iVal = l; return S_OK;
	case VT_I4  : pRet->lVal = l; return S_OK;
	case VT_INT : pRet->intVal = l; return S_OK;
	case VT_UI1 : pRet->bVal = l; return S_OK;
	case VT_UI2 : pRet->uiVal = l; return S_OK;
	case VT_UI4 : pRet->ulVal = l; return S_OK;
	case VT_UINT: pRet->uintVal = l; return S_OK;
	case VT_R4  : pRet->fltVal = d; return S_OK;
	case VT_R8  : pRet->dblVal = d; return S_OK;
	case VT_BSTR: pRet->bstrVal = _com_util::ConvertStringToBSTR(in); return S_OK;
	case VT_VARIANT :
		{
			pRet->vt |= VT_BYREF;

			VARIANT *pVar = new VARIANT;
			::VariantInit(pVar);
			if (in != pEnd)
			{
				pVar->vt = VT_INT;
				pVar->intVal = l;
				pRet->pvarVal = pVar;
			}
			else
			{
				pVar->vt = VT_BSTR;
				pVar->bstrVal = _com_util::ConvertStringToBSTR(in);
				pRet->pvarVal = pVar;
			}
		}
		return S_OK;
	}

	pRet->vt = VT_EMPTY;

	return E_INVALIDARG;
}

HRESULT SysStatsUtils::FreeVariant(const VARIANT &var)
{
	switch (var.vt)
	{
	case VT_BSTR: ::SysFreeString(var.bstrVal); return S_OK;
	case VT_DISPATCH: var.pdispVal->Release();
	case VT_SAFEARRAY: ::SafeArrayDestroy(var.parray); return S_OK;
	default:
		if (var.vt & VT_ARRAY)
		{
			::SafeArrayDestroy(var.parray);
		}
		return S_OK;;
	}

	return E_INVALIDARG;
}

void SysStatsUtils::CLSIDToName(const CLSID& clsid, LPCTSTR szFriendlyName, int iLength)
{
	HKEY hKey;
	char szKeyBuf[1024];
	BSTR bCLSID;

	// Convert the clsid to a string.
	StringFromCLSID(clsid, &bCLSID);

	// Make the key.
	sprintf(szKeyBuf, "CLSID\\%s", (char*)_bstr_t(bCLSID));

	// Create and open key and subkey.
	long lResult = RegOpenKeyEx(	HKEY_CLASSES_ROOT,
									szKeyBuf, 
									0, 
									KEY_READ, 
									&hKey);
	if (lResult != ERROR_SUCCESS)
	{
		return;
	}

	// Set the Value.
	DWORD dwSize = iLength ;
	lResult = RegQueryValueEx( hKey, NULL, NULL, NULL, (BYTE *)szFriendlyName, &dwSize);

	if (lResult != ERROR_SUCCESS)
	{
	}
	
	RegCloseKey(hKey);

	::CoTaskMemFree(bCLSID);

	return;
}

static _bstr_t ampersand = L"&amp;";
static _bstr_t lt = L"&lt;";
static _bstr_t gt = L"&gt;";

void SysStatsUtils::EncodeXMLString(const char *in, _bstr_t *appendTo)
{
	int len = strlen(in);
	char *copy = strdup(in);
	const char *fragment = copy;

	for (int i=0; i<len; i++)
	{
		switch (copy[i])
		{
		case '&': copy[i] = 0; (*appendTo) += fragment; (*appendTo) += ampersand; fragment = &copy[i+1]; break;
		case '<': copy[i] = 0; (*appendTo) += fragment; (*appendTo) += lt; fragment = &copy[i+1]; break;
		case '>': copy[i] = 0; (*appendTo) += fragment; (*appendTo) += gt; fragment = &copy[i+1]; break;
		}
	}

	(*appendTo) += fragment;

	free(copy);
}
