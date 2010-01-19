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

#include <stdio.h>
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <comdef.h>
#include "resource.h"
#include "SysStatsCOM.h"

#include "COverlayPanel.h"
#include "CCompositeOverlay.h"
#include "CRenameDialog.h"
#include "COverlayFactory.h"
#include "PickList.cpp"

extern void ToValL(long *out, char *s);
extern void ToStrL(char *buf, int maxlen, long val);

template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

COverlayPanel::COverlayPanel(IClientModel *model, IObserver *observer, WORD idd) :
	modeList(Identity<long>, ToStrL, ToValL, /* fixed */ true),
	CSubject(observer),
	Dialog(idd),
	model(model)
{
	model->get_Meters(&meters);
	model->get_Overlays(&overlayRoot);
}

COverlayPanel::~COverlayPanel()
{
	if (meters)
		meters->Release();

	if (overlayRoot)
		overlayRoot->Release();
}

int COverlayPanel::add(ICompositeOverlay *root, const char *overlayType)
{	
	IOverlay *o = COverlayFactory::createInstance(overlayType, model);
	o->put_Name(_bstr_t("New Overlay"));
	root->Add(o);
	o->Release();

	resetList(o);
	enable(o);

	return TRUE;
}

int COverlayPanel::duplicate(ICompositeOverlay *root, IOverlay *item)
{
	IOverlay *o;
	item->clone(&o);
	root->Add(o);
	o->Release();

	resetList(o);
	enable(o);

	return TRUE;
}

int COverlayPanel::remove(ICompositeOverlay *root, IOverlay *item)
{
	short i;
	root->GetIndex(item, &i);
	root->Remove(CComVariant(i));

	root->Item(CComVariant(i), &item);

	resetList(item);

	enable(item);

	if (item)
		item->Release();

	return TRUE;
}

int COverlayPanel::up(ICompositeOverlay *superRoot, ICompositeOverlay *root, IOverlay *item)
{
	short i;
	root->GetIndex(item, &i);
	CComVariant index(i);
	if (i != 0)
	{
		// This is not the first item in the container
		IOverlay *prev;
		root->Item(CComVariant(i-1), &prev);
		ICompositeOverlay *pCO = NULL;
		HRESULT hr = prev->QueryInterface(IID_ICompositeOverlay, (void **)&pCO);
		if (SUCCEEDED(hr))
		{
			// Previous item IS composite - add to the end of THAT container.
			pCO->Release();
			// Add before remove - don't want to drop reference count to zero.
			((ICompositeOverlay*)prev)->Add(item);
			root->Remove(index);
		}
		else
		{
			// Previous item is not composite. Just swap them.
			root->Swap(CComVariant(i-1), index);
		}
		prev->Release();
	}
	else if (superRoot)
	{
		// This IS the first item in the container. Move into the container's
		// parent just before this container, if there is one.
		// Add before remove.
		superRoot->GetIndex(root, &i);
		superRoot->AddAt(item, CComVariant(i));
		root->Remove(index);
	}

	resetList(item);
	return TRUE;
}

int COverlayPanel::down(ICompositeOverlay *superRoot, ICompositeOverlay *root, IOverlay *item)
{
	short i;
	root->GetIndex(item, &i);
	CComVariant index(i);
	short max;
	root->Count(&max);
	if (i != max-1)
	{
		// This is not the last item in the container
		IOverlay *next;
		root->Item(CComVariant(i+1), &next);
		ICompositeOverlay *pCO = NULL;
		HRESULT hr = next->QueryInterface(IID_ICompositeOverlay, (void **)&pCO);
		if (SUCCEEDED(hr))
		{
			// Next item IS composite - add to the end of THAT container.
			pCO->Release();
			//Add before remove.
			((ICompositeOverlay*)next)->AddAt(item, CComVariant(0));
			root->Remove(index);
		}
		else
		{
			// Next item is not composite. Just swap them.
			root->Swap(CComVariant(i+1), index);
		}
		next->Release();
	}
	else if (superRoot)
	{
		// This IS the last item in the container, Move into the container's
		// parent, if there is one, just after this container.

		// Add before remove.
		superRoot->GetIndex(root, &i);
		superRoot->AddAt(item, CComVariant(i+1));
		root->Remove(index);
	}

	resetList(item);

	return TRUE;
}

int COverlayPanel::rename(IOverlay *pOverlay)
{
	char buf[64];
	BSTR name;
	pOverlay->get_Name(&name);
	strcpy(buf, (char*)_bstr_t(name, false));
	CRenameDialog *d = new CRenameDialog(buf);
	d->DoModal(m_hWnd);
	delete d;

	if (*buf != 0)	// Can't have empty strings
	{
		pOverlay->put_Name(_bstr_t(buf));
		resetList(pOverlay);
	}

	return FALSE;
}

