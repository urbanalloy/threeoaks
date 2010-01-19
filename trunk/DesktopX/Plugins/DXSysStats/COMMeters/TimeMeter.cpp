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

// TimeMeter.cpp : Implementation of CTimeMeter
#include "stdafx.h"
#include "stdio.h"
#include <time.h>
#include <locale.h>
#include "SysStatsUtils.h"
#include "MeterImpl.cpp"
#include "COMMeters.h"
#include "TimeMeter.h"
#include "TimeDialog.h"

const int CTimeMeter::TIME_SEL = 0;
const int CTimeMeter::TZDESC_SEL = 1;
const int CTimeMeter::ISDST_SEL = 2;
const int CTimeMeter::GMTOFFSET_SEL = 3;

/////////////////////////////////////////////////////////////////////////////
// CTimeMeter

STDMETHODIMP CTimeMeter::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ITimeMeter
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CTimeMeter::CTimeMeter() :
	tzDescription(L"Local"),
	forceDST(false),
	tz(L""),
	isDST(false),
	pWeakTarget(0)
{
	GetTimeZoneInfo();
}

HRESULT CTimeMeter::FinalConstruct()
{
	CLSID clsid;

	::CLSIDFromProgID(L"SysStatsCOM.WeakTarget", &clsid);
	HRESULT res = ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pWeakTarget);

	::CLSIDFromProgID(L"SysStatsCOM.MetaData", &clsid);
	 res = ::CoCreateInstance(clsid, GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pMetaData);

	IMetaData *pMD = 0;
	pMetaData->QueryInterface(_uuidof(IMetaData), (void**)&pMD);
	if (pMD)
	{
		pMD->PutResourceInfo((long)_Module.GetModuleInstance(), BuildTimeZoneDoc());
		pMD->Release();
	}

	return res;
}

void CTimeMeter::FinalRelease()
{
	pWeakTarget->Release();
	pMetaData->Release();
}

STDMETHODIMP CTimeMeter::Update(long *dirty)
{
	SYSTEMTIME sysTime;
	::GetSystemTime(&sysTime);	// in UTC

	::SystemTimeToTzSpecificLocalTime(&tzInfo, &sysTime, &tzTime);

	*dirty = true;	// Come on. Its *always* going to be true.

	return S_OK;
}

STDMETHODIMP CTimeMeter::Configure(IObserver * observer, LONG hWnd)
{
	TimeDialog *d = new TimeDialog(this);
	d->DoModal((HWND)hWnd);
	delete d;

	return S_OK;
}

STDMETHODIMP CTimeMeter::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_TimeMeter, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CTimeMeter::GetAsLong(BSTR selector, long *pRet)
{
	int sel = ::_wtoi(selector);

	switch (sel)
	{
	case TIME_SEL:
		*pRet = tzTime.wHour * 3600 + tzTime.wMinute * 60 + tzTime.wSecond;
		break;

	case ISDST_SEL:
		*pRet = 0;
		break;

	case GMTOFFSET_SEL:
		*pRet = -tzInfo.Bias;
		break;
	}

	return S_OK;
}

STDMETHODIMP CTimeMeter::GetAsDouble(BSTR selector, double *pRet)
{
	long l = 0;
	GetAsLong(selector, &l);
	*pRet = (double)l;

	return S_OK;
}

