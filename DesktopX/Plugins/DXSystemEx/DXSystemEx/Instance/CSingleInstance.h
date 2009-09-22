///////////////////////////////////////////////////////////////////////////////////////////////
//
// CSingleInstance
//
// Copyright (c) 1996 - 2008 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)
//
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
// 
// Copyright / Usage Details:
//
// You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
// when your product is released in binary form. You are allowed to modify the source code in any way you want 
// except you cannot modify the copyright details at the top of each module. If you want to distribute source 
// code with your application, then you are only allowed to distribute versions released by the author. This is 
// to maintain a single distribution point for the source code. 
//
///////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"

#include <string>
using namespace std;

const int SI_SESSION_UNIQUE		= 0x0001;	// Allow only one instance per login session
const int SI_DESKTOP_UNIQUE		= 0x0002;	// Allow only one instance on current desktop
const int SI_TRUSTEE_UNIQUE		= 0x0004;	// Allow only one instance for current user
const int SI_SYSTEM_UNIQUE		= 0x0000;	// Allow only one instance at all (on the whole system)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instance Data
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Class which is used as a static to ensure that we
// only close the file mapping at the very last chance
class INSTANCE_DATA
{
public:
	// Constructors / Destructors
	INSTANCE_DATA() : hInstanceData(NULL)
	{

	}
	~INSTANCE_DATA()
	{
		if (hInstanceData != NULL)
		{
			CloseHandle(hInstanceData);
			hInstanceData = NULL;
		}
	}

protected:
	// Member variables
	HANDLE hInstanceData;

	friend class CSingleInstance;
};



class CSingleInstance
{

private:
	// Mutexes & Lock	
	DWORD  m_mutexWait;
	HANDLE m_hExecuteLock;
	HANDLE m_hInstanceDataMutex;
	
	// Data
	string m_name;	// used for the memory mapped file & mutexes
	string m_mmfilename;
	INSTANCE_DATA m_instanceData;

	bool m_isFirstInstance;
	void ReleaseLock();

	///////////////////////////////////////////////////////////////////////////////
	// LPTSTR CreateUniqueName( pszGUID, pszBuffer, nMode )
	//
	// Creates a "unique" name, where the meaning of "unique" depends on the nMode 
	// flag values. Returns pszBuffer
	//
	// pszGUID:		Copied to the beginning of pszBuffer, should be an GUID
	// pszBuffer:	Buffer for unique name. Length (in chars) must be >= MAX_PATH
	// nMode:		Information, that should be used to create the unique name.
	//				Can be one of the following values:
	//
	//
	//				SI_SESSION_UNIQUE						- Allow one instance per login session
	//				SI_DESKTOP_UNIQUE						- Allow one instance per desktop
	//				SI_TRUSTEE_UNIQUE						- Allow one instance per user account
	//				SI_SESSION_UNIQUE | SI_DESKTOP_UNIQUE	- Allow one instance per login session,
	//														  instances in different login sessions
	//														  must also reside on a different desktop
	//				SI_TRUSTEE_UNIQUE | SI_DESKTOP_UNIQUE	- Allow one instance per user account,
	//														  instances in login sessions running a
	//														  different user account must also reside	
	//														  on different desktops.
	//				SI_SYSTEM_UNIQUE						- Allow only one instance on the whole system	
	//
	char* CreateUniqueName(const char* pszGUID, char* pszBuffer, int nMode /* = SI_DESKTOP_UNIQUE */);

public:
	// Data struct
	struct SingleInstanceData
	{
		HWND hwnd;
	};

	// Constructor/Destructor
	CSingleInstance(const string& name);
	~CSingleInstance();

	// First Instance handling
	bool IsFirstInstance();
	void ActivateInstance();
	bool CreateFirstInstanceData(HWND hwnd);
	bool NotifyFirstInstance(HWND hwndSender, LPCTSTR commandLine);
};