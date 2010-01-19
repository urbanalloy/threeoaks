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

// Sens.cpp : Implementation of CSens
#include "stdafx.h"
#include "ControllerImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#include "SysStatsUtils.h"
#include "COMControllers.h"
#include "Sens.h"
#include "SensDialog.h"


/////////////////////////////////////////////////////////////////////////////
// CSens

const UINT CSens::MONITOR_ON_WPARAM = 1;
const UINT CSens::MONITOR_OFF_WPARAM = 2;
const UINT CSens::MONITOR_STANDBY_WPARAM= 3;
const UINT CSens::LOGON_WPARAM = 4;
const UINT CSens::LOGOFF_WPARAM = 5;
const UINT CSens::LOCK_WPARAM = 6;
const UINT CSens::UNLOCK_WPARAM = 7;
const UINT CSens::STARTSHELL_WPARAM = 8;
const UINT CSens::SCREENSAVER_ON_WPARAM = 9;
const UINT CSens::SCREENSAVER_OFF_WPARAM = 10;
const UINT CSens::DISCONNECT_WPARAM = 11;
const UINT CSens::RECONNECT_WPARAM = 12;
const UINT CSens::POSTSHELL_WPARAM = 13;

STDMETHODIMP CSens::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_ISens
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/*
 * Constructor
 */
CSens::CSens() :
	userName(L""),
	away(WM_USER),
	present(WM_USER+1),
	sessionId(0),
	pEventSystem(NULL),
	bstrLogonSubscriptionID(L""),
	bstrLogon2SubscriptionID(L"")
{
	messageType = 0;
	passThrough = true;
}

HRESULT CSens::FinalConstruct()
{
	::ProcessIdToSessionId(::GetCurrentProcessId(), &sessionId);

	char buf[MAX_PATH];
	DWORD len = MAX_PATH;
	::GetUserName(buf, &len);
	userName = _bstr_t(buf).copy();

	// Register us with the event system
	HRESULT hr = ::CoCreateInstance (CLSID_CEventSystem,
						           NULL,
						           CLSCTX_SERVER,
						           IID_IEventSystem,
						           (void **) &pEventSystem);

	if (pEventSystem) {
		// Create a subscribtion record
		IEventSubscription *pEventSubscription = NULL;
		hr = ::CoCreateInstance (CLSID_CEventSubscription,
									   NULL,
									   CLSCTX_SERVER,
									   IID_IEventSubscription,
									   (void **) &pEventSubscription);

		if (pEventSubscription) {
			WCHAR wbuf[MAX_PATH];
			GUID guid;
			::CoCreateGuid(&guid);
			::StringFromGUID2(guid, wbuf, MAX_PATH);
			bstrLogonSubscriptionID = wbuf;
			pEventSubscription->put_SubscriptionID (bstrLogonSubscriptionID);
			pEventSubscription->put_SubscriptionName (L"SysStats SENS Subscription");
			::StringFromGUID2(SENSGUID_EVENTCLASS_LOGON, wbuf, MAX_PATH);
			pEventSubscription->put_EventClassID (wbuf);
	        pEventSubscription->put_SubscriberInterface (GetControllingUnknown());

			// Add the subscription record to the event system
			pEventSystem->Store (PROGID_EventSubscription, pEventSubscription);
			pEventSubscription->Release();
		}

		hr = ::CoCreateInstance (CLSID_CEventSubscription,
									   NULL,
									   CLSCTX_SERVER,
									   IID_IEventSubscription,
									   (void **) &pEventSubscription);

		if (pEventSubscription) {
			WCHAR wbuf[MAX_PATH];
			GUID guid;
			::CoCreateGuid(&guid);
			::StringFromGUID2(guid, wbuf, MAX_PATH);
			bstrLogon2SubscriptionID = wbuf;
			pEventSubscription->put_SubscriptionID (bstrLogon2SubscriptionID);
			pEventSubscription->put_SubscriptionName (L"SysStats SENS Subscription");
			::StringFromGUID2(SENSGUID_EVENTCLASS_LOGON2, wbuf, MAX_PATH);
			pEventSubscription->put_EventClassID (wbuf);
	        pEventSubscription->put_SubscriberInterface (GetControllingUnknown());

			// Add the subscription record to the event system
			pEventSystem->Store (PROGID_EventSubscription, pEventSubscription);
			pEventSubscription->Release();
		}
	}

	RECT whatever = { 0, 0, 0, 0 };
	Create(0, whatever, "HiddenSensWindow", 0);

	// Install global hook
	setSysCommandHook();

	return hr;
}

CSens::~CSens() {
}

void CSens::FinalRelease()
{
	// This code removes the subscription.
	_bstr_t bstrQueryCriteria (L"SubscriptionID");
	bstrQueryCriteria += L" == ";
	bstrQueryCriteria += bstrLogonSubscriptionID;
    INT nErr;
	HRESULT hr = pEventSystem->Remove (PROGID_EventSubscription, bstrQueryCriteria, &nErr);

	bstrQueryCriteria = L"SubscriptionID";
	bstrQueryCriteria += L" == ";
	bstrQueryCriteria += bstrLogon2SubscriptionID;
	hr = pEventSystem->Remove (PROGID_EventSubscription, bstrQueryCriteria, &nErr);
	pEventSystem->Release();

	clearSysCommandHook();

	if (m_hWnd != 0)
		DestroyWindow();
}

