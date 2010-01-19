#pragma once

#include <windows.h>
#include "MenuAction.h"
#include "SysStatsApp.h"

class SysStatsAppMenuAction : public MenuAction
{
public:
	SysStatsAppMenuAction(const char *_label, SysStatsApp *app, void (SysStatsApp::*pFunc)()) : app(app), pFunc(pFunc) {
		strncpy(label, _label, sizeof(label));
	}

	virtual void action() { (app->*pFunc)(); }
	virtual const char* getName() const { return label; }

private:
	SysStatsApp *app;
	void (SysStatsApp::*pFunc)();
	char label[80];
};