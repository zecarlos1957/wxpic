#ifndef CONFIGMEMORYPANEL_H
#define CONFIGMEMORYPANEL_H

//(*Headers(TConfigMemoryPanel)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/radiobox.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/button.h>
//*)

class TConfigMemoryPanel: public wxPanel
{
	public:

		TConfigMemoryPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size, long style, const wxString& name);
		virtual ~TConfigMemoryPanel();

		//(*Declarations(TConfigMemoryPanel)
		wxStaticText* StaticText10;
		wxCheckBox* aShowAllCfgCellsChk;
		wxStaticText* StaticText11;
		wxStaticText* aDevId;
		wxGrid* aCfgMemGrid;
		wxStaticText* aDevIdTitle;
		wxButton* aHelpOnIdButton;
		wxPanel* Panel6;
		wxButton* aApplyIdLocsButton;
		wxStaticText* aDevIdDecoded;
		wxRadioBox* aIdBinHexRadio;
		//*)
        char     m_sz80DetectedPicDevName[82];

        void ApplyIdLocationDisplay(void);
        void UpdateIdAndConfMemDisplay(void);

	protected:

		//(*Identifiers(TConfigMemoryPanel)
		static const long ID_CFG_MEM_GRID;
		static const long ID_STATICTEXT10;
		static const long ID_ID_BIN_HEX_RADIO;
		static const long ID_SHOW_ALL_CFG_CELLS_CHK;
		static const long ID_STATICTEXT11;
		static const long ID_DEV_ID_TITLE;
		static const long ID_DEV_ID;
		static const long ID_DEV_ID_DECODED;
		static const long ID_APPLY_ID_LOCS_BUTTON;
		static const long ID_BUTTON6;
		static const long ID_PANEL6;
		//*)

	private:

		//(*Handlers(TConfigMemoryPanel)
		void onCfgMemGridCellChange(wxGridEvent& event);
		void onIdBinHexRadioSelect(wxCommandEvent& event);
		void onShowAllCfgCellsChkClick(wxCommandEvent& event);
		void onApplyIdLocsButtonClick(wxCommandEvent& event);
		void onHelpOnIdButtonClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
