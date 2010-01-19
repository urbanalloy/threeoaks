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

// MAPIMailCheck.h: interface for the MAPIMailCheck class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPIMAILCHECK_H__3A40C455_DFDD_41A9_BC87_773C99BA1AC9__INCLUDED_)
#define AFX_MAPIMAILCHECK_H__3A40C455_DFDD_41A9_BC87_773C99BA1AC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <MAPI.h>
#include "IMailCheck.h"

class MAPIMailCheck : public IMailCheck  
{
public:
	MAPIMailCheck();
	virtual ~MAPIMailCheck();

// IMailCheck
	virtual LRESULT Initialize(_bstr_t username, _bstr_t password, long validDays);
	virtual LRESULT Destroy();
	virtual LRESULT Configure(IObserver * observer, LONG hWnd);
	virtual LRESULT NewMessageCount(long *pRet);

protected:
	virtual LRESULT Logon();
	virtual LRESULT Logoff();
	virtual BOOL IsInDate(char *id, char *validDate);

	_bstr_t name;
	_bstr_t password;
	long validDays;

	HINSTANCE			m_hMapi; //Instance handle of the MAPI dll
	LHANDLE				m_hSession; //Mapi Session handle

	ULONG				m_nLastError; //Last Mapi error value

	LPMAPILOGON			m_lpfnMAPILogon; //MAPILogon function pointer
	LPMAPILOGOFF		m_lpfnMAPILogoff; //MAPILogoff function pointer
	LPMAPISENDMAIL		m_lpfnMAPISendMail; //MAPISendMail function pointer
	LPMAPIRESOLVENAME	m_lpfnMAPIResolveName; //MAPIResolveName function pointer
	LPMAPIFREEBUFFER	m_lpfnMAPIFreeBuffer; //MAPIFreeBuffer function pointer
	LPMAPIFINDNEXT		m_lpfnMAPIFindNext;  //MAPIFindNext function pointer
	LPMAPIREADMAIL		m_lpfnMAPIReadMail;	//MAPIReadMail function pointer
};

#endif // !defined(AFX_MAPIMAILCHECK_H__3A40C455_DFDD_41A9_BC87_773C99BA1AC9__INCLUDED_)
