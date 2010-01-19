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

// Animation.cpp : Implementation of CAnimation
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "MeterImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#include "SysStatsUtils.h"
#include "COMMeters.h"
#include "Animation.h"
#include "AnimationDialog.h"

const BSTR CAnimation::SEL_FRAMERATE	= L"framerate";
const BSTR CAnimation::SEL_FRAMECOUNT	= L"framecount";
const BSTR CAnimation::SEL_LOOP			= L"loop";
const BSTR CAnimation::SEL_AUTOREVERSE	= L"autoreverse";
const BSTR CAnimation::SEL_SINGLESTEP	= L"singelstep";
const BSTR CAnimation::SEL_AUTOSTART	= L"autostart";
const BSTR CAnimation::SEL_PLAYSTATE	= L"playstate";
const BSTR CAnimation::SEL_POSITION		= L"position";
const BSTR CAnimation::SEL_CURFRAME		= L"currentframe";
const BSTR CAnimation::SEL_FRAMEDELTA	= L"framedelta";

const BSTR CAnimation::CTL_STEP			= L"step";

/////////////////////////////////////////////////////////////////////////////
// CAnimation

STDMETHODIMP CAnimation::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IAnimation
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/*
 * Constructor
 */
CAnimation::CAnimation() :
	pWeakTarget(0),
	pMetaData(0),
	currentframe(0),
	framedelta(1),
	framerate(0),
	framecount(0),
	loop(false),
	autoreverse(false),
	singlestep(false),
	playstate(STOPPED),
	dirty(true),
	autostart(false),
	stateMsg(0),
	stateLParam(0)
{
	interval = 0.0;
}

HRESULT CAnimation::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	HRESULT res = ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);

	::CLSIDFromProgID(L"SysStatsCOM.MetaData", &clsid);
	 res = ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pMetaData);

	 char buf[MAX_PATH];
	 _snprintf(buf, sizeof(buf), "#%d", IDR_XML_ANIMATION);

	IMetaData *pMD = 0;
	pMetaData->QueryInterface(_uuidof(IMetaData), (void**)&pMD);
	if (pMD)
	{
		pMD->PutResourceInfo((long)_Module.GetModuleInstance(), _bstr_t(buf));
		pMD->Release();
	}

	return res;
}

void CAnimation::FinalRelease()
{
	pWeakTarget->Release();
	pMetaData->Release();
}

/*
 * This method is called every 'interval' seconds by SysStats.
 * Some meters will use it to actually update their value.
 * Other meters update asynchronously. All meters should set
 * the 'dirty' output parameter to true if the value of the meter
 * has changed since the last time that Update() was called.
 */