int COverlayPanel::configure(IOverlay *item)
{
	item->Configure(observer, meters, (long)m_hWnd);

	return TRUE;
}

void COverlayPanel::enable(IOverlay *item)
{
	if (item != NULL)
	{
		BSTR type;
		item->get_Type(&type);

		SendDlgItemMessage(IDC_OVERLAY_TYPE,CB_SELECTSTRING,0,(LPARAM)((char*)_bstr_t(type, false)));

		VARIANT_BOOL visible = false;
		item->get_Visible(&visible);
		SendDlgItemMessage(IDC_CHECK_VISIBLE, BM_SETCHECK, visible ? BST_CHECKED : BST_UNCHECKED, 0);
		::EnableWindow(GetDlgItem(IDC_CHECK_VISIBLE), TRUE);

#ifndef INPLACE_RENAME
		::EnableWindow(GetDlgItem(IDC_OVERLAY_RENAME), TRUE);
#endif
		::EnableWindow(GetDlgItem(IDC_OVERLAY_DUPLICATE), TRUE);
		::EnableWindow(GetDlgItem(IDC_OVERLAY_DELETE), TRUE);
		::EnableWindow(GetDlgItem(IDC_OVERLAY_CONFIGURE), TRUE);
		::EnableWindow(GetDlgItem(IDC_OVERLAY_MOVEUP), TRUE);
		::EnableWindow(GetDlgItem(IDC_OVERLAY_MOVEDOWN), TRUE);
	}
}

void COverlayPanel::disable()
{
#ifndef INPLACE_RENAME
	::EnableWindow(GetDlgItem(IDC_OVERLAY_RENAME), FALSE);
#endif
	::EnableWindow(GetDlgItem(IDC_CHECK_VISIBLE), FALSE);
	::EnableWindow(GetDlgItem(IDC_OVERLAY_DUPLICATE), FALSE);
	::EnableWindow(GetDlgItem(IDC_OVERLAY_DELETE), FALSE);
	::EnableWindow(GetDlgItem(IDC_OVERLAY_CONFIGURE), FALSE);
	::EnableWindow(GetDlgItem(IDC_OVERLAY_MOVEUP), FALSE);
	::EnableWindow(GetDlgItem(IDC_OVERLAY_MOVEDOWN), FALSE);
}

LRESULT COverlayPanel::overlaySelChanged(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	HTREEITEM h = (HTREEITEM) SendDlgItemMessage(idCtrl, TVM_GETNEXTITEM, (WPARAM)TVGN_CARET, 0);
	TVITEMEX oItem;
	oItem.hItem = h;
	oItem.mask = TVIF_HANDLE | TVIF_PARAM;
	SendDlgItemMessage(IDC_OVERLAY_LIST, TVM_GETITEM, (WPARAM)0, (LPARAM)&oItem);

	if (h)
		enable((IOverlay*)oItem.lParam);
	else
		disable();

	return 1;
}

IOverlay *COverlayPanel::getOverlayFromHandle(HTREEITEM h, IOverlay *def)
{
	if (h)
	{
		TVITEMEX oItem;
		memset(&oItem, 0, sizeof(TVITEMEX));
		oItem.hItem = h;
		oItem.mask = TVIF_HANDLE | TVIF_PARAM;
		SendDlgItemMessage(IDC_OVERLAY_LIST, TVM_GETITEM, (WPARAM)0, (LPARAM)&oItem);
		return (IOverlay*)oItem.lParam;
	}

	return def;
}

void COverlayPanel::getHierarchy(ICompositeOverlay **superParent, ICompositeOverlay **parent, IOverlay **item)
{
	if (item)
	{
		*item = NULL;
		if (parent)
			*parent = (ICompositeOverlay*)overlayRoot;
		HTREEITEM h = (HTREEITEM) SendDlgItemMessage(IDC_OVERLAY_LIST, TVM_GETNEXTITEM, (WPARAM)TVGN_CARET, 0);
		if (h)
		{
			*item = getOverlayFromHandle(h, NULL);

			if (parent)
			{
				HTREEITEM parentH = (HTREEITEM) SendDlgItemMessage(IDC_OVERLAY_LIST, TVM_GETNEXTITEM, (WPARAM)TVGN_PARENT, (LPARAM)h);
				*parent = (ICompositeOverlay*)getOverlayFromHandle(parentH, *parent);

				if (superParent)
				{
					HTREEITEM superParentH = (HTREEITEM) SendDlgItemMessage(IDC_OVERLAY_LIST, TVM_GETNEXTITEM, (WPARAM)TVGN_PARENT, (LPARAM)parentH);
					*superParent = (ICompositeOverlay*)getOverlayFromHandle(superParentH, (ICompositeOverlay*)overlayRoot);
					if (!parentH)
						*superParent = NULL;
				}
			}

		}
	}
}

