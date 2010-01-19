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

// IINIPersister.h : Declaration of the CIINIPersister

#ifndef __IINIPERSISTER_H_
#define __IINIPERSISTER_H_

#include "resource.h"       // main symbols
#include <comdef.h>
#include "DictionaryPtr.h"

struct IP_KV
{
	char *key;
	char *value;
};

struct IP_PROPMAP
{
	char *key;
	IP_KV *value;
};

template <class K, class V>
class AutoDictionaryPtr2 : public DictionaryPtr<K, V>
{
public:
	AutoDictionaryPtr2(bool (*pEq)(const K* lhs, const K* rhs)) : DictionaryPtr<K,V>(pEq)
	{
	}

	~AutoDictionaryPtr2()
	{
		clearAndDestroy();
	}
};

template <class K, class V>
class AutoDictionaryPtr3 : public DictionaryPtr<K, V>
{
public:
	AutoDictionaryPtr3(bool (*pEq)(const K* lhs, const K* rhs)) : DictionaryPtr<K,V>(pEq)
	{
	}

	~AutoDictionaryPtr3()
	{
		for (int i=0; i<pTable->count(); i++)
		{
			Entry *e = (*pTable)[i];
			delete e->value;
		}
		clearAndDestroy();
	}
};

/////////////////////////////////////////////////////////////////////////////
// CIINIPersister
class ATL_NO_VTABLE CIINIPersister : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CIINIPersister, &CLSID_IINIPersister>,
	public ISupportErrorInfo,
	public IDispatchImpl<IIINIPersister, &IID_IIINIPersister, &LIBID_SYSSTATSCOMLib>
{
public:
typedef HRESULT (CIINIPersister::*IteratorCallbackPtr)(IDispatch*, ITypeInfo*, FUNCDESC*);

	CIINIPersister();
	~CIINIPersister();

DECLARE_REGISTRY_RESOURCEID(IDR_IINIPERSISTER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CIINIPersister)
	COM_INTERFACE_ENTRY(IIINIPersister)
	COM_INTERFACE_ENTRY(IPersister)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IIINIPersister
public:
	STDMETHOD(Configure)(/*[in]*/ ICOMINIFile *configFile, /*[in]*/ BSTR iniGroup, /*[in]*/ VARIANT_BOOL full, /*[in]*/ BSTR prefix, /*[in]*/ IClientModel *model, /*[in]*/ VARIANT_BOOL oldFormat);

// IPersister
public:
	STDMETHOD(Save)(/*[in]*/ IDispatch *pVal);
	STDMETHOD(Load)(/*[in]*/ IDispatch *pVal);

protected:
	HRESULT Iterate(IDispatch *pVal, long funcType, IteratorCallbackPtr pCallBack);

	// Functions that can be called from Iterate()
	HRESULT SaveValue(IDispatch *pVal, ITypeInfo *pTypeInfo, FUNCDESC *pFuncDesc);
	HRESULT LoadValue(IDispatch *pVal, ITypeInfo *pTypeInfo, FUNCDESC *pFuncDesc);

	ICOMINIFile *configFile;
	_bstr_t groupName;
	VARIANT_BOOL full;
	VARIANT_BOOL oldFormat;
	_bstr_t prefix;
	IClientModel *model;

	// A map to help convert from old format to new!
	static AutoDictionaryPtr3<char, AutoDictionaryPtr2<char, char> > propertyMap;
	static int count;

private:
	CIINIPersister(const CIINIPersister&) { AtlTrace("CIINIPersister::CIINIPersister(const CIINIPersister&)\n"); }
};

#endif //__IINIPERSISTER_H_