STDMETHODIMP CAnimation::Update(long *dirty)
{
	if (dirty == NULL)
		return E_POINTER;

	*dirty = this->dirty;
	this->dirty = false;

	// Provide implementation.
	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the meter should
 * be displayed so that the user can configure the meter to meet their
 * requirements.
 */
STDMETHODIMP CAnimation::Configure(IObserver * observer, LONG hWnd)
{
	AnimationDialog *d = new AnimationDialog(this);
	d->DoModal((HWND)hWnd);
	delete d;

	return S_OK;
}

/*
 * Return a string representation of the meter’s type.
 */
STDMETHODIMP CAnimation::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_Animation, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CAnimation::put_TimerInterval(double val)
{
	if (playstate == PLAYING)
		interval = val;
	else
		interval = 0.0;

	ICOMTimerService *ts = NULL;
	model->get_TimerService(&ts);
	if (ts)
	{
		ts->SetTimerClient(this);
		ts->Release();
	}

	return S_OK;
}

STDMETHODIMP CAnimation::get_TimerInterval(double * pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = interval;

	return S_OK;
}

/*
 * Return the meter's value as a long - if that makes sense. Some
 * overlays require a long value, some don't.
 *
 * Parameter 'sel' can be used to select one value from several if the
 * meter actually stores multiple values. It is up to the meter to
 * interpret the contents of 'sel' as it sees fit.
 */
STDMETHODIMP CAnimation::GetAsLong(BSTR sel, long *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	if (::wcscmp(sel, SEL_FRAMERATE) == 0)
		*pRet = (long)framerate;
	else if (::wcscmp(sel, SEL_FRAMECOUNT) == 0)
		*pRet = framecount;
	else if (::wcscmp(sel, SEL_LOOP) == 0)
		*pRet = loop;
	else if (::wcscmp(sel, SEL_AUTOREVERSE) == 0)
		*pRet = autoreverse;
	else if (::wcscmp(sel, SEL_SINGLESTEP) == 0)
		*pRet = singlestep;
	else if (::wcscmp(sel, SEL_AUTOSTART) == 0)
		*pRet = autostart;
	else if (::wcscmp(sel, SEL_PLAYSTATE) == 0)
		*pRet = playstate;
	else if (::wcscmp(sel, SEL_POSITION) == 0)
	{
		short _tmp;
		get_Position(&_tmp);
		*pRet = _tmp;
	}
	else if (::wcscmp(sel, SEL_CURFRAME) == 0)
		*pRet = currentframe;
	else if (::wcscmp(sel, SEL_FRAMEDELTA) == 0)
		*pRet = framedelta;
	else
		*pRet = -1;

	return S_OK;
}

/*
 * Return the meter's value as a double - if that makes sense. Some
 * overlays require a double value, some don't.
 *
 * Parameter 'sel' can be used to select one value from several if the
 * meter actually stores multiple values. It is up to the meter to
 * interpret the contents of 'sel' as it sees fit.
 */
STDMETHODIMP CAnimation::GetAsDouble(BSTR sel, double *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	if (::wcscmp(sel, SEL_FRAMERATE) == 0)
		*pRet = framerate;
	else
	{
		long l = 0;
		(double)GetAsLong(sel, &l);
		*pRet = (double)l;
	}

	return S_OK;
}

/*
 * Return the meter's value as a string. If that makes sense. Some
 * overlays require a string value, some don't.
 *
 * Parameter 'format' should always be used to produce the resulting string.
 * It is a 'printf' style format string.
 *
 * Parameter 'sel' can be used to select one value from several if the
 * meter actually stores multiple values. It is up to the meter to
 * interpret the contents of 'sel' as it sees fit.
 */
STDMETHODIMP CAnimation::GetAsString(BSTR format, BSTR selector, BSTR *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	try
	{
		char buf[MAX_PATH];
		// Replace 'value' with the meter's actual value.
		double value = 0.0;
		GetAsDouble(selector, &value);
		if (::wcscmp(selector, SEL_CURFRAME) != 0)
			_snprintf(buf, sizeof(buf), _bstr_t(format), value);
		else
		{
			struct tm lt;
			long s = value;
			::memset(&lt, 0, sizeof(lt));
			lt.tm_sec = s % 60;
			lt.tm_min = (s/60) % 60;
			lt.tm_hour = (s/3600);

			strftime(buf, MAX_PATH, _bstr_t(format), &lt);

			*pRet = _com_util::ConvertStringToBSTR(buf);
		}

		buf[MAX_PATH-1] = 0;
		*pRet = _com_util::ConvertStringToBSTR(buf);
	}
	catch (...)
	{
	}

	return S_OK;
}

/*
 * Return the meter's value in the most native format possible.
 * This is how scripts get a hold of the value.
 */
STDMETHODIMP CAnimation::GetValue(VARIANT *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

	VARIANT ret;
	::VariantInit(&ret);
	CComVariant varResult;
	long value = 0;
	GetAsLong(SEL_PLAYSTATE, &value);
	varResult = value;
	varResult.Detach(&ret);
	*pRet = ret;

	return S_OK;
}

STDMETHODIMP CAnimation::get_FrameCount(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = framecount;

	return S_OK;
}

STDMETHODIMP CAnimation::put_FrameCount(long newVal)
{
	if (framecount != newVal)
		dirty = true;

	framecount = newVal;

	return S_OK;
}

STDMETHODIMP CAnimation::get_FrameRate(double *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = framerate;

	return S_OK;
}

STDMETHODIMP CAnimation::put_FrameRate(double newVal)
{
	if (framerate != newVal)
		dirty = true;

	framerate = newVal;

	if (playstate == PLAYING && !singlestep)
		put_TimerInterval(framerate ? 1.0/framerate : 0);

	return S_OK;
}

STDMETHODIMP CAnimation::get_PlayState(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = playstate;

	return S_OK;
}

void CAnimation::PostStateMsg()
{
	if (stateMsg)
	{
		short pos = 0;
		get_Position(&pos);
		model->PostMessage(stateMsg, playstate | (pos * 16), stateLParam);
	}
}

STDMETHODIMP CAnimation::put_PlayState(short newVal)
{
	short oldstate = playstate;

	playstate = newVal;	// Or put_TimerInterval won't work!

	switch (oldstate)
	{
	case STOPPED:
		if (newVal == PLAYING)
		{
			framedelta = +1;
			currentframe = 0;
			if (!singlestep)
				put_TimerInterval(framerate ? 1.0/framerate : 0);
		}
		break;

	case PAUSED:
		if (newVal == PLAYING)
		{
			if (!singlestep)
				put_TimerInterval(framerate ? 1.0/framerate : 0);
		}
		break;

	case PLAYING:
		if (newVal == STOPPED)
		{
			framedelta = +1;
			currentframe = 0;
		}
		break;
	}

	if (oldstate != newVal)
	{
		dirty = true;
		PostStateMsg();
	}

	return S_OK;
}

STDMETHODIMP CAnimation::get_Position(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	short position = NEITHER;
	
	if (framecount > 0)
	{
		if (currentframe == 0)
			position |= START;
		if (currentframe == (framecount-1))
			position |= END;
	}

	*pVal = position;

	return S_OK;
}

STDMETHODIMP CAnimation::put_Position(short newVal)
{
	if (newVal == START)
		currentframe = 0;
	if (newVal == END)
		currentframe = framecount-1;

	dirty = true;

	return S_OK;
}

STDMETHODIMP CAnimation::get_Loop(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = loop;

	return S_OK;
}

STDMETHODIMP CAnimation::put_Loop(short newVal)
{
	if (loop != newVal)
		dirty = true;

	loop = newVal;

	return S_OK;
}

STDMETHODIMP CAnimation::get_AutoReverse(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = autoreverse;

	return S_OK;
}

STDMETHODIMP CAnimation::put_AutoReverse(short newVal)
{
	if (autoreverse != newVal)
		dirty = true;

	autoreverse = newVal;

	return S_OK;
}

STDMETHODIMP CAnimation::get_SingleStep(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = singlestep;

	return S_OK;
}

STDMETHODIMP CAnimation::put_SingleStep(short newVal)
{
	if (singlestep != newVal)
		dirty = true;

	singlestep = newVal;
	if (playstate == PLAYING && singlestep)
		put_TimerInterval(0);

	return S_OK;
}

STDMETHODIMP CAnimation::PutValue(BSTR sel, BSTR value)
{
	long l = _wtol(value);

	AtlTrace(L"CAnimation::PutValue(%s, %d)\n",sel, l);

	if (::wcscmp(sel, SEL_FRAMERATE) == 0)
		put_FrameRate(wcstod(value, NULL));
	else if (::wcscmp(sel, SEL_FRAMECOUNT) == 0)
		put_FrameCount(l);
	else if (::wcscmp(sel, SEL_FRAMEDELTA) == 0)
		framedelta = (short)l;
	else if (::wcscmp(sel, SEL_LOOP) == 0)
		put_Loop((short)l);
	else if (::wcscmp(sel, SEL_AUTOREVERSE) == 0)
		put_AutoReverse((short)l);
	else if (::wcscmp(sel, SEL_SINGLESTEP) == 0)
		put_SingleStep((short)l);
	else if (::wcscmp(sel, SEL_AUTOSTART) == 0)
		put_AutoStart((short)l);
	else if (::wcscmp(sel, SEL_PLAYSTATE) == 0)
		put_PlayState((short)l);
	else if (::wcscmp(sel, SEL_POSITION) == 0)
		put_Position((short)l);
	else if (::wcscmp(sel, CTL_STEP) == 0)
		Step(l);

	return S_OK;
}

static long lasttime = 0;

// Override standard TimerFired implementation
STDMETHODIMP CAnimation::TimerFired()
{
	Step(0);

	return S_OK;
}

STDMETHODIMP CAnimation::Step(/*[in]*/ long delta)
{
	if (delta != 0)
		framedelta = delta;

	// Could maybe do this directly in the timer callback?
	if (playstate == PLAYING)
	{
		currentframe += framedelta;

		if (currentframe >= framecount)
		{
			// Must be playing forwards
			if (!loop)
			{
				if (autoreverse)
				{
					currentframe = framecount-1;
					framedelta = -1;
				}
				else
				{
					currentframe = 0;
					framedelta = +1;
				}

				put_PlayState(PAUSED);

				return S_OK;
			}

			// else keep playing
			if (autoreverse)
			{
				currentframe = framecount-2;
				if (currentframe < 0)
					currentframe = 0;

				framedelta = -1;
			}
			else
			{
				currentframe = 0;
				framedelta = +1;
			}

			PostStateMsg();
		}
		else if (currentframe < 0)
		{
			// must be playing backwards
			if (!loop)
			{
				if (autoreverse)
				{
					currentframe = 0;
					framedelta = +1;
				}
				else
				{
					currentframe = framecount-1;
					framedelta = -1;
				}

				put_PlayState(PAUSED);

				return S_OK;
			}

			// else keep playing
			if (autoreverse)
			{
				currentframe = framecount > 0 ? 1 : 0;
				framedelta = +1;
			}
			else
			{
				currentframe = framecount-1;
				framedelta = -1;
			}

			PostStateMsg();
		}
	}
	else
		put_TimerInterval(0);

	if (model)
		model->put_dirty(true);

	dirty = true;

	return S_OK;
}

STDMETHODIMP CAnimation::get_AutoStart(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = autostart;

	return S_OK;
}

STDMETHODIMP CAnimation::put_AutoStart(short newVal)
{
	if (autostart != newVal)
		dirty = true;

	autostart = newVal;

	if (autostart)
		put_PlayState(PLAYING);

	return S_OK;
}

STDMETHODIMP CAnimation::get_StateMsg(UINT *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = stateMsg;

	return S_OK;
}

STDMETHODIMP CAnimation::put_StateMsg(UINT newVal)
{
	stateMsg = newVal;

	return S_OK;
}

STDMETHODIMP CAnimation::get_StateLParam(UINT *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = stateLParam;

	return S_OK;
}

STDMETHODIMP CAnimation::put_StateLParam(UINT newVal)
{
	stateLParam = newVal;

	return S_OK;
}
