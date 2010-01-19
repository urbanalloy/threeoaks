#pragma once

#include <windows.h>
#include "MenuAction.h"
#include "SysStatsClientModel.h"

class SysStatsClientModelMenuAction : public MenuAction
{
public:
	SysStatsClientModelMenuAction(const char *_label, SysStatsClientModel *model, void (SysStatsClientModel::*pFunc)()) : model(model), pFunc(pFunc) {
		strncpy(label, _label, sizeof(label));
	}

	virtual void action() { (model->*pFunc)(); }
	virtual const char* getName() const { return label; }

private:
	SysStatsClientModel *model;
	void (SysStatsClientModel::*pFunc)();
	char label[80];
};