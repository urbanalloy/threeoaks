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
#include <atlbase.h>        // Base ATL classes
extern CComModule _Module;  // Global _Module
#include <atlwin.h>         // ATL windowing classes
#include <CommCtrl.h>

#include "CSubject.h"
#include "Dialog.h"
#include "resource.h"
#include "PickList.h"
#include "TreeViewCombo.h"

struct IMeter;
struct IOverlay;
struct IObserver;
class CompositeOverlay;
template <class I> class ArrayPtr;

class COverlayPanel : public CSubject, public Dialog
{
public:
	COverlayPanel(IClientModel *model, IObserver *observer, WORD idd=IDD_OVERLAYHOMEPROPS);
	virtual ~COverlayPanel();

	int up(ICompositeOverlay *superParent, ICompositeOverlay *root, IOverlay *item);
	int down(ICompositeOverlay *superParent, ICompositeOverlay *root, IOverlay *item);
	int add(ICompositeOverlay *root, const char *overlayType);
	int duplicate(ICompositeOverlay *root, IOverlay *item);
	int remove(ICompositeOverlay *root, IOverlay *item);
	int configure(IOverlay *item);
	int rename(IOverlay *pOverlay);

	// Specific to this class
	virtual void resetList(const IOverlay *selected=NULL);

protected:
	virtual void resetOverlayList();
	HTREEITEM drawOverlayList(ICompositeOverlay *root, HTREEITEM parent, const IOverlay *selected, TVINSERTSTRUCT &itvIns);

	void enable(IOverlay *item);
	void disable();

	HTREEITEM drawList(ICompositeOverlay *root, HTREEITEM parent, const IOverlay *selected, TVINSERTSTRUCT &itvIns);
	IOverlay *getOverlayFromHandle(HTREEITEM h, IOverlay *def);
	void getHierarchy(ICompositeOverlay **superParent, ICompositeOverlay **parent, IOverlay **item);

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT overlaySelChanged(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	virtual LRESULT moveUp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT moveDown(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT add(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT duplicate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT remove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT configure(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT rename(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT visible(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT setOverlay(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT mode(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT typeSelChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	IClientModel *model;
	IMeterHome *meters;
	IOverlayHome *overlayRoot;

	PickList<long> modeList;
	TreeViewCombo overlaysCombo;

public:
	BEGIN_MSG_MAP(COverlayPanel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		NOTIFY_HANDLER(IDC_OVERLAY_LIST, TVN_SELCHANGED, overlaySelChanged)
		COMMAND_HANDLER(IDC_OVERLAY_MOVEUP, BN_CLICKED, moveUp)
		COMMAND_HANDLER(IDC_OVERLAY_MOVEDOWN, BN_CLICKED, moveDown)
		COMMAND_HANDLER(IDC_OVERLAY_DELETE, BN_CLICKED, remove)
		COMMAND_HANDLER(IDC_OVERLAY_DUPLICATE, BN_CLICKED, duplicate)
		COMMAND_HANDLER(IDC_OVERLAY_CONFIGURE, BN_CLICKED, configure)
		COMMAND_HANDLER(IDC_OVERLAY_RENAME, BN_CLICKED, rename)
		COMMAND_HANDLER(IDC_OVERLAY_ADD, BN_CLICKED, add)
		COMMAND_HANDLER(IDC_OVERLAY_TYPE, CBN_SELENDOK, typeSelChanged)
		COMMAND_HANDLER(IDC_CHECK_VISIBLE, BN_CLICKED, visible)
		COMMAND_HANDLER(IDC_COMBO_OVERLAY, CBN_SELENDOK, setOverlay)
		COMMAND_HANDLER(IDC_COMBO_MODE, CBN_SELENDOK, mode)
		CHAIN_MSG_MAP(Dialog)
		REFLECT_NOTIFICATIONS()	// So that TreeViewCombo can handle its own DrawItem code.
	END_MSG_MAP()
};