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

// Player.h : Declaration of the CPlayer

#ifndef __PLAYER_H_
#define __PLAYER_H_

#include <atlwin.h>
#include "resource.h"       // main symbols
#include "SysStatsMeterCategory.h"
#include "MeterImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CPlayer
class ATL_NO_VTABLE CPlayer :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPlayer, &CLSID_Player>,
	public ISupportErrorInfo,
	public IDispatchImpl<MeterImpl<IPlayer>, &IID_IPlayer, &LIBID_COMMETERSLib>,
	public CWindowImpl<CPlayer, CWindow, CWinTraits<0, 0> >
{
public:
	CPlayer();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();

DECLARE_REGISTRY_RESOURCEID(IDR_PLAYER)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CPlayer)
	COM_INTERFACE_ENTRY(IPlayer)
	COM_INTERFACE_ENTRY(IMeter)
	COM_INTERFACE_ENTRY(ITimerClient)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_AGGREGATE(__uuidof(IMetaData), pMetaData)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CPlayer)
	IMPLEMENTED_CATEGORY(CATID_SysStatsMeterCategory)
END_CATEGORY_MAP()


BEGIN_MSG_MAP(CPlayer)
	MESSAGE_HANDLER(MM_MCINOTIFY, Notify)
END_MSG_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IPlayer
public:
	STDMETHOD(Resume)();
	STDMETHOD(get_Volume)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Volume)(/*[in]*/ long newVal);
	STDMETHOD(get_AutoStart)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_AutoStart)(/*[in]*/ short newVal);
	STDMETHOD(PutValue)(/*[in]*/ BSTR selector, /*[in]*/ BSTR value);
	STDMETHOD(Stop)();
	STDMETHOD(Pause)();
	STDMETHOD(Start)();
	STDMETHOD(get_Loop)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_Loop)(/*[in]*/ short newVal);
	STDMETHOD(get_Path)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Path)(/*[in]*/ BSTR newVal);

// IMeter
	STDMETHOD(Update)(/*[out, retval]*/ long *dirty);
	STDMETHOD(Configure)(IObserver * observer, LONG hWnd);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(GetAsString)(/*[in]*/ BSTR format, /*[in]*/ BSTR selector, /*[out, retval]*/ BSTR *pRet);
	STDMETHOD(GetAsDouble)(/*[in]*/ BSTR selector, /*[out, retval]*/ double *pRet);
	STDMETHOD(GetAsLong)(/*[in]*/ BSTR selector, /*[out,retval]*/ long *pRet);
	STDMETHOD(GetValue)(/*[out,retval]*/ VARIANT *pRet);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel* newVal);

	LRESULT Notify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	short IsOpen();

	IUnknown *pWeakTarget;
	IUnknown *pMetaData;

	_bstr_t path;

	short loop;
	short autoStart;
	long volume;
	MCIDEVICEID devId;

	static const _bstr_t SEL_CONTROL;
	static const _bstr_t SEL_CMD;
	static const _bstr_t SEL_PATH;
	static const _bstr_t SEL_CONFIG;

	static const _bstr_t CMD_PLAY;
	static const _bstr_t CMD_PAUSE;
	static const _bstr_t CMD_RESUME;
	static const _bstr_t CMD_STOP;
	static const _bstr_t CMD_OPEN;
	static const _bstr_t CMD_CLOSE;
	static const _bstr_t CMD_STATUS;
	static const _bstr_t CMD_SEEK;
	static const _bstr_t CMD_SETAUDIO;
	static const _bstr_t CMD_INFO;
};


#endif //__PLAYER_H_
