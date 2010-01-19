// ControllerHome.h: Definition of the CControllerHome class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLLERHOME_H__9244E5B9_EAAD_4E54_91A7_C45B135E8910__INCLUDED_)
#define AFX_CONTROLLERHOME_H__9244E5B9_EAAD_4E54_91A7_C45B135E8910__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ArrayPtrVariant.cpp"

#include "resource.h"       // main symbols

class COMTimerService;
class Observer;

/////////////////////////////////////////////////////////////////////////////
// CControllerHome

class CControllerHome : 
	public IDispatchImpl<ArrayPtrVariant<IController, IControllerHome>, &IID_IControllerHome, &LIBID_SYSSTATSCOMLib>, 
	public ISupportErrorInfo,
	public CComObjectRoot,
	public CComCoClass<CControllerHome,&CLSID_ControllerHome>
{
// COM Part
public:
	CControllerHome();

BEGIN_COM_MAP(CControllerHome)
 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IControllerHome)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CControllerHome) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_REGISTRY_RESOURCEID(IDR_CONTROLLERHOME)
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IControllerHome
public:
	STDMETHOD(Remove)(/*[in]*/ VARIANT index);
	STDMETHOD(Add)(/*[in]*/ IController *pController);
	STDMETHOD(get_Controllers)(/*[out, retval]*/ LPSAFEARRAY *pVal);
	STDMETHOD(put_Controllers)(/*[in]*/ LPSAFEARRAY newVal);

// Non-COM part
public:
	STDMETHOD(get_Model)(/*[out, retval]*/ IClientModel **pVal);
	STDMETHOD(put_Model)(/*[in]*/ IClientModel *newVal);
	STDMETHOD(Update)(/*[out, retval]*/ long *dirty);
#ifdef notdef
	STDMETHOD(Save)(/*[in]*/ BSTR ini, /*[in]*/ BSTR iniGroup, /*[in]*/ VARIANT_BOOL full, /*[in]*/ BSTR prefix);
	STDMETHOD(Load)(/*[in]*/ BSTR ini, /*[in]*/ BSTR iniGroup, /*[in]*/ BSTR prefix);
#endif
	virtual ~CControllerHome();

protected:
	static const char* UNKNOWN;
	static const char* CONTROLLER_COUNT_PROPERTY;
	static const char* CONTROLLER_INDEX_PROPERTY;
	static const char* CONTROLLER_PREFIX;
	static const char* CONTROLLER_TYPE_PROPERTY;

private:
	IClientModel *model;
};

#endif // !defined(AFX_CONTROLLERHOME_H__9244E5B9_EAAD_4E54_91A7_C45B135E8910__INCLUDED_)
