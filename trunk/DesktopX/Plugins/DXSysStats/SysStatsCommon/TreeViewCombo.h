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

// TreeViewCombo.h: interface for the TreeViewCombo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREEVIEWCOMBO_H__88E1E609_9703_4F7A_990B_1DE49DAAD1C2__INCLUDED_)
#define AFX_TREEVIEWCOMBO_H__88E1E609_9703_4F7A_990B_1DE49DAAD1C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlapp.h>
#include <atlmisc.h>
#include <atlcrack.h>
#include <atlctrls.h>

#define WM_SELECTITEM_CHANGE (WM_USER + 102)

class TreeViewCombo;

/*
 * A sub-class of the basic TreeView Control that plays nicely with TreeViewCombo.
 *
 * This code requires WTL.
 */
class TreeViewComboCtrl : public CWindowImpl<TreeViewComboCtrl, CTreeViewCtrl>  
{
public:
	TreeViewComboCtrl(TreeViewCombo* pCombo);
	virtual ~TreeViewComboCtrl();

	void Display(CRect rc);
	void SelectNextItem(BOOL selectNext);
	BOOL Select(HTREEITEM hItem, UINT nCode);
	int SelectItem(HTREEITEM hItem);

protected:
	void HideAndInform();
	void Inform();
	HWND GetParentOwner(HWND hWnd);

	// Treeview handlers
	void OnKeyDown(TCHAR keyCode, UINT nRptCnt, UINT nFlags);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnKillFocus(HWND newHwnd);
	virtual LRESULT OnSelChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	TreeViewCombo *m_combo;

	BEGIN_MSG_MAP_EX(TreeViewComboCtrl)
		MSG_WM_KILLFOCUS(OnKillFocus)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MESSAGE_HANDLER(WM_SELECTITEM_CHANGE, OnSelChanged)
	END_MSG_MAP()
};

/*
 * A sub-class of the basic combobox that uses a treeview drop down rather than
 * a list view.
 *
 * This code requires WTL.
 */
class TreeViewCombo : public CWindowImpl<TreeViewCombo, CComboBox>  
{
public:
	TreeViewCombo();
	virtual ~TreeViewCombo();

	// Override some CComboBox functions
	BOOL		SubclassWindow(HWND hWnd, UINT ctrlID);
	BOOL		GetDroppedState();

	// Add some extra ones
	int			GetDroppedHeight();
	UINT		GetControlID();
	void		SetDroppedHeight(int height);

	// Proxy for the tree view control
	HTREEITEM	InsertItem(TVINSERTSTRUCT *ivns);
	BOOL		Expand(HTREEITEM hItem, UINT nCode = TVE_EXPAND);
	BOOL		Select(HTREEITEM hItem, UINT nCode);
	BOOL		Delete(HTREEITEM hItem);
	HTREEITEM	GetNext(HTREEITEM hItem, UINT flags);
	BOOL		GetItem(LPTVITEM hItem);

	virtual LRESULT SetText(LPCTSTR lpszText);

protected:
	void DisplayTree();

	// TreeViewCombo Handlers
	void OnKeyDown(TCHAR keyCode, UINT nRptCnt, UINT nFlags);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnDrawItem(UINT ctrlID, LPDRAWITEMSTRUCT diStruct);
	void OnMeasureItem(UINT ctrlID, LPMEASUREITEMSTRUCT miStruct);

	virtual LRESULT OnInsertItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnExpand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDelete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnGetNext(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnGetItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

//#define TRACE_MESSAGES

#ifdef TRACE_MESSAGES
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0)
	{
		switch (uMsg)
		{
		case WM_NOTIFY:
			AtlTrace("Notification: %d - MsgMap ID=%d\n", ((LPNMHDR)lParam)->code, dwMsgMapID);
			break;
		case WM_COMMAND:
			AtlTrace("Command: 0x%0.4x - MsgMap ID=%d\n", HIWORD(wParam), dwMsgMapID);
			break;
		default:
			AtlTrace("Message: 0x%0.4x - MsgMap ID=%d\n", uMsg, dwMsgMapID);
			break;
		}

		return false;
	}
#else
	BEGIN_MSG_MAP_EX(TreeViewCombo)
		// Messages we need to operate
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		MSG_OCM_DRAWITEM(OnDrawItem)
		MSG_WM_MEASUREITEM(OnMeasureItem)

		// Pass through TreeView messages
		MESSAGE_HANDLER(TVM_INSERTITEM, OnInsertItem)
		MESSAGE_HANDLER(TVM_EXPAND, OnExpand)
		MESSAGE_HANDLER(TVM_SELECTITEM, OnSelect)
		MESSAGE_HANDLER(TVM_DELETEITEM, OnDelete)
		MESSAGE_HANDLER(TVM_GETNEXTITEM, OnGetNext)
		MESSAGE_HANDLER(TVM_GETITEM, OnGetItem)
	END_MSG_MAP()
#endif


protected:
	TreeViewComboCtrl m_treeCtrl;
	int m_droppedHeight;
	UINT ctrlID;
};

#endif // !defined(AFX_TREEVIEWCOMBO_H__88E1E609_9703_4F7A_990B_1DE49DAAD1C2__INCLUDED_)
