
#include <stdio.h>
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include "resource.h"

#include "CompositeOverlayDialog.h"
#include "App.h"
#include "CompositeOverlay.h"

CompositeOverlayDialog::CompositeOverlayDialog(CompositeOverlay *overlay, const char *name, Observer *observer) :
	Dialog(name, observer)
{
	this->overlay = overlay;
}

CompositeOverlayDialog::~CompositeOverlayDialog()
{
}

/*
 * Called from the main docklet class.
 */
void CompositeOverlayDialog::show(HWND hwnd, HINSTANCE hInstance)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_COMPOSITEOVERLAY), hwnd, Dialog::proc, (LPARAM) this);
}

int CompositeOverlayDialog::x(int x)
{
	overlay->setX(x);
	return TRUE;
}

int CompositeOverlayDialog::y(int y)
{
	overlay->setY(y);
	return TRUE;
}

int CompositeOverlayDialog::rotation(int rotation)
{
	overlay->setRotation(rotation);
	return TRUE;
}

/*
 * Called from the dialog proc when handling command input.
 */
int CompositeOverlayDialog::handleCommand(WPARAM wParam, LPARAM lParam)
{
	// Call superclass handleCommand() first
	boolean ret = Dialog::handleCommand(wParam, lParam);

	if (!ret)
	{
		BOOL iRet = TRUE;	// For GetDlgItemInt
		int i = 0;			// Ditto

		switch(LOWORD(wParam))
		{
		case IDC_XPOS:
			i = GetDlgItemInt(hDlg, LOWORD(wParam), &iRet, TRUE);
			if (iRet)
				ret = x(i);
			break;

		case IDC_YPOS:
			i = GetDlgItemInt(hDlg, LOWORD(wParam), &iRet, TRUE);
			if (iRet)
				ret = y(i);
			break;

		case IDC_ROT:
			i = GetDlgItemInt(hDlg, LOWORD(wParam), &iRet, TRUE);
			if (iRet)
				ret = rotation(i);
			break;
		}
	}

	return ret;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
int CompositeOverlayDialog::init(HWND hDlg)
{
	// Call superclass init() first
	Dialog::init(hDlg);

	SendDlgItemMessage(hDlg,IDC_XSPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(300, -100));
	SetDlgItemInt(hDlg, IDC_XPOS, (UINT)overlay->getX(), TRUE);

	SendDlgItemMessage(hDlg,IDC_YSPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(-100, 300));
	SetDlgItemInt(hDlg, IDC_YPOS, (UINT)overlay->getY(), TRUE);

	SendDlgItemMessage(hDlg,IDC_ROTSPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(359, 0));
	SetDlgItemInt(hDlg, IDC_ROT, (UINT)overlay->getRotation(), TRUE);

	return TRUE;
}
