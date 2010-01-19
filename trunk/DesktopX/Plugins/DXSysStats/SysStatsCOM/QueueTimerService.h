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

// QueueTimerService.h: interface for the QueueTimerService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUEUETIMERSERVICE_H__52152A42_C7DA_4DEF_8D61_E674D58C49BF__INCLUDED_)
#define AFX_QUEUETIMERSERVICE_H__52152A42_C7DA_4DEF_8D61_E674D58C49BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SysStatsCOM.h"
#include "SysStatsUtils.h"
#include "APVDummy.h"
#include "DictionaryPtr.h"

struct TimerData;

class __declspec(dllexport) QueueTimerService : public ICOMTimerService  
{
public:
	QueueTimerService();
	virtual ~QueueTimerService();

	STDMETHOD(SetTimerClient)(ITimerClient *pClient);
	STDMETHOD(RemoveTimerClient)(ITimerClient *pClient);
	STDMETHOD(ClearAllTimers)();
	STDMETHOD(UpdateTimerClients)();

	// QueueTimerService-specific methods
	void setHWND(HWND hWnd);
	HWND getHWND() { return hWnd; }
	BOOL handleTimerMessage(UINT id);

	// Those pesky IUnknown methods:
	DUMMY_DUAL

	static const UINT MESSAGE;

private:
	QueueTimerService(const QueueTimerService&);

	DictionaryPtr<ITimerClient, TimerData> clients;
	/*
	DictionaryPtr<void, ITimerClient> idToClientMap;
	DictionaryPtr<ITimerClient, void> clientToIdMap;
	*/
	HWND hWnd;
	HANDLE hQueue;
};

#endif // !defined(AFX_QUEUETIMERSERVICE_H__52152A42_C7DA_4DEF_8D61_E674D58C49BF__INCLUDED_)
