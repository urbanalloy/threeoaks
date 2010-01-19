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

// MAPIMailCheck.cpp: implementation of the MAPIMailCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <time.h>
#include "MAPIMailCheck.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MAPIMailCheck::MAPIMailCheck() :
	name(""),
	password(""),
	m_hMapi(0), //Instance handle of the MAPI dll
	m_hSession(0), //Mapi Session handle
	m_nLastError(0), //Last Mapi error value
	m_lpfnMAPILogon(0), //MAPILogon function pointer
	m_lpfnMAPILogoff(0), //MAPILogoff function pointer
	m_lpfnMAPISendMail(0), //MAPISendMail function pointer
	m_lpfnMAPIResolveName(0), //MAPIResolveName function pointer
	m_lpfnMAPIFreeBuffer(0), //MAPIFreeBuffer function pointer
	m_lpfnMAPIReadMail(0), //MAPIReadMail function pointer
	m_lpfnMAPIFindNext(0),  //MAPIFindNext function pointer
	validDays(0)
{
}

MAPIMailCheck::~MAPIMailCheck()
{
	Destroy();

	if (m_hMapi)
	{
		//Unload the MAPI dll and reset the function pointers to NULL
		FreeLibrary(m_hMapi);
		m_hMapi = NULL;
		m_lpfnMAPILogon = NULL;
		m_lpfnMAPILogoff = NULL;
		m_lpfnMAPISendMail = NULL;
		m_lpfnMAPIResolveName = NULL;
		m_lpfnMAPIFreeBuffer = NULL;
		m_lpfnMAPIFindNext = NULL;
		m_lpfnMAPIReadMail = NULL;
	}
}

LRESULT MAPIMailCheck::Logon()
{
	LRESULT res = E_FAIL;

	if (m_hMapi)
	{
		Logoff();

		//Setup the flags & UIParam parameters used in the MapiLogon call
		FLAGS flags = 0;
		ULONG nUIParam = 0;

		//First try to acquire a new MAPI session using the supplied settings using the MAPILogon functio
		ULONG nError = m_lpfnMAPILogon(nUIParam, name, password, flags | MAPI_NEW_SESSION, 0, &m_hSession);
		if (nError != SUCCESS_SUCCESS && nError != MAPI_E_USER_ABORT)
		{
			//Failed to create a create mapi session, try to acquire a shared mapi session
			AtlTrace("Failed to logon to MAPI using a new session, trying to acquire a shared one\n");
			nError = m_lpfnMAPILogon(nUIParam, NULL, NULL, 0, 0, &m_hSession);
			if (nError == SUCCESS_SUCCESS)
			{
				m_nLastError = SUCCESS_SUCCESS;
				res = S_OK;
			}
			else
			{
				AtlTrace("Failed to logon to MAPI using a shared session, Error:%d\n", nError);
				m_nLastError = nError;
			}
		}
		else if (nError == SUCCESS_SUCCESS)
		{
			m_nLastError = SUCCESS_SUCCESS;
			res = S_OK;
		}
	}

	return res;
}

LRESULT MAPIMailCheck::Logoff()
{
	LRESULT res = E_FAIL;

	if (m_hSession)
	{
		//Call the MAPILogoff function
		ULONG nError = m_lpfnMAPILogoff(m_hSession, 0, 0, 0); 
		if (nError != SUCCESS_SUCCESS)
		{
			AtlTrace("Failed in call to MapiLogoff, Error:%d\n", nError);
			m_nLastError = nError;
		}
		else
		{
			m_nLastError = SUCCESS_SUCCESS;
			res = S_OK;
		}
		m_hSession = 0;
	}

	return S_OK;
}