STDMETHODIMP CTimeMeter::GetAsString(BSTR format, BSTR selector, BSTR *pRet)
{
	try {
		int sel = ::_wtoi(selector);

		switch (sel)
		{
		case TIME_SEL:
			{
				char buf[64];
				struct tm lt;
				SystemTime2tm(&tzTime, &lt);

				strftime(buf, 63, _bstr_t(format), &lt);

				*pRet = _com_util::ConvertStringToBSTR(buf);
			}
			break;

		case TZDESC_SEL:
			{
				wchar_t buf[MAX_PATH];
				_snwprintf(buf, MAX_PATH, format, (BSTR)tzDescription);
				buf[MAX_PATH-1] = 0;
				*pRet = ::SysAllocString(buf);
			}
			break;

		case ISDST_SEL:
			{
				wchar_t buf[MAX_PATH];
				_snwprintf(buf, MAX_PATH, format, "not implemented");
				buf[MAX_PATH-1] = 0;
				*pRet = ::SysAllocString(buf);
			}
			break;

		case GMTOFFSET_SEL:
			{
				wchar_t buf[MAX_PATH];
				_snwprintf(buf, MAX_PATH, format, ((double)-tzInfo.Bias) / 60);
				buf[MAX_PATH-1] = 0;
				*pRet = ::SysAllocString(buf);
			}
			break;
		}

	}
	catch (...)
	{
	}
	return S_OK;
}

STDMETHODIMP CTimeMeter::GetValue(VARIANT *pRet)
{
	long l = 0;
	GetAsLong(0, &l);
	CComVariant(l).Detach(pRet);

	return S_OK;
}

void CTimeMeter::SystemTime2tm(SYSTEMTIME *sysTime, struct tm *pLT)
{
	pLT->tm_sec = sysTime->wSecond;
	pLT->tm_min = sysTime->wMinute;
	pLT->tm_hour = sysTime->wHour;
	pLT->tm_year = sysTime->wYear - 1900;
	pLT->tm_wday = sysTime->wDayOfWeek;
	pLT->tm_mday = sysTime->wDay;
	pLT->tm_mon = sysTime->wMonth - 1;
}

bool CTimeMeter::GetTimeZoneInfo()
{
	bool ret = false;

	if (tz == _bstr_t(L""))
	{
		::GetTimeZoneInformation(&tzInfo);
		ret = true;
	}
	else
	{
		HKEY hKey;
		char key[1024];
		_snprintf(key, 1023, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\%s", (char*)tz);
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			TZI tzi;
			DWORD size = sizeof(TZI);
			if (RegQueryValueEx(hKey,_T("TZI"),NULL,NULL,(UCHAR*)&(tzi),&size) == ERROR_SUCCESS)
			{
				tzInfo.Bias = tzi.Bias;
				tzInfo.DaylightBias = tzi.DaylightBias;
				tzInfo.DaylightDate = tzi.DaylightDate;
				tzInfo.StandardBias = tzi.StandardBias;
				tzInfo.StandardDate = tzi.StandardDate;
				char buf[33];
				size = 32;
				if (RegQueryValueEx(hKey,_T("Dlt"),NULL,NULL,(UCHAR*)&buf,&size) == ERROR_SUCCESS)
				{
					::_snwprintf(tzInfo.DaylightName, 32, L"%s", (wchar_t*)_bstr_t(buf));
				}

				size = 32;
				if (RegQueryValueEx(hKey,_T("Std"),NULL,NULL,(UCHAR*)&buf,&size) == ERROR_SUCCESS)
				{
					::_snwprintf(tzInfo.StandardName, 32, L"%s", (wchar_t*)_bstr_t(buf));
				}

				ret = true;
			}
		}
	}

	return ret;
}

