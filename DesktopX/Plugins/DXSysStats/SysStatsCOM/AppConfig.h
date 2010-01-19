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

// AppConfig.h : Declaration of the CAppConfig

#ifndef __APPCONFIG_H_
#define __APPCONFIG_H_

#include "resource.h"       // main symbols
#include <comdef.h>

/////////////////////////////////////////////////////////////////////////////
// CAppConfig
class ATL_NO_VTABLE CAppConfig : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAppConfig, &CLSID_AppConfig>,
	public ISupportErrorInfo,
	public IDispatchImpl<IAppConfig, &IID_IAppConfig, &LIBID_SYSSTATSCOMLib>
{
public:
	CAppConfig();
	~CAppConfig();

DECLARE_REGISTRY_RESOURCEID(IDR_APPCONFIG)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CAppConfig)
	COM_INTERFACE_ENTRY(IAppConfig)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IAppConfig
public:
	STDMETHOD(AltCanonicalPath)(/*[in]*/ BSTR path, /*[out, retval]*/ BSTR *pRet);
	STDMETHOD(get_AltRoot)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_AltRoot)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_InstallDir)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_InstallDir)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ApplicationKey)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ApplicationKey)(/*[in]*/ BSTR newVal);
	STDMETHOD(CanonicalPath)(/*[in]*/ BSTR path, /*[out, retval]*/ BSTR *pRet);
	STDMETHOD(get_CWD)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_CWD)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_RootPath)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(NormalizePath)(/*[in]*/ BSTR path, /*[out, retval]*/ BSTR *pRet);
	STDMETHOD(ReadString)(HKEY base, BSTR path, BSTR key, BSTR *pRet);

protected:
	_bstr_t cwd;
	_bstr_t installDir;
	_bstr_t altRoot;
	_bstr_t applicationKey;

	static const _bstr_t APPLICATION_KEY;
	static const _bstr_t INSTALL_DIR_KEY;
	static const _bstr_t CWD_KEY;
};

#endif //__APPCONFIG_H_
