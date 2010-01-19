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

// IINIPersister.cpp : Implementation of CIINIPersister
#include "stdafx.h"

#include "stdio.h"
#include "SysStatsCOM.h"
#include "IINIPersister.h"
#include "SysStatsUtils.h"
#include "CMeterFactory.h"
#include "COverlayFactory.h"
#include "CControllerFactory.h"
#include "DictionaryPtr.cpp"
#include <GdiplusH.h>

static BSTR IMAGE_ROOT = L"docklets\\sysstats";
static _bstr_t _path_t = "Path";
static BSTR NO_SUCH_VALUE = L"!%@$*";

// Property map to help convert from old to new style
static IP_KV imageOverlayMap[] =
{
	{ "MeterIndex", "Meter" },
	{ "selector", "Selector" },
	{ NULL, NULL }
};

static IP_KV imageOverlayMap2[] =
{
	{ "MeterIndex", "ImageMeter" },
	{ "selector", "ImageSelector" },
	{ "Path", "ImageNameFormat" },
	{ NULL, NULL }
};

static IP_KV compositeOverlayMap[] =
{
	{ NULL, NULL }
};

static IP_KV gaugeOverlayMap[] =
{
	{ "MeterIndex", "Meter" },
	{ "selector", "Selector" },
	{ "Thickness", "Width" },
	{ "Alpha", "Color" },
	{ NULL, NULL }
};

static IP_KV meterOverlayMap[] =
{
	{ "MeterIndex", "Meter" },
	{ "selector", "Selector" },
	{ "Length", "Width" },
	{ "Thickness", "Height" },
	{ "Alpha", "Color" },
	{ NULL, NULL }
};

static IP_KV textOverlayMap[] =
{
	{ "MeterIndex", "Meter" },
	{ "selector", "Selector" },
	{ "Alpha", "FontColor" },
	{ NULL, NULL }
};

static IP_KV memoryMeterMap[] =
{
	{ "TimerInterval", "Interval" },
	{ NULL, NULL }
};

static IP_KV pdhMeterMap[] =
{
	{ "TimerInterval", "Interval" },
	{ NULL, NULL }
};

static IP_KV timeMeterMap[] =
{
	{ "TimerInterval", "Interval" },
	{ NULL, NULL }
};

static IP_KV diskMeterMap[] =
{
	{ "TimerInterval", "Interval" },
	{ NULL, NULL }
};

static IP_KV mbmMeterMap[] =
{
	{ "TimerInterval", "Interval" },
	{ NULL, NULL }
};

static IP_KV fixedMeterMap[] =
{
	{ "TimerInterval", "Interval" },
	{ NULL, NULL }
};

static IP_KV xmlMeterMap[] =
{
	{ "TimerInterval", "Interval" },
	{ "ProxyURL", "Proxy" },
	{ NULL, NULL }
};

static IP_KV shortcutMap[] =
{
	{ "MeterIndex", "appMeterIndex" },
	{ "StartIn", "appStartIn" },
	{ "selector", "appSelector" },
	{ "Shortcut", "appName" },
	{ "Args", "appArgs" },
	{ NULL, NULL }
};

static IP_KV labelMap[] =
{
	{ "MeterIndex", "LabelMeter" },
	{ "format", "LabelFormat" },
	{ "selector", "LabelSelector" },
	{ NULL, NULL }
};

static IP_PROPMAP doodah[] =
{
	{ "IImageOverlay-1", imageOverlayMap },
	{ "IImageOverlay-2", imageOverlayMap2 },
	{ "IMeterOverlay-1", meterOverlayMap },
	{ "ICompositeOverlay-1", compositeOverlayMap },
	{ "IGaugeOverlay-1", gaugeOverlayMap },
	{ "ITextOverlay-1", textOverlayMap },
	{ "IMemoryMeter-1", memoryMeterMap },
	{ "IPDHMeter-1", pdhMeterMap },
	{ "IDiskMeter-1", diskMeterMap },
	{ "IMBMMeter-1", mbmMeterMap },
	{ "IXMLMeter-1", xmlMeterMap },
	{ "ITimeMeter-1", timeMeterMap },
	{ "IFixedMeter-1", fixedMeterMap },
	{ "IShortcut-1", shortcutMap },
	{ "ILabel-1", labelMap },
	{ NULL, NULL }
};

