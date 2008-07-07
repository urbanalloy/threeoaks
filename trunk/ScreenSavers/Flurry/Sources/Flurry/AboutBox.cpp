///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : CAboutBox Class
//
// Declaration of CAboutBox
//
// (C) 2003 Matt Ginzton (magi@cs.stanford.edu)
// (C) 2006-2008 Julien Templier
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////

#include <atlbase.h>
#include <atlcom.h>
#include "AboutBox.h"
#include "resource.h"
#include "VersionNo.h"

using namespace Flurry;

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
