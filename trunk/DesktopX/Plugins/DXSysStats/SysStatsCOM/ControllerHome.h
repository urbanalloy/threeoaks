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

// ControllerHome.h : Declaration of the CControllerHome

#ifndef __CONTROLLERHOME_H_
#define __CONTROLLERHOME_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ArrayPtrVariant.cpp"

#include "resource.h"       // main symbols

class COMTimerService;
class Observer;

/////////////////////////////////////////////////////////////////////////////
// CControllerHome

class CControllerHome : 
	public IDispatchImpl<ArrayPtrVariant<IController, IControllerHome>, &IID_IControllerHome, &LIBID_SYSSTATSCOMLib>, 
	public ISupportErrorInfo,
	public CComObjectRoot,
	public CComCoClass<CControllerHome,&CLSID_ControllerHome>
{
// COM Part
public:
	CControllerHome();

BEGIN_COM_MAP(CControllerHome)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IControllerHome)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CControllerHome) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_REGISTRY_RESOURCEID(IDR_CONTROLLERHOME)
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IControllerHome
public:
	STDMETHOD(Remove)(/*[in]*/ VARIANT index);
	STDMETHOD(Add)(/*[in]*/ IController *pController);
	STDMETHOD(Item)(VARIANT, IController**);
	STDMETHOD(get_Controllers)(/*[out, retval]*/ LPSAFEARRAY *pVal);
	STDMETHOD(put_Controllers)(/*[in]*/ LPSAFEARRAY newVal);

// Non-COM part
public:
	STDMETHOD(Swap)(/*[in]*/ VARIANT one, /*[in]*/ VARIANT two);
	STDMETHOD(get_Model)(/*[out, retval]*/ IClientModel **pVal);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel *newVal);
	STDMETHOD(HandleMessage)(/*[in]*/ short messageID, /*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *dirty);
#ifdef notdef
	STDMETHOD(Save)(/*[in]*/ BSTR ini, /*[in]*/ BSTR iniGroup, /*[in]*/ VARIANT_BOOL full, /*[in]*/ BSTR prefix);
	STDMETHOD(Load)(/*[in]*/ BSTR ini, /*[in]*/ BSTR iniGroup, /*[in]*/ BSTR prefix);
#endif
	virtual ~CControllerHome();

protected:
	static const char* UNKNOWN;
	static const char* CONTROLLER_COUNT_PROPERTY;
	static const char* CONTROLLER_INDEX_PROPERTY;
	static const char* CONTROLLER_PREFIX;
	static const char* CONTROLLER_TYPE_PROPERTY;

private:
	IClientModel *model;
	ISimpleOverlay2 *pMouseOverOverlay;
};

#endif //__CONTROLLERHOME_H_
