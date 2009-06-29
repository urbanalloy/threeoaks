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

#include "CSingleInstance.h"
#include <CrtDbg.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instance Data
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Class which is used as a static to ensure that we
// only close the file mapping at the very last chance
class INSTANCE_DATA
{
public:
	// Constructors / Destructors
	INSTANCE_DATA();
	~INSTANCE_DATA();

protected:
	// Member variables
	HANDLE hInstanceData;

	friend class CSingleInstance;
};

INSTANCE_DATA::INSTANCE_DATA() : hInstanceData(NULL)
{
}

INSTANCE_DATA::~INSTANCE_DATA()
{
	if (hInstanceData != NULL)
	{
		CloseHandle(hInstanceData);
		hInstanceData = NULL;
	}
}

static INSTANCE_DATA g_instanceData;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructors / Destructors
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSingleInstance::CSingleInstance(const string& name)
{
	// Init vars
	char buffer[MAX_PATH];
	m_name = string(CreateUniqueName(name.c_str(), (char*)&buffer, SI_SESSION_UNIQUE));
	m_mmfilename = name + "_MMF";
	m_isFirstInstance = false;

	// Only one object of type CSingleInstance should be created
	_ASSERT(g_instanceData.hInstanceData == NULL);

	m_hInstanceDataMutex = CreateMutex(NULL, false, "CSingleInstanceDataMutex");
	m_pExecuteLock = NULL;
}

CSingleInstance::~CSingleInstance()
{
	CloseHandle(m_hInstanceDataMutex);
	SAFE_DELETE(m_pExecuteLock);

	g_instanceData.~INSTANCE_DATA();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// First Instance
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CSingleInstance::ActivateInstance()
{
	_ASSERT(m_pExecuteLock == NULL);

	// Ensure there is only ever one CSingleInstance instance 
	// active at any one time throughout the system
	m_pExecuteLock = new CComCriticalSection();
	m_pExecuteLock->Lock();
}

bool CSingleInstance::IsFirstInstance()
{
	// We have been initialized as the first instance
	if (m_isFirstInstance)
		return true;

	// Try opening the MMF
	HANDLE hMMF = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, m_mmfilename.c_str());

	// We are the first instance
	if (hMMF == NULL)
		return true;

	CloseHandle(hMMF);

	return false;
}

bool CSingleInstance::CreateFirstInstanceData(HWND hwnd)
{
	// Check if we are the first instance
	if (!IsFirstInstance())
		return false;

	// Set us as first instance for later calls of IsFirstInstance
	m_isFirstInstance = true;

	// Create our memory mapped file
	int size = sizeof(SingleInstanceData);
	g_instanceData.hInstanceData = CreateFileMapping(INVALID_HANDLE_VALUE,
													 NULL,
													 PAGE_READWRITE,
													 0,
													 size,
													 m_mmfilename.c_str());

	if (g_instanceData.hInstanceData == NULL)
		return false;

	// Open the memory mapped file
	SingleInstanceData* pInstanceData = static_cast<SingleInstanceData*>(MapViewOfFile(g_instanceData.hInstanceData,
																					   FILE_MAP_READ | FILE_MAP_WRITE,
																					   0,
																					   0,
																					   size));
	// Opening the MMF should work
	_ASSERT(pInstanceData != NULL);   

	// Lock the data prior to updating it
	DWORD dwWaitMutex = WaitForSingleObject(m_hInstanceDataMutex, INFINITE);	
	switch (dwWaitMutex) 
	{
		case WAIT_OBJECT_0: 			
			pInstanceData->hwnd = hwnd;
			UnmapViewOfFile(pInstanceData);

			ReleaseMutex(m_hInstanceDataMutex);
			break;

		default:
			break;
	}

	// Since this will be the last function that will be called 
	// when this is the first instance we can release the lock
	m_pExecuteLock->Unlock();
	SAFE_DELETE(m_pExecuteLock);

	return true;
}