LRESULT COverlayPanel::moveUp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	IOverlay *item = NULL;
	ICompositeOverlay *parent = NULL, *superParent = NULL;
	getHierarchy(&superParent, &parent, &item);

	if (item)
	{
		up(superParent, parent, item);
		enable(item);
		notifyObserver();
	}
	else
		disable();

	return 1;
}

LRESULT COverlayPanel::moveDown(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	IOverlay *item = NULL;
	ICompositeOverlay *parent = NULL, *superParent = NULL;
	getHierarchy(&superParent, &parent, &item);

	if (item)
	{
		down(superParent, parent, item);
		enable(item);
		notifyObserver();
	}
	else
		disable();

	return 1;
}

LRESULT COverlayPanel::remove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	IOverlay *item = NULL;
	ICompositeOverlay *parent = NULL;
	getHierarchy(NULL, &parent, &item);

	if (item)
	{
		remove(parent, item);
		SendDlgItemMessage(IDC_OVERLAY_LIST, TVM_SELECTITEM, (WPARAM)TVGN_CARET, 0);
		notifyObserver();
	}

	return 1;
}

LRESULT COverlayPanel::duplicate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	IOverlay *item = NULL;
	ICompositeOverlay *parent = NULL;
	getHierarchy(NULL, &parent, &item);

	if (item)
	{
		duplicate(parent, item);
		SendDlgItemMessage(IDC_OVERLAY_LIST, TVM_SELECTITEM, (WPARAM)TVGN_CARET, 0);
		notifyObserver();
	}

	return 1;
}

LRESULT COverlayPanel::configure(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	IOverlay *item = NULL;
	getHierarchy(NULL, NULL, &item);

	if (item)
	{
		configure(item);
	}

	return 1;
}

LRESULT COverlayPanel::rename(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	IOverlay *item = NULL;
	getHierarchy(NULL, NULL, &item);

	if (item)
	{
		rename(item);
	}

	return 1;
}

LRESULT COverlayPanel::visible(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	IOverlay *item = NULL;
	getHierarchy(NULL, NULL, &item);

	if (item)
	{
		LRESULT state = SendDlgItemMessage(IDC_CHECK_VISIBLE, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
		item->put_Visible(state == BST_CHECKED);
		notifyObserver();
	}

	return 1;
}

LRESULT COverlayPanel::add(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[256];
	IOverlay *item = NULL;
	ICompositeOverlay *parent = NULL;
	getHierarchy(NULL, &parent, &item);
	*text = 0;
	GetDlgItemText(IDC_OVERLAY_TYPE, text, sizeof(text));
	if (item)
	{
		ICompositeOverlay *pCO = NULL;
		HRESULT hr = item->QueryInterface(IID_ICompositeOverlay, (void **)&pCO);
		if (SUCCEEDED(hr))
		{
			parent = (ICompositeOverlay*)item;
			pCO->Release();
		}
	}

	if (add(parent, text))
		notifyObserver();

	return 1;
}

LRESULT COverlayPanel::typeSelChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SendDlgItemMessage(IDC_OVERLAY_LIST, TVM_SELECTITEM, (WPARAM)-1, 0);

	return 1;
}
	
void COverlayPanel::resetList(const IOverlay *item)
{
	SendDlgItemMessage(IDC_OVERLAY_LIST,TVM_DELETEITEM,0,(LPARAM)NULL);

	TVINSERTSTRUCT itvIns;
	memset(&itvIns, 0, sizeof(TVINSERTSTRUCT));
	itvIns.hInsertAfter = TVI_LAST;
	itvIns.itemex.mask = TVIF_TEXT | TVIF_PARAM;

	HTREEITEM selectItem = drawList(overlayRoot, TVI_ROOT, item, itvIns);

	SendDlgItemMessage(IDC_OVERLAY_LIST, TVM_SELECTITEM, (WPARAM)TVGN_CARET, (LPARAM)selectItem);
}

HTREEITEM COverlayPanel::drawList(ICompositeOverlay *coll, HTREEITEM parent, const IOverlay *selected, TVINSERTSTRUCT &itvIns)
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
		HTREEITEM h = (HTREEITEM)SendDlgItemMessage(IDC_OVERLAY_LIST,TVM_INSERTITEM,0,(LPARAM)&itvIns);
		ICompositeOverlay *pCO = NULL;
		HRESULT hr = pOverlay->QueryInterface(IID_ICompositeOverlay, (void **)&pCO);
		if (SUCCEEDED(hr))
		{
			pCO->Release();
			HTREEITEM sel = drawList((ICompositeOverlay*)pOverlay, h, selected, itvIns);
			if (sel != NULL)
				selectItem = sel;

			SendDlgItemMessage(IDC_OVERLAY_LIST,TVM_EXPAND,(WPARAM)TVE_EXPAND,(LPARAM)h);
		}

		if (pOverlay == selected)
			selectItem = h;

		pOverlay->Release();
	}

	return selectItem;
}

