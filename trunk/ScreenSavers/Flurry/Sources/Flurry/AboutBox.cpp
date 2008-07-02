//
// AboutBox.cpp : Implementation of CAboutBox
//

//#include "stdafx.h"
#include <atlbase.h>
#include <atlcom.h>
#include "AboutBox.h"
#include "resource.h"
#include "VersionNo.h"

#pragma warning(disable:4100)

static char szCompileTime[] = __TIME__ " on " __DATE__;


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
