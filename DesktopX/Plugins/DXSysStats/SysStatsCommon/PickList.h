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

// PickList.h: interface for the PickList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PICKLIST_H__41BE3ABF_C154_4649_8DB4_FCA680056348__INCLUDED_)
#define AFX_PICKLIST_H__41BE3ABF_C154_4649_8DB4_FCA680056348__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <comdef.h>
#ifndef MSXML4_IMPORTED
#import <msxml4.dll> no_implementation
#endif

struct IMeterHome;
struct ICompositeOverlay;

template <class C>
class PickList  
{
typedef bool (*EqFuncPtr)(C lhs, C rhs);
typedef void (*ToStrFuncPtr)(char* buf, int maxlen, C val);
typedef void (*FromStrFuncPtr)(C *out, char* s);

public:
	PickList(EqFuncPtr equals, ToStrFuncPtr toStr, FromStrFuncPtr fromStr, BOOL fixed);
	virtual ~PickList();

	LRESULT init(HWND hDlg, WORD wId, const char* labels[], C* data, C curVal);
	LRESULT init(HWND hDlg, WORD wId, _bstr_t labels[], C* data, C curVal);
	LRESULT init(HWND hDlg, WORD wId, _bstr_t filename, C curVal);
	LRESULT init(HWND hDlg, WORD wId, IMeterHome *meters, C curVal);
	LRESULT init(HWND hDlg, WORD wId, ICompositeOverlay *overlays, C curVal);

	LRESULT getData(C *out, C def);
	LRESULT select(C curVal);

	// These are only used with the xml metadata version
	LRESULT init(HWND hDlg, WORD wId, BSTR xmlDoc, BSTR xpath, C curVal);
	LRESULT getValue(C *out, C def);
	LRESULT getItemID(C *out, C def);
	LRESULT selectValue(C curVal);

protected:
	BOOL fixed;
	HWND hDlg;
	WORD wId;

	::MSXML2::IXMLDOMNodePtr valueSet;

	EqFuncPtr		pEquals;
	ToStrFuncPtr	pToStr;
	FromStrFuncPtr	pFromStr;
};

#endif // !defined(AFX_PICKLIST_H__41BE3ABF_C154_4649_8DB4_FCA680056348__INCLUDED_)
