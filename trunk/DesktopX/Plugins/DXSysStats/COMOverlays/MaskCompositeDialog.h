// MaskCompositeDialog.h : Declaration of the MaskCompositeDialog

#ifndef __MASKCOMPOSITEDIALOG_H_
#define __MASKCOMPOSITEDIALOG_H_

#include "OverlayDialog.h"
#include "Picklist.h"

/////////////////////////////////////////////////////////////////////////////
// MaskCompositeDialog

class CMaskComposite;
struct IMeterHome;

class MaskCompositeDialog : public OverlayDialog
{
public:
	MaskCompositeDialog(CMaskComposite *overlay, IObserver *observer, IMeterHome *meters, WORD idd=IDD_MASKCOMPOSITE);
	virtual ~MaskCompositeDialog();

protected:
	LRESULT setMaskOverlay(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	PickList<long> overlayList;

public:
	BEGIN_MSG_MAP(MaskCompositeDialog)
		CHAIN_MSG_MAP(OverlayDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_COMBO_MASKOVERLAY, setMaskOverlay)
	END_MSG_MAP()
};
#endif // __MASKCOMPOSITEDIALOG_H_
