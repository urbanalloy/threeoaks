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

// ClientModel.h: Definition of the CClientModel class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTMODEL_H__1822BE69_B7F2_42F3_97CC_1235B98931E3__INCLUDED_)
#define AFX_CLIENTMODEL_H__1822BE69_B7F2_42F3_97CC_1235B98931E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CMeterHome.h"       // main symbols
#include "COverlayHome.h"       // main symbols
#include "ControllerHome.h"       // main symbols
#include "Label.h"       // main symbols
#include "PluginManager.h"       // main symbols

#include "resource.h"       // main symbols

class CConfigureDialog;

/////////////////////////////////////////////////////////////////////////////
// CClientModel

class CClientModel : 
	public IDispatchImpl<IClientModel, &IID_IClientModel, &LIBID_SYSSTATSCOMLib>, 
	public ISupportErrorInfo,
	public CComObjectRoot,
	public CComCoClass<CClientModel,&CLSID_ClientModel>
{
	// COM part
public:
	STDMETHOD(SetActive)(short);
	STDMETHOD(get_FitToWindow)(/*[out, retval]*/ short *pVal);
	STDMETHOD(put_FitToWindow)(/*[in]*/ short newVal);
	STDMETHOD(get_Author)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Author)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Version)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Version)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Description)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Description)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Name)(/*[in]*/ BSTR newVal);
	STDMETHOD(HandleSubclassMessage)(UINT msg, UINT wparam, UINT lparam);
	STDMETHOD(RemoveWindowSubclass)(/*[in]*/ IController *pSubclass);
	STDMETHOD(SetWindowSubclass)(/*[in]*/ IController *pSubclass);
	STDMETHOD(get_hWnd)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_hWnd)(/*[in]*/ long newVal);
	STDMETHOD(PostMessage)(/*[in]*/ short messageID, /*[in]*/ UINT wParam, /*[in]*/ UINT lParam);
	STDMETHOD(get_Height)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Height)(/*[in]*/ long newVal);
	STDMETHOD(get_Width)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Width)(/*[in]*/ long newVal);
	STDMETHOD(get_Y)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Y)(/*[in]*/ long newVal);
	STDMETHOD(get_X)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_X)(/*[in]*/ long newVal);
	STDMETHOD(get_Label)(/*[out, retval]*/ ILabel* *pVal);
	STDMETHOD(HandleMessage)(/*[in]*/ short messageID, /*[in]*/ UINT wParam, /*[in]*/ UINT lParam, /*[in]*/ short x, /*[in]*/ short y, /*[out, retval]*/ VARIANT_BOOL *handled);
	STDMETHOD(get_Controllers)(/*[out, retval]*/ IControllerHome* *pVal);
	STDMETHOD(put_AppConfig)(/*[in]*/ IAppConfig* newVal);
	STDMETHOD(get_AppConfig)(/*[out, retval]*/ IAppConfig* *pVal);
	STDMETHOD(get_dirty)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_dirty)(/*[in]*/ long newVal);
	STDMETHOD(Configure)(IObserver *observer, long hWnd);
	STDMETHOD(SaveAs)();
	STDMETHOD(Save)(/*[in*/ short interactive);
	STDMETHOD(Load)();
	STDMETHOD(UpdateOverlays)(long hdc);
	STDMETHOD(UpdateMeters)();
	STDMETHOD(get_ConfigFile)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ConfigFile)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_TimerService)(/*[out, retval]*/ ICOMTimerService **pVal);
	STDMETHOD(put_TimerService)(/*[in]*/ ICOMTimerService *newVal);
	STDMETHOD(get_Overlays)(/*[out, retval]*/ IOverlayHome **pVal);
	STDMETHOD(get_Meters)(/*[out, retval]*/ IMeterHome **pVal);

	CClientModel();
	virtual ~CClientModel();
	void FinalRelease();

BEGIN_COM_MAP(CClientModel)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IClientModel)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CClientModel) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_REGISTRY_RESOURCEID(IDR_ClientModel)
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IClientModel
protected:

	IController* addShortcut();

	static const _bstr_t OVERLAY_PREFIX;
	static const _bstr_t CONTROLLER_PREFIX;
	static const _bstr_t METER_PREFIX;
	static const _bstr_t LABEL_PREFIX;
	static const _bstr_t CONFIG_GENERAL;
	static const _bstr_t WINDOW_X;
	static const _bstr_t WINDOW_Y;
	static const _bstr_t WINDOW_WIDTH;
	static const _bstr_t WINDOW_HEIGHT;
	static const _bstr_t NAME;
	static const _bstr_t DESCRIPTION;
	static const _bstr_t DVERSION;
	static const _bstr_t AUTHOR;
	static const _bstr_t FITTOWINDOW;
	static const _bstr_t VERSION_KEY;
	static const int VERSION;

private:
	long x;	// Desired window location
	long y;	// Desired window location
	long width;	// Desired window size
	long height;	// Desired window size
	HWND m_hWnd;

	// An array of *pointers* to meters.
	IMeterHome *pMeters;

	// A shortcut class
	IControllerHome *pControllers;

	// An array of *pointers* to overlays. Well. One overlay actually.
	IOverlayHome *pOverlays;

	// A label class
	ILabel *pLabel;

	// An application configuration class
	IAppConfig *appConfig;

	// A plugin manager!
	PluginManager *plugins;

	// A timer service
	ICOMTimerService *timerService;

	// Dialog
	CConfigureDialog *d;

	_bstr_t name;
	_bstr_t description;
	_bstr_t version;
	_bstr_t author;
	short fitToWindow;
	short active;

	// Indicates that one or more of our meters have changed their value
	long dirty;

	_bstr_t configFile;

	// Window subclassing stuff
	IController *pSubclass;
	WNDPROC pSuperProc;

	static void MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static CSimpleMap<HWND, IClientModel*> subclassMap;
};

#endif // !defined(AFX_CLIENTMODEL_H__1822BE69_B7F2_42F3_97CC_1235B98931E3__INCLUDED_)
