// MaskComposite.cpp : Implementation of MaskCompositeDialog.cpp
#include "stdafx.h"
#include "commctrl.h"
#include "COMOverlays.h"
#include "MaskComposite.h"
#include "MaskCompositeDialog.h"
#include "PickList.cpp"

static void ToValL(long *out, char *s)
{
	sscanf(s, "%d", out);
}

static void ToStrL(char *buf, int maxlen, long val)
{
	_snprintf(buf, maxlen, "%d", val);
}

template <class C>
static bool Identity(C lhs, C rhs)
{
	return lhs == rhs;
}

MaskCompositeDialog::MaskCompositeDialog(CMaskComposite *overlay, IObserver *observer, IMeterHome *meters, WORD idd) :
	OverlayDialog(overlay, observer, meters, idd),
	overlayList(Identity<long>, ToStrL, ToValL, /* fixed */ true)
{
}

MaskCompositeDialog::~MaskCompositeDialog()
{
}

LRESULT MaskCompositeDialog::setMaskOverlay(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	long val = -1;
	overlayList.getData(&val, -1 /* default value */);
	((IMaskComposite*)overlay)->put_MaskOverlayIndex(val);
	notifyObserver();

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 * Should initialize all of the controls using values from the meter.
 */
LRESULT MaskCompositeDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Set up the list of overlays
	long current = -1;
	((IMaskComposite*)overlay)->get_MaskOverlayIndex(&current);

	overlayList.init(m_hWnd, IDC_COMBO_MASKOVERLAY, (IMaskComposite*)overlay, current);
// Add your own initializations here
	bHandled = FALSE; // Make CWindow go through all the message maps.

	return 1;
}
