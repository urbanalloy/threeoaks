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
#include "ArrayPtrVariant.cpp"
#include "SysStatsUtils.h"
#include <map>

#import "SysStatsCOM.tlb" raw_interfaces_only raw_native_types no_namespace named_guids 

class WindowTimerService : public ICOMTimerService
{
public:
	WindowTimerService();
	virtual ~WindowTimerService();

	STDMETHOD(SetTimerClient)(ITimerClient *pClient);
	STDMETHOD(RemoveTimerClient)(ITimerClient *pClient);
	STDMETHOD(ClearAllTimers)();
	STDMETHOD(UpdateTimerClients)();

	// WindowTimerService-specific methods
	void setHWND(HWND hWnd);
	HWND getHWND() { return hWnd; }
	BOOL handleTimerMessage(UINT id);

	// Those pesky IUnknown methods:
	DUMMY_DUAL

private:
//	ArrayPtrVariant<ITimerClient> clients;
	std::map<double, ArrayPtrVariant<ITimerClient>* > m_Clients;

	/*
	DictionaryPtr<void, ITimerClient> idToClientMap;
	DictionaryPtr<ITimerClient, void> clientToIdMap;
	*/
	HWND hWnd;

	static UINT timerIds;
};