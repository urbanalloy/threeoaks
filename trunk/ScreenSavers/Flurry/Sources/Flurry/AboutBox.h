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

#pragma once

#include "resource.h"       // main symbols
#include <atlhost.h>
#include <vector>

namespace Flurry {

	class CAboutBox : public CDialogImpl<CAboutBox>
	{
		public:
			// "factory"
			static void AutomaticDoModal (void)
			{
				CAboutBox* pAboutBox = new CAboutBox;
				pAboutBox->DoModal();
				delete pAboutBox;
			}

			// constructors
			CAboutBox() {};
			~CAboutBox() {};

			enum { IDD = IDD_ABOUTBOX };

		protected:
			BEGIN_MSG_MAP(CAboutBox)
				MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
				COMMAND_ID_HANDLER(IDCANCEL, OnDismiss)			// bind both IDOK and IDCANCEL to the dismiss function, as well as WM_CLOSE
				COMMAND_ID_HANDLER(IDOK, OnDismiss)				// of OK and CANCEL, CANCEL is preferred because then Esc works as well as Enter
				COMMAND_ID_HANDLER(IDC_CREDITS, OnCredits)		// Show Credits dialog
			END_MSG_MAP()

			// Handler prototypes:

			LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
			LRESULT OnDismiss(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
			LRESULT OnCredits(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);			
	};

}
