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

// ControllerPanel.cpp: implementation of the CControllerPanel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "SysStatsCOM.h"
#include "ControllerPanel.h"
#include "CControllerFactory.h"
#include "CRenameDialog.h"
#include "PickList.cpp"

#define IS_MOUSE_EVENT(uMsg) ((uMsg >= WM_MOUSEFIRST) && (uMsg <= WM_MOUSELAST))

void ToValWP(WPARAM *out, char *s)
{
	sscanf(s, "%d", out);
}

void ToStrWP(char *buf, int maxlen, WPARAM val)
{
	_snprintf(buf, maxlen, "%d", val);
}

template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CControllerPanel::CControllerPanel(IClientModel *model, IObserver *observer, WORD idd) :
	CSubject(observer),
	Dialog(idd),
	model(model),
	controllers(controllers),
	keyList(Identity<WPARAM>, ToStrWP, ToValWP, /* fixed */ true),
	messageList(Identity<UINT>, ToStrWP, ToValWP, /* not fixed */ false),
	commandList(Identity<UINT>, ToStrWP, ToValWP, /* fixed */ true)
{
	model->get_Meters(&meters);
	model->get_Controllers(&controllers);
}

CControllerPanel::~CControllerPanel()
{
	if (meters)
		meters->Release();

	if (controllers)
		controllers->Release();
}

LRESULT CControllerPanel::setOverlay(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	HTREEITEM h = TreeView_GetSelection(hWndCtl);

	TVITEMEX oItem;
	::memset(&oItem, 0, sizeof(oItem));
	oItem.hItem = h;
	oItem.mask = TVIF_HANDLE | TVIF_PARAM;
	::SendMessage(hWndCtl, TVM_GETITEM, (WPARAM)0, (LPARAM)&oItem);

	LRESULT mList = SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_GETCURSEL, 0, 0);
	IController *pController;
	controllers->Item(CComVariant((int)mList), &pController);

	if (pController)
	{
		pController->put_Overlay((IOverlay*)oItem.lParam);
		pController->Release();
	}

	return 1;
}

LRESULT CControllerPanel::setMessage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	UINT value;
	LRESULT mList = SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_GETCURSEL, 0, 0);
	IController *pController;
	controllers->Item(CComVariant((int)mList), &pController);

	if (pController)
	{
		short oldValue;
		pController->get_MessageType(&oldValue);

		messageList.getData(&value, 0);
		LRESULT res = SendDlgItemMessage(wID,CB_GETCURSEL,0,0);
		if (res != CB_ERR)
		{
			pController->put_MessageType(value);
			if (oldValue < WM_USER && value >= WM_USER)
				pController->put_Key(-1);

			enable(mList);
		}
		else
		{
			BOOL succeeded = false;
			value = (UINT)GetDlgItemInt(wID, &succeeded);
			if (succeeded)
			{
				pController->put_MessageType(value);

				if (oldValue < WM_USER && value >= WM_USER)
					pController->put_Key(-1);
			}
		}

		pController->Release();
	}

	return 1;
}

LRESULT CControllerPanel::setKey(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT mList = SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_GETCURSEL, 0, 0);
	IController *pController;
	controllers->Item(CComVariant((int)mList), &pController);

	if (pController)
	{
		WPARAM val = 0;
		keyList.getData(&val, 0 /* default value */);
		pController->put_Key(val);
		pController->Release();
	}

	return 1;
}

LRESULT CControllerPanel::setCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT mList = SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_GETCURSEL, 0, 0);
	IController *pController;
	controllers->Item(CComVariant((int)mList), &pController);

	if (pController)
	{
		UINT val = 0;
		commandList.getData(&val, 0 /* default value */);
		pController->put_Key(val);
		pController->Release();
	}

	return 1;
}