bool strEq(const char *lhs, const char *rhs)
{
	return strcmp(lhs, rhs) == 0;
}

AutoDictionaryPtr3<char, AutoDictionaryPtr2<char, char> > CIINIPersister::propertyMap(strEq);
int CIINIPersister::count = 0;

/////////////////////////////////////////////////////////////////////////////
// CIINIPersister

STDMETHODIMP CIINIPersister::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IIINIPersister
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

CIINIPersister::CIINIPersister() :
	configFile(NULL),
	groupName(""),
	full(FALSE),
	prefix(""),
	model(NULL),
	oldFormat(false)
{
	AtlTrace("CIINIPersister::CIINIPersister(%d)\n", ++count);
	if (propertyMap.count() == 0)
	{
		for (int i=0; doodah[i].key != NULL; i++)
		{
			IP_KV *pKV = doodah[i].value;
			AutoDictionaryPtr2<char, char> *pVal = new AutoDictionaryPtr2<char, char>(strEq);

			for (int j=0; pKV[j].key != NULL; j++)
			{
				pVal->put(pKV[j].key, pKV[j].value);
			}

			propertyMap.put(doodah[i].key, pVal);
		}
	}
}

CIINIPersister::~CIINIPersister()
{
	AtlTrace("CIINIPersister::~CIINIPersister(%d)\n", --count);
	if (configFile)
		configFile->Release();

	if (model)
		model->Release();
}

STDMETHODIMP CIINIPersister::Save(IDispatch *pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	// Clear the section
//	AtlTrace(L"Writing to %s[%s]\n", (BSTR)fileName, (BSTR)groupName);
//	::WritePrivateProfileString(groupName, NULL, NULL, fileName);
	return Iterate(pVal, INVOKE_PROPERTYGET, &CIINIPersister::SaveValue);
}

STDMETHODIMP CIINIPersister::Load(IDispatch *pVal)
{
	if (pVal == NULL)
		return E_POINTER;
		
//	AtlTrace(L"Reading from %s[%s]\n", (BSTR)fileName, (BSTR)groupName);
	return Iterate(pVal, INVOKE_PROPERTYPUT, &CIINIPersister::LoadValue);
}

