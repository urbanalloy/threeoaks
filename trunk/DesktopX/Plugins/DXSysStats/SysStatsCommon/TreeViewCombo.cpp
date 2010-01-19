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

// TreeViewCombo.cpp: implementation of the TreeViewCombo class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_DEBUG)
#define AFX_CDECL __cdecl


// Memory tracking allocation
void* AFX_CDECL operator new(size_t nSize, const char* lpszFileName, int nLine);
#define DEBUG_NEW new(THIS_FILE, __LINE__)
#if _MSC_VER >= 1200
void AFX_CDECL operator delete(void* p, const char* lpszFileName, int nLine);
#endif

#endif // _DEBUG

#include <windows.h>
#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlctl.h>

#include "resource.h"
#include "TreeViewCombo.h"

/*************** TreeCtrl functions ******************/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TreeViewComboCtrl::TreeViewComboCtrl(TreeViewCombo *pCombo) :
	m_combo(pCombo)
{
}

TreeViewComboCtrl::~TreeViewComboCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// TreeViewComboCtrl message handlers

void TreeViewComboCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	UINT flags ;
	HTREEITEM item = HitTest(point, &flags);
	if (item != NULL)
		SelectItem(item);
		
	SetMsgHandled(false);	// Allow default processing
}

void TreeViewComboCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	UINT flags;
	HTREEITEM item = HitTest(point, &flags);
	if (flags!=TVHT_ONITEMBUTTON)
	{
		if (item == NULL)
			item = CTreeViewCtrl::GetSelectedItem();
		SelectItem(item);
		HideAndInform();
		return;
	}

	SetMsgHandled(false);	// Allow default processing
}

void TreeViewComboCtrl::OnKillFocus(HWND newHwnd) 
{
	ShowWindow(SW_HIDE);

	SetMsgHandled(false);	// Allow default processing
}

void TreeViewComboCtrl::OnKeyDown(TCHAR keyCode, UINT nRepCnt, UINT nFlags) 
{
	switch (keyCode)
	{
	case VK_RETURN:
		Inform();
		// Fall through
	case VK_ESCAPE:
		ShowWindow(SW_HIDE);
		break;
	default:
		SetMsgHandled(false);
	}
}

HWND TreeViewComboCtrl::GetParentOwner(HWND hWnd)
{
	HWND wnd = ::GetParent(hWnd);
	WINDOWINFO wi;
	::memset(&wi, 0, sizeof(wi));
	::GetWindowInfo(wnd, &wi);
	if (wi.dwStyle & WS_CHILD)
		return GetParentOwner(wnd);
	else
		return wnd;
}

void TreeViewComboCtrl::Display(CRect rc)
{	
	SetWindowPos(HWND_NOTOPMOST, rc.left, rc.top, rc.right, rc.bottom, SWP_SHOWWINDOW);
	ShowWindow(SW_SHOWNORMAL);	// SO that IsVisible() works :(

	HWND topParent = GetParentOwner(GetParent());

    if (topParent != NULL)
	{
		::SendMessage(topParent, WM_NCACTIVATE, (WPARAM)TRUE, (LPARAM)0);
		::SendMessage(topParent, WM_SETREDRAW, (WPARAM)TRUE, (LPARAM)0);
	}
}

void TreeViewComboCtrl::HideAndInform()
{
	ShowWindow(SW_HIDE);
	Inform();
}

void TreeViewComboCtrl::Inform()
{
	HWND parent = GetParent();
	if (parent != NULL)
	{
		m_combo->SetFocus();

		::SendMessage(m_combo->GetParent(), WM_COMMAND, MAKEWPARAM(m_combo->GetControlID(), CBN_SELENDOK), (LPARAM)m_combo->m_hWnd);
	}
}

int TreeViewComboCtrl::SelectItem(HTREEITEM item)
{
	return CWindowImpl<TreeViewComboCtrl, CTreeViewCtrl>::SelectItem(item);
}

LRESULT TreeViewComboCtrl::OnSelChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::SendMessage(m_combo->GetParent(), WM_COMMAND, MAKEWPARAM(m_combo->GetControlID(), CBN_SELENDOK), (LPARAM)m_combo->m_hWnd);

	return 1;
}

BOOL TreeViewComboCtrl::Select(HTREEITEM hItem, UINT nCode)
{
	return CWindowImpl<TreeViewComboCtrl, CTreeViewCtrl>::Select(hItem, nCode);
}

void TreeViewComboCtrl::SelectNextItem(BOOL selectNext)
{
	HTREEITEM item = GetSelectedItem();
	if (selectNext)
		item = GetNextVisibleItem(item);
	else
		item = GetPrevVisibleItem(item);
	if (item != NULL)
	{
		SelectItem(item);
		Inform();
	}
}

/*************** ComboBox functions ******************/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TreeViewCombo::TreeViewCombo() : m_treeCtrl(this), ctrlID(0)
{
}

TreeViewCombo::~TreeViewCombo()
{

}

HTREEITEM TreeViewCombo::InsertItem(TVINSERTSTRUCT *ivns)
{
	HTREEITEM ret = m_treeCtrl.InsertItem(ivns);
	Invalidate();
	return ret;
}

LRESULT TreeViewCombo::OnInsertItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return (LRESULT)InsertItem((TVINSERTSTRUCT*)lParam);
}

BOOL TreeViewCombo::Expand(HTREEITEM hItem, UINT nCode)
{
	return m_treeCtrl.Expand(hItem, nCode);
}

