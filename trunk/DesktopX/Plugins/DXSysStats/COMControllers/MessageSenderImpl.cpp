// MessageSender.cpp : Implementation of MessageSenderImpl
#include "stdafx.h"
#include "ControllerImpl.cpp"
#include "MessageSenderImpl.h"
#pragma comment(lib, "SysStatsCommon.lib")
#include "SysStatsUtils.h"
#include "COMControllers.h"
#include "MessageSender.h"
#include "MessageSenderDialog.h"

/////////////////////////////////////////////////////////////////////////////
// MessageSenderImpl


/*
 * Constructor
 */
template <class B>
MessageSenderImpl<B>::MessageSenderImpl() : sendMessage(1024), wParam(0), lParam(0), useWParam(true), useLParam(false)
{
}

/*
 * Destructor
 */
template <class B>
MessageSenderImpl<B>::~MessageSenderImpl()
{
	AtlTrace("Destroying MessageSenderImpl\n");
}

/*
 * This method is called when a windows message of the correct type has been posted.
 * To be useful for sub-classes, we will post the passed lParam as the lParam though
 * we use our own wParam as the wParam.
 */
template <class B>
STDMETHODIMP MessageSenderImpl<B>::HandleMessage(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled)
{
	*handled = !passThrough;
	model->PostMessage(sendMessage, useWParam? (WPARAM)this->wParam : (WPARAM)wParam, useLParam ? (LPARAM)this->lParam : (LPARAM)lParam);

	// Provide implementation.
	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the controller should
 * be displayed so that the user can configure the controller to meet their
 * requirements.
 */
template <class B>
STDMETHODIMP MessageSenderImpl<B>::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	MessageSenderDialog *d = new MessageSenderDialog(this, observer, meters);
	d->DoModal((HWND)hDlg);
	delete d;

	return S_OK;
}

/*
 * Return a string representation of the controller’s type.
 */
template <class B>
STDMETHODIMP MessageSenderImpl<B>::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_MessageSender, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

template <class B>
STDMETHODIMP MessageSenderImpl<B>::get_SendMessage(UINT *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = sendMessage;

	return S_OK;
}

template <class B>
STDMETHODIMP MessageSenderImpl<B>::put_SendMessage(UINT newVal)
{
	sendMessage = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP MessageSenderImpl<B>::get_WParam(UINT *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = wParam;

	return S_OK;
}

template <class B>
STDMETHODIMP MessageSenderImpl<B>::put_WParam(UINT newVal)
{
	wParam = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP MessageSenderImpl<B>::get_LParam(UINT *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = lParam;

	return S_OK;
}

template <class B>
STDMETHODIMP MessageSenderImpl<B>::put_LParam(UINT newVal)
{
	lParam = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP MessageSenderImpl<B>::get_UseWParam(VARIANT_BOOL *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = useWParam;

	return S_OK;
}

template <class B>
STDMETHODIMP MessageSenderImpl<B>::put_UseWParam(VARIANT_BOOL newVal)
{
	useWParam = newVal;

	return S_OK;
}

template <class B>
STDMETHODIMP MessageSenderImpl<B>::get_UseLParam(VARIANT_BOOL *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = useLParam;

	return S_OK;
}

template <class B>
STDMETHODIMP MessageSenderImpl<B>::put_UseLParam(VARIANT_BOOL newVal)
{
	useLParam = newVal;

	return S_OK;
}
