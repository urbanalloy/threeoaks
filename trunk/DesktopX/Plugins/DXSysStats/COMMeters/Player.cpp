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

// Player.cpp : Implementation of CPlayer
#include "stdafx.h"
#include <stdio.h>
#include "MeterImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#pragma comment(lib, "Winmm.lib")
#include "SysStatsUtils.h"
#include "COMMeters.h"
#include "Player.h"
#include "PlayerDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CPlayer

static _bstr_t percentS("%s");
static _bstr_t emptyStr("");

const _bstr_t CPlayer::SEL_CONTROL	= L"control";
const _bstr_t CPlayer::SEL_CMD		= L"cmd";
const _bstr_t CPlayer::SEL_PATH		= L"path";
const _bstr_t CPlayer::SEL_CONFIG	= L"configure";

const _bstr_t CPlayer::CMD_PLAY		= L"play";
const _bstr_t CPlayer::CMD_PAUSE	= L"pause";
const _bstr_t CPlayer::CMD_RESUME	= L"resume";
const _bstr_t CPlayer::CMD_STOP		= L"stop";
const _bstr_t CPlayer::CMD_OPEN		= L"open";
const _bstr_t CPlayer::CMD_CLOSE	= L"close";
const _bstr_t CPlayer::CMD_STATUS	= L"status";
const _bstr_t CPlayer::CMD_SEEK		= L"seek";
const _bstr_t CPlayer::CMD_SETAUDIO	= L"setaudio";
const _bstr_t CPlayer::CMD_INFO		= L"info";

/////////////////////////////////////////////////////////////////////////////
// CPlayer

STDMETHODIMP CPlayer::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IPlayer
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

/*
 * Constructor
 */
CPlayer::CPlayer() :
	pWeakTarget(0),
	pMetaData(0),
	autoStart(0),
	loop(0),
	volume(500),
	path("")
{
}

HRESULT CPlayer::FinalConstruct()
{
	CLSID clsid;
	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	HRESULT res = ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);

	::CLSIDFromProgID(L"SysStatsCOM.MetaData", &clsid);
	 res = ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pMetaData);

	 char buf[MAX_PATH];
	 _snprintf(buf, sizeof(buf), "#%d", IDR_XML_PLAYER);

	IMetaData *pMD = 0;
	pMetaData->QueryInterface(_uuidof(IMetaData), (void**)&pMD);
	if (pMD)
	{
		pMD->PutResourceInfo((long)_Module.GetModuleInstance(), _bstr_t(buf));
		pMD->Release();
	}

	return res;
}

void CPlayer::FinalRelease()
{
	Stop();
	if (m_hWnd != 0)
		DestroyWindow();
	pWeakTarget->Release();
	pMetaData->Release();
}

short CPlayer::IsOpen()
{
	char status[129];

	MCIERROR err = ::mciSendString(CMD_STATUS + L" \"" + path + L"\" mode wait", status, 129, 0);

	return err == 0;
}
/*
 * This method is called every 'interval' seconds by SysStats.
 * Some meters will use it to actually update their value.
 * Other meters update asynchronously. All meters should set
 * the 'dirty' output parameter to true if the value of the meter
 * has changed since the last time that Update() was called.
 */
