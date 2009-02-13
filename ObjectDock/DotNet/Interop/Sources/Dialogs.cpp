///////////////////////////////////////////////////////////////////////////////////////////////
//
// DotNetDocklets : bringing .NET to ObjectDock
//
// Copyright (c) 2004-2009, Julien Templier
// All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer. 
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution. 
//  3. The name of the author may not be used to endorse or promote products derived from this
//     software without specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "Interop.h"
#include "Dialogs.h"
#include "resource.h"

// Build numbers
#include "version.h"

#include <algorithm>

using namespace std;

//////////////////////////////////////////////////////////////////////////
int CALLBACK ConfigureDocklet(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	DOCKLET_DATA *data = (DOCKLET_DATA *) GetProp(hDlg, "data");

	switch(iMsg)
	{
		case WM_INITDIALOG:
			LOG("Entering Config Dialog...\n");
			SetWindowText(hDlg, "Choose the .Net Docklet to load...");

			// Get Data
			SetProp(hDlg, "data", (HANDLE) (char*) lParam);
			data = (DOCKLET_DATA *) GetProp(hDlg, "data");
			if(!data)
			{
				EndDialog(hDlg, 0);
				return TRUE;
			}				

			// Populate the listbox
			LOG("  Populating the listbox\n");
			for (int i = 0; i < (signed)data->docklets.size(); i++) {
				SendDlgItemMessage( hDlg, IDC_DOCKLET_LIST, LB_ADDSTRING,
								   (WPARAM)NULL, (LPARAM)data->docklets[i].name);
			}

			// Select the first string & show the info
			if ((signed)data->docklets.size() != 0) {
				LOG("  Selecting the first item\n");
				SendDlgItemMessage( hDlg, IDC_DOCKLET_LIST, LB_SETCURSEL, (WPARAM)0, (LPARAM)NULL);
				showInfo(hDlg, data, 0);
				if (data->docklets[0].invalid)
					EnableWindow(GetDlgItem(hDlg, ID_OK), false);
			}

			// Set the version text
			char version[50];
			sprintf_s(version, 50*sizeof(char), ".Net Docklet Interop v%i.%i Build %i", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
			SetDlgItemText(hDlg, IDC_TXT_VERSION,version);

			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_DOCKLET_LIST:
					// Show the info for selected docklet
					if (HIWORD(wParam) == LBN_SELCHANGE) {
						int index = SendDlgItemMessage(hDlg, IDC_DOCKLET_LIST, LB_GETCURSEL, (WPARAM)NULL, (LPARAM)NULL);
						showInfo(hDlg, data, index);

						// Deactivate the OK button if invalid
						if (data->docklets[index].invalid)
							EnableWindow(GetDlgItem(hDlg, ID_OK), false);						
						else
							EnableWindow(GetDlgItem(hDlg, ID_OK), true);

					}
					break;

				case ID_OK:
					if(!data)
						break;

					// Get the selected docklet index & set docklet info
					if ((signed)data->docklets.size() != 0) {
						data->index = SendDlgItemMessage(hDlg, IDC_DOCKLET_LIST, LB_GETCURSEL, (WPARAM)NULL, (LPARAM)NULL);
						data->autoload = true;
						LOG("  Selected Docklet: "); LOG(data->docklets[data->index].name); LOG("\n");
					}
					
					// no break on purpose
				case ID_CANCEL:
					EndDialog(hDlg, 0);
					return TRUE;
			}
			break;

		case WM_DESTROY:
			RemoveProp(hDlg, "data");
			LOG("Quitting Config Dialog\n\n");
			break;

		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return TRUE;

	}

	return FALSE;
}