LRESULT CControllerPanel::setKeyKey(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT mList = SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_GETCURSEL, 0, 0);
	IController *pController;
	controllers->Item(CComVariant((int)mList), &pController);

	if (pController)
	{
		short message = 0;
		pController->get_MessageType(&message);

		if (message >= WM_USER)
		{
			BOOL succeeded = false;
			int value = GetDlgItemInt(wID, &succeeded);
			if (succeeded)
			{
				pController->put_Key(value);
			}
			else
			{
				pController->put_Key(-1);
			}
		}
		else
		{
			char text[MAX_PATH];
			*text = 0;
			GetDlgItemText(wID, text, sizeof(text));

			LRESULT mList = SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_GETCURSEL, 0, 0);
			IController *pController;
			controllers->Item(CComVariant((int)mList), &pController);

			if (pController)
			{
				pController->put_Key(text[0]);
				pController->Release();
			}
		}

		pController->Release();
	}

	return 1;
}

LRESULT CControllerPanel::add(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[256];
	*text = 0;
	GetDlgItemText(IDC_CONTROLLER_TYPE, text, sizeof(text));
	IController *c = CControllerFactory::createInstance(text, model);
	c->put_Name(_bstr_t("New Controller"));
	short i = -1;
	controllers->Count(&i);
	controllers->Add(c);
	c->Release();

	resetList(i);
	enable(i);

	return 1;
}

LRESULT CControllerPanel::remove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT cList = SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_GETCURSEL, 0, 0);

	if (cList != CB_ERR)
	{
		CComVariant index((int)cList);
		IController *pController;
		controllers->Item(index, &pController);
		controllers->Remove(index);

		if (pController)
		{
			resetList((int)cList);
			pController->Release();
		}
	}

	return 1;
}

LRESULT CControllerPanel::moveUp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT cList = SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_GETCURSEL, 0, 0);

	if (cList != CB_ERR && cList > 0)
	{
		CComVariant index((int)cList);
		CComVariant indexup((int)cList-1);
		controllers->Swap(index, indexup);

		resetList((int)cList-1);
	}

	return 1;
}

LRESULT CControllerPanel::moveDown(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT cList = SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_GETCURSEL, 0, 0);

	short count = 0;
	controllers->Count(&count);
	if (cList != CB_ERR && cList < count-1)
	{
		CComVariant index((int)cList);
		CComVariant indexdown((int)cList+1);
		controllers->Swap(index, indexdown);

		resetList((int)cList+1);
	}

	return 1;
}

LRESULT CControllerPanel::rename(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT cList = SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_GETCURSEL, 0, 0);

	if (cList != CB_ERR)
	{
		char buf[64];
		SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_GETTEXT, (WPARAM)cList, (LPARAM)buf);
		CRenameDialog *d = new CRenameDialog(buf);
		d->DoModal(m_hWnd);
		delete d;

		if (*buf != 0)	// Can't have empty strings
		{
			IController *controller;
			controllers->Item(CComVariant((int)cList), &controller);
			controller->put_Name(_bstr_t(buf));
			resetList((int)cList);
			controller->Release();
		}
	}

	return 1;
}

LRESULT CControllerPanel::configure(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT cList = SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_GETCURSEL, 0, 0);

	if (cList != CB_ERR)
	{
		IController *controller;
		controllers->Item(CComVariant((int)cList), &controller);
		controller->Configure(observer, meters, (long)m_hWnd);
		controller->Release();
	}

	return 1;
}

LRESULT CControllerPanel::passThrough(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT cList = SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_GETCURSEL, 0, 0);

	if (cList != CB_ERR)
	{
		IController *controller;
		controllers->Item(CComVariant((int)cList), &controller);

		LRESULT state = SendDlgItemMessage(IDC_CHECK_PASSTHROUGH, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
		controller->put_PassThrough(state == BST_CHECKED);
		controller->Release();
	}

	return 1;
}

LRESULT CControllerPanel::OnControllerType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_SETCURSEL, (WPARAM)-1, 0);

	return 1;
}

LRESULT CControllerPanel::listSelChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	LRESULT cList = SendDlgItemMessage(wID, LB_GETCURSEL, 0, 0);

	if (cList != CB_ERR)
		enable((int)cList);
	else
		disable();

	return 1;
}

