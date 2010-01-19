#include "DialogMenuAction.h"
#include "Dialog.h"

DialogMenuAction::DialogMenuAction(Dialog *dialog, HWND hWnd, HINSTANCE hInstance) :
	dialog(dialog), hWnd(hWnd), hInstance(hInstance)
{
}

void DialogMenuAction::action()
{
	dialog->show(hWnd, hInstance);
}

const char* DialogMenuAction::getName() const
{
	return dialog->getName();
}

