/*
 * SysStats Widget Framework
 * Copyright (C) 2002-2006 Paul Andrews
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include "MenuAction.h"

#import "SysStatsCOM.tlb" no_namespace raw_interfaces_only

class SysStatsClientModelConfigure : public MenuAction
{
public:
	SysStatsClientModelConfigure(const char *_label, IClientModel *model, IObserver *observer, HWND hWnd) :
	  model(model), observer(observer), hWnd(hWnd)
	{
		strncpy(label, _label, sizeof(label));
	}

	virtual void action() { model->Configure(observer, (long)hWnd); }
	virtual const char* getName() const { return label; }

private:
	IClientModel *model;
	IObserver *observer;
	HWND hWnd;
	char label[80];
};