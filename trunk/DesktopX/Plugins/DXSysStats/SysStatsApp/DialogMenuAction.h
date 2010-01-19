#pragma once

#include <windows.h>
#include "MenuAction.h"

class Dialog;

class DialogMenuAction : public MenuAction
{
public:
	DialogMenuAction(Dialog *dialog, HWND hWnd, HINSTANCE hInstance);

	virtual void action();
	virtual const char* getName() const;

private:
	Dialog *dialog;
	HWND hWnd;
	HINSTANCE hInstance;
};