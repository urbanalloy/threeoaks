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

// HiddenWindow.cpp: implementation of the HiddenWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "COMControllers.h"
#include "HiddenWindow.h"
#include "ScriptThread.h"

#define OK 1

const unsigned long HiddenWindow::CONTROLLER_HANDLEMESSAGE		= WM_USER;
const unsigned long HiddenWindow::CONTROLLER_PUTFILENAME	= WM_USER+1;
const unsigned long HiddenWindow::CONTROLLER_PUTMODEL	= WM_USER+2;
const unsigned long HiddenWindow::WINDOW_QUIT		= WM_USER+3;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HiddenWindow::HiddenWindow() :
	pScript(NULL)
{
}

HiddenWindow::~HiddenWindow()
{
	if (pScript)
		pScript->Release();
}

void HiddenWindow::setScript(IScriptController *newVal)
{
	if (pScript != newVal)
	{
		if (pScript)
			pScript->Release();

		pScript = newVal;
		pScript->AddRef();
	}
}

// All params are ignored, event = long *dirty
LRESULT HiddenWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MessageParams *wp = (MessageParams*)wParam;
	VARIANT_BOOL handled = false;
	pScript->HandleMessage(wp->wParam, wp->lParam, wp->x, wp->y, &handled);
	delete wp;

	return OK;
}

// wParam = BSTR filename, lParam = ignored, event = long out
LRESULT HiddenWindow::PutFilename(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BSTR filename = (BSTR)wParam;
	pScript->put_FileName(filename);
	::SysFreeString(filename);

	return OK;
}

// wParam = BSTR value, lParam = ignored, event = nothing out
LRESULT HiddenWindow::PutModel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	IClientModel *model = (IClientModel*)wParam;
	pScript->put_Model(model);

	return OK;
}