/*
<?xml version="1.0"?>
<metadata>
	<valueset id="selector">
		<item id="selector:uniqueid">
			<value type="BSTR">value</value>
			<label>GUI label</label>
			<help>GUI help</help>
		</item>
		... etc. one for each timezone ...
	</valueset>
	
	<method name="GetValue">
		<arg name="selector" pos="0">
			<valuesetref idref="selector"/>
		</arg>
	</method>
</metadata>

  Need to add sorting by GMT offset :(
*/
_bstr_t CTimeMeter::BuildTimeZoneDoc()
{
	HKEY hkey;
	_bstr_t doc = L"<?xml version=\"1.0\"?>\n<metadata>\n";

	// Open the timezone registry entry (a list of timezones)
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones"), 0, KEY_ENUMERATE_SUB_KEYS, &hkey) == ERROR_SUCCESS)
	{
		doc += L"<valueset id=\"timezone\">\n";
		FILETIME fileTime;
		DWORD index=0;
		char tzKey[MAX_PATH];
		DWORD size = sizeof(tzKey);
		wchar_t buf[MAX_PATH];

		doc += L"<item id=\"timezone:0\"><value type = \"BSTR\"/><label>(none)</label><help/></item>\n";

		// Get the subkey name (timzeone name)
		while(RegEnumKeyEx(hkey, index, tzKey, &size, NULL, NULL, NULL, &fileTime) == ERROR_SUCCESS)
		{
			HKEY hsubkey;

			doc += L"<item id=\"timezone:";
			doc += _itow(index+1, buf, 10);
			doc += L"\">";

			// Add value element to item element
			doc += L"<value type=\"BSTR\">";
			SysStatsUtils::EncodeXMLString(tzKey, &doc);
			doc += L"</value>";
			AtlTrace("%s=", tzKey);

			// Get the display name of this timezone
			if (RegOpenKeyEx(hkey, tzKey, 0, KEY_QUERY_VALUE, &hsubkey) == ERROR_SUCCESS)
			{
				size=sizeof(tzKey);
				RegQueryValueEx(hsubkey,_T("Display"),NULL,NULL,(UCHAR*)tzKey,&size);

				// Add label element to item element
				doc += L"<label>";
				SysStatsUtils::EncodeXMLString(tzKey, &doc);
				doc += L"</label>";
			}

			doc += L"<help/></item>\n";

			index++;
			size=sizeof(tzKey);
		}

		doc += L"</valueset>\n";

		doc += L"<property name=\"TimeZone\"><valuesetref idref=\"timezone\"/></property>\n";
	}

	doc += L"\
	<valueset id=\"selector\">\n\
		<item id=\"selector:0\">\n\
			<value type=\"BSTR\"></value>\n\
			<label>Time</label>\n\
			<help>Get the time</help>\n\
		</item>\n\
		<item id=\"selector:1\">\n\
			<value type=\"BSTR\">1</value>\n\
			<label>TimeZone Description</label>\n\
			<help>Get the time</help>\n\
		</item>\n\
		<item id=\"selector:2\">\n\
			<value type=\"BSTR\">2</value>\n\
			<label>Is DST</label>\n\
			<help>Is DST in effect?</help>\n\
		</item>\n\
		<item id=\"selector:3\">\n\
			<value type=\"BSTR\">3</value>\n\
			<label>GMT Offset</label>\n\
			<help>Offset of timezone from GMT in hours</help>\n\
		</item>\n\
	</valueset>\n\
	\
	<method name=\"GetValue\">\n\
		<arg name=\"selector\" pos=\"0\">\n\
			<valuesetref idref=\"selector\"/>\n\
		</arg>\n\
	</method>\n";

	doc += L"</metadata>\n";

#ifdef _DEBUG
	FILE *file = fopen("time.xml", "w");
	fprintf(file, "%s\n", (char*)doc);
	::fflush(file);
	fclose(file);
#endif

	return doc;
}

STDMETHODIMP CTimeMeter::get_TimeZone(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = tz.copy();

	return S_OK;
}

STDMETHODIMP CTimeMeter::put_TimeZone(BSTR newVal)
{
	tz = newVal;

	GetTimeZoneInfo();

	return S_OK;
}

STDMETHODIMP CTimeMeter::get_ForceDST(short *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = forceDST;

	return S_OK;
}

STDMETHODIMP CTimeMeter::put_ForceDST(short newVal)
{
	forceDST = newVal;

	return S_OK;
}

STDMETHODIMP CTimeMeter::get_TZDescription(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = tzDescription.copy();

	return S_OK;
}

STDMETHODIMP CTimeMeter::put_TZDescription(BSTR newVal)
{
	tzDescription = newVal;

	return S_OK;
}
