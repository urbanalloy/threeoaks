// DawerOverlayDialog.h : Declaration of the DawerOverlayDialog

#ifndef __DAWEROVERLAYDIALOG_H_
#define __DAWEROVERLAYDIALOG_H_

#include "OverlayDialog.h"

/////////////////////////////////////////////////////////////////////////////
// DawerOverlayDialog

class CDawerOverlay;

struct IMeterHome;

class DawerOverlayDialog : public OverlayDialog
{
public:
	DawerOverlayDialog(CDawerOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd=IDD_DRAWEROVERLAY);
	virtual ~DawerOverlayDialog();

protected:
	LRESULT alpha(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT alphaEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT path(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT chooseImage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT xEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT yEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT xRotEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT yRotEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT rotationEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT hScaleEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT vScaleEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(DawerOverlayDialog)
		CHAIN_MSG_MAP(OverlayDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_ALPHA_START, EN_CHANGE, alpha)
		COMMAND_HANDLER(IDC_ALPHA_END, EN_CHANGE, alphaEnd)
		COMMAND_HANDLER(IDC_FORMAT, EN_CHANGE, path)
		COMMAND_HANDLER(IDC_BROWSE, BN_CLICKED, chooseImage)
		COMMAND_HANDLER(IDC_XPOS_END, EN_CHANGE, xEnd)
		COMMAND_HANDLER(IDC_YPOS_END, EN_CHANGE, yEnd)
		COMMAND_HANDLER(IDC_CXPOS_END, EN_CHANGE, xRotEnd)
		COMMAND_HANDLER(IDC_CYPOS_END, EN_CHANGE, yRotEnd)
		COMMAND_HANDLER(IDC_ROT_END, EN_CHANGE, rotationEnd)
		COMMAND_HANDLER(IDC_XSCALE_END, EN_CHANGE, hScaleEnd)
		COMMAND_HANDLER(IDC_YSCALE_END, EN_CHANGE, vScaleEnd)
	END_MSG_MAP()
};
#endif // __DAWEROVERLAYDIALOG_H_
