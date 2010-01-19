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

#include "stdafx.h"

#include "WindowTimerService.h"

static VOID CALLBACK WTS_Handler(      
    HWND hwnd,
    UINT uMsg,
    UINT_PTR idEvent,
    DWORD dwTime
)
{
	ArrayPtrVariant<ITimerClient> *pClients = (ArrayPtrVariant<ITimerClient>*)idEvent;

	short count;
	pClients->Count(&count);
	for (int i=0; i<count; i++)
	{
		ITimerClient *pClient;
		pClients->Item(CComVariant(i), &pClient);
		pClient->TimerFired();
		pClient->Release();
	}
}

WindowTimerService::WindowTimerService()
{
}

WindowTimerService::~WindowTimerService()
{
	MSG msg;
	::PeekMessage(&msg, hWnd, WM_TIMER, WM_TIMER, PM_REMOVE);
	ClearAllTimers();
}

void WindowTimerService::setHWND(HWND hWnd)
{
	this->hWnd = hWnd;

	std::map<double, ArrayPtrVariant<ITimerClient>* >::iterator iter = m_Clients.begin();
	while (iter != m_Clients.end())
	{
		ArrayPtrVariant<ITimerClient> *pClients = iter->second;
		if (iter->first == 0.0)
			KillTimer(hWnd, (UINT)pClients);
		else
			SetTimer(hWnd, (UINT)pClients, (long)(iter->first * 1000), WTS_Handler);

		iter++;
	}
}
 
STDMETHODIMP WindowTimerService::ClearAllTimers()
{
	std::map<double, ArrayPtrVariant<ITimerClient>* >::iterator iter = m_Clients.begin();
	while (iter != m_Clients.end())
	{
		ArrayPtrVariant<ITimerClient> *pClients = iter->second;
		if (hWnd != NULL)
			KillTimer(hWnd, (UINT)pClients);
		pClients->ClearAndDestroy();
		delete pClients;

		iter++;
	}
	m_Clients.clear();

	return S_OK;
}

STDMETHODIMP WindowTimerService::SetTimerClient(ITimerClient *pClient)
{
	RemoveTimerClient(pClient);	// In case the interval has changed.

	double time;
	pClient->get_TimerInterval(&time);

	ArrayPtrVariant<ITimerClient>* pClients = NULL;
	std::map<double, ArrayPtrVariant<ITimerClient>* >::iterator iter = m_Clients.find(time);
	if (iter == m_Clients.end())
	{
		m_Clients[time] = pClients = new ArrayPtrVariant<ITimerClient>();
	}
	else
		pClients = iter->second;

	short i;
	pClients->GetIndex(pClient, &i);
	if (i == -1)
		pClients->Add(pClient);

	if (hWnd != NULL)
	{
		if (time == 0.0)
		{
			KillTimer(hWnd, (UINT)pClients);
		}
		else
			SetTimer(hWnd, (UINT)pClients, (long)(time * 1000), WTS_Handler);
	}

	return S_OK;
}

STDMETHODIMP WindowTimerService::RemoveTimerClient(ITimerClient *pClient)
{
	std::map<double, ArrayPtrVariant<ITimerClient>* >::iterator iter = m_Clients.begin();
	while (iter != m_Clients.end())
	{
		ArrayPtrVariant<ITimerClient> *pClients = iter->second;

		short i;
		pClients->GetIndex(pClient, &i);
		pClients->Remove(CComVariant(i));
		short count;
		pClients->Count(&count);
		if (count == 0)
		{
			if (hWnd != NULL)
				KillTimer(hWnd, (UINT)pClients);
			m_Clients.erase(iter);
			delete pClients;
			break;	// We only need to remove the first one we come across.
		}

		iter++;
	}

	return S_OK;
}

STDMETHODIMP WindowTimerService::UpdateTimerClients()
{
	// One or more of our clients may have changed their timer interval
	// So now we need to find whichever one it is and move it to a different bucket!
restart:
	{	
		std::map<double, ArrayPtrVariant<ITimerClient>* >::iterator iter = m_Clients.begin();
		while (iter != m_Clients.end())
		{
			double bucket = iter->first;
			ArrayPtrVariant<ITimerClient> *pClients = iter->second;

			short count = 0;
			pClients->Count(&count);
			for (int i=count-1; i >= 0; i--)
			{
				ITimerClient *pClient = 0;
				pClients->Item(CComVariant(i), &pClient);

				double interval = 0.0;
				pClient->get_TimerInterval(&interval);
				if (interval != bucket)
				{
					pClients->Remove(CComVariant(i));
					if (count == 0)
					{
						if (hWnd != NULL)
							KillTimer(hWnd, (UINT)pClients);
						m_Clients.erase(iter);
						delete pClients;
					}
					// Unfortunately, the code below might screw the iterator so we have to start again
					SetTimerClient(pClient);
					pClient->Release();
					goto restart;
				}
				pClient->Release();
			}

			iter++;
		}
	}

	return S_OK;
}

BOOL WindowTimerService::handleTimerMessage(UINT id)
{
	return TRUE;
}


