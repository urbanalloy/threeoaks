#pragma once

#include "Dialog.h"

class CompositeOverlay;
class Overlay;

class CompositeOverlayDialog : public Dialog
{
public:
	CompositeOverlayDialog(CompositeOverlay *overlay, const char *name, Observer *observer);
	virtual ~CompositeOverlayDialog();

	/*
	 * Creates the on-screen dialog
	 */
	virtual void show(HWND, HINSTANCE);

	/*
	 * Called from the dialog window proc
	 */
	virtual int init(HWND hDlg);

	int x(int x);
	int y(int y);
	int rotation(int rotation);

protected:
	virtual int handleCommand(WPARAM wParam, LPARAM lParam);

	Overlay *overlay;
};