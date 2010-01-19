// ScriptMeterImpl.cpp : Implementation of ScriptMeterImplDialog.cpp
#include "stdafx.h"
#include "COMMeters.h"
#include "ScriptMeterImpl.h"
#include "ScriptMeterImplDialog.h"

ScriptMeterImplDialog::ScriptMeterImplDialog(CScriptMeterImpl *pMeter, WORD idd) :
	pMeter(pMeter),
	Dialog(idd)
{
}

ScriptMeterImplDialog::~ScriptMeterImplDialog()
{
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 * Should initialize all of the controls using values from the meter.
 */
LRESULT ScriptMeterImplDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;	// Make CWindow go through all the message maps.

	return 1;
}
