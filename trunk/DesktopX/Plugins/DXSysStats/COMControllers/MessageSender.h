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

// MessageSender.h : Declaration of the CMessageSender

#ifndef __MESSAGESENDER_H_
#define __MESSAGESENDER_H_

#include "resource.h"       // main symbols
#include "SysStatsControllerCategory.h"
#include "MessageSenderImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CMessageSender
class ATL_NO_VTABLE CMessageSender :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMessageSender, &CLSID_MessageSender>,
	public IDispatchImpl<MessageSenderImpl<IMessageSender>, &IID_IMessageSender, &LIBID_COMCONTROLLERSLib>
{
public:
	CMessageSender();

DECLARE_REGISTRY_RESOURCEID(IDR_MESSAGESENDER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMessageSender)
	COM_INTERFACE_ENTRY(IMessageSender)
	COM_INTERFACE_ENTRY(IController)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CMessageSender)
	IMPLEMENTED_CATEGORY(CATID_SysStatsControllerCategory)
END_CATEGORY_MAP()


// IMessageSender
public:

// IController
};

#endif //__MESSAGESENDER_H_
