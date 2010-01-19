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

// MeterHome.h: Definition of the CMeterHome class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_METERHOME_H__9244E5B9_EAAD_4E54_91A7_C45B135E8910__INCLUDED_)
#define AFX_METERHOME_H__9244E5B9_EAAD_4E54_91A7_C45B135E8910__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ArrayPtrVariant.cpp"

#include "resource.h"       // main symbols

class COMTimerService;
class Observer;

/////////////////////////////////////////////////////////////////////////////
// CMeterHome

class CMeterHome : 
	public IDispatchImpl<ArrayPtrVariant<IMeter, IMeterHome>, &IID_IMeterHome, &LIBID_SYSSTATSCOMLib>, 
	public ISupportErrorInfo,
	public CComObjectRoot,
	public CComCoClass<CMeterHome,&CLSID_MeterHome>
{
// COM Part
public:
	CMeterHome();

BEGIN_COM_MAP(CMeterHome)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IMeterHome)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CMeterHome) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_REGISTRY_RESOURCEID(IDR_MeterHome)
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IMeterHome
public:
	STDMETHOD(Remove)(/*[in]*/ VARIANT index);
	STDMETHOD(Add)(/*[in]*/ IMeter *pMeter);
	STDMETHOD(Item)(VARIANT, IMeter**);
	STDMETHOD(get_Meters)(/*[out, retval]*/ LPSAFEARRAY *pVal);
	STDMETHOD(put_Meters)(/*[in]*/ LPSAFEARRAY newVal);

// Non-COM part
public:
	STDMETHOD(get_Model)(/*[out, retval]*/ IClientModel **pVal);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel *newVal);
	STDMETHOD(Update)(/*[out, retval]*/ long *dirty);
	STDMETHOD(get_TimerService)(/*[out, retval]*/ ICOMTimerService* *pVal);
	STDMETHOD(put_TimerService)(/*[in]*/ ICOMTimerService* newVal);
#ifdef notdef
	STDMETHOD(Save)(/*[in]*/ BSTR ini, /*[in]*/ BSTR iniGroup, /*[in]*/ VARIANT_BOOL full, /*[in]*/ BSTR prefix);
	STDMETHOD(Load)(/*[in]*/ BSTR ini, /*[in]*/ BSTR iniGroup, /*[in]*/ BSTR prefix);
#endif
	virtual ~CMeterHome();

protected:
	static const char* UNKNOWN;
	static const char* METER_COUNT_PROPERTY;
	static const char* METER_INDEX_PROPERTY;
	static const char* METER_PREFIX;
	static const char* METER_TYPE_PROPERTY;

private:
	ICOMTimerService *timerService;
	IClientModel *model;
// IMeter
};

#endif // !defined(AFX_METERHOME_H__9244E5B9_EAAD_4E54_91A7_C45B135E8910__INCLUDED_)