HRESULT CIINIPersister::SaveValue(IDispatch *pVal, ITypeInfo *pTypeInfo, FUNCDESC *pFuncDesc)
{
	EXCEPINFO ex = {0};
	unsigned int errorIndex = 0;
	VARIANT retVal;
	DISPPARAMS params;
	memset(&params, 0, sizeof(params));
	HRESULT hr = E_UNEXPECTED;

	// Get the value
	try {
		hr = pTypeInfo->Invoke(pVal, pFuncDesc->memid, DISPATCH_PROPERTYGET, &params, &retVal, &ex, &errorIndex);
		if (SUCCEEDED(hr))
		{
			BSTR names[1];
			unsigned int numNames;

			// Get the name
			hr = pTypeInfo->GetNames(pFuncDesc->memid, &names[0], 1, &numNames);
			if (SUCCEEDED(hr))
			{
				wchar_t keyBuf[MAX_PATH];

				if (pFuncDesc->elemdescFunc.tdesc.vt == VT_SAFEARRAY)
				{
					// We got us an array
					long ub = 0;
					long lb = 0;
					LPSAFEARRAY pArray = V_ARRAY(&retVal);

					HRESULT hr = ::SafeArrayGetUBound(pArray, 1, &ub);
					hr = ::SafeArrayGetLBound(pArray, 1, &lb);
					if (SUCCEEDED(hr))
					{
						char buf[MAX_PATH];
						AtlTrace(L"%s[%ld]\n", names[0], ub-lb+1);
						wcscpy(keyBuf, prefix);
						wcscat(keyBuf, names[0]);
						wcscat(keyBuf, L"-Count");
						configFile->PutInt(groupName, keyBuf, ub-lb+1);
						//::WritePrivateProfileString(groupName, prefix + names[0] + "-Count", itoa(ub-lb+1, buf, 10), fileName);

						ARRAYDESC *aDesc = pFuncDesc->elemdescFunc.tdesc.lpadesc;
						VARIANT vTmp;
						vTmp.vt = aDesc->tdescElem.vt;
	/*
						::SafeArrayGetVartype(pArray, &vTmp.vt);
	*/
						long indices[1];
						wchar_t ordinal[16];
						IIINIPersister *persister = NULL;
						if (vTmp.vt == VT_DISPATCH)
						{
							HRESULT hr = ::CoCreateInstance(CLSID_IINIPersister, NULL, CLSCTX_INPROC_SERVER, IID_IIINIPersister, (void**)&persister);
						}

						for (int i=lb; i<=ub; i++)
						{
							indices[0] = i;
							_snwprintf(ordinal, sizeof(ordinal)/sizeof(wchar_t), L"-%d", i);
							vTmp.vt = aDesc->tdescElem.vt;
							if (vTmp.vt == VT_VARIANT)
								hr = ::SafeArrayGetElement(pArray, indices, &vTmp);
							else
								hr = ::SafeArrayGetElement(pArray, indices, &vTmp.byref);

							if (vTmp.vt == VT_DISPATCH)
							{
								wcscpy(keyBuf, groupName);
								wcscat(keyBuf, ordinal);
								persister->Configure(configFile, keyBuf, full, prefix, model, oldFormat);
								persister->Save(vTmp.pdispVal);
							}
							else
							{
								hr = SysStatsUtils::VariantToString(&vTmp, buf, sizeof(buf));
								if (SUCCEEDED(hr))
								{
									AtlTrace(L"%s[%d]=%s\n", names[0], i, (BSTR)(_bstr_t(buf)));
									wcscpy(keyBuf, prefix);
									wcscat(keyBuf, names[0]);
									wcscat(keyBuf, ordinal);
									configFile->PutString(groupName, keyBuf, (BSTR)(_bstr_t(buf)));
//									::WritePrivateProfileString(groupName, prefix + names[0] + ordinal, buf, fileName);
								}
								else
								{
									AtlTrace("Could not convert result[%d] to a string!\n", i);
								}
							}
							::VariantClear(&vTmp);
						}

						if (persister != NULL)
							persister->Release();
					}
					else
					{
						AtlTrace("Could not get array size\n");
					}
				}
				else
				{
					// We got us a scalar
					char buf[MAX_PATH];
					hr = SysStatsUtils::VariantToString(&retVal, buf, sizeof(buf));
					if (SUCCEEDED(hr))
					{
						AtlTrace(L"%s=%s\n", names[0], (BSTR)(_bstr_t(buf)));
						wcscpy(keyBuf, prefix);
						wcscat(keyBuf, names[0]);
						configFile->PutString(groupName, keyBuf, (BSTR)(_bstr_t(buf)));
//						::WritePrivateProfileString(groupName, prefix + names[0], buf, fileName);
					}
					else
					{
						AtlTrace("Could not convert result to a string!\n");
					}
				}

				::SysFreeString(names[0]);
				::VariantClear(&retVal);
			}
			else
			{
				AtlTrace("Could not get property name\n");
			}
		}
		else
		{
			AtlTrace(L"Problem retrieving property: hr=%x, ex='%s', errorArg=%d\n", hr, ex.bstrDescription, errorIndex);
		}
	} catch (...) {
		AtlTrace(L"Problem retrieving property\n");
	}
		

	return hr;
}

