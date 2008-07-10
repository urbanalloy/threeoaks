///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : CEditor Class
//
// Flurry Editor
//
// Copyright (c) 2008, Julien Templier
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

#include "resource.h"
#include "FlurryPreset.h"
#include <atlhost.h>


namespace Flurry {


	class CEditor : public CDialogImpl<CEditor>
	{
		private:
			Spec* spec;

			BOOL IsPresetValid();
			void SavePreset();

			void UpdateStreams();
			void UpdateTemplate();

		public:
			// "factory"
			static void AutomaticDoModal (Spec* preset)
			{
				CEditor* pEditor = new CEditor(preset);
				pEditor->DoModal();
				delete pEditor;
			}

			// constructors			
			CEditor(Spec* preset) : spec(preset) {};
			~CEditor() {};

			enum { IDD = DLG_EDITOR };

		protected:

			BEGIN_MSG_MAP(CEditor)
				MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
				COMMAND_ID_HANDLER(IDOK, OnOK)
				COMMAND_ID_HANDLER(IDCANCEL, OnCancel)	
				COMMAND_ID_HANDLER(IDC_BUTTON_ADD, OnAdd)	
				COMMAND_ID_HANDLER(IDC_BUTTON_REMOVE, OnRemove)	
				COMMAND_ID_HANDLER(IDC_BUTTON_PREVIEW, OnPreview)					
			END_MSG_MAP()

			// Handler prototypes:
			LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
			LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
			LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

			LRESULT OnAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
			LRESULT OnRemove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
			LRESULT OnPreview(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		
	};

}

