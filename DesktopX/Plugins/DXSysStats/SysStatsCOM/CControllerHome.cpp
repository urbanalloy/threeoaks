// CControllerHome.cpp : Implementation of CSysStatsCOMApp and DLL registration.

#include "stdafx.h"
#include <comdef.h>
#include "SysStatsCOM.h"
#include "CControllerHome.h"
#include "CControllerFactory.h"
#include "IINIPersister.h"

/////////////////////////////////////////////////////////////////////////////
//

STDMETHODIMP CControllerHome::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IControllerHome,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (::ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

const char* CControllerHome::UNKNOWN = "unknown";
const char* CControllerHome::CONTROLLER_COUNT_PROPERTY = "ControllerCount";
const char* CControllerHome::CONTROLLER_INDEX_PROPERTY = "Controller";
const char* CControllerHome::CONTROLLER_PREFIX = "controller";
const char* CControllerHome::CONTROLLER_TYPE_PROPERTY = "Type";

static _bstr_t EMPTY_BSTR = "";

CControllerHome::CControllerHome() : model(0)
{
}

CControllerHome::~CControllerHome()
{
	ClearAndDestroy();

#ifdef CIRCULAR_REFERENCES_OK
	if (model)
		model->Release();
#endif
}

#ifdef notdef
STDMETHODIMP CControllerHome::Load(BSTR fileName, BSTR iniGroup, BSTR prefix)
{
	CComObject<CIINIPersister> *persister;
	CComObject<CIINIPersister>::CreateInstance(&persister);
	persister->AddRef();

	_bstr_t fileName_t(fileName);
	_bstr_t iniGroup_t(iniGroup);
	_bstr_t prefix_t(prefix);

	// Clear what we already have
	ClearAndDestroy();

	// Load the controllers first
	int count = GetPrivateProfileInt(iniGroup_t, CONTROLLER_COUNT_PROPERTY, 0, fileName_t);

	char type[64];
	char groupName[64];
	strcpy(groupName, CONTROLLER_PREFIX);
	strcat(groupName, "-");
	int offset = strlen(groupName);
	for (int i=0; i<count; i++)
	{
		itoa(i, groupName+offset, 10);

		GetPrivateProfileString(groupName, CONTROLLER_TYPE_PROPERTY, UNKNOWN, type, 64, fileName_t);
		IController *pController = CControllerFactory::createInstance(type, model);
		if (pController != NULL)
		{
			Add(pController);
			persister->Configure(fileName_t, _bstr_t(groupName), FALSE, EMPTY_BSTR, model);
			persister->Load(pController);
		}
	}

	return S_OK;
}

STDMETHODIMP CControllerHome::Save(BSTR fileName, BSTR iniGroup, VARIANT_BOOL full, BSTR prefix)
{
	CComObject<CIINIPersister> *persister;
	CComObject<CIINIPersister>::CreateInstance(&persister);
	persister->AddRef();

	_bstr_t fileName_t(fileName);
	_bstr_t iniGroup_t(iniGroup);
	_bstr_t prefix_t(prefix);

	short count = 0;
	Count(&count);

	// Save controllers
	char val[32];
	WritePrivateProfileString(iniGroup_t, CONTROLLER_COUNT_PROPERTY, itoa(count, val, 10), fileName_t);

	char groupName[64];
	strcpy(groupName, CONTROLLER_PREFIX);
	strcat(groupName, "-");
	int offset = strlen(groupName);
	for (int i=0; i<count; i++)
	{
		IController *pItem;
		Item(CComVariant(i), &pItem);
		itoa(i, groupName+offset, 10);
		persister->Configure(fileName_t, _bstr_t(groupName), full, EMPTY_BSTR, model);
		persister->Save(pItem);
		pItem->Release();
	}
	persister->Release();
	return S_OK;
}
#endif

STDMETHODIMP CControllerHome::Add(/*[in]*/ IController *pController)
{
	if (timerService)
		timerService->SetTimerClient(pController);

	if (model)
		pController->put_Model(model);

	return ArrayPtrVariant<IController, IControllerHome>::Add(pController);
}

STDMETHODIMP CControllerHome::Remove(/*[in]*/ VARIANT index)
{
	IController *pController;
	Item(index, &pController);

	if (timerService)
		timerService->RemoveTimerClient(pController);

	HRESULT hr = ArrayPtrVariant<IController, IControllerHome>::Remove(index);
	pController->Release();
	return hr;
}

STDMETHODIMP CControllerHome::HandleMessage(VARIANT_BOOL *handled)
{
	short count = 0;
	Count(&count);
	VARIANT_BOOL d = false;
	IController *pItem;

	for (int i=0; i<count; i++)
	{
		Item(CComVariant(i), &pItem);
		pItem->HandleMessage(&d);
		*handled &= d;
		pItem->Release();
	}

	return S_OK;
}

STDMETHODIMP CControllerHome::get_Model(IClientModel **pVal)
{
	*pVal = model;

	return S_OK;
}

STDMETHODIMP CControllerHome::put_Model(IClientModel *newVal)
{
	if (model == newVal)
		return S_OK;

#ifdef CIRCULAR_REFERENCES_OK
	if (model)
		model->Release();

	newVal->AddRef();
#endif
	model = newVal;

	short count = 0;
	Count(&count);

	for (int i=0; i<count; i++)
	{
		IController *pItem;
		Item(CComVariant(i), &pItem);
		pItem->put_Model(newVal);
		pItem->Release();
	}

	return S_OK;
}

STDMETHODIMP CControllerHome::get_Controllers(LPSAFEARRAY *pVal)
{
	short count;
	Count(&count);
	SAFEARRAY *pArray = ::SafeArrayCreateVector(VT_DISPATCH, 0, count);

	IController *pController;
	long indices[1];
	for (int i=0; i<count; i++)
	{
		indices[0] = i;
		Item(CComVariant(i), &pController);
		::SafeArrayPutElement(pArray, indices, pController);
	}

	*pVal = pArray;

	return S_OK;
}

STDMETHODIMP CControllerHome::put_Controllers(LPSAFEARRAY newVal)
{
	ClearAndDestroy();
	long lb = 0;
	long ub = 0;
	if( !(newVal->fFeatures & FADF_DISPATCH) )
		return E_INVALIDARG;

	HRESULT hr = ::SafeArrayGetUBound(newVal, 1, &ub);
	hr = ::SafeArrayGetLBound(newVal, 1, &lb);

	IDispatch *pDisp;
	IController *pController;
	long indices[1];
	for (int i=lb; i<=ub; i++)
	{
		indices[0] = i;
		pDisp = 0;
		::SafeArrayGetElement(newVal, indices, &pDisp);
		if (pDisp)
		{
			if (SUCCEEDED(pDisp->QueryInterface(IID_IController, (void**)&pController)))
			{
				AtlTrace("Added controller to ControllerHome\n");
				Add(pController);
				pController->Release();
			}
			pDisp->Release();
		}
	}

	return S_OK;
}