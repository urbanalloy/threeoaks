//
// AboutBox.h : Declaration of CAboutBox
//

#ifndef __ABOUTBOX_H_
#define __ABOUTBOX_H_

#include "resource.h"       // main symbols
#include <atlhost.h>
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CAboutBox
class CAboutBox : 
	public CDialogImpl<CAboutBox>
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
		END_MSG_MAP()

		// Handler prototypes:

		LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnDismiss(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	private:
		void GetModuleVersionInfo (char* pszVersion);
};


#endif //__ABOUTBOX_H_
