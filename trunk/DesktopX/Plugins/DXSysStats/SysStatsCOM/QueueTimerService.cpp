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

// QueueTimerService.cpp: implementation of the QueueTimerService class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "QueueTimerService.h"
#include "DictionaryPtr.cpp"

const UINT QueueTimerService::MESSAGE = WM_USER+143;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

struct TimerData
{
	TimerData(HWND hWnd, HANDLE hTimer, ITimerClient *pClient) :
		m_hWnd(hWnd), hTimer(hTimer), pClient(pClient)
	{}

	HANDLE hTimer;
	HWND m_hWnd;
	ITimerClient *pClient;
};

bool ClientEQ(const ITimerClient* lhs, const ITimerClient* rhs)
{
	return lhs == rhs;
}

static VOID CALLBACK QTS_Handler(      
  PVOID lpParameter,
  BOOLEAN TimerOrWaitFired
)
{
	::PostMessage(((TimerData*)lpParameter)->m_hWnd, QueueTimerService::MESSAGE, (UINT)(((TimerData*)lpParameter)->pClient), 0);
}

QueueTimerService::QueueTimerService() : hQueue(0), clients(ClientEQ)
{
	hQueue = ::CreateTimerQueue();
}

// Private so should never be called
QueueTimerService::QueueTimerService(const QueueTimerService&) : clients(ClientEQ)
{
}

QueueTimerService::~QueueTimerService()
{
//	ClearAllTimers();
}

void QueueTimerService::setHWND(HWND hWnd)
{
	this->hWnd = hWnd;

	short count = clients.count();
	ITimerClient **keys = new ITimerClient*[count];
	clients.getKeys(keys);

	for (int i=count-1; i >= 0; i--)
	{
		TimerData *data = clients.get(keys[i]);
		data->m_hWnd = hWnd;
	}

	delete[] keys;
}

STDMETHODIMP QueueTimerService::ClearAllTimers()
{
	short count = clients.count();
	ITimerClient **keys = new ITimerClient*[count];
	clients.getKeys(keys);

	for (int i=count-1; i >= 0; i--)
	{
		TimerData *data = clients.get(keys[i]);
		::DeleteTimerQueueTimer(hQueue, data->hTimer, NULL);
		clients.remove(keys[i]);
		delete data;
	}

	delete[] keys;

	clients.clear();

	return S_OK;
}

STDMETHODIMP QueueTimerService::SetTimerClient(ITimerClient *pClient)
{
	TimerData *data = clients.get(pClient);

	double time;
	pClient->get_TimerInterval(&time);

	if (time == 0.0 && data != 0)
		RemoveTimerClient(pClient);
	else
	{
		if (!data)
		{
			data = new TimerData(hWnd, NULL, pClient);
			clients.put(pClient, data);
		}
		else
			::DeleteTimerQueueTimer(hQueue, data->hTimer, NULL);

		AtlTrace("Setting timer to %ld\n", (long)(time * 1000));
		::CreateTimerQueueTimer(&data->hTimer, hQueue, QTS_Handler, (PVOID)data, (long)(time * 1000), (long)(time * 10000), WT_EXECUTEINTIMERTHREAD);
	}

	return S_OK;
}

STDMETHODIMP QueueTimerService::RemoveTimerClient(ITimerClient *pClient)
{
	TimerData *data = clients.get(pClient);

	if (data != NULL)
	{
		::DeleteTimerQueueTimer(hQueue, data->hTimer, NULL);
		clients.remove(pClient);
		delete data;
	}

	return S_OK;
}

STDMETHODIMP QueueTimerService::UpdateTimerClients()
{
	// One or more of our clients may have changed their timer interval
	short count = clients.count();
	ITimerClient **keys = new ITimerClient*[count];
	clients.getKeys(keys);

	for (int i=count-1; i >= 0; i--)
	{
		double time;
		keys[i]->get_TimerInterval(&time);
		TimerData *data = clients.get(keys[i]);
		::DeleteTimerQueueTimer(hQueue, data->hTimer, NULL);
		::CreateTimerQueueTimer(&data->hTimer, hQueue, QTS_Handler, (PVOID)data, (long)(time * 1000), (long)(time * 10000), WT_EXECUTEINTIMERTHREAD);
	}

	delete[] keys;
	
	return S_OK;
}

/*
 * So the timer handler posts a WM_USER+143 message. The windows loop needs
 * to call this with the argument equal to wParam. This dispatches it to the timer client.
 */
BOOL QueueTimerService::handleTimerMessage(UINT idEvent)
{
	ITimerClient *tc = (ITimerClient*)idEvent;
	tc->TimerFired();

	return TRUE;
}


