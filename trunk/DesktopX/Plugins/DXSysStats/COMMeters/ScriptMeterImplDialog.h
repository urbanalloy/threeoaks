// ScriptMeterImplDialog.h : Declaration of the ScriptMeterImplDialog

#ifndef __SCRIPTMETERIMPLDIALOG_H_
#define __SCRIPTMETERIMPLDIALOG_H_

#include "Dialog.h"

/////////////////////////////////////////////////////////////////////////////
// ScriptMeterImplDialog

class CScriptMeterImpl;

class ScriptMeterImplDialog : public Dialog
{
public:
	ScriptMeterImplDialog(CScriptMeterImpl *pMeter, WORD idd=IDD_SCRIPTMETERIMPL);
	virtual ~ScriptMeterImplDialog();

protected:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CScriptMeterImpl *pMeter;

public:
	BEGIN_MSG_MAP(ScriptMeterImplDialog)
		CHAIN_MSG_MAP(Dialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()
};
#endif // __SCRIPTMETERIMPLDIALOG_H_
