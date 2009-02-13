///////////////////////////////////////////////////////////////////////////////////////////////
//
// DotNetDocklets : bringing .NET to ObjectDock
//
// Copyright (c) 2004-2009, Julien Templier
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

#include "Utils.h"
#include "Register.h"

///////////////////////////////////////////////////////////////////////////////////////////////
void PopulateStaticData(DOCKLET_DATA *data) {

	char root[FOLDER_SIZE];
	strcpy_s(root, FOLDER_SIZE, "");
	DockletGetRootFolder(data->hwndDocklet, root);

	char interop[FOLDER_SIZE];
	strcpy_s(interop, FOLDER_SIZE, "");
	DockletGetRelativeFolder(data->hwndDocklet,interop);

	// Allocate memory
	data->static_data.rootFolder = (LPSTR)malloc((FOLDER_SIZE)*sizeof(char));
	data->static_data.interopFolder = (LPSTR)malloc((FOLDER_SIZE)*sizeof(char));
	data->static_data.relativeFolder = (LPSTR)malloc((FOLDER_SIZE)*sizeof(char));

	strcpy_s(data->static_data.rootFolder, (FOLDER_SIZE)*sizeof(char), root);
	strcpy_s(data->static_data.interopFolder, (FOLDER_SIZE)*sizeof(char), interop);
	strcpy_s(data->static_data.relativeFolder, (FOLDER_SIZE)*sizeof(char), "");
	// relativeFolder needs to be set on plugin load
}

///////////////////////////////////////////////////////////////////////////////////////////////
void PopulateStaticData_Relative(DOCKLET_DATA *data) {
	sprintf(data->static_data.relativeFolder, "%s%s\\",
											  data->static_data.interopFolder,
											  data->docklets[data->index].path);
}

///////////////////////////////////////////////////////////////////////////////////////////////
void PopulateDockletList(DOCKLET_DATA *data)
{
	LOG("Populating Docklet list...\n");

	// Clean the data if already existing
	data->docklets.clear();

	// Check for each docklet ini file
	WIN32_FIND_DATA FindFileData;
	char docklet_path[FOLDER_SIZE];

	LOG("  Adding:\n");
	char search[FOLDER_SIZE];
	sprintf(search, "%s%s\\*", data->static_data.rootFolder, data->static_data.interopFolder);

	HANDLE hFind = FindFirstFile(search, &FindFileData);
	while (hFind != INVALID_HANDLE_VALUE && (GetLastError() != ERROR_NO_MORE_FILES) ) {

		// For each directory
		if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
			sprintf(docklet_path, "%s%s%s\\%s", data->static_data.rootFolder,
												data->static_data.interopFolder,
												FindFileData.cFileName,
												DOCKLET_DLL_INI);

			WIN32_FIND_DATA FindIniData;
			HANDLE hFindIni = FindFirstFile(docklet_path, &FindIniData);
			if (hFindIni != INVALID_HANDLE_VALUE)
			{
				char name[FOLDER_SIZE];
				strcpy(name, "");
				GetPrivateProfileString(DOCKLET_INI_DLL_SECTION, DOCKLET_INI_NAME, "", (LPSTR)&name, FOLDER_SIZE, docklet_path);

				char icon[FOLDER_SIZE];
				strcpy(icon, "");
				GetPrivateProfileString(DOCKLET_INI_DLL_SECTION, DOCKLET_INI_ICON, "", (LPSTR)&icon, FOLDER_SIZE, docklet_path);				

				char dll[GUID_SIZE];
				strcpy(dll, "");
				GetPrivateProfileString(DOCKLET_INI_DLL_SECTION, DOCKLET_INI_DLL, "", (LPSTR)&dll, FOLDER_SIZE, docklet_path);

				char guid[GUID_SIZE];
				strcpy(guid, "");
				GetPrivateProfileString(DOCKLET_INI_DLL_SECTION, DOCKLET_INI_GUID, "", (LPSTR)&guid, GUID_SIZE, docklet_path);

				// Add to list if GUID is valid
				GUID CLSID_Docklet;
				if (UuidFromString((unsigned char *)guid, &CLSID_Docklet) != RPC_S_INVALID_STRING_UUID) {
					DOCKLET_LIST dlist;
					strcpy(dlist.name, name);
					strcpy(dlist.icon, icon);
					strcpy(dlist.path, FindFileData.cFileName);
					dlist.CLSID_Docklet = CLSID_Docklet;

					// Try Registering the docklet
					dlist.invalid = false;
					char regpath[FOLDER_SIZE];
					sprintf(regpath, "%s%s%s/%s", data->static_data.rootFolder, data->static_data.interopFolder, dlist.path, dll);
					char interop_path[FOLDER_SIZE];
					sprintf(interop_path, "%s%s", data->static_data.rootFolder, data->static_data.interopFolder);
					if (!RegisterDocklet(regpath, interop_path))
						dlist.invalid = true;

					data->docklets.push_back(dlist);

					LOG("     "); LOG((char *)name);
				}
			}
			FindClose(hFindIni);
		}

		FindNextFile(hFind, &FindFileData);
	}
	FindClose(hFind);


	LOG("\nFinished Populating Docklet list.\n\n");
}