LRESULT COverlayPanel::mode(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ISimpleOverlay2 *pISO2 = 0;
	HRESULT hr = overlayRoot->QueryInterface(__uuidof(ISimpleOverlay2), (void **)&pISO2);
	if (SUCCEEDED(hr))
	{
		long val = 0;
		modeList.getData(&val, 0 /* default value */);
		pISO2->put_EffectOverlayEffect(val);
		pISO2->Release();
		notifyObserver();
	}

	return 1;
}

LRESULT COverlayPanel::setOverlay(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ISimpleOverlay2 *pISO2 = 0;
	HRESULT hr = overlayRoot->QueryInterface(__uuidof(ISimpleOverlay2), (void **)&pISO2);
	if (SUCCEEDED(hr))
	{
		HTREEITEM h = TreeView_GetSelection(hWndCtl);

		TVITEMEX oItem;
		::memset(&oItem, 0, sizeof(oItem));
		oItem.hItem = h;
		oItem.mask = TVIF_HANDLE | TVIF_PARAM;
		::SendMessage(hWndCtl, TVM_GETITEM, (WPARAM)0, (LPARAM)&oItem);

		pISO2->put_EffectOverlay((IOverlay*)oItem.lParam);
		pISO2->Release();

		if (oItem.lParam)
			::EnableWindow(GetDlgItem(IDC_COMBO_MODE), TRUE);
		else
			::EnableWindow(GetDlgItem(IDC_COMBO_MODE), FALSE);
	}

	return 1;
}

static const char *modeLabels[] =
{
	"Mask out",
	"Mask in",
	NULL
};

static long modeData[] =
{
	0,
	1
};

void COverlayPanel::resetOverlayList()
{
	ISimpleOverlay2 *pISO2 = 0;
	HRESULT hr = overlayRoot->QueryInterface(__uuidof(ISimpleOverlay2), (void **)&pISO2);
	if (FAILED(hr))
	{
		::EnableWindow(GetDlgItem(IDC_COMBO_OVERLAY), FALSE);
		::EnableWindow(GetDlgItem(IDC_COMBO_MODE), FALSE);

		return;
	}

	::EnableWindow(GetDlgItem(IDC_COMBO_OVERLAY), TRUE);

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

	IOverlay *item;
	pISO2->get_EffectOverlay(&item);

	long l;
	pISO2->get_EffectOverlayEffect(&l);
	modeList.init(m_hWnd, IDC_COMBO_MODE, modeLabels, modeData, l);

	pISO2->Release();

	HTREEITEM selectItem = drawOverlayList(overlayRoot, TVI_ROOT, item, itvIns);

	if (item)
	{
		SendDlgItemMessage(IDC_COMBO_OVERLAY, TVM_SELECTITEM, (WPARAM)TVGN_CARET, (LPARAM)selectItem);
		::EnableWindow(GetDlgItem(IDC_COMBO_MODE), TRUE);
	}
	else
	{
		SendDlgItemMessage(IDC_COMBO_OVERLAY, TVM_SELECTITEM, (WPARAM)TVGN_CARET, (LPARAM)h);
		::EnableWindow(GetDlgItem(IDC_COMBO_MODE), FALSE);
	}
}

HTREEITEM COverlayPanel::drawOverlayList(ICompositeOverlay *coll, HTREEITEM parent, const IOverlay *selected, TVINSERTSTRUCT &itvIns)
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
		HRESULT hr = pOverlay->QueryInterface(__uuidof(ICompositeOverlay), (void **)&pCO);
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
/*
 * 
 */
LRESULT COverlayPanel::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Subclass the overlays combo box window
	overlaysCombo.SubclassWindow(GetDlgItem(IDC_COMBO_OVERLAY), IDC_COMBO_OVERLAY);

	// Sort out the overlay drop down list
	char **keys = (char**) malloc(sizeof(const char*) * COverlayFactory::typeCount());
	COverlayFactory::listTypes(keys);

	for (int i=0; i<COverlayFactory::typeCount(); i++)
	{
		SendDlgItemMessage(IDC_OVERLAY_TYPE,CB_ADDSTRING,0,(LPARAM)keys[i]);
	}

	SendDlgItemMessage(IDC_OVERLAY_TYPE,CB_SETCURSEL,0,0);

	free(keys);

	// Draw the tree of overlays.
	resetList();

	// Draw the tree of effect overlays
	resetOverlayList();

	return TRUE;
}