void CControllerPanel::enable(int item)
{
	short max;
	controllers->Count(&max);
	if (item < max)
	{
		IController *controller;
		controllers->Item(CComVariant(item), &controller);

		// Select the correct controller type
		BSTR type;
		controller->get_Type(&type);	// We own the BSTR now

		SendDlgItemMessage(IDC_CONTROLLER_TYPE,CB_SELECTSTRING,0,(LPARAM)((char*)_bstr_t(type, false)));

		// Select the correct message type in the dialog
		bool set = false;
		short message = 0;
		controller->get_MessageType(&message);
		messageList.select(message);

		::ShowWindow(GetDlgItem(IDC_COMBO_KEY), FALSE);
		::ShowWindow(GetDlgItem(IDC_COMBO_COMMAND), FALSE);
		::ShowWindow(GetDlgItem(IDC_EDIT_KEY), FALSE);
		::EnableWindow(GetDlgItem(IDC_EDIT_KEY), FALSE);
		::EnableWindow(GetDlgItem(IDC_COMBO_KEY), FALSE);
		::EnableWindow(GetDlgItem(IDC_COMBO_COMMAND), FALSE);

		UINT key = 0;
		controller->get_Key(&key);
		SetDlgItemText(IDC_STATIC_KEY, "Key");

		if (IS_MOUSE_EVENT(message))
		{
			::EnableWindow(GetDlgItem(IDC_COMBO_KEY), TRUE);
			::ShowWindow(GetDlgItem(IDC_COMBO_KEY), TRUE);
			keyList.select(key);
		}
		else if (message == WM_CHAR)
		{
			::ShowWindow(GetDlgItem(IDC_EDIT_KEY), TRUE);
			::EnableWindow(GetDlgItem(IDC_EDIT_KEY), TRUE);
			char buf[2]; buf[0] = key; buf[1] = 0;
			SetDlgItemText(IDC_EDIT_KEY, buf);
		}
		else if (message >= WM_USER)
		{
			::ShowWindow(GetDlgItem(IDC_EDIT_KEY), TRUE);
			::EnableWindow(GetDlgItem(IDC_EDIT_KEY), TRUE);
			if (key != -1)
				SetDlgItemInt(IDC_EDIT_KEY, key);
			else
				SetDlgItemText(IDC_EDIT_KEY, "");

			SetDlgItemText(IDC_STATIC_KEY, "WPARAM");
		}
		else if (message == WM_APPCOMMAND)
		{
			::EnableWindow(GetDlgItem(IDC_COMBO_COMMAND), TRUE);
			::ShowWindow(GetDlgItem(IDC_COMBO_COMMAND), TRUE);
			SetDlgItemText(IDC_STATIC_KEY, "Command");
			commandList.select(key);
		}
		else
			::EnableWindow(GetDlgItem(IDC_COMBO_KEY), FALSE);

		VARIANT_BOOL passThrough = false;
		controller->get_PassThrough(&passThrough);
		SendDlgItemMessage(IDC_CHECK_PASSTHROUGH, BM_SETCHECK, passThrough ? BST_CHECKED : BST_UNCHECKED, 0);

		// Select the correct overlay
		IOverlay *overlay = NULL;
		controller->get_Overlay(&overlay);
		resetOverlayList(overlay);

		// Enable all the controls
		::EnableWindow(GetDlgItem(IDC_CONTROLLER_DELETE), TRUE);
		::EnableWindow(GetDlgItem(IDC_CONTROLLER_RENAME), TRUE);
		::EnableWindow(GetDlgItem(IDC_CONTROLLER_CONFIGURE), TRUE);
		::EnableWindow(GetDlgItem(IDC_CONTROLLER_MOVEUP), TRUE);
		::EnableWindow(GetDlgItem(IDC_CONTROLLER_MOVEDOWN), TRUE);
		::EnableWindow(GetDlgItem(IDC_COMBO_MESSAGE), TRUE);
		::EnableWindow(GetDlgItem(IDC_COMBO_OVERLAY), TRUE);
		::EnableWindow(GetDlgItem(IDC_CHECK_PASSTHROUGH), TRUE);

		controller->Release();
	}
}

