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

// IScriptConsole.cpp: implementation of the IScriptConsole class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "IScriptConsole.h"
#include "ScriptConsole.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IScriptConsole::IScriptConsole() : pConsole(new CScriptConsole(this))
{
	pConsole->Create(0);
}

IScriptConsole::~IScriptConsole()
{
	if (pConsole)
	{
		if (::IsWindow(pConsole->m_hWnd))
			pConsole->DestroyWindow();
		delete pConsole;
	}
}

IScriptConsole* IScriptConsole::createConsole()
{
	IScriptConsole *console = new IScriptConsole();

	return console;
}

void IScriptConsole::AddString(BSTR s)
{
	if (pConsole)
		pConsole->AddString(s);
}

bool IScriptConsole::Show(bool show)
{
	bool wasVisible = pConsole != NULL;

	if (show)
	{
		if (pConsole)
		{
			if (::IsWindow(pConsole->m_hWnd))
			{
				pConsole->ShowWindow(SW_SHOW);
				return wasVisible;
			}
			else
				delete pConsole;
		}

		pConsole = new CScriptConsole(this);
		pConsole->Create(0);
		pConsole->ShowWindow(SW_SHOWNORMAL);
	}
	else
	{
		if (pConsole)
		{
			pConsole->DestroyWindow();
			delete pConsole;
			pConsole = NULL;
		}
	}

	return wasVisible;
}

