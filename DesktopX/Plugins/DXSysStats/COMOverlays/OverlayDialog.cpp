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

#include "COMOverlays.h"

#include <stdio.h>
#include <commctrl.h>
#include <comdef.h>

#include "resource.h"
#include "OverlayDialog.h"
#include "PickList.cpp"
#import <msxml4.dll> implementation_only

void ToValL(long *out, char *s)
{
	sscanf(s, "%d", out);
}

void ToStrL(char *buf, int maxlen, long val)
{
	_snprintf(buf, maxlen, "%d", val);
}

template <class C>
bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

void ToValBSTR(_bstr_t *out, char *s)
{
	*out = _bstr_t(s);
}

void ToStrBSTR(char *buf, int maxlen, _bstr_t val)
{
	::strncpy(buf, val, maxlen);
}

COLORREF OverlayDialog::custColors[] =
{
	RGB(0,0,0),
	RGB(0,0,0),
	RGB(0,0,0),
	RGB(0,0,0),
	RGB(0,0,0),
	RGB(0,0,0),
	RGB(0,0,0),
	RGB(0,0,0),
	RGB(0,0,0),
	RGB(0,0,0),
	RGB(0,0,0),
	RGB(0,0,0),
	RGB(0,0,0),
	RGB(0,0,0),
	RGB(0,0,0),
	RGB(0,0,0)
};

OverlayDialog::OverlayDialog(ISimpleOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd) :
	CSubject(observer),
	Dialog(idd),
	overlay(overlay),
	meters(meters),
	meterList(Identity<long>, ToStrL, ToValL, /* fixed */ true),
	modeList(Identity<long>, ToStrL, ToValL, /* fixed */ true),
	selectorList(Identity<_bstr_t>, ToStrBSTR, ToValBSTR, /* not fixed */ false)
{
}

OverlayDialog::~OverlayDialog()
{
}

LRESULT OverlayDialog::x(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		overlay->put_X(i);
		notifyObserver();
	}

	return 1;
}

LRESULT OverlayDialog::y(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		overlay->put_Y(i);
		notifyObserver();
	}

	return 1;
}

LRESULT OverlayDialog::xRot(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		overlay->put_CenterX(i);
		notifyObserver();
	}

	return 1;
}

LRESULT OverlayDialog::yRot(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		overlay->put_CenterY(i);
		notifyObserver();
	}

	return 1;
}

LRESULT OverlayDialog::rotation(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		overlay->put_Rotation(i);
		notifyObserver();
	}

	return 1;
}

LRESULT OverlayDialog::hScale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	double scale = 1.0L;
	char val[64];
	GetDlgItemText(wID, val, sizeof(val));
	sscanf(val, "%lg", &scale);
	overlay->put_HScale(scale);
	notifyObserver();

	return 1;
}

LRESULT OverlayDialog::vScale(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	double scale = 1.0L;
	char val[64];
	GetDlgItemText(wID, val, sizeof(val));
	sscanf(val, "%lg", &scale);
	overlay->put_VScale(scale);
	notifyObserver();

	return 1;
}

LRESULT OverlayDialog::setMeter(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val = -1;
	meterList.getData(&val, -1 /* default value */);
	overlay->put_MeterIndex(val);
	overlay->put_selector(L"");
	resetSelectorCombo(val);
	notifyObserver();

	return 1;
}

