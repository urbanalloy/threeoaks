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

#pragma once

#include "resource.h"       // main symbols
#include <atlhost.h>
#include <vector>

namespace Flurry {

	class CAboutBox : public CDialogImpl<CAboutBox>
	{
		public:
			// "factory"
			static void AutomaticDoModal (void)
			{
				CAboutBox* pAboutBox = new CAboutBox;
				pAboutBox->DoModal();
				delete pAboutBox;
			}

			// constructors
			CAboutBox() {};
			~CAboutBox() {};

			enum { IDD = IDD_ABOUTBOX };

		protected:
			BEGIN_MSG_MAP(CAboutBox)
				MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
				COMMAND_ID_HANDLER(IDCANCEL, OnDismiss)			// bind both IDOK and IDCANCEL to the dismiss function, as well as WM_CLOSE
				COMMAND_ID_HANDLER(IDOK, OnDismiss)				// of OK and CANCEL, CANCEL is preferred because then Esc works as well as Enter
				COMMAND_ID_HANDLER(IDC_CREDITS, OnCredits)		// Show Credits dialog
			END_MSG_MAP()

			// Handler prototypes:

			LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
			LRESULT OnDismiss(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
			LRESULT OnCredits(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);			
	};

}
