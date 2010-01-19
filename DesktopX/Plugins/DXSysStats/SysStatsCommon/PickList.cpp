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

// PickList.cpp: implementation of the PickList class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <ComUtil.h>
#include "PickList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template <class C>
PickList<C>::PickList(EqFuncPtr equals, ToStrFuncPtr toStr, FromStrFuncPtr fromStr, BOOL fixed) :
	fixed(fixed), pEquals(equals), pToStr(toStr), pFromStr(fromStr)
{
}

template <class C>
PickList<C>::~PickList()
{
}

template <class C>
LRESULT PickList<C>::init(HWND hDlg, WORD wId, const char* labels[], C* data, C curVal)
{
	this->hDlg = hDlg;
	this->wId = wId;

	char buf[MAX_PATH];
	(*pToStr)(buf, sizeof(buf), curVal);
	SetDlgItemText(hDlg, wId, buf);

	for (int i=0; labels[i]; i++)
	{
		int index = SendDlgItemMessage(hDlg,wId,CB_ADDSTRING,0,(LPARAM)labels[i]);
		SendDlgItemMessage(hDlg,wId,CB_SETITEMDATA,index,(LPARAM)data[i]);
		if ((*pEquals)(curVal,data[i]))
			SendDlgItemMessage(hDlg,wId,CB_SETCURSEL,(WPARAM)index,0);
	}

	return 1;
}

template <class C>
LRESULT PickList<C>::init(HWND hDlg, WORD wId, _bstr_t labels[], C* data, C curVal)
{
	this->hDlg = hDlg;
	this->wId = wId;

	char buf[MAX_PATH];
	(*pToStr)(buf, sizeof(buf), curVal);
	SetDlgItemText(hDlg, wId, buf);

	for (int i=0; !!labels[i]; i++)
	{
		int index = SendDlgItemMessage(hDlg,wId,CB_ADDSTRING,0,(LPARAM)((char*)labels[i]));
		SendDlgItemMessage(hDlg,wId,CB_SETITEMDATA,index,(LPARAM)data[i]);
		if ((*pEquals)(curVal,data[i]))
			SendDlgItemMessage(hDlg,wId,CB_SETCURSEL,(WPARAM)index,0);
	}

	return 1;
}

template <class C>
LRESULT PickList<C>::init(HWND hDlg, WORD wId, _bstr_t filename, C curVal)
{
	this->hDlg = hDlg;
	this->wId = wId;

	char buf[MAX_PATH];
	(*pToStr)(buf, sizeof(buf), curVal);
	SetDlgItemText(hDlg, wId, buf);

	FILE *messageFile = _wfopen(filename, L"r");
	if (messageFile)
	{
		char key[MAX_PATH];
		short value;
		*key=0;
		value=0;
		while (fscanf(messageFile, "%[^=]=%hi\n", key, &value) == 2)
		{
			int index = SendDlgItemMessage(hDlg,wId,CB_ADDSTRING,0,(LPARAM)key);
			SendDlgItemMessage(hDlg,wId,CB_SETITEMDATA,index,(LPARAM)value);
			if ((*pEquals)(curVal,value))
				SendDlgItemMessage(hDlg,wId,CB_SETCURSEL,(WPARAM)index,0);
		}

		fclose(messageFile);
	}

	return 1;
}

template <class C>
LRESULT PickList<C>::init(HWND hDlg, WORD wId, IMeterHome *meters, C curVal)
{
	this->hDlg = hDlg;
	this->wId = wId;

	BSTR s;
	short count;
	meters->Count(&count);

	int index = SendDlgItemMessage(hDlg,wId,CB_ADDSTRING,0,(LPARAM)"(none)");
	SendDlgItemMessage(hDlg,wId,CB_SETITEMDATA,index,(LPARAM)-1);

	if ((*pEquals)(curVal,-1))
		SendDlgItemMessage(hDlg,wId,CB_SETCURSEL,(WPARAM)index,0);

	for (int i=0; i<count; i++)
	{
		IMeter *meter;
		meters->Item(CComVariant(i), &meter);
		meter->get_Name(&s);
		index = SendDlgItemMessage(hDlg,wId,CB_ADDSTRING,0,(LPARAM)((char*)_bstr_t(s, false)));
		SendDlgItemMessage(hDlg,wId,CB_SETITEMDATA,index,(LPARAM)i);
		if ((*pEquals)(curVal,i))
			SendDlgItemMessage(hDlg,wId,CB_SETCURSEL,(WPARAM)index,0);
		meter->Release();
	}

	return 1;
}

template <class C>
LRESULT PickList<C>::init(HWND hDlg, WORD wId, ICompositeOverlay *overlays, C curVal)
{
	this->hDlg = hDlg;
	this->wId = wId;

	BSTR s;
	short count;
	overlays->Count(&count);

	int index = SendDlgItemMessage(hDlg,wId,CB_ADDSTRING,0,(LPARAM)"(none)");
	SendDlgItemMessage(hDlg,wId,CB_SETITEMDATA,index,(LPARAM)-1);
	if ((*pEquals)(curVal,-1))
		SendDlgItemMessage(hDlg,wId,CB_SETCURSEL,(WPARAM)index,0);

	for (int i=0; i<count; i++)
	{
		IOverlay *overlay;
		overlays->Item(CComVariant(i), &overlay);
		overlay->get_Name(&s);
		index = SendDlgItemMessage(hDlg,wId,CB_ADDSTRING,0,(LPARAM)((char*)_bstr_t(s, false)));
		SendDlgItemMessage(hDlg,wId,CB_SETITEMDATA,index,(LPARAM)i);
		if ((*pEquals)(curVal,i))
			SendDlgItemMessage(hDlg,wId,CB_SETCURSEL,(WPARAM)index,0);
		overlay->Release();
	}

	return 1;
}

