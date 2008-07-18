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


CEditor::CEditor(int index, Settings* settings) : index(index), settings(settings)
{
	hBrushBackground = CreateSolidBrush(RGB(255,255,206));
}

CEditor::~CEditor()
{
	DeleteObject(hBrushBackground);
}

// Create the listview and update contents
LRESULT CEditor::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Initialize the listview
	HWND hList = GetDlgItem(IDC_STREAM_LIST); 
	
	LVCOLUMN LvCol;
	memset(&LvCol, 0, sizeof(LvCol));

	LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;  // Type of mask
	LvCol.fmt = LVCFMT_LEFT | LVCFMT_FIXED_WIDTH;		 // Format
	LvCol.cx = 0x70;                                     // Width of column
	LvCol.pszText = "Streams";                           // Header Text
	ListView_InsertColumn(hList, 0, &LvCol);

	LvCol.pszText = "Color";
	ListView_InsertColumn(hList, 1, &LvCol);

	LvCol.cx = 0x69;
	LvCol.pszText = "Thickness";
	ListView_InsertColumn(hList, 2, &LvCol);

	LvCol.cx = 0x69;
	LvCol.pszText = "Speed";
	ListView_InsertColumn(hList, 3, &LvCol);

	ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT);

	// Max size for edit box
	SendDlgItemMessage(IDC_NAME, EM_SETLIMITTEXT, TEMPLATE_MAX_SIZE_NAME, (LPARAM)NULL);
	SendDlgItemMessage(IDC_TEMPLATE, EM_SETLIMITTEXT, TEMPLATE_MAX_SIZE, (LPARAM)NULL);
	SendDlgItemMessage(IDC_STREAMS, EM_SETLIMITTEXT, 10, (LPARAM)NULL);
	SendDlgItemMessage(IDC_THICKNESS, EM_SETLIMITTEXT, 10, (LPARAM)NULL);
	SendDlgItemMessage(IDC_SPEED, EM_SETLIMITTEXT, 10, (LPARAM)NULL);

	// Add the list of colors
	for (int i = 0; i < Spec::nColors; i++)
		ComboBox_AddString(GetDlgItem(IDC_COLOR), Spec::colorTable[i]);

	ComboBox_SetCurSel(GetDlgItem(IDC_COLOR), 0);

	// If we got a preset, load data
	if (index == -1) {
		spec = new Spec("");
		UpdateOKButton();
		return 1;
	}

	// Get the spec to edit
	spec = new Spec(*settings->visuals[index]);

	SetDlgItemText(IDC_NAME, spec->GetName().c_str());
	UpdateClusters();
	UpdateTemplate();
	UpdateOKButton();

	return 1;  // Let the system set the focus
}


LRESULT CEditor::OnName(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return 0;

	// Update the name
	char name[TEMPLATE_MAX_SIZE_NAME];
	GetDlgItemText(IDC_NAME, name, TEMPLATE_MAX_SIZE_NAME);

	if (string(name).compare(spec->GetName()) == 0)
		return 0;

	spec->SetName(string(name));
	UpdateTemplate();
	UpdateOKButton();

	return 0;
}

LRESULT CEditor::OnListView(int wID, LPNMHDR pNMHDR, BOOL& bHandled)
{
	// On select event, update the buttons
	int selected = ListView_GetNextItem(GetDlgItem(IDC_STREAM_LIST), -1, LVNI_FOCUSED);

	// List view empty
	if (selected == -1)
		return 0;

	// Update cluster editor contents
	char text[256];

	sprintf_s(text, sizeof(text), "%d", spec->clusters[selected].nStreams);
	SetDlgItemText(IDC_STREAMS, text);

	ComboBox_SetCurSel(GetDlgItem(IDC_COLOR), spec->clusters[selected].color);

	sprintf_s(text, sizeof(text), "%.2f", spec->clusters[selected].thickness);
	SetDlgItemText(IDC_THICKNESS, text);

	sprintf_s(text, sizeof(text), "%.2f", spec->clusters[selected].speed);
	SetDlgItemText(IDC_SPEED, text);
	
	UpdateAddCancelButtons(false, false, false, true);

	return 0;
}



//////////////////////////////////////////////////////////////////////////
// Clusters
//////////////////////////////////////////////////////////////////////////

// Add or update a cluster
LRESULT CEditor::OnClusterAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) {
	
	HWND hList = GetDlgItem(IDC_STREAM_LIST); 

	// check if we are editing a cluster (a line is selected in the listview)
	



	// Remove any selection in the listview



	//UpdateAddCancelButtons();

	return 0;
}

// Remove or cancel editing a cluster
LRESULT CEditor::OnClusterRemove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) 
{
	// Check if an item is selected -> Remove or cancel clicked
	int selected = ListView_GetNextItem(GetDlgItem(IDC_STREAM_LIST), -1, LVNI_FOCUSED);
	if (selected != -1)
	{
		// Check if the update button is disabled -> Remove clicked
		CWindow wnd;
		wnd.Attach(GetDlgItem(IDC_BUTTON_ADD_EDIT));
		if (!wnd.IsWindowEnabled())
		{
			// Remove the cluster from the listview and from the spec
			ListView_DeleteItem(GetDlgItem(IDC_STREAM_LIST), selected);			

			spec->clusters.erase(spec->clusters.begin() + selected);
			UpdateTemplate();
			UpdateOKButton();
		}
		else
		{		
			// Cancel clicked - clear the selection
			ListView_SetItemState(GetDlgItem(IDC_STREAM_LIST), -1, 0, LVIS_FOCUSED|LVIS_SELECTED);
		}
	}

	// Cancel adding new item -> clear the fields
	SetDlgItemText(IDC_STREAMS, "");
	ComboBox_SetCurSel(GetDlgItem(IDC_COLOR), 0);
	SetDlgItemText(IDC_THICKNESS, "");
	SetDlgItemText(IDC_SPEED, "");

	UpdateAddCancelButtons(true, false, true, false);

	return 0;
}


