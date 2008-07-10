///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : CEditor Class
//
// Flurry Editor
//
// (C) 2008 Julien Templier
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
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