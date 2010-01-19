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

// MappedController.cpp : Implementation of MappedControllerDialog.cpp
#include "stdafx.h"
#include "COMControllers.h"
#include "MappedController.h"
#include "MappedControllerDialog.h"

MappedControllerDialog::MappedControllerDialog(IMappedController *pController, IObserver *observer, IMeterHome *meters, WORD idd) :
	MessageSenderDialog(pController, observer, meters, idd)
{
}

MappedControllerDialog::~MappedControllerDialog()
{
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 * Should initialize all of the controls using values from the meter.
 */
LRESULT MappedControllerDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;	// Make CWindow go through all the message maps.

	return 1;
}
