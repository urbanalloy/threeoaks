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

#pragma once

#include <windows.h>
#include <atlbase.h>
#include <atlconv.h>
#include <stdio.h>
#include <Rpc.h>
#include <RpcDce.h>
#include <string>
#include <vector>

#include <DockletSDK.h>
#include "InteropSDK/DockletSetImageManaged.h"

#pragma comment(lib, "comsuppw.lib")

#pragma warning (disable: 4100)
#pragma warning (disable: 4278)

// Import the common language runtime
#import <mscorlib.tlb> raw_interfaces_only

// Import Interfaces
#import "System.Drawing.tlb"
//#import "../../Release/NetDockletHelper.tlb" no_namespace named_guids
#include "InteropSDK/IDockletInterface.h"

// TODO Add log file
#define LOG(str)

///////////////////////////////////////////////////////////////////////////////////////////////
#define CREATE_INSTANCE(clsid, iid, ptr)  \
	CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, iid, reinterpret_cast<void**>(ptr));

///////////////////////////////////////////////////////////////////////////////////////////////

#define DOCKLET_NAME ".Net Docklet"
#define DOCKLET_AUTHOR "Julien Templier"
#define DOCKLET_NOTES "The .Net Docklet let you choose the .Net based docklet you want to use."

#define DOCKLET_TITLE "Click to choose the .Net Docklet to load..."
#define DOCKLET_INVALID_TITLE "Invalid .Net Docklet..."
#define DOCKLET_INVALID_MESSAGE "This Docklet is not a valid .Net Docklet !"

///////////////////////////////////////////////////////////////////////////////////////////////

#define DOCKLET_INI	"config.ini"
#define DOCKLET_ICON "Interop.png"
#define DOCKLET_DLL_INI "docklet.ini"

#define DOCKLET_INI_SECTION "main"
#define DOCKLET_INI_AUTOLOAD "Autoload"
#define DOCKLET_INI_DOCKLET "NetDocklet"

#define DOCKLET_INI_DLL_SECTION "docklet"
#define DOCKLET_INI_NAME "Name"
#define DOCKLET_INI_ICON "Icon"
#define DOCKLET_INI_GUID "GUID"
#define DOCKLET_INI_DLL  "DLL"

#define GUID_SIZE 37
#define FOLDER_SIZE MAX_PATH+1	// used for char[] creation & malloc

///////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
	char name[FOLDER_SIZE];
	char icon[FOLDER_SIZE];
	char path[FOLDER_SIZE];
	GUID CLSID_Docklet;
	BOOL invalid;
} DOCKLET_LIST;

// Docklet 'Data' struct.
typedef struct {
	// Interop
	HWND hwndDocklet;
	HINSTANCE hInstanceDll;
	DOCKLET_STATIC_DATA static_data;
	BOOL autoload;

	SAFEARRAY* files;
	int filesSize;

	// .Net Docklet
	std::vector< DOCKLET_LIST > docklets;	// List of available docklets
	int index; // index of the selected docklet in docklets.
	IDockletInterface *cpi;
} DOCKLET_DATA;