void showInfo(HWND hDlg, DOCKLET_DATA *data, int selected)
{
	int size = 128;

	// Get the full icon path
	char fullpath[FOLDER_SIZE];
	if (strcmp(data->docklets[selected].icon, "") == 0)
		sprintf(fullpath, "%s%s\\%s", data->static_data.rootFolder,
									  data->static_data.interopFolder,
									  DOCKLET_ICON);
	else
		sprintf(fullpath, "%s%s%s\\%s", data->static_data.rootFolder,
										data->static_data.interopFolder,
										data->docklets[selected].path,
										data->docklets[selected].icon);

	// Load the image
	Bitmap* image = DockletLoadGDIPlusImage(fullpath);
	
	if (image != NULL) {
		// Get the background color
		Color* background = new Color();
		background->SetFromCOLORREF(GetSysColor(COLOR_BTNFACE));

		// Create the bitmap
		Bitmap bitmap(size+64,size+64);
		Graphics bitmapGraphics(&bitmap);
		SolidBrush solidBrush(*background);
		bitmapGraphics.FillRectangle(&solidBrush, 0, 0, image->GetHeight()+64, image->GetWidth()+64);

		// Apply color transformation and draw the icon
		RectF rect(0.0f, 0.0f, (REAL)size+64, (REAL)size+64);
		ImageAttributes attributes;
		ColorMatrix colorMatrix = {
				0.45f, 0.33f, 0.33f, 0.0f, 0.0f,
				0.33f, 0.33f, 0.33f, 0.0f, 0.0f,
				0.33f, 0.33f, 0.33f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.1f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

		attributes.SetColorMatrix(&colorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeDefault);
		bitmapGraphics.DrawImage(image, rect, 0, 0, (REAL)image->GetHeight(), (REAL)image->GetWidth(), UnitPixel, &attributes, NULL, NULL);

		// Init Brushes & Font
		SolidBrush  blackBrush(Color(255, 0, 0, 0));
		SolidBrush  whiteBrush(Color(128, 160, 160, 160));
		SolidBrush  grayBrush(Color(255, 128, 128, 128));
		FontFamily  fontFamily(L"Arial");
		Font        font(&fontFamily, 13, FontStyleBold, UnitPixel);
		Font        smallFont(&fontFamily, 10, FontStyleBold, UnitPixel);

		// Draw the name, author, version and notes
		IDockletInterface *cpi = NULL;

		CoInitialize(NULL);
		HRESULT hr = CREATE_INSTANCE(data->docklets[selected].CLSID_Docklet, IID_IDockletInterface, &cpi);
		
		if (SUCCEEDED(hr)) {
			// Get the info from the .net DLL
			BSTR name1 = NULL; BSTR author1 = NULL; BSTR notes1 = NULL; long version1 = 0;
			cpi->OnGetInformation(&name1, &author1, &version1, &notes1);

			wchar_t name[200];
			wcscpy(name, L"");
			wcscat(name, name1);

			wchar_t notes[500];
			wcscpy(notes, L"");
			wcscat(notes, notes1);

			wchar_t author[200];
			wcscpy(author, L"By ");
			wcscat(author, author1);

			int major = version1/100;
			int minor = version1 - 100*major;

			wchar_t version[40];
			wcscpy(version, L"");
			if (minor > 9)
				swprintf(version, 40, L"Version %d.%d", major, minor);
			else
				swprintf(version, 40, L"Version %d.0%d", major, minor);

			////////////////////////////////////////////////////////////////////////////
			bitmapGraphics.DrawString((WCHAR *)&name, -1, &font, PointF(1.0f, 1.0f), &whiteBrush);
			bitmapGraphics.DrawString((WCHAR *)&name, -1, &font, PointF(0.0f, 0.0f), &blackBrush);

			bitmapGraphics.DrawString((WCHAR *)&author, -1, &font, PointF(1.0f, 21.0f), &whiteBrush);
			bitmapGraphics.DrawString((WCHAR *)&author, -1, &font, PointF(0.0f, 20.0f), &blackBrush);

			bitmapGraphics.DrawString((WCHAR *)&version, -1, &font, PointF(1.0f, 41.0f), &whiteBrush);
			bitmapGraphics.DrawString((WCHAR *)&version, -1, &font, PointF(0.0f, 40.0f), &blackBrush);

			RectF rect1(1.0f, 101.0f, 200.0f, 200.0f);
			RectF rect2(0.0f, 100.0f, 200.0f, 200.0f);
			bitmapGraphics.DrawString((WCHAR *)&notes, -1, &font, rect1, StringFormat::GenericDefault(), &whiteBrush);
			bitmapGraphics.DrawString((WCHAR *)&notes, -1, &font, rect2, StringFormat::GenericDefault(), &blackBrush);
			////////////////////////////////////////////////////////////////////////////

			SysFreeString(name1); name1 = NULL;
			SysFreeString(author1); author1 = NULL;
			SysFreeString(notes1); notes1 = NULL;

			cpi->Release();
			cpi = NULL;

		}
		else
		{
			// Show an error Message
			wchar_t message[200];
			wcscpy(message, L"This docklet is not a valid .Net Docklet...");

			RectF rect1(1.0f, 71.0f, 201.0f, 401.0f);
			RectF rect2(0.0f, 70.0f, 200.0f, 400.0f);
			bitmapGraphics.DrawString((WCHAR *)&message, -1, &font, rect1, StringFormat::GenericDefault(), &whiteBrush);
			bitmapGraphics.DrawString((WCHAR *)&message, -1, &font, rect2, StringFormat::GenericDefault(), &blackBrush);

			// Set the docklet as invalid
			data->docklets[selected].invalid = true;
		}
		
		// If we are set as invalid, this means we got an error registering.
		// Assume we are running a version of the SDK that is too old for this docklet
		if (data->docklets[selected].invalid) 
		{
		  wchar_t message[200];
		  wcscpy(message, L"This docklet needs a newer version of the ObjectDock .Net SDK...");

		  RectF rect1(1.0f, 166.0f, 201.0f, 401.0f);
		  RectF rect2(0.0f, 165.0f, 200.0f, 400.0f);
		  bitmapGraphics.DrawString((WCHAR *)&message, -1, &smallFont, rect1, StringFormat::GenericDefault(), &whiteBrush);
		  bitmapGraphics.DrawString((WCHAR *)&message, -1, &smallFont, rect2, StringFormat::GenericDefault(), &grayBrush);
		}			
			
		// clean up COM
		CoUninitialize();

		HBITMAP hBitmap;
		bitmap.GetHBITMAP(*background, &hBitmap);

		// Display the image
		SendDlgItemMessage(hDlg, IDC_ICON_PIC, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM) hBitmap);
		delete(background);
	}
	delete(image);
}
