// AnimatorOverlay.cpp : Implementation of AnimatorOverlayDialog.cpp
#include "stdafx.h"
#include "commctrl.h"
#include "COMOverlays.h"
#include "AnimatorOverlay.h"
#include "AnimatorOverlayDialog.h"
#include "PickList.cpp"

AnimatorOverlayDialog::AnimatorOverlayDialog(CAnimatorOverlay *overlay, IObserver *observer, IMeterHome *meters, WORD idd) :
	OverlayDialog(overlay, observer, meters, idd)
{
}

AnimatorOverlayDialog::~AnimatorOverlayDialog()
{
}

LRESULT AnimatorOverlayDialog::path(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char text[MAX_PATH];
	*text = 0;
	GetDlgItemText(wID, text, sizeof(text));
	CAnimatorOverlay *pOverlay = (CAnimatorOverlay*)overlay;
	((CAnimatorOverlay*)overlay)->put_Path(_bstr_t(text));
	notifyObserver();

	return 1;
}

LRESULT AnimatorOverlayDialog::chooseImage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	OPENFILENAME of;
    memset(&of,0,sizeof(OPENFILENAME));   
    of.lStructSize=sizeof(OPENFILENAME);
	char _path[MAX_PATH];
	*_path = 0;
	of.lpstrFile = _path;
	of.nMaxFile = sizeof(_path);
	of.lpstrTitle = "Choose mask...";
	of.lpstrFilter = "Bitmap\0*.bmp\0GIF\0*.gif\0JPEG\0*.jpg\0PNG\0*.png\0Icon\0*.ico\0All\0*.*\0\0\0";
	of.lpstrDefExt = "png";
	of.nFilterIndex = 4;
	of.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	of.hwndOwner = m_hWnd;

	if (GetOpenFileName(&of))
	{
		SetDlgItemText(IDC_FORMAT, of.lpstrFile);
		((CAnimatorOverlay*)overlay)->put_Path(_bstr_t(of.lpstrFile));
		notifyObserver();
	}

	return 1;
}

LRESULT AnimatorOverlayDialog::alpha(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CAnimatorOverlay*)overlay)->put_Alpha(i);
		notifyObserver();
	}

	return 1;
}

LRESULT AnimatorOverlayDialog::alphaEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CAnimatorOverlay*)overlay)->put_AlphaEnd(i);
		notifyObserver();
	}

	return 1;
}

LRESULT AnimatorOverlayDialog::xEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CAnimatorOverlay*)overlay)->put_XEnd(i);
		notifyObserver();
	}

	return 1;
}

LRESULT AnimatorOverlayDialog::yEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CAnimatorOverlay*)overlay)->put_YEnd(i);
		notifyObserver();
	}

	return 1;
}

LRESULT AnimatorOverlayDialog::xRotEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CAnimatorOverlay*)overlay)->put_CenterXEnd(i);
		notifyObserver();
	}

	return 1;
}

LRESULT AnimatorOverlayDialog::yRotEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CAnimatorOverlay*)overlay)->put_CenterYEnd(i);
		notifyObserver();
	}

	return 1;
}

LRESULT AnimatorOverlayDialog::rotationEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	BOOL iRet = TRUE;	// For GetDlgItemInt
	int i = GetDlgItemInt(wID, &iRet, TRUE);

	if (iRet)
	{
		((CAnimatorOverlay*)overlay)->put_RotationEnd(i);
		notifyObserver();
	}

	return 1;
}

LRESULT AnimatorOverlayDialog::hScaleEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	double scale = 1.0L;
	char val[64];
	GetDlgItemText(wID, val, sizeof(val));
	sscanf(val, "%lg", &scale);
	((CAnimatorOverlay*)overlay)->put_HScaleEnd(scale);
	notifyObserver();

	return 1;
}

LRESULT AnimatorOverlayDialog::vScaleEnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	double scale = 1.0L;
	char val[64];
	GetDlgItemText(wID, val, sizeof(val));
	sscanf(val, "%lg", &scale);
	((CAnimatorOverlay*)overlay)->put_VScaleEnd(scale);
	notifyObserver();

	return 1;
}

/*
 * Called from the dialog proc when the dialog is being initialized.
 */
LRESULT AnimatorOverlayDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BYTE b;

	SendDlgItemMessage(IDC_ALPHASPIN_START,UDM_SETRANGE,0,(LPARAM)MAKELONG(255, 0));
	((CAnimatorOverlay*)overlay)->get_Alpha(&b);
	SetDlgItemInt(IDC_ALPHA_START, (UINT)b, TRUE);

	SendDlgItemMessage(IDC_ALPHASPIN_END,UDM_SETRANGE,0,(LPARAM)MAKELONG(255, 0));
	((CAnimatorOverlay*)overlay)->get_AlphaEnd(&b);
	SetDlgItemInt(IDC_ALPHA_END, (UINT)b, TRUE);

	BSTR f;
	((CAnimatorOverlay*)overlay)->get_Path(&f);
	SetDlgItemText(IDC_FORMAT, _bstr_t(f, false));

	long l;

	SendDlgItemMessage(IDC_XSPIN_END,UDM_SETRANGE,0,(LPARAM)MAKELONG(300, -100));
	((CAnimatorOverlay*)overlay)->get_XEnd(&l);
	SetDlgItemInt(IDC_XPOS_END, (UINT)l, TRUE);

	SendDlgItemMessage(IDC_YSPIN_END,UDM_SETRANGE,0,(LPARAM)MAKELONG(-100, 300));
	((CAnimatorOverlay*)overlay)->get_YEnd(&l);
	SetDlgItemInt(IDC_YPOS_END, (UINT)l, TRUE);

	SendDlgItemMessage(IDC_CXSPIN_END,UDM_SETRANGE,0,(LPARAM)MAKELONG(300, -100));
	((CAnimatorOverlay*)overlay)->get_CenterXEnd(&l);
	SetDlgItemInt(IDC_CXPOS_END, (UINT)l, TRUE);

	SendDlgItemMessage(IDC_CYSPIN_END,UDM_SETRANGE,0,(LPARAM)MAKELONG(-100, 300));
	((CAnimatorOverlay*)overlay)->get_CenterYEnd(&l);
	SetDlgItemInt(IDC_CYPOS_END, (UINT)l, TRUE);

	SendDlgItemMessage(IDC_ROTSPIN_END,UDM_SETRANGE,0,(LPARAM)MAKELONG(359, 0));
	((CAnimatorOverlay*)overlay)->get_RotationEnd(&l);
	SetDlgItemInt(IDC_ROT_END, (UINT)l, TRUE);

	double scale;
	char val[64];

	((CAnimatorOverlay*)overlay)->get_HScaleEnd(&scale);
	sprintf(val, "%.20g", scale);
	SetDlgItemText(IDC_XSCALE_END, val);

	((CAnimatorOverlay*)overlay)->get_VScaleEnd(&scale);
	sprintf(val, "%.20g", scale);
	SetDlgItemText(IDC_YSCALE_END, val);

	bHandled = FALSE;

	return TRUE;
}