HRESULT CIINIPersister::LoadValue(IDispatch *pVal, ITypeInfo *pTypeInfo, FUNCDESC *pFuncDesc)
{
	BSTR names[1];
	unsigned int numNames;
	HRESULT hr = pTypeInfo->GetNames(pFuncDesc->memid, &names[0], 1, &numNames);
	if (SUCCEEDED(hr))
	{
		BSTR tmpStr;	// Must free this BSTR
		//char buf[MAX_PATH];
		EXCEPINFO ex = {0};
		unsigned int errorIndex = 0;
		SAFEARRAY *pArray = NULL;
		VARIANT retVal;
		VARIANTARG args[1];
		DISPID ids[1];
		DISPPARAMS params;
		memset(&params, 0, sizeof(params));
		params.cArgs = 1;
		params.cNamedArgs = 1;
		params.rgvarg = args;
		params.rgdispidNamedArgs = ids;
		ids[0] = DISPID_PROPERTYPUT;
		args[0].vt = VT_EMPTY;
		VARTYPE argType = pFuncDesc->lprgelemdescParam[0].tdesc.vt;
		AtlTrace("Argument type=%d\n", argType);

		if (argType == VT_SAFEARRAY)
		{
			// We got us an array
			ARRAYDESC *aDesc = pFuncDesc->lprgelemdescParam[0].tdesc.lpadesc;
			VARTYPE elType = aDesc->tdescElem.vt;
			configFile->GetString(groupName, prefix+names[0] + L"-Count", NO_SUCH_VALUE, &tmpStr);
//			::GetPrivateProfileString(groupName, prefix+names[0] + "-Count", NO_SUCH_VALUE, buf, sizeof(buf), fileName);

			// Could be an old style file. Look for an overlay count (or a PDHMeter counters count).
			if (wcscmp(tmpStr, NO_SUCH_VALUE) == 0)
			{
				::SysFreeString(tmpStr);
				configFile->GetString(groupName, L"Count", NO_SUCH_VALUE, &tmpStr);
//				::GetPrivateProfileString(groupName, "Count", NO_SUCH_VALUE, buf, sizeof(buf), fileName);
			}

			// Could be an old style file. Look for a MeterCount.
			if (wcscmp(tmpStr, NO_SUCH_VALUE) == 0)
			{
				::SysFreeString(tmpStr);
				configFile->GetString(groupName, L"MeterCount", NO_SUCH_VALUE, &tmpStr);
//				::GetPrivateProfileString(groupName, "MeterCount", NO_SUCH_VALUE, buf, sizeof(buf), fileName);
			}

			if (wcscmp(tmpStr, NO_SUCH_VALUE) != 0)
			{
				long count[1];
				count[0] = _wtol(tmpStr);
				AtlTrace("Array element count=%d, type=%d\n", count[0], elType);

				pArray = ::SafeArrayCreateVector(elType, 0, count[0]);

				long indices[1];
				char ordinal[16];
				VARIANT vTmp;
				IIINIPersister *persister = NULL;
				if (elType == VT_DISPATCH)
				{
					HRESULT hr = ::CoCreateInstance(CLSID_IINIPersister, NULL, CLSCTX_INPROC_SERVER, IID_IIINIPersister, (void**)&persister);
				}

				// Loop around the array
				for (int i=0; i<count[0]; i++)
				{
					indices[0] = i;
//					_snprintf(ordinal, sizeof(ordinal), "-%d", i);
					_snprintf(ordinal, sizeof(ordinal), "%d", i);
					vTmp.vt = VT_EMPTY;
					if (elType == VT_DISPATCH)
					{
						// We've got an object to create
						::SysFreeString(tmpStr);
						_bstr_t newGroupName = groupName + "-" + ordinal;
						configFile->GetString(newGroupName, L"Type", NO_SUCH_VALUE, &tmpStr);						
//						::GetPrivateProfileString(newGroupName, "Type", NO_SUCH_VALUE, buf, sizeof(buf), fileName);

						// Could be an old style file look for 'MeterType'  in '[meter<n>]'
						if (wcscmp(tmpStr, NO_SUCH_VALUE) == 0)
						{
							// Only one count in old [General] and that is MeterCount
							if (groupName == _bstr_t("General"))
							{
								::SysFreeString(tmpStr);
								newGroupName = _bstr_t("meter") + ordinal;
								configFile->GetString(newGroupName, L"MeterType", NO_SUCH_VALUE, &tmpStr);						
//								::GetPrivateProfileString(newGroupName, "MeterType", NO_SUCH_VALUE, buf, sizeof(buf), fileName);
								if (wcscmp(tmpStr, NO_SUCH_VALUE) != 0)
								{
									_bstr_t tmp(tmpStr);

									// PDH meter doesn't have same name
									if (wcscmp(tmpStr, L"Pdh") == 0)
										tmp = L"PDH";

									tmp += L"Meter Class";
									::SysFreeString(tmpStr);
									tmpStr = tmp.copy();	// Yeuk
								}
							}
						}

						// Could be an old style file look for 'OverlayType'  in '[overlay-<n>]'
						if (wcscmp(tmpStr, NO_SUCH_VALUE) == 0)
						{
							::SysFreeString(tmpStr);
							newGroupName = groupName + "-" + ordinal;
							configFile->GetString(newGroupName, L"OverlayType", NO_SUCH_VALUE, &tmpStr);						
//							::GetPrivateProfileString(newGroupName, "OverlayType", NO_SUCH_VALUE, buf, sizeof(buf), fileName);
							if (wcscmp(tmpStr, NO_SUCH_VALUE) != 0)
							{
								_bstr_t tmp(tmpStr);
								tmp += L"Overlay Class";
								::SysFreeString(tmpStr);
								tmpStr = tmp.copy();
							}
						}

						if (wcscmp(tmpStr, NO_SUCH_VALUE) != 0)
						{
							_bstr_t type(tmpStr);
							// Yeuk. Surely we can just create one straight-off?
							vTmp.pdispVal = CMeterFactory::createInstance(type, model);
							if (!vTmp.pdispVal)
								vTmp.pdispVal = COverlayFactory::createInstance(type, model);
							if (!vTmp.pdispVal)
								vTmp.pdispVal = CControllerFactory::createInstance(type, model);

							if (vTmp.pdispVal != NULL)
							{
								persister->Configure(configFile, newGroupName, full, prefix, model, oldFormat);
								persister->Load(vTmp.pdispVal);
								vTmp.vt = elType;
							}
							else
							{
								AtlTrace(L"Could not create instance of %s\n", tmpStr);
							}
						}
					}
					else	// End if VT_DISPATCH
					{
						// We've got an array of primitives

						::SysFreeString(tmpStr);
						configFile->GetString(groupName, prefix+names[0] + L"-" + ordinal, NO_SUCH_VALUE, &tmpStr);						
//						::GetPrivateProfileString(groupName, prefix+names[0] + "-" + ordinal, NO_SUCH_VALUE, buf, sizeof(buf), fileName);

						// Maybe we've got an old file. Only array of scalars
						// is for PDHMeter.
						if (wcscmp(tmpStr, NO_SUCH_VALUE) == 0)
						{
							::SysFreeString(tmpStr);
							configFile->GetString(groupName, _bstr_t("Counter") + ordinal, NO_SUCH_VALUE, &tmpStr);						
//							::GetPrivateProfileString(groupName, _bstr_t("Counter") + ordinal, NO_SUCH_VALUE, buf, sizeof(buf), fileName);
						}

						if (wcscmp(tmpStr, NO_SUCH_VALUE) != 0)
						{
							AtlTrace(L"Value=%s\n", tmpStr);
							hr = SysStatsUtils::StringToVariant(_bstr_t(tmpStr), elType, &vTmp);
							if (SUCCEEDED(hr))
							{
								AtlTrace("Converted element %d\n", i);
							}
							else
							{
								AtlTrace("Could not convert element %d\n", i);
							}
						}
						else
						{
							AtlTrace("Could not read element %d\n", i);
						}
					}

					// Should have something by now.
					if (vTmp.vt != VT_EMPTY)
					{
						::SafeArrayPutElement(pArray, indices, vTmp.byref);
						if (vTmp.vt & VT_BYREF)
							delete vTmp.byref;

						::VariantClear(&vTmp);
					}
				}	// End for

				if (persister)
					persister->Release();

				args[0].vt = VT_ARRAY | elType;
				args[0].parray = pArray;
			}
			else
			{
				AtlTrace("Could not get element count\n");
			}

			::SysFreeString(tmpStr);
		}
		else	// End of VT_SAFEARRAY
		{
			// We got us a scalar
			_bstr_t _name0 = names[0];

			configFile->GetString(groupName, prefix+_name0, NO_SUCH_VALUE, &tmpStr);						
//			::GetPrivateProfileString(groupName, prefix+_name0, NO_SUCH_VALUE, buf, sizeof(buf), fileName);

			// Couldn't find this property. Look through the alternatives.
			for (int i=1; wcscmp(tmpStr, NO_SUCH_VALUE) == 0; i++)
			{
				AtlTrace("Looking for alternative property name\n");

				// Might be an old-style config
				ITypeLib *pTypeLib = NULL;
				UINT thisIndex;
				hr = pTypeInfo->GetContainingTypeLib(&pTypeLib, &thisIndex);
				if (SUCCEEDED(hr) && pTypeLib)
				{
					AtlTrace("Got TypeLib\n");
					BSTR name;
					hr = pTypeLib->GetDocumentation(thisIndex, &name, NULL, NULL, NULL);
					if (SUCCEEDED(hr))
					{
						char ordinal[16];
						_snprintf(ordinal, sizeof(ordinal), "-%d", i);

						DictionaryPtr<char, char> *pMap = propertyMap.get(_bstr_t(name) + ordinal);
						if (pMap)
						{
							char *altName = pMap->get(_name0);
							if (altName)
							{
								AtlTrace("Got alternative name %s for property %s of interface %s\n", altName, (char*)_name0, (char*)_bstr_t(name));
								::SysFreeString(tmpStr);
								configFile->GetString(groupName, prefix+altName, NO_SUCH_VALUE, &tmpStr);						
//								::GetPrivateProfileString(groupName, prefix+altName, NO_SUCH_VALUE, buf, sizeof(buf), fileName);
							}
							::SysFreeString(name);
						}
						else
						{
							::SysFreeString(name);
							break;
						}
					}
					else
					{
						AtlTrace("Could not get name\n");
					}

					pTypeLib->Release();
				}
			}

			if (wcscmp(tmpStr, NO_SUCH_VALUE) != 0)
			{
				AtlTrace(L"Value=%s\n", tmpStr);
				// Hack for old config file support
				if (wcsstr(tmpStr, L"0x") == tmpStr)
				{
					if (_bstr_t(_name0) == _bstr_t("Alpha"))
					{
						tmpStr[4] = 0;
						AtlTrace(L"Setting alpha to %s\n", tmpStr);
						SysStatsUtils::StringToVariant(_bstr_t(tmpStr), argType, &args[0]);
					}
					else
					{
						SysStatsUtils::StringToVariant(_bstr_t(tmpStr), argType, &args[0]);
						Color color;
						color.SetValue(args[0].lVal);
						args[0].lVal = color.ToCOLORREF();
						AtlTrace("Setting color to %ld\n", args[0].lVal);
					}
				}
				else if (_wcsnicmp(tmpStr, IMAGE_ROOT, wcslen(IMAGE_ROOT)) == 0)
				{
					// We have an old relative path
					SysStatsUtils::StringToVariant(_bstr_t(tmpStr+wcslen(IMAGE_ROOT)+1), argType, &args[0]);
				}
				else if (oldFormat && (_name0 == _path_t))
				{
					// This is a Path. It might be relative to the old docklet root.
					IAppConfig *appConfig;
					model->get_AppConfig(&appConfig);
					BSTR newVal;
					appConfig->AltCanonicalPath(tmpStr, &newVal);
					args[0].bstrVal = newVal;
					args[0].vt = argType;
					appConfig->Release();
				}
				else
					SysStatsUtils::StringToVariant(_bstr_t(tmpStr), argType, &args[0]);

			}
			else
			{
				AtlTrace(L"Could not read value for key %s\n", (BSTR)(prefix + names[0]));
			}

			::SysFreeString(tmpStr);
		}

		if (args[0].vt != 0)
		{
			hr = pTypeInfo->Invoke(pVal, pFuncDesc->memid, DISPATCH_PROPERTYPUT, &params, &retVal, &ex, &errorIndex);
			if (SUCCEEDED(hr))
			{
				AtlTrace(L"Property set\n");
			}
			else
			{
				AtlTrace(L"Problem setting property: hr=%x, ex='%s', errorArg=%d\n", hr, ex.bstrDescription, errorIndex);
			}

			::VariantClear(&args[0]);
		}
		else
		{
			AtlTrace("Problem reading property somewhere along the line\n");
		}

		::SysFreeString(names[0]);
	}
	else
	{
		AtlTrace("Could not get property name\n");
	}

	return hr;
}