LRESULT MAPIMailCheck::Initialize(_bstr_t username, _bstr_t password, long validDays)
{
	LRESULT res = E_FAIL;

	this->name = username;
	this->password = password;
	this->validDays = validDays;

	//First make sure the "WIN.INI" entry for MAPI is present aswell 
	//as the MAPI32 dll being present on the system
	BOOL bMapiInstalled = (GetProfileInt("MAIL", "MAPI", 0) != 0) && 
		(SearchPath(NULL, "MAPI32.DLL", NULL, 0, NULL, NULL) != 0);

	if (bMapiInstalled)
	{
		//Load up the MAPI dll and get the function pointers we are interested in
		m_hMapi = LoadLibrary("MAPI32.DLL");
		if (m_hMapi)
		{
			m_lpfnMAPILogon = (LPMAPILOGON) GetProcAddress(m_hMapi, "MAPILogon");
			m_lpfnMAPILogoff = (LPMAPILOGOFF) GetProcAddress(m_hMapi, "MAPILogoff");
			m_lpfnMAPISendMail = (LPMAPISENDMAIL) GetProcAddress(m_hMapi, "MAPISendMail");
			m_lpfnMAPIResolveName = (LPMAPIRESOLVENAME) GetProcAddress(m_hMapi, "MAPIResolveName");
			m_lpfnMAPIFreeBuffer = (LPMAPIFREEBUFFER) GetProcAddress(m_hMapi, "MAPIFreeBuffer");
			m_lpfnMAPIFindNext = (LPMAPIFINDNEXT) GetProcAddress(m_hMapi, "MAPIFindNext");
			m_lpfnMAPIReadMail = (LPMAPIREADMAIL) GetProcAddress(m_hMapi, "MAPIReadMail");

			//If any of the functions are not installed then fail the load
			if (m_lpfnMAPILogon == NULL ||
				m_lpfnMAPILogoff == NULL ||
				m_lpfnMAPISendMail == NULL ||
				m_lpfnMAPIResolveName == NULL ||
				m_lpfnMAPIFreeBuffer == NULL ||
				m_lpfnMAPIReadMail == NULL ||
				m_lpfnMAPIFindNext == NULL)
			{
				AtlTrace("Failed to get one of the functions pointer in MAPI32.DLL\n");
				Destroy();
			}
			else
			{
				Logon();
				res = S_OK;
			}
		}
	}
	else
		AtlTrace("Mapi is not installed on this computer\n");

	return res;
}

LRESULT MAPIMailCheck::Destroy()
{
	if (m_hMapi)
	{
		Logoff();
	}

	return S_OK;
}

LRESULT MAPIMailCheck::Configure(IObserver * observer, LONG hWnd)
{
	return S_OK;
}

BOOL MAPIMailCheck::IsInDate(char *id, char *validDate)
{
	BOOL ret = false;

	if (validDays <= 0)
		return true;

	FLAGS flags = MAPI_ENVELOPE_ONLY | MAPI_PEEK;
	lpMapiMessage pMsg;
	ULONG nError = m_lpfnMAPIReadMail(m_hSession, NULL, id, flags, 0, &pMsg );
	if (nError == SUCCESS_SUCCESS)
	{
		AtlTrace("Message received on %s ", pMsg->lpszDateReceived);

		if (strcmp(pMsg->lpszDateReceived, validDate) > 0)
		{
			AtlTrace("is in date\n");
			ret = true;
		}
		else
		{
			AtlTrace("is out of date\n");
		}

		nError = m_lpfnMAPIFreeBuffer(pMsg);
	}
	else
		AtlTrace("Could net get message envelope");

	return ret;
}

LRESULT MAPIMailCheck::NewMessageCount(long *pRet)
{
	int ret = 0;

	char timeBuf[64];
	*timeBuf = 0;

	if (validDays > 0)
	{
		time_t t;
		::time(&t);
		t -= validDays*3600*24;
		struct tm *lt;
		lt = localtime(&t);
		// YYYY/MM/DD HH:MM
		strftime(timeBuf, 63, "%Y/%m/%d %H:%M", lt);
	}

	LPTSTR id;
	id = new char[512];
	*id = NULL;
	FLAGS flags = MAPI_UNREAD_ONLY;

	ULONG nError = m_lpfnMAPIFindNext(m_hSession, 0, NULL, id, flags, 0, id );
	while ( nError == SUCCESS_SUCCESS )
	{
		if (IsInDate(id, timeBuf))
			ret++;

		nError = m_lpfnMAPIFindNext(m_hSession, 0, NULL, id, flags, 0, id );
	}

	delete [] id;

	*pRet = ret;

	return S_OK;
}
