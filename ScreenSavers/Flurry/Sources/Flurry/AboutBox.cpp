///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : CAboutBox Class
//
// Declaration of CAboutBox
//
// Copyright (c) 2003, Matt Ginzton
// Copyright (c) 2005-2008, Julien Templier
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// o Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// o Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// o Neither the name of the author nor the names of its contributors may
//   be used to endorse or promote products derived from this software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////////////////

#include <atlbase.h>
#include <atlcom.h>
#include "AboutBox.h"
#include "resource.h"
#include "VersionNo.h"

using namespace Flurry;

#pragma warning(disable:4100)

static char szCompileTime[] = __TIME__ " on " __DATE__;

static BOOL WINAPI CreditsDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

/////////////////////////////////////////////////////////////////////////////
// CAboutBox

LRESULT CAboutBox::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetDlgItemText (IDC_ABOUT_VERSION_BUILD, VERSION_STRING);
	SetDlgItemText (IDC_ABOUT_VERSION_COMPILE, szCompileTime);

	return 1;  // Let the system set the focus
}


LRESULT CAboutBox::OnDismiss(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}

LRESULT CAboutBox::OnCredits(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DialogBoxParam(NULL, MAKEINTRESOURCE(DLG_CREDITS), hWndCtl, CreditsDialog, 0);
	return 0;
}


static BOOL WINAPI CreditsDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_INITDIALOG:
			return TRUE;
			break;

		case WM_COMMAND:
			EndDialog(hDlg, 1);
			break;
	}

	return FALSE;
}
