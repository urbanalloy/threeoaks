// MeterHomeProps.h : Declaration of the CMeterHomeProps

#ifndef __METERHOMEPROPS_H_
#define __METERHOMEPROPS_H_

#include "resource.h"       // main symbols

EXTERN_C const CLSID CLSID_MeterHomeProps;

/////////////////////////////////////////////////////////////////////////////
// CMeterHomeProps
class ATL_NO_VTABLE CMeterHomeProps :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMeterHomeProps, &CLSID_MeterHomeProps>,
	public IPropertyPageImpl<CMeterHomeProps>,
	public CDialogImpl<CMeterHomeProps>
{
public:
	CMeterHomeProps() 
	{
		m_dwTitleID = IDS_TITLEMeterHomeProps;
		m_dwHelpFileID = IDS_HELPFILEMeterHomeProps;
		m_dwDocStringID = IDS_DOCSTRINGMeterHomeProps;
	}

	enum {IDD = IDD_METERHOMEPROPS};

DECLARE_REGISTRY_RESOURCEID(IDR_METERHOMEPROPS)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMeterHomeProps) 
	COM_INTERFACE_ENTRY(IPropertyPage)
END_COM_MAP()

	void remove(ICompositeOverlay *root, IMeter *pMeter);
	void enable(int item);
	void disable();

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT add(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT remove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT rename(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT configure(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT shortcut(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT setInterval(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT listSelChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	virtual LRESULT OnMeterType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	
	// Don't want to override the one in Dialog. So its not virtual.
	LRESULT save(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	// Specific to this class
	virtual void resetList(int selected);

	IMeterHome *meters;
	ICompositeOverlay *overlayRoot;
	COverlayPanel overlayPanel;
	HINSTANCE hInstance;
	HWND parentHWnd;


	STDMETHOD(Apply)(void)
	{
		ATLTRACE(_T("CMeterHomeProps::Apply\n"));
		for (UINT i = 0; i < m_nObjects; i++)
		{
			// Do something interesting here
			// ICircCtl* pCirc;
			// m_ppUnk[i]->QueryInterface(IID_ICircCtl, (void**)&pCirc);
			// pCirc->put_Caption(CComBSTR("something special"));
			// pCirc->Release();
		}
		m_bDirty = FALSE;
		return S_OK;
	}

public:
	BEGIN_MSG_MAP(CMeterHomeProps)
		CHAIN_MSG_MAP(IPropertyPageImpl<CMeterHomeProps>)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_METER_LIST, listSelChanged)
		COMMAND_ID_HANDLER(IDC_METER_DELETE, remove)
		COMMAND_ID_HANDLER(IDC_METER_RENAME, rename)
		COMMAND_ID_HANDLER(IDC_METER_ADD, add)
		COMMAND_ID_HANDLER(IDC_METER_CONFIGURE, configure)
		COMMAND_ID_HANDLER(IDC_METER_TYPE, OnMeterType)
		COMMAND_HANDLER(IDC_METER_INTERVAL, EN_CHANGE, setInterval)
		COMMAND_ID_HANDLER(IDC_METER_SHORTCUT, shortcut)
		COMMAND_ID_HANDLER(IDOK, save)
	END_MSG_MAP()
};

#endif //__METERHOMEPROPS_H_
