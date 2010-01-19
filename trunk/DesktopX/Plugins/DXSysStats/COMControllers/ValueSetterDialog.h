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

// ValueSetterDialog.h : Declaration of the ValueSetterDialog

#ifndef __VALUESETTERDIALOG_H_
#define __VALUESETTERDIALOG_H_

#include "Dialog.h"
#include "PickList.h"

/////////////////////////////////////////////////////////////////////////////
// ValueSetterDialog

class CValueSetter;

class ValueSetterDialog : public Dialog
{
public:
	ValueSetterDialog(CValueSetter *pController, IObserver *observer, IMeterHome *meters, WORD idd=IDD_VALUESETTER);
	virtual ~ValueSetterDialog();

protected:
	void resetCombos(long meterIndex);
	void resetValueCombo(_bstr_t selectorValue);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT value(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT selector(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT setMeter(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	CValueSetter *pController;
	PickList<long> meterList;
	PickList<_bstr_t> selectorList;
	PickList<_bstr_t> valueList;
	IMeterHome *meters;
	_bstr_t xml;

public:
	BEGIN_MSG_MAP(ValueSetterDialog)
		CHAIN_MSG_MAP(Dialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_COMBO_METER, CBN_SELENDOK, setMeter)
		COMMAND_ID_HANDLER(IDC_COMBO_VALUE, value)
		COMMAND_ID_HANDLER(IDC_COMBO_SELECTOR, selector)
	END_MSG_MAP()
};
#endif // __VALUESETTERDIALOG_H_
