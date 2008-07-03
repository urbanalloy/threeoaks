///////////////////////////////////////////////////////////////////////////////////////////////
//
// TodoList : a ToDo Docklet for ObjectDock
//
// (c) 2004 - Julien Templier
// Parts from Weather Docklet by Jeff Bargmann
//
// This work is licensed under the Creative Commons
// Attribution-ShareAlike License. To view a copy of this license, visit
// http://creativecommons.org/licenses/by-sa/2.0/ or send a letter to
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////


#include "TodoList.h"
#include "Dialogs.h"
#include "resource.h"

#include <CommCtrl.h>

///////////////////////////////////////////////////////////////////////////////////////////////
int CALLBACK ConfigureDocklet(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	DOCKLET_DATA *lpData = (DOCKLET_DATA *) GetProp(hDlg, "lpData");
	vector<string> tasksNames;

	switch(iMsg)
	{
		case WM_INITDIALOG:
			SetWindowText(hDlg, "Edit Task...");

			// Get Data
			SetProp(hDlg, "lpData", (HANDLE) (char*) lParam);
			lpData = (DOCKLET_DATA *) GetProp(hDlg, "lpData");
			if(!lpData)
				return TRUE;

			// Set dialog icon
			SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)lpData->todo->GetTaskIcon());
			SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)lpData->todo->GetTaskIcon());

			// Fill the title & description fields
			SetDlgItemText(hDlg, IDC_NAME, lpData->todo->GetName().c_str());
			SetDlgItemText(hDlg, IDC_DESCRIPTION, lpData->todo->GetDescription().c_str());

			// Fix the max size of the title & description
			SendDlgItemMessage(hDlg, IDC_NAME, EM_SETLIMITTEXT, 50, (LPARAM)NULL);
			SendDlgItemMessage(hDlg, IDC_DESCRIPTION, EM_SETLIMITTEXT, 500, (LPARAM)NULL);

			// Fill the combo box
			for (int i = 0; i < lpData->todo->PrioritySize; ++i)
				SendDlgItemMessage(hDlg, IDC_PRIORITY, CB_INSERTSTRING, i,
							reinterpret_cast<LPARAM>((LPCTSTR)lpData->todo->PriorityNames[i]));

			// Select the current priority
			SendDlgItemMessage(hDlg, IDC_PRIORITY, CB_SETCURSEL, lpData->todo->GetPriority(), (LPARAM)NULL);

			// Select the stored date
			if (lpData->todo->HasDate()) {
				SendDlgItemMessage(hDlg, IDC_DATEPICKER, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)lpData->todo->GetDate());

				if (lpData->todo->HasTime())
					SendDlgItemMessage(hDlg, IDC_TIMEPICKER, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)lpData->todo->GetDate());
				else
					SendDlgItemMessage(hDlg, IDC_TIMEPICKER, DTM_SETSYSTEMTIME, GDT_NONE, (LPARAM)NULL );

				// Show the overdue icon if necessary
				showOverdue(hDlg, lpData);
			} else {
				EnableWindow(GetDlgItem(hDlg, IDC_TIMEPICKER), false);
				SendDlgItemMessage(hDlg, IDC_DATEPICKER, DTM_SETSYSTEMTIME, GDT_NONE, (LPARAM)NULL );
				SendDlgItemMessage(hDlg, IDC_TIMEPICKER, DTM_SETSYSTEMTIME, GDT_NONE, (LPARAM)NULL );
			}
				
			// Fill the task combo box
			tasksNames = lpData->todo->GetTasksNames();
			if (tasksNames.size() !=0) {
				for (int i = 0; i < (signed)tasksNames.size(); ++i) {
					SendDlgItemMessage(hDlg, IDC_TASK, CB_INSERTSTRING, i,
							reinterpret_cast<LPARAM>((LPCTSTR)tasksNames[i].c_str()));
				}

				// Activate the combo box
				EnableWindow(GetDlgItem(hDlg, IDC_TASK), true);
			}


			// Select the current task if available
			if (lpData->todo->GetCurrentTask() != Todo::NO_TASK) {
				SendDlgItemMessage(hDlg, IDC_TASK, CB_SETCURSEL, lpData->todo->GetCurrentTask(), (LPARAM)NULL);
			}

			// Set the version text
			char version[30];
			sprintf(version,"TodoList v%i.%i Build %i",VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
			SetDlgItemText(hDlg, IDC_TXT_VERSION,version);
			
			// Set the icon
			showPreviewIcon(hDlg, lpData);
			break;

		case WM_NOTIFY:
			LPNMDATETIMECHANGE lpChange;
			switch(LOWORD(wParam))
			{
				case IDC_DATEPICKER:
					lpChange = (LPNMDATETIMECHANGE) lParam;
					if (lpChange->nmhdr.code == DTN_DATETIMECHANGE)
					{
						if (lpChange->dwFlags == GDT_NONE) {
							lpData->todo->SetNoDate();
							EnableWindow(GetDlgItem(hDlg, IDC_TIMEPICKER), false);
						} else {
							lpData->todo->SetDate(lpChange->st);
							EnableWindow(GetDlgItem(hDlg, IDC_TIMEPICKER), true);
						}

						showOverdue(hDlg, lpData);
					}					
					break;
				case IDC_TIMEPICKER:
					lpChange = (LPNMDATETIMECHANGE) lParam;
					if (lpChange->nmhdr.code == DTN_DATETIMECHANGE)
					{
						if (lpChange->dwFlags == GDT_NONE)
							lpData->todo->SetNoTime();
						else
							lpData->todo->SetTime(lpChange->st);

						showOverdue(hDlg, lpData);
					}					
					break;
			}
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				// Reminder Options
				case IDC_REMINDER:
					DialogBoxParam(lpData->docklet->GetInstance(), MAKEINTRESOURCE(IDD_REMINDER), hDlg, ReminderOptions, (LPARAM) lpData);

					break;

				case IDC_TASK:
					if (HIWORD(wParam) == CBN_SELCHANGE) {
						int index = SendDlgItemMessage(hDlg, IDC_TASK, CB_GETCURSEL, (WPARAM)NULL, (LPARAM)NULL);

						lpData->todo->SetCurrentTask(index);
						showPreviewIcon(hDlg, lpData);
					}
						
					break;

				case IDC_CHANGE_ICON:
					char szImage[MAX_PATH+10];
					strcpy(szImage, "");
					if (lpData->docklet->BrowseForImage(hDlg, szImage)) {
						lpData->todo->SetIcon(szImage);
						showPreviewIcon(hDlg, lpData);
						SendDlgItemMessage(hDlg, IDC_TASK, CB_SETCURSEL, (WPARAM)Todo::NO_TASK, (LPARAM)NULL);
					}
					break;

				case IDC_DEFAULT_ICON:
					lpData->todo->SetDefaultIcon();			
					showPreviewIcon(hDlg, lpData);
					SendDlgItemMessage(hDlg, IDC_TASK, CB_SETCURSEL, (WPARAM)Todo::NO_TASK, (LPARAM)NULL);
					break;

				case ID_OK:
					if(!lpData)
						break;

					// Get Title and description
					char name[Todo::SIZE_NAME]; strcpy(name, "");
					char description[Todo::SIZE_DESCRIPTION]; strcpy(description, "");
					GetWindowText(GetDlgItem(hDlg, IDC_NAME), (char*)&name, Todo::SIZE_NAME*sizeof(char));
					GetWindowText(GetDlgItem(hDlg, IDC_DESCRIPTION), (char*)&description,  Todo::SIZE_DESCRIPTION*sizeof(char));
					lpData->todo->setName(string(name));
					lpData->todo->setDescription(string(description));

					// Get Priority
					lpData->todo->SetPriority((Todo::Priority)SendDlgItemMessage(hDlg, IDC_PRIORITY, CB_GETCURSEL, (WPARAM)NULL, (LPARAM)NULL));

					// Set docklet label & image
					lpData->docklet->SetLabel(name);
					lpData->docklet->SetImageFile(lpData->todo->GetIcon());
					lpData->docklet->SetImageOverlay(lpData->todo->CreateOverlayImage());

				case ID_CANCEL:
					EndDialog(hDlg, 0);
					return TRUE;
			}
			break;

		case WM_DESTROY:
			RemoveProp(hDlg, "lpData");
			break;

		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return TRUE;

	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