bool CSingleInstance::NotifyFirstInstance(HWND hwndSender, LPCTSTR commandLine)
{
	DWORD dwTimeout = 30000;
	bool success = true;

	// Try to open the previous instances memory mapped file
	HANDLE hPrevInstance = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, m_mmfilename.c_str());

	// Failed to open previous instance!
	if (!hPrevInstance)
		return false;

	int size = sizeof(SingleInstanceData);
	SingleInstanceData* pInstanceData = static_cast<SingleInstanceData*>(MapViewOfFile(hPrevInstance,
																		 FILE_MAP_READ | FILE_MAP_WRITE,
																		 0,
																		 0,
																		 size));

	if (!pInstanceData)
		return false;

	// Lock the data prior to reading from it
	DWORD dwWaitMutex = WaitForSingleObject(m_hInstanceDataMutex, INFINITE);	
	switch (dwWaitMutex) 
	{
		case WAIT_OBJECT_0: 
		{
			_ASSERT(pInstanceData->hwnd); // Something gone wrong with the MMF

			// Get the first instance hwnd
			HWND hWindow = pInstanceData->hwnd;
			if (hWindow) {

				// Restore the focus to the previous instance and bring it to the foreground
				ShowWindow(hWindow, SW_RESTORE);
				SetForegroundWindow(hWindow);

				if (commandLine)
				{  
					// Send the current instance command line to the previous instance using WM_COPYDATA
					COPYDATASTRUCT cds;
					cds.dwData = NULL;
					DWORD dwCmdLength = static_cast<DWORD>(_tcslen(commandLine) + 1);
					cds.cbData = dwCmdLength * sizeof(TCHAR); 

					// We use a local buffer so that we can specify a constant parameter to this function
					TCHAR* pszLocalCmdLine = new TCHAR[dwCmdLength]; 
					_tcscpy_s(pszLocalCmdLine, dwCmdLength, commandLine);
					cds.lpData = pszLocalCmdLine;

					// Send the message to the previous instance. Use SendMessageTimeout instead of SendMessage to ensure we 
					// do not hang if the previous instance itself is hung
					DWORD_PTR dwResult = 0;
					if (SendMessageTimeout(hWindow,
										   WM_COPYDATA,
										   reinterpret_cast<WPARAM>(hwndSender),
										   reinterpret_cast<LPARAM>(&cds),
										   SMTO_ABORTIFHUNG,
										   dwTimeout,
										   &dwResult) == 0)
					{
						//Previous instance is not responding to messages
						success = false;
					}

					//Tidy up the heap memory we have used
					delete [] pszLocalCmdLine;
				}
			}

			// Unmap the MMF we were using
			UnmapViewOfFile(pInstanceData);

			ReleaseMutex(m_hInstanceDataMutex);
			break;
		}

		default:
			break;
	}

	// When we have activated the previous instance, we can release the lock
	if (success) {
		m_pExecuteLock->Unlock();
		SAFE_DELETE(m_pExecuteLock);
	}

	return success;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* CSingleInstance::CreateUniqueName(const char* pszGUID, char* pszBuffer, int nMode /* = SI_DESKTOP_UNIQUE */)
{
	if(pszBuffer == NULL) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	// First copy GUID to destination buffer
	if(pszGUID)
		_tcscpy_s(pszBuffer, MAX_PATH, pszGUID);
	else
		*pszBuffer = 0;

	// Name should be desktop unique, so add current desktop name
	if(nMode & SI_DESKTOP_UNIQUE) {		
		_tcscat_s(pszBuffer, MAX_PATH, _T("-"));
		HDESK hDesk		= GetThreadDesktop(GetCurrentThreadId());
		ULONG cchDesk	= MAX_PATH - _tcslen(pszBuffer) - 1;

		if(!GetUserObjectInformation(hDesk, UOI_NAME, pszBuffer + _tcslen(pszBuffer), cchDesk, &cchDesk))
			// Call will fail on Win9x
			_tcsncat_s(pszBuffer, MAX_PATH, _T("Win9x"), cchDesk);
	}

	// Name should be session unique, so add current session id
	if(nMode & SI_SESSION_UNIQUE) {
		
		HANDLE hToken = NULL;

		// Try to open the token (fails on Win9x) and check necessary buffer size
		if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken) 
		  && (MAX_PATH - _tcslen(pszBuffer) > 9 )) 
		{
			DWORD cbBytes = 0;

			if(!GetTokenInformation(hToken, TokenStatistics, NULL, cbBytes, &cbBytes) 
			   && GetLastError() == ERROR_INSUFFICIENT_BUFFER )
			{
				PTOKEN_STATISTICS pTS = (PTOKEN_STATISTICS) _alloca( cbBytes );
				if(GetTokenInformation(hToken, TokenStatistics, (LPVOID)pTS, cbBytes, &cbBytes)) {
					wsprintf(pszBuffer + _tcslen(pszBuffer), 
							 _T("-%08x%08x"), 
							 pTS->AuthenticationId.HighPart, 
							 pTS->AuthenticationId.LowPart);
				}
			}
		}
	}

	// Name should be unique to the current user
	if(nMode & SI_TRUSTEE_UNIQUE) {
		
		TCHAR szUser[64] = {0};
		TCHAR szDomain[64] = {0};
		DWORD cchUser	= 64;
		DWORD cchDomain	= 64;

		if(GetUserName(szUser, &cchUser)) {
			// Since NetApi() calls are quite time consuming
			// we retrieve the domain name from an environment variable
			cchDomain = GetEnvironmentVariable(_T("USERDOMAIN"), szDomain, cchDomain);

			UINT cchUsed = _tcslen(pszBuffer);
			if(MAX_PATH - cchUsed > cchUser + cchDomain + 3) {
				wsprintf(pszBuffer + cchUsed,
						 _T("-%s-%s"),
						 szDomain,
						 szUser);
			}
		}
	}

	return pszBuffer;
}