LRESULT TreeViewCombo::OnExpand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return Expand((HTREEITEM)lParam, (UINT)wParam);
}

BOOL TreeViewCombo::Select(HTREEITEM hItem, UINT nCode)
{
	return m_treeCtrl.Select(hItem, nCode);
}

LRESULT TreeViewCombo::OnSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return Select((HTREEITEM)lParam, (UINT)wParam);
}

BOOL TreeViewCombo::Delete(HTREEITEM hItem)
{
	return m_treeCtrl.DeleteItem(hItem);
}

LRESULT TreeViewCombo::OnDelete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT ret = Delete((HTREEITEM)lParam);
	Invalidate();
	return ret;
}

HTREEITEM TreeViewCombo::GetNext(HTREEITEM hItem, UINT flags)
{
	return m_treeCtrl.GetNextItem(hItem, flags);
}

LRESULT TreeViewCombo::OnGetNext(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT ret = (LRESULT)GetNext((HTREEITEM)lParam, wParam);
	return ret;
}

BOOL TreeViewCombo::GetItem(LPTVITEM item)
{
	return m_treeCtrl.GetItem(item);
}

LRESULT TreeViewCombo::OnGetItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT ret = GetItem((LPTVITEM)lParam);
	return ret;
}

BOOL TreeViewCombo::GetDroppedState( )
{
	return m_treeCtrl.IsWindowVisible();
}

void TreeViewCombo::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_treeCtrl.IsWindowVisible())
		m_treeCtrl.ShowWindow(SW_HIDE);
	else
		DisplayTree();
}

void TreeViewCombo::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags, point);
}

void TreeViewCombo::DisplayTree()
{
	CRect rc;
	GetWindowRect(rc);
	rc.top = rc.bottom + 1;
	rc.right = GetDroppedWidth();
	rc.bottom = GetDroppedHeight();
	m_treeCtrl.Display(rc);
}

void TreeViewCombo::OnDrawItem(UINT ctrlID, LPDRAWITEMSTRUCT pDIStruct)
{
	CDC dc;

	dc.Attach(pDIStruct->hDC);

	CRect rc(pDIStruct->rcItem);

	if (pDIStruct->itemState & ODS_DISABLED)
		dc.FillSolidRect(rc, GetSysColor(COLOR_BTNFACE) );
	else if(pDIStruct->itemState & ODS_SELECTED)
	{
		dc.FillSolidRect(rc, GetSysColor(COLOR_HIGHLIGHT) );
		dc.DrawFocusRect(rc);
		dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else
		dc.SetTextColor( GetSysColor(COLOR_WINDOWTEXT) );

	char buf[MAX_PATH];
	if (m_treeCtrl.GetItemText(m_treeCtrl.GetSelectedItem(), buf, sizeof(buf)))
	{
		rc.top += 1;
		rc.bottom -= 1;
		rc.left += 1;
		CSize size;
		dc.GetTextExtent(buf, strlen(buf), &size);
		rc.right = rc.left + size.cx + 4;
		rc.left += 2;
		dc.DrawText(buf, strlen(buf), rc, DT_SINGLELINE | DT_VCENTER);
	}

	dc.Detach();
}

void TreeViewCombo::OnMeasureItem(UINT ctrlID, LPMEASUREITEMSTRUCT pMIStruct)
{
	// Whatever
}

LRESULT TreeViewCombo::SetText(LPCTSTR lpszText)
{
	return ::SendMessage(m_hWnd,(UINT) WM_SETTEXT, (WPARAM) 0, (LPARAM) lpszText);
}

void TreeViewCombo::OnKeyDown(TCHAR keyCode, UINT nRepCnt, UINT nFlags) 
{
	switch (keyCode)
	{
	case VK_DOWN:
	case VK_RIGHT:
		m_treeCtrl.SelectNextItem(TRUE);
		Invalidate();
		break;
	case VK_UP:
	case VK_LEFT:
		m_treeCtrl.SelectNextItem(TRUE);
		Invalidate();
		break;
	default:
		SetMsgHandled(false);
	}
}

UINT TreeViewCombo::GetControlID()
{
	return ctrlID;
}

int TreeViewCombo::GetDroppedHeight()
{
	return m_droppedHeight;
}

void TreeViewCombo::SetDroppedHeight(int height)
{
	m_droppedHeight = height;
}

BOOL TreeViewCombo::SubclassWindow(HWND hWnd, UINT cID)
{
	ctrlID = cID;
	BOOL ret = CWindowImpl<TreeViewCombo, CComboBox>::SubclassWindow(hWnd);

	if (ret)
	{
		COMBOBOXINFO ci;
		ci.cbSize = sizeof(ci);

		// Subclass the treeview control and get the desired drop-down height.
		if (::GetComboBoxInfo(m_hWnd, &ci))
		{
			CRect rc(0, 0, 10, 10);
			UINT style =  WS_POPUP | WS_BORDER | TVS_DISABLEDRAGDROP | TVS_LINESATROOT | TVS_HASLINES | TVS_HASBUTTONS | TVS_FULLROWSELECT | TVS_SHOWSELALWAYS;
			HWND ddHwnd = ::CreateWindowEx (0, WC_TREEVIEW, NULL, style, 0, 0, 10, 10, GetParent(), 0, NULL, 0);
			m_treeCtrl.SubclassWindow(ddHwnd);

			RECT rect;
			::GetWindowRect(ci.hwndList, &rect);
			SetDroppedHeight(rect.bottom - rect.top);
		}
	}

	return ret;
}