template <class C>
LRESULT PickList<C>::init(HWND hDlg, WORD wId, BSTR xmlDoc, BSTR xpath, C curVal)
{
	::SendDlgItemMessage(hDlg, wId, CB_RESETCONTENT, 0, 0);

	this->hDlg = hDlg;
	this->wId = wId;

	// In case any of the XML stuff fails
	selectValue(curVal);

	try
	{
		::MSXML2::IXMLDOMDocumentPtr pXMLDoc;
		pXMLDoc.CreateInstance("Msxml2.DOMDocument", NULL, CLSCTX_INPROC_SERVER);
		pXMLDoc->async = false;
		pXMLDoc->loadXML(xmlDoc);

		valueSet = pXMLDoc->selectSingleNode(xpath);
		if (valueSet != NULL)
		{
			::MSXML2::IXMLDOMNodeListPtr items = valueSet->childNodes;
			::MSXML2::IXMLDOMNodePtr item = items->nextNode();
			int i=0;
			while (item != NULL)
			{
				_bstr_t value = item->selectSingleNode(L"value")->nodeTypedValue;
				_bstr_t label = item->selectSingleNode(L"label")->nodeTypedValue;
				int index = SendDlgItemMessage(hDlg,wId,CB_ADDSTRING,0,(LPARAM)((char*)label));
				SendDlgItemMessage(hDlg,wId,CB_SETITEMDATA,index,(LPARAM)i);
				if ((*pEquals)(curVal,value))
					SendDlgItemMessage(hDlg,wId,CB_SETCURSEL,(WPARAM)index,0);
				item = items->nextNode();
				i++;
			}

		}
	} catch (...)
	{
		AtlTrace("Exception trying to build picklist from XML fragment\n");
	}

	return 1;
}

template <class C>
LRESULT PickList<C>::getData(C *out, C def)
{
	*out = def;

	LRESULT res = SendDlgItemMessage(hDlg,wId,CB_GETCURSEL,0,0);
	if (res != CB_ERR)
	{
		*out = SendDlgItemMessage(hDlg,wId,CB_GETITEMDATA,(WPARAM)res,0);
	}
	else if (!fixed)
	{
		char s[MAX_PATH];
		GetDlgItemText(hDlg,wId, s, sizeof(s));
		(*pFromStr)(out, s);
	}

	return 1;
}

template <class C>
LRESULT PickList<C>::select(C curVal)
{
	C data;
	bool set = false;
	short message = 0;

	for (LRESULT i = SendDlgItemMessage(hDlg,wId,CB_GETCOUNT,0,0); i > 0 && !set; i--)
	{
		data = SendDlgItemMessage(hDlg,wId,CB_GETITEMDATA,i-1,0);
		if (data == curVal)
		{
			SendDlgItemMessage(hDlg,wId,CB_SETCURSEL,i-1,0);
			set = true;
		}
	}

	if (!set)
	{
		char buf[MAX_PATH];
		(*pToStr)(buf, sizeof(buf), curVal);
		SetDlgItemText(hDlg, wId, buf);
		set = true;
	}

	return set;
}

template <class C>
LRESULT PickList<C>::getValue(C *out, C def)
{
	int data = -1;

	LRESULT res = SendDlgItemMessage(hDlg,wId,CB_GETCURSEL,0,0);
	if (res != CB_ERR)
	{
		data = SendDlgItemMessage(hDlg,wId,CB_GETITEMDATA,(WPARAM)res,0);
		try {
			wchar_t buf[MAX_PATH];
			::_snwprintf(buf, MAX_PATH, L"item[%d]/value", data);
			*out = valueSet->selectSingleNode(buf)->nodeTypedValue;
		} catch (...) {
			AtlTrace("Excepton tying to retrieve picklist value from XML fragment\n");
		}
	}
	else if (!fixed)
	{
		char s[MAX_PATH];
		GetDlgItemText(hDlg,wId, s, sizeof(s));
		(*pFromStr)(out, s);
	}
	else
		*out = def;

	return 1;
}

template <class C>
LRESULT PickList<C>::getItemID(C *out, C def)
{
	int data = -1;

	LRESULT res = SendDlgItemMessage(hDlg,wId,CB_GETCURSEL,0,0);
	if (res != CB_ERR)
	{
		data = SendDlgItemMessage(hDlg,wId,CB_GETITEMDATA,(WPARAM)res,0);
		try {
			wchar_t buf[MAX_PATH];
			::_snwprintf(buf, MAX_PATH, L"item[%d]/@id", data);
			*out = valueSet->selectSingleNode(buf)->nodeTypedValue;
			return 1;
		} catch (...) {
			AtlTrace("Excepton tying to retrieve picklist value from XML fragment\n");
		}
	}

	return 0;
}

template <class C>
LRESULT PickList<C>::selectValue(C curVal)
{
	LRESULT i = ::SendDlgItemMessage(hDlg, wId,  CB_FINDSTRINGEXACT, -1, (LPARAM)((char*)curVal));
	if (i != CB_ERR)
	{
		SendDlgItemMessage(hDlg,wId,CB_SETCURSEL,i,0);
	}
	else
	{
		char buf[MAX_PATH];
		(*pToStr)(buf, sizeof(buf), curVal);
		SetDlgItemText(hDlg, wId, buf);
	}

	return true;
}