int CALLBACK ReminderOptions(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	DOCKLET_DATA *lpData = (DOCKLET_DATA *) GetProp(hDlg, "lpData");
	vector<string> tasksNames;

	switch(iMsg)
	{
		case WM_INITDIALOG:
			//SetWindowText(hDlg, "Edit Task...");

			// Get Data
			SetProp(hDlg, "lpData", (HANDLE) (char*) lParam);
			lpData = (DOCKLET_DATA *) GetProp(hDlg, "lpData");
			if(!lpData)
				return TRUE;

			// Set dialog icon
			SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)lpData->todo->GetReminderIcon());
			SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)lpData->todo->GetReminderIcon());

			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_OK:
					if(!lpData)
						break;
				case ID_CANCEL:
					EndDialog(hDlg, 0);
					return TRUE;
			}
			break;

		case WM_DESTROY:
			RemoveProp(hDlg, "lpData");
			break;

		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return TRUE;

	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// FIXME: can only show icons from the objectdock folder
void showPreviewIcon(HWND hDlg, DOCKLET_DATA *lpData)
{
	showIcon(hDlg, IDC_ICON_PIC, lpData, (char*)lpData->todo->GetIcon().c_str(), 64);
}

///////////////////////////////////////////////////////////////////////////////////////////////
void showOverdue(HWND hDlg, DOCKLET_DATA *lpData)
{
	// check the data & the hasDate field (if the date is "unchecked")
	if (!lpData->todo->IsOverdue() || !lpData->todo->HasDate()) {
		ShowWindow(GetDlgItem(hDlg, IDC_ICON_OVERDUE), SW_HIDE);
	} else {
        // Is the image already drawn ?	
		if (SendDlgItemMessage(hDlg, IDC_ICON_OVERDUE, STM_GETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)NULL) == NULL)
			showIcon(hDlg, IDC_ICON_OVERDUE, lpData, (char*)lpData->todo->GetIconOverdue().c_str(), 16);
		
		ShowWindow(GetDlgItem(hDlg, IDC_ICON_OVERDUE), SW_SHOW);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
void showIcon(HWND hDlg, int ID_DIALOG, DOCKLET_DATA *lpData, char* name, int size)
{
	// Get the full icon path
	char fullpath[MAX_PATH+10];
	sprintf(fullpath, "%s%s", lpData->docklet->GetRootFolder().c_str(), name);

	// Load the image
	Bitmap* image = lpData->docklet->LoadGDIPlusImage(fullpath);

	if (image != NULL) {
		// Get the background color
		Color* background = new Color();
		background->SetFromCOLORREF(GetSysColor(COLOR_BTNFACE));

		// Create the bitmap
		Bitmap bitmap(image->GetHeight(),image->GetWidth());
		Graphics bitmapGraphics(&bitmap);
		SolidBrush solidBrush(*background);
		bitmapGraphics.FillRectangle(&solidBrush, 0, 0, image->GetHeight(), image->GetWidth());

		bitmapGraphics.DrawImage(image,0,0,image->GetHeight(),image->GetWidth());

		Bitmap* icon = (Bitmap*)bitmap.GetThumbnailImage(size,size,NULL,NULL);
		HBITMAP hBitmap;
		icon->GetHBITMAP(*background, &hBitmap);

		// Display the image
		SendDlgItemMessage(hDlg, ID_DIALOG, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM) hBitmap);

		delete(icon);
		delete(background);
	}

	delete(image);
}