bool CSens::itsUs(BSTR bstrUserName, ULONG dwSessionId)
{
	if (::wcschr(bstrUserName, '\\') != 0) 
		bstrUserName = ::wcschr(bstrUserName, '\\') + 1;

	AtlTrace(L"this->userName=%s, userName=%s, this->sessionId=%d, sessionId=%d\n",
		userName, bstrUserName, sessionId, dwSessionId);

	return (::wcscmp(userName, bstrUserName) == 0) && (dwSessionId == sessionId);
}

/*
 * In addition to the various SENS callbacks - handle monitor power events
 */
STDMETHODIMP CSens::HandleMessage(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled)
{
	*handled = false;

	AtlTrace ("wparam=0x%x, lparam=%d\n", wParam, lParam);
	if ((wParam & 0xFFF0) == SC_MONITORPOWER)
	{
		switch (lParam) {
		case 1:		// Standby
		case 2:		// Power off
			model->PostMessage(away, MONITOR_STANDBY_WPARAM, 0);
			break;

		case -1:	// Power on
			model->PostMessage(present, MONITOR_ON_WPARAM, 0);
			break;
		}
	}

	return S_OK;
}

/*
 * In addition to the various SENS callbacks - handle monitor power events
 */
STDMETHODIMP CSens::SysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	VARIANT_BOOL handled = false;
	HandleMessage(wParam, lParam, 0, 0, &handled);
	bHandled = false;

	return S_OK;
}

STDMETHODIMP CSens::Logon(BSTR bstrUserName)
{
	if (itsUs(bstrUserName, sessionId))
		model->PostMessage(present, LOGON_WPARAM, 0);

	return S_OK;
}
STDMETHODIMP CSens::Logoff(BSTR bstrUserName)
{
	AtlTrace("SENS: Logoff\n");
	if (itsUs(bstrUserName, sessionId))
		model->PostMessage(away, LOGOFF_WPARAM, 0);

	return S_OK;
}
STDMETHODIMP CSens::StartShell(BSTR bstrUserName)
{
	return E_NOTIMPL;
}
STDMETHODIMP CSens::DisplayLock(BSTR bstrUserName)
{
	AtlTrace("SENS: Display Lock\n");
	if (itsUs(bstrUserName, sessionId))
		model->PostMessage(away, LOCK_WPARAM, 0);

	return S_OK;
}
STDMETHODIMP CSens::DisplayUnlock(BSTR bstrUserName)
{
	if (itsUs(bstrUserName, sessionId))
		model->PostMessage(present, UNLOCK_WPARAM, 0);

	return S_OK;
}
STDMETHODIMP CSens::StartScreenSaver(BSTR bstrUserName)
{
	AtlTrace("SENS: Start ScreenSaver\n");
	if (itsUs(bstrUserName, sessionId))
		model->PostMessage(away, SCREENSAVER_ON_WPARAM, 0);

	return S_OK;
}
STDMETHODIMP CSens::StopScreenSaver(BSTR bstrUserName)
{
	if (itsUs(bstrUserName, sessionId))
		model->PostMessage(present, SCREENSAVER_OFF_WPARAM, 0);

	return S_OK;
}
// ISensLogon2
STDMETHODIMP CSens::Logon(BSTR bstrUserName, ULONG dwSessionId)
{
	return E_NOTIMPL;
}
STDMETHODIMP CSens::Logoff(BSTR bstrUserName, ULONG dwSessionId)
{
	AtlTrace("SENS: Logoff2\n");
	return E_NOTIMPL;
}
STDMETHODIMP CSens::SessionDisconnect(BSTR bstrUserName, ULONG dwSessionId)
{
	AtlTrace("SENS: Session Disconnect\n");
	if (itsUs(bstrUserName, dwSessionId))
		model->PostMessage(away, DISCONNECT_WPARAM, 0);

	return S_OK;
}
STDMETHODIMP CSens::SessionReconnect(BSTR bstrUserName, ULONG dwSessionId)
{
	if (itsUs(bstrUserName, dwSessionId))
		model->PostMessage(present, RECONNECT_WPARAM, 0);

	return S_OK;
}
STDMETHODIMP CSens::PostShell(BSTR bstrUserName, ULONG dwSessionId)
{
	return E_NOTIMPL;
}

/*
 * Called by SysStats when a configuration dialog for the controller should
 * be displayed so that the user can configure the controller to meet their
 * requirements.
 */
STDMETHODIMP CSens::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	SensDialog *d = new SensDialog(this, observer, meters);
	d->DoModal((HWND)hDlg);
	delete d;

	return S_OK;
}

/*
 * Return a string representation of the controller’s type.
 */
STDMETHODIMP CSens::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_SysStatsSens, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CSens::get_Away(UINT *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = away;

	return S_OK;
}

STDMETHODIMP CSens::put_Away(UINT newVal)
{
	away = newVal;

	return S_OK;
}

STDMETHODIMP CSens::get_Present(UINT *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = present;

	return S_OK;
}

STDMETHODIMP CSens::put_Present(UINT newVal)
{
	present = newVal;

	return S_OK;
}


