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

#include <atlbase.h>
#include <atlcom.h>
#include <WindowsX.h>
#include "Editor.h"
#include "resource.h"

using namespace Flurry;

#pragma warning(disable:4100)

LRESULT CEditor::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// If we got a preset, load data
	if (spec == NULL)
		return 1;

	SetDlgItemText(IDC_NAME, spec->name.c_str());
	UpdateStreams();
	UpdateTemplate();

	return 1;  // Let the system set the focus
}



LRESULT CEditor::OnAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {return 0;}
LRESULT CEditor::OnRemove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {return 0;}
LRESULT CEditor::OnPreview(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {return 0;}


LRESULT CEditor::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (!IsPresetValid())
		return 1;
	
	SavePreset();
	EndDialog(wID);
	
	return 0;
}

LRESULT CEditor::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}


//////////////////////////////////////////////////////////////////////////
BOOL CEditor::IsPresetValid() { return FALSE;}
void CEditor::SavePreset() {}

void CEditor::UpdateStreams() {}
void CEditor::UpdateTemplate() {}