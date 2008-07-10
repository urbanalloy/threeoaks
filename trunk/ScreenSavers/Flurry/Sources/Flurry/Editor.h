///////////////////////////////////////////////////////////////////////////////////////////////
//
// Flurry : CEditor Class
//
// Flurry Editor
//
// (C) 2008 Julien Templier
//
///////////////////////////////////////////////////////////////////////////////////////////////
// * $LastChangedRevision$
// * $LastChangedDate$
// * $LastChangedBy$
///////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "resource.h"
#include "FlurryPreset.h"
#include <atlhost.h>


namespace Flurry {


	class CEditor : public CDialogImpl<CEditor>
	{
		private:
			Spec* spec;

			BOOL IsPresetValid();
			void SavePreset();

			void UpdateStreams();
			void UpdateTemplate();

		public:
			// "factory"
			static void AutomaticDoModal (Spec* preset)
			{
				CEditor* pEditor = new CEditor(preset);
				pEditor->DoModal();
				delete pEditor;
			}

			// constructors			
			CEditor(Spec* preset) : spec(preset) {};
			~CEditor() {};

			enum { IDD = DLG_EDITOR };

		protected:

			BEGIN_MSG_MAP(CEditor)
				MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
				COMMAND_ID_HANDLER(IDOK, OnOK)
				COMMAND_ID_HANDLER(IDCANCEL, OnCancel)	
				COMMAND_ID_HANDLER(IDC_BUTTON_ADD, OnAdd)	
				COMMAND_ID_HANDLER(IDC_BUTTON_REMOVE, OnRemove)	
				COMMAND_ID_HANDLER(IDC_BUTTON_PREVIEW, OnPreview)					
			END_MSG_MAP()

			// Handler prototypes:
			LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
			LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
			LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

			LRESULT OnAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
			LRESULT OnRemove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
			LRESULT OnPreview(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		
	};

}

