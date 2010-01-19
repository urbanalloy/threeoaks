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

// IMailCheck.h: interface for the IMailCheck class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAILCHECK_H__1D1FF879_59D2_47F4_8C34_1B7B0A4BBF8F__INCLUDED_)
#define AFX_IMAILCHECK_H__1D1FF879_59D2_47F4_8C34_1B7B0A4BBF8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comutil.h"       // main symbols

struct IObserver;

class IMailCheck  
{
public:
	IMailCheck();
	virtual ~IMailCheck();

	virtual LRESULT Initialize(_bstr_t username, _bstr_t password, long validDays) = 0;
	virtual LRESULT Destroy() = 0;
	virtual LRESULT Configure(IObserver * observer, LONG hWnd) = 0;
	virtual LRESULT NewMessageCount(long *pRet) = 0;
};

#endif // !defined(AFX_IMAILCHECK_H__1D1FF879_59D2_47F4_8C34_1B7B0A4BBF8F__INCLUDED_)
