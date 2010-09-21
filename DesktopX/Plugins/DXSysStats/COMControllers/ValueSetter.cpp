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

// ValueSetter.cpp : Implementation of CValueSetter
#include "stdafx.h"
#include "ControllerImpl.cpp"
#pragma comment(lib, "SysStatsCommon.lib")
#include "SysStatsUtils.h"
#include "COMControllers.h"
#include "ValueSetter.h"
#include "ValueSetterDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CValueSetter

STDMETHODIMP CValueSetter::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IValueSetter
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
CValueSetter::CValueSetter() : value(""), selector("")
{
}

/*
 * Destructor
 */
CValueSetter::~CValueSetter()
{
	AtlTrace("ValueSetter %s destroyed\n", (char*)_bstr_t(name));
}

/*
 * This method is called when a windows message of the correct type has been posted.
 */
STDMETHODIMP CValueSetter::HandleMessage(/*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled)
{
	*handled = !passThrough;

	IMeter *iMeter = 0;
	get_meter(&iMeter);
	if (iMeter)
	{
		LPOLESTR name = L"PutValue";   
		DISPID id;
		HRESULT hr = iMeter->GetIDsOfNames(IID_NULL, 
                               &name, 
                               1, 
                               LOCALE_USER_DEFAULT, 
                               &id);

		if (SUCCEEDED(hr))
		{
			AtlTrace("MappedController: Found PutValue method\n");
			CComVariant varResult;
			CComVariant* pvars = new CComVariant[2];

			VariantClear(&varResult);
			pvars[0] = (BSTR)value;
			pvars[1] = (BSTR)selector;
			DISPPARAMS disp = { pvars, NULL, 2, 0 };
			hr = iMeter->Invoke(id, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, &varResult, NULL, NULL);
			delete[] pvars;

			if (SUCCEEDED(hr))
			{
				AtlTrace("ValueSetter: Set Value via PutValue\n");
				return S_OK;
			}
		}

		name = L"Value";   
		hr = iMeter->GetIDsOfNames(IID_NULL, 
                               &name, 
                               1, 
                               MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), 
                               &id);

		if (SUCCEEDED(hr))
		{
			AtlTrace("ValueSetter: Found Value Property\n");
			CComVariant varResult;
			CComVariant* pvars = new CComVariant[1];

			VariantClear(&varResult);
			DISPID dispidNamed = DISPID_PROPERTYPUT;
			DISPPARAMS disp = { pvars, &dispidNamed, 1, 1 };
			pvars[0] = (BSTR)value;
			hr = iMeter->Invoke(id, 
									IID_NULL, 
									MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), 
									DISPATCH_PROPERTYPUT, 
									&disp, 
									&varResult, 
									NULL, 
									NULL);
			delete[] pvars;

			if (SUCCEEDED(hr))
			{
				AtlTrace("ValueSetter: Set Value Property\n");
				return S_OK;
			}
		}
	}

	return S_OK;
}

/*
 * Called by SysStats when a configuration dialog for the controller should
 * be displayed so that the user can configure the controller to meet their
 * requirements.
 */
STDMETHODIMP CValueSetter::Configure(IObserver * observer, IMeterHome * meters, LONG hDlg)
{
	ValueSetterDialog *d = new ValueSetterDialog(this, observer, meters);
	d->DoModal((HWND)hDlg);
	delete d;

	return S_OK;
}

/*
 * Return a string representation of the controller’s type.
 */
STDMETHODIMP CValueSetter::get_Type(BSTR * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	char szFriendlyName[MAX_PATH];
	SysStatsUtils::CLSIDToName(CLSID_ValueSetter, szFriendlyName, sizeof(szFriendlyName));
	*pVal = _com_util::ConvertStringToBSTR(szFriendlyName);

	return S_OK;
}

STDMETHODIMP CValueSetter::get_Value(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = value.copy();

	return S_OK;
}

STDMETHODIMP CValueSetter::put_Value(BSTR newVal)
{
	value = newVal;

	return S_OK;
}

STDMETHODIMP CValueSetter::get_Selector(BSTR *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	*pVal = selector.copy();

	return S_OK;
}

STDMETHODIMP CValueSetter::put_Selector(BSTR newVal)
{
	selector = newVal;

	return S_OK;
}