void CControllerPanel::disable()
{
	SendDlgItemMessage(IDC_COMBO_MESSAGE,CB_SETCURSEL,-1,0);
	::EnableWindow(GetDlgItem(IDC_COMBO_KEY), FALSE);
	::EnableWindow(GetDlgItem(IDC_EDIT_KEY), FALSE);
	::EnableWindow(GetDlgItem(IDC_COMBO_COMMAND), FALSE);
	::EnableWindow(GetDlgItem(IDC_CONTROLLER_DELETE), FALSE);
	::EnableWindow(GetDlgItem(IDC_CONTROLLER_RENAME), FALSE);
	::EnableWindow(GetDlgItem(IDC_CONTROLLER_CONFIGURE), FALSE);
	::EnableWindow(GetDlgItem(IDC_CONTROLLER_MOVEUP), TRUE);
	::EnableWindow(GetDlgItem(IDC_CONTROLLER_MOVEDOWN), FALSE);
	::EnableWindow(GetDlgItem(IDC_COMBO_MESSAGE), FALSE);
	::EnableWindow(GetDlgItem(IDC_COMBO_OVERLAY), FALSE);
	::EnableWindow(GetDlgItem(IDC_CHECK_PASSTHROUGH), FALSE);
}

void CControllerPanel::resetList(int item)
{
	SendDlgItemMessage(IDC_CONTROLLER_LIST,LB_RESETCONTENT,0,0);
	short max;
	controllers->Count(&max);
	for (int i=0; i<max; i++)
	{
		BSTR name;
		IController *controller;
		controllers->Item(CComVariant(i), &controller);

		controller->get_Name(&name);
		SendDlgItemMessage(IDC_CONTROLLER_LIST,LB_ADDSTRING,0,(LPARAM)((char*)_bstr_t(name, false)));
		controller->Release();
	}

	if (item < 0)
		return;
	else if (item < max)
		SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_SETCURSEL, (WPARAM)item, 0);
	else if (max != 0)
		SendDlgItemMessage(IDC_CONTROLLER_LIST, LB_SETCURSEL, (WPARAM)(max-1), 0);
}

void CControllerPanel::resetOverlayList(const IOverlay *item)
{
	SendDlgItemMessage(IDC_COMBO_OVERLAY,TVM_DELETEITEM,0,(LPARAM)NULL);

	TVINSERTSTRUCT itvIns;
	memset(&itvIns, 0, sizeof(TVINSERTSTRUCT));
	itvIns.hInsertAfter = TVI_LAST;
	itvIns.itemex.mask = TVIF_TEXT | TVIF_PARAM;

	// Add '(None)' as first item...
	itvIns.hParent = TVI_ROOT;
	itvIns.itemex.pszText = "(None)";
	itvIns.itemex.cchTextMax = strlen(itvIns.itemex.pszText)+1;
	itvIns.itemex.lParam = (LPARAM)0;
	HTREEITEM h = (HTREEITEM)SendDlgItemMessage(IDC_COMBO_OVERLAY,TVM_INSERTITEM,0,(LPARAM)&itvIns);

	// Now add rest of tree
	IOverlayHome *overlayRoot;
	model->get_Overlays(&overlayRoot);
	HTREEITEM selectItem = drawOverlayList(overlayRoot, TVI_ROOT, item, itvIns);
	if (overlayRoot)
		overlayRoot->Release();

	if (item)
		SendDlgItemMessage(IDC_COMBO_OVERLAY, TVM_SELECTITEM, (WPARAM)TVGN_CARET, (LPARAM)selectItem);
	else
		SendDlgItemMessage(IDC_COMBO_OVERLAY, TVM_SELECTITEM, (WPARAM)TVGN_CARET, (LPARAM)h);
}

