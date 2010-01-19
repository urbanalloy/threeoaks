// ASyncScriptController.cpp : Implementation of ASyncScriptControllerDialog.cpp
#include "stdafx.h"
#include "COMControllers.h"
#include "ASyncScriptController.h"
#include "ASyncScriptControllerDialog.h"

ASyncScriptControllerDialog::ASyncScriptControllerDialog(CASyncScriptController *pController, IObserver *observer, IMeterHome *meters, WORD idd) :
	pController(pController),
	Dialog(idd)
{
}

ASyncScriptControllerDialog::~ASyncScriptControllerDialog()
{
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 * Should initialize all of the controls using values from the meter.
 */
LRESULT ASyncScriptControllerDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;	// Make CWindow go through all the message maps.

	return 1;
}
