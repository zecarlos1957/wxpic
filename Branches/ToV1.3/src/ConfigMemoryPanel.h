#ifndef CONFIGMEMORYPANEL_H
#define CONFIGMEMORYPANEL_H

#include "MemAddrGetter.h"

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
		wxStaticText* aDevIdDecoded;
		wxRadioBox* aIdBinHexRadio;
		//*)
        char     m_sz80DetectedPicDevName[82];

        TMemAddrGetter aCfgMemAddrGetter;
        TMemAddrGetter aIdMemAddrGetter;
        bool           isIdSeparated;     //-- Indicates that IDs are not in the config Memory and that aIdMemAddrGetter is used
        int            aFirstId;           //-- When ID are separated indicated the first row containing an ID

        void LoadConfigBuffer         (void);
        //-- Refresh the list, if pRebuild is true, it is first destroyed (in case the number of rows would have changed)
        void UpdateIdAndConfMemDisplay(bool pRebuild);
        void ApplyConfigEdit          (void);

        uint32_t *GetConfigWord (int pWordNum) { return aCfgBufferBase + (aConfigWordIndex + pWordNum); }

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
		static const long ID_BUTTON6;
		static const long ID_PANEL6;
		//*)

	private:

		//(*Handlers(TConfigMemoryPanel)
		void onCfgMemGridCellChange(wxGridEvent& event);
		void onIdBinHexRadioSelect(wxCommandEvent& event);
		void onShowAllCfgCellsChkClick(wxCommandEvent& event);
		void onHelpOnIdButtonClick(wxCommandEvent& event);
		//*)

		uint32_t  *aCfgBufferBase;
		uint32_t  *aIdBufferBase;
		uint32_t   aConfigWordIndex;

		DECLARE_EVENT_TABLE()
};

#endif