HTREEITEM CControllerPanel::drawOverlayList(ICompositeOverlay *coll, HTREEITEM parent, const IOverlay *selected, TVINSERTSTRUCT &itvIns)
{
	HTREEITEM selectItem = NULL;

	short max;
	coll->Count(&max);
	for (int i=0; i<max; i++)
	{
		IOverlay *pOverlay;
		coll->Item(CComVariant(i), &pOverlay);
		itvIns.hParent = parent;
		BSTR name;
		pOverlay->get_Name(&name);
		_bstr_t tb(name, false);
		itvIns.itemex.pszText = (char*)tb;
		itvIns.itemex.cchTextMax = strlen(itvIns.itemex.pszText)+1;
		itvIns.itemex.lParam = (LPARAM)pOverlay;
		HTREEITEM h = (HTREEITEM)SendDlgItemMessage(IDC_COMBO_OVERLAY,TVM_INSERTITEM,0,(LPARAM)&itvIns);
		ICompositeOverlay *pCO = NULL;
		HRESULT hr = pOverlay->QueryInterface(IID_ICompositeOverlay, (void **)&pCO);
		if (SUCCEEDED(hr))
		{
			pCO->Release();
			HTREEITEM sel = drawOverlayList((ICompositeOverlay*)pOverlay, h, selected, itvIns);
			if (sel != NULL)
				selectItem = sel;

			SendDlgItemMessage(IDC_COMBO_OVERLAY,TVM_EXPAND,(WPARAM)TVE_EXPAND,(LPARAM)h);
		}

		if (pOverlay == selected)
			selectItem = h;

		pOverlay->Release();
	}

	return selectItem;
}

static const char *keyLabels[] =
{
	"(None)",
	"Ctrl",
	"Shift",
	"Ctrl+Shift",
	"Alt",
	"Alt+Ctrl",
	"Alt+Shift",
	"Alt+Ctrl+Shift",
	NULL
};

static WPARAM keyData[] =
{
	0,
	MK_CONTROL,
	MK_SHIFT,
	MK_CONTROL | MK_SHIFT,
	0x80,
	0x80 | MK_CONTROL,
	0x80 | MK_SHIFT,
	0x80 | MK_SHIFT | MK_CONTROL,
	NULL
};

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT CControllerPanel::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Subclass the overlays combo box window
	overlaysCombo.SubclassWindow(GetDlgItem(IDC_COMBO_OVERLAY), IDC_COMBO_OVERLAY);

	// Populate the modifier keys combo
	keyList.init(m_hWnd, IDC_COMBO_KEY, keyLabels, keyData, 0);

	// Sort out the controller dropdown list
	char** keys;
	keys = (char**) malloc(sizeof(const char*) * CControllerFactory::typeCount());
	CControllerFactory::listTypes(keys);

	for (int i=0; i<CControllerFactory::typeCount(); i++)
	{
		SendDlgItemMessage(IDC_CONTROLLER_TYPE,CB_ADDSTRING,0,(LPARAM)keys[i]);
	}

	SendDlgItemMessage(IDC_CONTROLLER_TYPE,CB_SETCURSEL,0,0);

	free(keys);

	IClientModel *pModel;
	meters->get_Model(&pModel);
	IAppConfig *pAppConfig;
	pModel->get_AppConfig(&pAppConfig);

	_bstr_t messageName("system\\Windows Messages.txt");
	BSTR configFile;
	pAppConfig->CanonicalPath(messageName, &configFile);

	messageList.init(m_hWnd, IDC_COMBO_MESSAGE, _bstr_t(configFile), 0);

	messageName = "system\\App Commands.txt";
	pAppConfig->CanonicalPath(messageName, &configFile);
	commandList.init(m_hWnd, IDC_COMBO_COMMAND, _bstr_t(configFile), 0);

	// Draw the list of controllers.
	resetList(-1);

	// Initialize the overlay combo tree view.
	resetOverlayList();

	pAppConfig->Release();
	pModel->Release();

	bHandled = FALSE;

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being destroyed.
 */
LRESULT CControllerPanel::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}