STDMETHODIMP CPlayer::Update(long *dirty)
{
	if (dirty == NULL)
		return E_POINTER;

	// Provide implementation.
	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the meter should
 * be displayed so that the user can configure the meter to meet their
 * requirements.
 */
STDMETHODIMP CPlayer::Configure(IObserver * observer, LONG hWnd)
{
	PlayerDialog *d = new PlayerDialog(this);
	d->DoModal((HWND)hWnd);
	delete d;

	return S_OK;
}

/*
 * Return a string representation of the meter’s type.
 */
STDMETHODIMP CPlayer::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_Player, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

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
STDMETHODIMP CPlayer::GetAsLong(BSTR sel, long *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

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
STDMETHODIMP CPlayer::GetAsDouble(BSTR sel, double *pRet)
{
	if (pRet == NULL)
		return E_POINTER;

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
STDMETHODIMP CPlayer::GetAsString(BSTR format, BSTR sel, BSTR *pRet)
{
	try
	{
		if (::wcscmp(sel, SEL_CMD) == 0) {
			char buf[129];
			::mciSendString(_bstr_t(sel), buf, 128, 0);
			_bstr_t _buf(buf);
			*pRet = _buf.copy();
		} else {
			wchar_t buf[MAX_PATH];
			_snwprintf(buf, MAX_PATH, format, (wchar_t*)path);
			buf[MAX_PATH-1] = 0;
			*pRet = ::SysAllocString(buf);
		}
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
STDMETHODIMP CPlayer::GetValue(VARIANT *pRet)
{
	BSTR s = L"";
	GetAsString(percentS, emptyStr, &s);
	CComVariant(s).Detach(pRet);
	::SysFreeString(s);

	return S_OK;
}

STDMETHODIMP CPlayer::get_Path(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	IClientModel *_model = 0;
	get_Model(&_model);
	IAppConfig *appConfig = 0;
	_model->get_AppConfig(&appConfig);

	appConfig->NormalizePath(path, pVal);

	appConfig->Release();
	_model->Release();

	return S_OK;
}

STDMETHODIMP CPlayer::put_Path(BSTR newVal)
{
	Stop();

	IAppConfig *appConfig = 0;
	model->get_AppConfig(&appConfig);

	BSTR tmp;
	appConfig->CanonicalPath(newVal, &tmp);
	path = tmp;
	::SysFreeString(tmp);

	appConfig->Release();

	if (autoStart)
		Start();

	return S_OK;
}

STDMETHODIMP CPlayer::get_Loop(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = loop;

	return S_OK;
}

STDMETHODIMP CPlayer::put_Loop(short newVal)
{
	loop = newVal;

	return S_OK;
}

STDMETHODIMP CPlayer::Start()
{
	MCIERROR err = 0;
	if (!IsOpen()) {
		err = ::mciSendString(CMD_OPEN + L" \"" + path + L"\"", NULL, 0, 0);
		devId = ::mciGetDeviceID(path);
		AtlTrace("Device id =%d\n", devId);
		put_Volume(volume);
	} else {
		::mciSendString(CMD_SEEK + L" \"" + path + L"\" to start", NULL, 0, 0);
	}

	if (err == 0)
		err = ::mciSendString(CMD_PLAY + L" \"" + path + L"\" notify", NULL, 0, m_hWnd);

#ifdef _DEBUG
	if (err != 0) {
		char buf[129];
		::mciGetErrorString(err, buf, 129);
		AtlTrace("MCI error: %s\n", buf);
	}
#endif

	return S_OK;
}

STDMETHODIMP CPlayer::Pause()
{
	MCIERROR err = ::mciSendString(CMD_PAUSE + L" \"" + path + L"\"", NULL, 0, 0);

#ifdef _DEBUG
	if (err != 0) {
		char errBuf[129];
		::mciGetErrorString(err, errBuf, 128);
		AtlTrace("MCI error: %s\n", errBuf);
	}
#endif

	return S_OK;
}

STDMETHODIMP CPlayer::Stop()
{
	::mciSendString(CMD_STOP + L" \"" + path + L"\"", NULL, 0, 0);
	::mciSendString(CMD_CLOSE + L" \"" + path + L"\"", NULL, 0, 0);

	return S_OK;
}

STDMETHODIMP CPlayer::PutValue(BSTR sel, BSTR value)
{
	AtlTrace(L"CPlayer::PutValue(%s, %s)\n",sel, value);

	if (::wcscmp(sel, SEL_CONTROL) == 0) {
		// A set of simplified commands
		if (::wcscmp(value, CMD_PLAY) == 0) {
			Start();
		} else if (::wcscmp(value, CMD_STOP) == 0) {
			Stop();
		} else if (::wcscmp(value, CMD_PAUSE) == 0) {
			Pause();
		} else if (::wcscmp(value, CMD_RESUME) == 0) {
			Resume();
		}
	} else if (::wcscmp(sel, SEL_CMD) == 0) {
		::mciSendString(_bstr_t(value), NULL, 0 , NULL);
	} else if (::wcscmp(sel, SEL_CONFIG) == 0) {
		Configure(NULL, (long)m_hWnd);
	} else if (::wcscmp(sel, SEL_PATH) == 0) {
		put_Path(value);
	}

	return S_OK;
}

STDMETHODIMP CPlayer::get_AutoStart(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = autoStart;

	return S_OK;
}

STDMETHODIMP CPlayer::put_AutoStart(short newVal)
{
	autoStart = newVal;

	if (autoStart)
		Start();

	return S_OK;
}

STDMETHODIMP CPlayer::put_Model(IClientModel *newVal) {
	MeterImpl<IPlayer>::put_Model(newVal);
	if (m_hWnd == 0)
	{
		long parent = 0;
		newVal->get_hWnd(&parent);
		RECT whatever = { 0, 0, 0, 0 };
		Create((HWND)parent, whatever, "HiddenPlayerWindow", 0);
	}

	return S_OK;
}

LRESULT CPlayer::Notify(UINT uMsg, WPARAM wFlags, LPARAM lDevID, BOOL& bHandled)
{
	if (wFlags == MCI_NOTIFY_SUCCESSFUL) {
		if (loop)
			Start();
		else
			Stop();
	}

	return 1;
}

STDMETHODIMP CPlayer::get_Volume(long *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	*pVal = volume;

	return S_OK;
}

STDMETHODIMP CPlayer::put_Volume(long newVal)
{
	volume = newVal;

	char buf[1024];
	_snprintf(buf, sizeof(buf), "setaudio \"%s\" volume %d", (char*)path, volume); 
	MCIERROR err = ::mciSendString(buf, NULL, 0, 0);
	if (err != 0) {
		waveOutSetVolume((HWAVEOUT)0, MAKELONG(volume*0xffff/1000,volume*0xffff/1000));
/*		WAVEFORMATEX wf;
		wf.wFormatTag = WAVE_FORMAT_PCM;
		wf.nChannels = 1;
		wf.nSamplesPerSec = 8000 * 1000;
		wf.wBitsPerSample = 8;
		wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
		wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
		wf.cbSize = 0;
		HWAVEOUT hwo;
		for (UINT id = 0; id < waveOutGetNumDevs(); id++)
		{
			if (waveOutOpen(&hwo, id, &wf, 0, 0, CALLBACK_NULL) == MMSYSERR_NOERROR)
			{
				waveOutSetVolume(hwo, volume*0xffff/1000);
				waveOutClose(hwo);
				break;
			}
		}
*/
	}

#ifdef _DEBUG
	if (err != 0) {
		char errBuf[129];
		::mciGetErrorString(err, errBuf, 128);
		AtlTrace("MCI error: %s\n", errBuf);
	}
#endif

	return S_OK;
}

STDMETHODIMP CPlayer::Resume()
{
	::mciSendString(CMD_RESUME + L" \"" + path + L"\"", NULL, 0, 0);

	return S_OK;
}