HRESULT CIINIPersister::Iterate(IDispatch *pVal, long funcType, IteratorCallbackPtr pCallBack)
{

	ITypeInfo *pTypeInfo = NULL;
	HRESULT hr = pVal->GetTypeInfo(0, MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), &pTypeInfo);
	if (SUCCEEDED(hr) && pTypeInfo)
	{
		AtlTrace("Got type info\n");

#ifdef _DEBUG
		// Get the interface name
		ITypeLib *pTypeLib = NULL;
		UINT thisIndex;
		hr = pTypeInfo->GetContainingTypeLib(&pTypeLib, &thisIndex);
		if (SUCCEEDED(hr) && pTypeLib)
		{
			AtlTrace("Got TypeLib\n");
			BSTR name;
			hr = pTypeLib->GetDocumentation(thisIndex, &name, NULL, NULL, NULL);
			if (SUCCEEDED(hr))
			{
				AtlTrace(L"Persisting interface %s\n", name);
				::SysFreeString(name);
			}
			else
			{
				AtlTrace("Could not get name\n");
			}

			pTypeLib->Release();
		}
		else
		{
			AtlTrace("Couldn't get TypeLib\n");
		}
#endif //_DEBUG

		// Iterate through the functions looking for property accessors
		TYPEATTR *pTypeAttr = NULL;
		hr = pTypeInfo->GetTypeAttr(&pTypeAttr);
		if (SUCCEEDED(hr) && pTypeAttr)
		{
			AtlTrace("Got TypeAttr. #vars=%d, #funcs=%d\n", pTypeAttr->cVars, pTypeAttr->cFuncs);
			for (int i=0; i<pTypeAttr->cFuncs; i++)
			{
				FUNCDESC *pFuncDesc = NULL;
				hr = pTypeInfo->GetFuncDesc(i, &pFuncDesc);
				if (SUCCEEDED(hr) && pFuncDesc)
				{
					if (!(pFuncDesc->wFuncFlags & FUNCFLAG_FRESTRICTED))
					{
						AtlTrace("Got FuncDesc for func %d, flags=0x%x\n", i, pFuncDesc->wFuncFlags);
						if (pFuncDesc->invkind == funcType)
						{
							AtlTrace("Got property (id=%d) of type %d\n", pFuncDesc->memid, pFuncDesc->elemdescFunc.tdesc.vt);

#ifdef _DEBUG
							// Dump the names associated with this property
							BSTR names[10];
							unsigned int numNames;
							hr = pTypeInfo->GetNames(pFuncDesc->memid, &names[0], 10, &numNames);
							if (SUCCEEDED(hr))
							{
								for (unsigned int j=0; j<numNames; j++)
								{
									AtlTrace(L"Property (id=%d), name[%d] = %s\n", pFuncDesc->memid, j, names[j]);
									::SysFreeString(names[j]);
								}
							}
							else
							{
								AtlTrace("Could not get propertyname for property id=%d\n", pFuncDesc->memid);
							}
#endif // _DEBUG

							hr = (this->*pCallBack)(pVal, pTypeInfo, pFuncDesc);
						}
					}

					pTypeInfo->ReleaseFuncDesc(pFuncDesc);
				}
				else
				{
					AtlTrace("No FuncDesc for func %d\n", i);
				}
			}

			pTypeInfo->ReleaseTypeAttr(pTypeAttr);
		}
		else
		{
			AtlTrace("No TypeAttr\n");
		}

		pTypeInfo->Release();
	}
	else
	{
		AtlTrace("No type info\n");
	}

	return S_OK;
}

STDMETHODIMP CIINIPersister::Configure(ICOMINIFile *configFile, BSTR iniGroup, VARIANT_BOOL full, BSTR prefix, IClientModel *model, VARIANT_BOOL oldFormat)
{
	this->oldFormat = oldFormat;
	configFile->AddRef();
	if (this->configFile)
		this->configFile->Release();
	this->configFile = configFile;
	this->groupName = iniGroup;
	this->full = full;
	this->prefix = prefix;
	if (this->model)
		this->model->Release();

	this->model = model;
	if (model)
		model->AddRef();

	return S_OK;
}
