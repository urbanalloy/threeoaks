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
#include "COMMeters.h"
#include "HiddenWindow.h"
#include "ScriptThread.h"

#define OK 1

const unsigned long HiddenWindow::METER_UPDATE		= WM_USER;
const unsigned long HiddenWindow::METER_GETASSTRING = WM_USER+1;
const unsigned long HiddenWindow::METER_GETASDOUBLE = WM_USER+2;
const unsigned long HiddenWindow::METER_GETASLONG	= WM_USER+3;
const unsigned long HiddenWindow::METER_GETVALUE	= WM_USER+4;
const unsigned long HiddenWindow::METER_PUTFILENAME	= WM_USER+5;
const unsigned long HiddenWindow::METER_PUTVALUE	= WM_USER+6;
const unsigned long HiddenWindow::METER_PUTMODEL	= WM_USER+7;
const unsigned long HiddenWindow::WINDOW_QUIT		= WM_USER+8;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HiddenWindow::HiddenWindow() :
	pScript(NULL),
	pThread(NULL)
{
}

HiddenWindow::~HiddenWindow()
{
	if (pScript)
		pScript->Release();
}

void HiddenWindow::setScript(IScriptMeter *newVal)
{
	if (pScript != newVal)
	{
		if (pScript)
			pScript->Release();

		pScript = newVal;
		pScript->AddRef();
	}
}

void HiddenWindow::setThread(ScriptThread *thread)
{
	pThread = thread;
}

// All params are ignored, event = long *dirty
LRESULT HiddenWindow::Update(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	long dirty = false;
	pScript->Update(&dirty);
	if (dirty)
	{
		IClientModel *model;
		pScript->get_Model(&model);
		model->put_dirty(dirty);
		model->Release();
	}

	return OK;
}

// wParam = BSTR format, lParam = BSTR selector, event = BSTR out
LRESULT HiddenWindow::GetAsString(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BSTR format = (BSTR)wParam;
	BSTR sel = (BSTR)lParam;
	BSTR ret = format;
	pScript->GetAsString(format, sel, &ret);
	::SysFreeString(format);
	::SysFreeString(sel);
	pThread->put_StringValue(ret);

	return OK;
}

// wParam = BSTR selector, lParam = ignored, event = double out
LRESULT HiddenWindow::GetAsDouble(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	double ret;
	BSTR sel = (BSTR)wParam;
	pScript->GetAsDouble(sel, &ret);
	::SysFreeString(sel);
	pThread->put_DoubleValue(ret);

	return OK;
}

// wParam = BSTR selector, lParam = ignored, event = long out
LRESULT HiddenWindow::GetAsLong(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	long ret;
	BSTR sel = (BSTR)wParam;
	pScript->GetAsLong(sel, &ret);
	::SysFreeString(sel);
	pThread->put_LongValue(ret);

	return OK;
}

// All params ignored, event = VARIANT out
LRESULT HiddenWindow::GetValue(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	VARIANT ret;
	::VariantInit(&ret);
	pScript->GetValue(&ret);
	pThread->put_Value(ret);

	return OK;
}

// wParam = BSTR filename, lParam = ignored, event = long out
LRESULT HiddenWindow::PutFilename(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BSTR filename = (BSTR)wParam;
	pScript->put_FileName(filename);
	::SysFreeString(filename);

	return OK;
}LRESULT HiddenWindow::PutModel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	IClientModel *model = (IClientModel*)wParam;
	pScript->put_Model(model);

	return OK;
}


// wParam = BSTR value, lParam = ignored, event = nothing out
LRESULT HiddenWindow::PutValue(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BSTR val = (BSTR)wParam;
	pScript->put_Value(val);
	::SysFreeString(val);

	return OK;
}

// wParam = BSTR value, lParam = ignored, event = nothing out
