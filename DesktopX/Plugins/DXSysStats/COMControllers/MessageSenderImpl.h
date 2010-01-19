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

// MessageSenderImpl.h : Declaration of the CMessageSender

#ifndef __MESSAGESENDERIMPL_H_
#define __MESSAGESENDERIMPL_H_

#include "resource.h"       // main symbols
#include "SysStatsControllerCategory.h"
#include "ControllerImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CMessageSender
template <class B>
class MessageSenderImpl : public ControllerImpl<B>
{
public:
	MessageSenderImpl();
	virtual ~MessageSenderImpl();

// IMessageSender
public:
	STDMETHOD(get_UseLParam)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_UseLParam)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_UseWParam)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_UseWParam)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_LParam)(/*[out, retval]*/ UINT *pVal);
	STDMETHOD(put_LParam)(/*[in]*/ UINT newVal);
	STDMETHOD(get_WParam)(/*[out, retval]*/ UINT *pVal);
	STDMETHOD(put_WParam)(/*[in]*/ UINT newVal);
	STDMETHOD(get_SendMessage)(/*[out, retval]*/ UINT *pVal);
	STDMETHOD(put_SendMessage)(/*[in]*/ UINT newVal);

// IController
	STDMETHOD(HandleMessage)(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled);
	STDMETHOD(Configure)(IObserver * observer, IMeterHome * meters, LONG hDlg);
	STDMETHOD(get_Type)(BSTR * pVal);

protected:
	UINT sendMessage;	// Message we will post with mapped value
	VARIANT_BOOL useWParam;
	UINT wParam;		// The WPARAM value we will post
	VARIANT_BOOL useLParam;
	UINT lParam;		// The LPARAM value we will post
};

#endif //__MESSAGESENDERIMPL_H_