LRESULT OverlayDialog::mode(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ISimpleOverlay2 *pISO2 = 0;
	HRESULT hr = overlay->QueryInterface(__uuidof(ISimpleOverlay2), (void **)&pISO2);
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

LRESULT OverlayDialog::selector(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	_bstr_t text = L"";
	selectorList.getValue(&text, text);
	overlay->put_selector(text);
	notifyObserver();

	return 1;
}

LRESULT OverlayDialog::setOverlay(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ISimpleOverlay2 *pISO2 = 0;
	HRESULT hr = overlay->QueryInterface(__uuidof(ISimpleOverlay2), (void **)&pISO2);
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

		notifyObserver();
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

void OverlayDialog::resetOverlayList()
{
	ISimpleOverlay2 *pISO2 = 0;
	HRESULT hr = overlay->QueryInterface(__uuidof(ISimpleOverlay2), (void **)&pISO2);
	if (FAILED(hr))
	{
		::EnableWindow(GetDlgItem(IDC_COMBO_EFFECTOVERLAY), FALSE);
		::EnableWindow(GetDlgItem(IDC_COMBO_MODE), FALSE);

		return;
	}

	::EnableWindow(GetDlgItem(IDC_COMBO_EFFECTOVERLAY), TRUE);

	SendDlgItemMessage(IDC_COMBO_EFFECTOVERLAY,TVM_DELETEITEM,0,(LPARAM)NULL);

	TVINSERTSTRUCT itvIns;
	memset(&itvIns, 0, sizeof(TVINSERTSTRUCT));
	itvIns.hInsertAfter = TVI_LAST;
	itvIns.itemex.mask = TVIF_TEXT | TVIF_PARAM;

	// Add '(None)' as first item...
	itvIns.hParent = TVI_ROOT;
	itvIns.itemex.pszText = "(None)";
	itvIns.itemex.cchTextMax = strlen(itvIns.itemex.pszText)+1;
	itvIns.itemex.lParam = (LPARAM)0;
	HTREEITEM h = (HTREEITEM)SendDlgItemMessage(IDC_COMBO_EFFECTOVERLAY,TVM_INSERTITEM,0,(LPARAM)&itvIns);

	// Now add rest of tree
	IOverlayHome *overlayRoot;
	IClientModel *model;
	overlay->get_Model(&model);
	model->get_Overlays(&overlayRoot);

	IOverlay *item;
	pISO2->get_EffectOverlay(&item);

	long l;
	pISO2->get_EffectOverlayEffect(&l);
	modeList.init(m_hWnd, IDC_COMBO_MODE, modeLabels, modeData, l);

	pISO2->Release();

	HTREEITEM selectItem = drawOverlayList(overlayRoot, TVI_ROOT, item, itvIns);

	if (overlayRoot)
		overlayRoot->Release();
	if (model)
		model->Release();

	if (item)
	{
		SendDlgItemMessage(IDC_COMBO_EFFECTOVERLAY, TVM_SELECTITEM, (WPARAM)TVGN_CARET, (LPARAM)selectItem);
		::EnableWindow(GetDlgItem(IDC_COMBO_MODE), TRUE);
	}
	else
	{
		SendDlgItemMessage(IDC_COMBO_EFFECTOVERLAY, TVM_SELECTITEM, (WPARAM)TVGN_CARET, (LPARAM)h);
		::EnableWindow(GetDlgItem(IDC_COMBO_MODE), FALSE);
	}
}

HTREEITEM OverlayDialog::drawOverlayList(ICompositeOverlay *coll, HTREEITEM parent, const IOverlay *selected, TVINSERTSTRUCT &itvIns)
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
		HTREEITEM h = (HTREEITEM)SendDlgItemMessage(IDC_COMBO_EFFECTOVERLAY,TVM_INSERTITEM,0,(LPARAM)&itvIns);
		ICompositeOverlay *pCO = NULL;
		HRESULT hr = pOverlay->QueryInterface(__uuidof(ICompositeOverlay), (void **)&pCO);
		if (SUCCEEDED(hr))
		{
			pCO->Release();
			HTREEITEM sel = drawOverlayList((ICompositeOverlay*)pOverlay, h, selected, itvIns);
			if (sel != NULL)
				selectItem = sel;

			SendDlgItemMessage(IDC_COMBO_EFFECTOVERLAY,TVM_EXPAND,(WPARAM)TVE_EXPAND,(LPARAM)h);
		}

		if (pOverlay == selected)
			selectItem = h;

		pOverlay->Release();
	}

	return selectItem;
}

void OverlayDialog::resetSelectorCombo(long meterIndex)
{
	xml = L"";
	BSTR xPath = L"";

	if (meterIndex >= 0)
	{
		IMeter *pMeter;
		meters->Item(CComVariant(meterIndex), &pMeter);
		IMetaData *pMetaData = 0;
		pMeter->QueryInterface(_uuidof(IMetaData), (void**)&pMetaData);
		if (pMetaData)
		{
			BSTR xmlString;
			if (SUCCEEDED(pMetaData->GetMetaData(&xmlString)))
			{
				xPath = L"//valueset[@id=//method[@name=\"GetValue\"]/arg[@name=\"selector\"]/valuesetref/@idref]";
				xml = xmlString;
				::SysFreeString(xmlString);
			}
			pMetaData->Release();
		}
		pMeter->Release();
	}

	BSTR s;

	overlay->get_selector(&s);
	_bstr_t _s(s);
	selectorList.init(m_hWnd, IDC_COMBO_SELECTOR, xml, xPath, _s);
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT OverlayDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Subclass the overlays combo box window
	overlaysCombo.SubclassWindow(GetDlgItem(IDC_COMBO_EFFECTOVERLAY), IDC_COMBO_EFFECTOVERLAY);

	long l;

	SendDlgItemMessage(IDC_XSPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(300, -100));
	overlay->get_X(&l);
	SetDlgItemInt(IDC_XPOS, (UINT)l, TRUE);

	SendDlgItemMessage(IDC_YSPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(-100, 300));
	overlay->get_Y(&l);
	SetDlgItemInt(IDC_YPOS, (UINT)l, TRUE);

	SendDlgItemMessage(IDC_CXSPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(300, -100));
	overlay->get_CenterX(&l);
	SetDlgItemInt(IDC_CXPOS, (UINT)l, TRUE);

	SendDlgItemMessage(IDC_CYSPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(-100, 300));
	overlay->get_CenterY(&l);
	SetDlgItemInt(IDC_CYPOS, (UINT)l, TRUE);

	SendDlgItemMessage(IDC_ROTSPIN,UDM_SETRANGE,0,(LPARAM)MAKELONG(359, 0));
	overlay->get_Rotation(&l);
	SetDlgItemInt(IDC_ROT, (UINT)l, TRUE);

	double scale;
	char val[64];

	overlay->get_HScale(&scale);
	sprintf(val, "%.20g", scale);
	SetDlgItemText(IDC_XSCALE, val);

	overlay->get_VScale(&scale);
	sprintf(val, "%.20g", scale);
	SetDlgItemText(IDC_YSCALE, val);

	overlay->get_MeterIndex(&l);
	meterList.init(m_hWnd, IDC_COMBO_METER, meters, l);
	resetSelectorCombo(l);

	// Initialize the overlay combo tree view.
	resetOverlayList();

	bHandled = FALSE;

	return 1;
}
