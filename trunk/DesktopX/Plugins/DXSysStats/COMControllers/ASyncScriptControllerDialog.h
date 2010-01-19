// ASyncScriptControllerDialog.h : Declaration of the ASyncScriptControllerDialog

#ifndef __ASYNCSCRIPTCONTROLLERDIALOG_H_
#define __ASYNCSCRIPTCONTROLLERDIALOG_H_

#include "Dialog.h"

/////////////////////////////////////////////////////////////////////////////
// ASyncScriptControllerDialog

class CASyncScriptController;

class ASyncScriptControllerDialog : public Dialog
{
public:
	ASyncScriptControllerDialog(CASyncScriptController *pController, IObserver *observer, IMeterHome *meters, WORD idd=IDD_ASYNCSCRIPTCONTROLLER);
	virtual ~ASyncScriptControllerDialog();

protected:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CASyncScriptController *pController;

public:
	BEGIN_MSG_MAP(ASyncScriptControllerDialog)
		CHAIN_MSG_MAP(Dialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()
};
#endif // __ASYNCSCRIPTCONTROLLERDIALOG_H_
