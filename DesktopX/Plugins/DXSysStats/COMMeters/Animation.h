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

// Animation.h : Declaration of the CAnimation

#ifndef __ANIMATION_H_
#define __ANIMATION_H_

#include "COMMeters.h"       // main symbols
#include "resource.h"       // main symbols
#include "SysStatsMeterCategory.h"
#include "MeterImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CAnimation
class ATL_NO_VTABLE CAnimation :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAnimation, &CLSID_Animation>,
	public ISupportErrorInfo,
	public IDispatchImpl<MeterImpl<IAnimation>, &IID_IAnimation, &LIBID_COMMETERSLib>
{
public:
	CAnimation();

	// Need these for inner IWeakTarget
	HRESULT FinalConstruct();
	void FinalRelease();

DECLARE_REGISTRY_RESOURCEID(IDR_ANIMATION)
DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CAnimation)
	COM_INTERFACE_ENTRY(IAnimation)
	COM_INTERFACE_ENTRY(IMeter)
	COM_INTERFACE_ENTRY(ITimerClient)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_AGGREGATE(__uuidof(IMetaData), pMetaData)
	// This is how we proxy for IWeakTarget - has to be last
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(pWeakTarget)
END_COM_MAP()

BEGIN_CATEGORY_MAP(CAnimation)
	IMPLEMENTED_CATEGORY(CATID_SysStatsMeterCategory)
END_CATEGORY_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IAnimation
public:
	STDMETHOD(get_StateLParam)(/*[out, retval]*/ UINT *pVal);
	STDMETHOD(put_StateLParam)(/*[in]*/ UINT newVal);
	STDMETHOD(get_StateMsg)(/*[out, retval]*/ UINT *pVal);
	STDMETHOD(put_StateMsg)(/*[in]*/ UINT newVal);
	STDMETHOD(get_AutoStart)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_AutoStart)(/*[in]*/ short newVal);
	STDMETHOD(Step)(/*[in]*/ long delta);
	STDMETHOD(PutValue)(/*[in]*/ BSTR selector, /*[in]*/ BSTR  value);
	STDMETHOD(get_SingleStep)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_SingleStep)(/*[in]*/ short newVal);
	STDMETHOD(get_AutoReverse)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_AutoReverse)(/*[in]*/ short newVal);
	STDMETHOD(get_Loop)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_Loop)(/*[in]*/ short newVal);
	STDMETHOD(get_Position)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_Position)(/*[in]*/ short newVal);
	STDMETHOD(get_PlayState)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_PlayState)(/*[in]*/ short newVal);
	STDMETHOD(get_FrameRate)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_FrameRate)(/*[in]*/ double newVal);
	STDMETHOD(get_FrameCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_FrameCount)(/*[in]*/ long newVal);

// IMeter
	STDMETHOD(Update)(/*[out, retval]*/ long *dirty);
	STDMETHOD(Configure)(IObserver * observer, LONG hWnd);
	STDMETHOD(get_Type)(BSTR * pVal);
	STDMETHOD(GetAsString)(/*[in]*/ BSTR format, /*[in]*/ BSTR selector, /*[out, retval]*/ BSTR *pRet);
	STDMETHOD(GetAsDouble)(/*[in]*/ BSTR selector, /*[out, retval]*/ double *pRet);
	STDMETHOD(GetAsLong)(/*[in]*/ BSTR selector, /*[out,retval]*/ long *pRet);
	STDMETHOD(GetValue)(/*[out,retval]*/ VARIANT *pRet);

// ITimerClient
	STDMETHOD(get_TimerInterval)(double * pVal);
	STDMETHOD(put_TimerInterval)(double pVal);
	STDMETHOD(TimerFired)();

protected:
	void PostStateMsg();

	double framerate;
	long framecount;

	short loop;
	short autoreverse;
	short singlestep;
	short autostart;

	short playstate;

	long currentframe;
	short framedelta; // +1 = forward, -1 = reverse
	short dirty;

	UINT stateMsg;
	UINT stateLParam;

	enum PlayState
	{
		STOPPED = 0,
		PLAYING = 1,
		PAUSED = 2
	};

	enum Position
	{
		NEITHER = 0,	// framecount == 0
		START = 1,		// currentframe == 0
		END = 2,		// currentframe == framecount
		BOTH = 3		// framecount == 1
	};

	static const BSTR SEL_FRAMERATE;
	static const BSTR SEL_FRAMECOUNT;
	static const BSTR SEL_LOOP;
	static const BSTR SEL_AUTOREVERSE;
	static const BSTR SEL_AUTOSTART;
	static const BSTR SEL_SINGLESTEP;
	static const BSTR SEL_PLAYSTATE;
	static const BSTR SEL_POSITION;
	static const BSTR SEL_CURFRAME;
	static const BSTR SEL_FRAMEDELTA;

	static const BSTR CTL_STEP;

	IUnknown *pWeakTarget;
	IUnknown *pMetaData;
};

#endif //__ANIMATION_H_
