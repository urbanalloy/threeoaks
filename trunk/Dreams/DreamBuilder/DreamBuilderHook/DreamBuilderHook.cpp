///////////////////////////////////////////////////////////////////////////////////////////////
//
// DreamBuilder
//
// Copyright (c) 2008, Three Oaks Crossing
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
//	1. Redistributions of source code must retain the above copyright notice, this list of
//		 conditions and the following disclaimer. 
//	2. Redistributions in binary form must reproduce the above copyright notice, this list
//		 of conditions and the following disclaimer in the documentation and/or other materials
//		 provided with the distribution. 
//	3. The name of the author may not be used to endorse or promote products derived from this
//		 software without specific prior written permission. 
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//	COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//	POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DreamBuilderHook.h"

TMessageBoxFuncPtr p_fnMessageBoxOrg = NULL;
WCHAR message[MAX_SIZE];

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}

    return TRUE;
}

BOOL BuildDream(LPCWSTR dreamDll, LPCWSTR output, LPCWSTR name, LPCWSTR description, LPCWSTR contents, LPCWSTR preview, LPCWSTR author, LPCWSTR url, LPCWSTR copyright, LPCWSTR permissions)
{		
	// Init the buffer to store messages
	lstrcpyW((LPWSTR)&message, L"");

	// Load the DreamMaker dll & get a pointer to the BuildDream_Direct1 function
	HMODULE hInstance = LoadLibrary(dreamDll);
	BuildDreamFile_Direct1FuncPtr buildDream = (BuildDreamFile_Direct1FuncPtr) GetProcAddress(hInstance, "BuildDreamFile_Direct1");

	// Intercept MessageBox calls
	InterceptDllCall(hInstance, L"user32.dll", "MessageBoxW", (PVOID)&MyMessageBox, (PVOID*)&p_fnMessageBoxOrg, NULL);

	BOOL ret = (*buildDream)(output, name, description, contents, preview, author, url, copyright, permissions);
	
	FreeLibrary(hInstance);
	
	return ret;
}

int GetLastMessage(LPCWSTR buffer, int length)
{		
	// if buffer is NULL, we return the size needed to hold the data
	if (buffer == NULL)
		return MAX_SIZE;

	int size = MAX_SIZE;
	if (length < MAX_SIZE)
		size = length;

	lstrcpynW((LPWSTR)buffer, (LPCWSTR)&message, size*sizeof(WCHAR));

	return size;
}

int WINAPI MyMessageBox(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
	lstrcpynW((LPWSTR)&message, lpText, MAX_SIZE*sizeof(WCHAR));

	return 1;
}

//////////////////////////////////////////////////////////////////////////
BOOL InterceptDllCall(HMODULE hLocalModule,
					  LPCWSTR c_szDllName,
					  const char* c_szApiName,
					  PVOID pApiNew,
					  PVOID* p_pApiOrg,
					  PVOID pApiToChange)
{
    PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hLocalModule;
    PIMAGE_NT_HEADERS pNTHeader;
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
    DWORD dwProtect;
	BOOL bSuccess = FALSE;
    
    DWORD dwAddressToIntercept; 

	if (pApiToChange)
		dwAddressToIntercept = (DWORD)pApiToChange;
	else
		dwAddressToIntercept = (DWORD)GetProcAddress(GetModuleHandle(c_szDllName), (char*)c_szApiName);

    if (IsBadReadPtr(hLocalModule, sizeof(PIMAGE_NT_HEADERS)))
        return FALSE;
    
    if (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE)
        return FALSE;
    
    pNTHeader = MakePtr(PIMAGE_NT_HEADERS, pDOSHeader, pDOSHeader->e_lfanew);
    if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
        return FALSE;
    
    pImportDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR, hLocalModule, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    
    if (pImportDesc == (PIMAGE_IMPORT_DESCRIPTOR)pNTHeader) 
		
		return FALSE;
    
	while (pImportDesc->Name)
	{
		PIMAGE_THUNK_DATA pThunk;
    
		pThunk = MakePtr(PIMAGE_THUNK_DATA, hLocalModule, pImportDesc->FirstThunk);
    
		while (pThunk->u1.Function)
		{
			if ((DWORD)pThunk->u1.Function == dwAddressToIntercept)
			{	
				if (!IsBadWritePtr((PVOID)&pThunk->u1.Function, sizeof(PVOID)))
				{
					if (p_pApiOrg) 
						*p_pApiOrg = (PVOID)pThunk->u1.Function;

					pThunk->u1.Function = (DWORD)pApiNew;
					bSuccess = TRUE;
				}
				else
				{
					if (VirtualProtect((PVOID)&pThunk->u1.Function, sizeof(PVOID), PAGE_EXECUTE_READWRITE, &dwProtect))
					{
						DWORD dwNewProtect;

						if (p_pApiOrg) 
							*p_pApiOrg = (PVOID)pThunk->u1.Function;

						pThunk->u1.Function = (DWORD)pApiNew;
						bSuccess = TRUE;

						dwNewProtect = dwProtect;

						VirtualProtect((PVOID)&pThunk->u1.Function, sizeof(PVOID), dwNewProtect, &dwProtect);
					}
				} 
			}
			pThunk++;
		}
		pImportDesc++;
	}

    return bSuccess;
}
