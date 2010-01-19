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

// IScriptConsole.h: interface for the IScriptConsole class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISCRIPTCONSOLE_H__F3C0C853_556D_42C7_B13C_A83E93F1B32F__INCLUDED_)
#define AFX_ISCRIPTCONSOLE_H__F3C0C853_556D_42C7_B13C_A83E93F1B32F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CScriptConsole;

class __declspec(dllexport) IScriptConsole  
{
public:
	virtual ~IScriptConsole();
	virtual bool Show(bool show);
	virtual void AddString(BSTR s);

	static IScriptConsole *createConsole();

private:
	IScriptConsole();

	CScriptConsole *pConsole;
};

#endif // !defined(AFX_ISCRIPTCONSOLE_H__F3C0C853_556D_42C7_B13C_A83E93F1B32F__INCLUDED_)
