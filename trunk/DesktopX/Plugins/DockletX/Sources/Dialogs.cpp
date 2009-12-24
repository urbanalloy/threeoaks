///////////////////////////////////////////////////////////////////////////////////////////////
//
// DockletX - Docklet support plugin for DesktopX
//
// Copyright (c) 2006-2010, Julien Templier
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

#include "stdafx.h"

#include "DockletX.h"
#include "CGdiPlusBitmap.h"
#include "Dialogs.h"
#include "resource.h"
#include "version.h"

#include <commctrl.h>

#include <algorithm>
using namespace std;

extern HINSTANCE dllInstance;

//////////////////////////////////////////////////////////////////////////
int CALLBACK ConfigurePlugin(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	PLUGIN_DATA *data = (PLUGIN_DATA *) GetProp(hDlg, L"data");

	switch(iMsg)
	{
		case WM_INITDIALOG:
			// Get Data
			SetProp(hDlg, L"data", (HANDLE) (char*) lParam);
			data = (PLUGIN_DATA *) GetProp(hDlg, L"data");

			// Populate the listbox
			for (int i = 0; i < (signed)data->docklets.size(); i++) {
				SendDlgItemMessageA( hDlg, IDC_DOCKLET_LIST, LB_ADDSTRING,
								   (WPARAM)NULL, (LPARAM)data->docklets[i].info.name);
			}

			// Select the first string & show the info
			if ((signed)data->docklets.size() != 0) {
				int index = (data->index == -1) ? 0 : data->index;
				SendDlgItemMessage( hDlg, IDC_DOCKLET_LIST, LB_SETCURSEL, (WPARAM)index, (LPARAM)NULL);
				showInfo(hDlg, data, 0);
			}

			// Set the version text
			char version[50];
			sprintf_s(version, "DockletX v%i.%i Build %i", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
			SetDlgItemTextA(hDlg, IDC_TXT_VERSION,version);

			if (!data)
				EnableWindow(GetDlgItem(hDlg, ID_OK), false);

			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_DOCKLET_LIST:
					// Show the info for selected docklet
					if (HIWORD(wParam) == LBN_SELCHANGE) {
						int index = SendDlgItemMessage(hDlg, IDC_DOCKLET_LIST, LB_GETCURSEL, (WPARAM)NULL, (LPARAM)NULL);
						showInfo(hDlg, data, index);
					}
					break;

				case ID_OK:
					if(!data)
						break;

					// Get the selected docklet index & set docklet info
					if ((signed)data->docklets.size() != 0) {
						data->index = SendDlgItemMessage(hDlg, IDC_DOCKLET_LIST, LB_GETCURSEL, (WPARAM)NULL, (LPARAM)NULL);
					}

					EndDialog(hDlg, 0);
					return TRUE;
				case ID_CANCEL:
					data->index = -1;
					EndDialog(hDlg, 0);
					return TRUE;
			}
			break;

		case WM_DESTROY:
			RemoveProp(hDlg, L"data");
			break;

		case WM_CLOSE:
			data->index = -1;
			EndDialog(hDlg, 0);
			return TRUE;

	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
int CALLBACK ConfigureDisplay(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	Docklet *data = (Docklet*) GetProp(hDlg, L"data");
	char* image = (char*) GetProp(hDlg, L"image");

	switch(iMsg)
	{
		case WM_INITDIALOG:
		{
			// Prepare Data
			SetProp(hDlg, L"data", (HANDLE) (char*) lParam);
			data = (Docklet*) GetProp(hDlg, L"data");

			char* path = (char*)malloc(MAX_PATH*sizeof(char));
			strcpy_s(path, MAX_PATH*sizeof(char), "");
			data->GetImageFile((char*)path);
			SetProp(hDlg, L"image", (HANDLE) (char*) path);

			image = (char*) GetProp(hDlg, L"image");

			showImage(hDlg, image, data);

			// Init the slider control
			SendDlgItemMessage(hDlg, IDC_DOCKLET_SIZE, TBM_SETRANGE, FALSE, MAKELONG(DOCKLET_SIZE_MIN, DOCKLET_SIZE_MAX));
			SendDlgItemMessage(hDlg, IDC_DOCKLET_SIZE, TBM_SETTICFREQ, DOCKLET_SIZE_MIN, NULL);

			SendDlgItemMessage(hDlg, IDC_DOCKLET_SIZE, TBM_SETPOS, TRUE, (LONG)data->GetSize());

			break;
		}

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_RESET_IMAGE:
					strcpy_s(image, MAX_PATH*sizeof(char), "");
					showImage(hDlg, image, data);
					break;

				case IDC_CHANGE_IMAGE:
					strcpy_s(image,  MAX_PATH*sizeof(char), "");
					data->BrowseForImage(hDlg, image);

					showImage(hDlg, image, data);
					break;

				case ID_OK:
					if(!data)
						break;

					if (strcmp(image, "") == 0)
						data->ShowDefaultImage();
					else
						data->SetImageFile(image);

					data->SetSize((int)SendDlgItemMessage(hDlg, IDC_DOCKLET_SIZE, TBM_GETPOS, NULL, NULL));

					// no break on purpose

				case ID_CANCEL:
					EndDialog(hDlg, 0);
					return TRUE;
			}
			break;

		case WM_DESTROY:
			RemoveProp(hDlg, L"data");
			free(image);
			RemoveProp(hDlg, L"image");
			break;

		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
#pragma warning(push)
#pragma warning(disable: 4100)
void showImage(HWND hDlg, char* image, Docklet* docklet)
{
	// Get the background color
	Color* background = new Color();
	background->SetFromCOLORREF(GetSysColor(COLOR_BTNFACE));

#define ICON_SIZE_PREVIEW 96
	// Create the bitmap
	Bitmap bitmap(ICON_SIZE_PREVIEW, ICON_SIZE_PREVIEW);
	Graphics bitmapGraphics(&bitmap);
	SolidBrush solidBrush(*background);
	bitmapGraphics.FillRectangle(&solidBrush, 0, 0, ICON_SIZE_PREVIEW, ICON_SIZE_PREVIEW);

	RectF rect(0.0f, 0.0f, (REAL)ICON_SIZE_PREVIEW, (REAL)ICON_SIZE_PREVIEW);

	if (strcmp(image, "") == 0)
	{
		CGdiPlusBitmapResource* res = new CGdiPlusBitmapResource();
		res->Load(IDB_BACKGROUND, _T("PNG"), dllInstance);

		bitmapGraphics.DrawImage(res->m_pBitmap, rect, 0.0f, 0.0f, (REAL)res->m_pBitmap->GetWidth(), (REAL)res->m_pBitmap->GetHeight(), UnitPixel);

		delete res;
	}
	else
	{
		Bitmap* img = docklet->LoadGDIPlusImage(image);
		bitmapGraphics.DrawImage(img, rect, 0.0f, 0.0f, (REAL)img->GetWidth(), (REAL)img->GetHeight(), UnitPixel);

		delete img;
	}

	HBITMAP hBitmap;
	bitmap.GetHBITMAP(*background, &hBitmap);

	// Display the image
	SendDlgItemMessage(hDlg, IDC_DOCKLET_IMAGE, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM) hBitmap);
	delete(background);
}
#pragma warning(pop)

//////////////////////////////////////////////////////////////////////////
void showInfo(HWND hDlg, PLUGIN_DATA *data, int selected)
{
	int size = 128;

	// Load the background image
	CGdiPlusBitmapResource* image = new CGdiPlusBitmapResource();
	image->Load(IDB_BACKGROUND, _T("PNG"), dllInstance);

	if (image->m_pBitmap != NULL) {
		// Get the background color
		Color* background = new Color();
		background->SetFromCOLORREF(GetSysColor(COLOR_BTNFACE));

		// Create the bitmap
		Bitmap bitmap(size+64,size+64);
		Graphics bitmapGraphics(&bitmap);
		SolidBrush solidBrush(*background);
		bitmapGraphics.FillRectangle(&solidBrush, 0, 0, image->m_pBitmap->GetHeight()+64, image->m_pBitmap->GetWidth()+64);

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
		bitmapGraphics.DrawImage(image->m_pBitmap, rect, 0, 0, (REAL)image->m_pBitmap->GetHeight(), (REAL)image->m_pBitmap->GetWidth(), UnitPixel, &attributes, NULL, NULL);

		// Init Brushes & Font
		SolidBrush  blackBrush(Color(255, 0, 0, 0));
		SolidBrush  whiteBrush(Color(128, 160, 160, 160));
		FontFamily  fontFamily(L"Arial");
		Font        font(&fontFamily, 13, FontStyleBold, UnitPixel);

		// Draw the name, author, version and notes
		Docklet::DOCKLET_INFO info = Docklet::GetInformationFromDll((char*)data->docklets[selected].path);

		USES_CONVERSION;

		wchar_t name[200];
		wcscpy_s(name, L"");
		wcscat_s(name, CA2W(info.name));

		wchar_t notes[500];
		wcscpy_s(notes, L"");
		wcscat_s(notes, CA2W(info.notes));

		wchar_t author[200];
		wcscpy_s(author, L"By ");
		wcscat_s(author, CA2W(info.author));

		int major = info.version/100;
		int minor = info.version - 100*major;

		wchar_t version[40];
		wcscpy_s(version, L"");
		if (minor > 9)
			swprintf_s(version, L"Version %d.%d", major, minor);
		else
			swprintf_s(version, L"Version %d.0%d", major, minor);

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

		HBITMAP hBitmap;
		bitmap.GetHBITMAP(*background, &hBitmap);

		// Display the image
		SendDlgItemMessage(hDlg, IDC_ICON_PIC, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM) hBitmap);
		delete(background);
	}
	delete(image);
}