// When the template is modified, try to parse the text and update clusters accordingly
LRESULT CEditor::OnTemplate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) 
{
	if (wNotifyCode != EN_CHANGE)
		return 0;

	// Check that the text has been modified
	char text[TEMPLATE_MAX_SIZE];
	GetDlgItemText(IDC_TEMPLATE, text, TEMPLATE_MAX_SIZE);

	if (spec->GetTemplate().compare(text) == 0)
		return 0;

	// Update spec with new template
	spec->SetTemplate(text);

	UpdateClusters();

	// Ask to redraw edit control
	CWindow wnd;
	wnd.Attach(hWndCtl);
	wnd.RedrawWindow();

	UpdateOKButton();
		
	return 0;
}

LRESULT CEditor::OnTemplateColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg != WM_CTLCOLOREDIT) 
		return 0;

	if (GetDlgItem(IDC_TEMPLATE) != (HWND)lParam)
		return 0;

	// Check that the template contains text
	char text[TEMPLATE_MAX_SIZE];
	GetDlgItemText(IDC_TEMPLATE, text, TEMPLATE_MAX_SIZE);
	if (string(text).empty())
		return 0;

	DeleteObject(hBrushBackground);

	// Paint green if valid, red otherwise
	if (spec->IsValid()) {
		SetBkColor((HDC)wParam, RGB(209,255,181));
		hBrushBackground = CreateSolidBrush(RGB(209,255,181));
	} else {
		SetBkColor((HDC)wParam, RGB(255,178,178));
		hBrushBackground = CreateSolidBrush(RGB(255,178,178));
	}

	return (LRESULT)hBrushBackground;
}


LRESULT CEditor::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (!spec->IsValid())
		return 1;
	
	if (index == -1)
		settings->visuals.push_back(spec);
	else
		settings->visuals[index]->SetTemplate(spec->GetTemplate());

	EndDialog(wID);
	
	return 0;
}

LRESULT CEditor::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// Utility functions
//////////////////////////////////////////////////////////////////////////

// Insert streams from spec
void CEditor::UpdateClusters()
{
	// Clear the list
	ListView_DeleteAllItems(GetDlgItem(IDC_STREAM_LIST));

	for (int i = 0; i < (signed)spec->clusters.size(); i++) {
		InsertCluster(spec->clusters[i], i);
	}
}

void CEditor::InsertCluster(ClusterSpec cluster, int line)
{
	HWND hList = GetDlgItem(IDC_STREAM_LIST); 

	char text[256];

	LVITEM LvItem;
	memset(&LvItem, 0, sizeof(LvItem));

	LvItem.mask = LVIF_TEXT;   // Text Style
	LvItem.cchTextMax = 256;   // Max size of text
	LvItem.iItem = line;       // Line

	// Clusters
	LvItem.iSubItem = 0;
	sprintf_s(text, sizeof(text), "%d", cluster.nStreams);
	LvItem.pszText = text;
	ListView_InsertItem(hList, &LvItem);

	// Color
	LvItem.iSubItem = 1;
	sprintf_s(text, sizeof(text), "%s", spec->ColorModeToName(cluster.color));
	LvItem.pszText = text;
	ListView_SetItem(hList, &LvItem);

	// Thickness
	LvItem.iSubItem = 2;
	sprintf_s(text, sizeof(text), "%.2f", cluster.thickness);
	LvItem.pszText = text;
	ListView_SetItem(hList, &LvItem);

	// Speed
	LvItem.iSubItem = 3;
	sprintf_s(text, sizeof(text), "%.2f", cluster.speed);
	LvItem.pszText = text;
	ListView_SetItem(hList, &LvItem);
}

// Get the preset text and put it into the template edit box
void CEditor::UpdateTemplate()
{
	SetDlgItemText(IDC_TEMPLATE, spec->GetTemplate().c_str());
}

// If the spec is not valid, disable the OK button
void CEditor::UpdateOKButton()
{
	CWindow wnd;
	wnd.Attach(GetDlgItem(IDOK));
	wnd.EnableWindow(spec->IsValid());
}

// Update the Add/Cancel buttons (name and enabled status)
void CEditor::UpdateAddCancelButtons(bool isAdd, bool isAddEnabled, bool isCancel, bool isCancelEnabled) 
{
	// Listview selected -> Update(Disabled) / Remove
	//	+ modified value -> Update / Cancel
	//
	// No selection -> Add(Disabled) / Cancel (Disabled)
	//  + modified  -> Add / Cancel

	if (isAdd)
		SetDlgItemText(IDC_BUTTON_ADD_EDIT, "Add");
	else
		SetDlgItemText(IDC_BUTTON_ADD_EDIT, "Update");

	CWindow wndAdd;
	wndAdd.Attach(GetDlgItem(IDC_BUTTON_ADD_EDIT));
	wndAdd.EnableWindow(isAddEnabled);

	if (isCancel)
		SetDlgItemText(IDC_BUTTON_REMOVE_CANCEL, "Cancel");
	else
		SetDlgItemText(IDC_BUTTON_REMOVE_CANCEL, "Remove");

	CWindow wndCancel;
	wndCancel.Attach(GetDlgItem(IDC_BUTTON_REMOVE_CANCEL));
	wndCancel.EnableWindow(isCancelEnabled);





	
}