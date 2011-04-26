#ifndef OPTIONPANEL_H
#define OPTIONPANEL_H

//(*Headers(TOptionPanel)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/colordlg.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/dirdlg.h>
//*)

class TOptionPanel: public wxPanel
{
	public:

		TOptionPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size, long style, const wxString& name);
		virtual ~TOptionPanel();

		//(*Declarations(TOptionPanel)
		wxCheckBox* aDontCareForOsccalChk;
		wxCheckBox* aVddBeforeMCLRChk;
		wxCheckBox* aProgDataMemChk;
		wxStaticText* StaticText1;
		wxCheckBox* aUseBulkEraseChk;
		wxButton* aMPLabDevDirButton;
		wxCheckBox* aVerifyDiffVddsChk;
		wxCheckBox* aClrBeforeLoadChk;
		wxCheckBox* aDisconnectAfterProgChk;
		wxButton* aStartTestButton;
		wxButton* aDataMemColourButton;
		wxCheckBox* aDontCareForBGCalibChk;
		wxStaticText* aMplabDirLabel;
		wxCheckBox* aVerboseMsgsChk;
		wxCheckBox* aProgCodeMemChk;
		wxButton* aCodeMemColourButton;
		wxDirDialog* aDirDialog;
		wxCheckBox* aSimulateOnlyChk;
		wxChoice* aTestChoice;
		wxTextCtrl* aMplabDevDirEdit;
		wxChoice* aLanguageChoice;
		wxCheckBox* aProgConfigMemChk;
		wxColourDialog* aColourDialog;
		//*)

        enum
        {
            radioDriverUSE_WIN_API_ONLY,
            radioDriverUSE_SMPORT,
            radioDriverUSE_PORTTALK,
            radioDriverALREADY_GRANTED,
        };

        void UpdateOptionsDisplay(void);

	protected:

		//(*Identifiers(TOptionPanel)
		static const long ID_PROG_CODE_MEM_CHK;
		static const long ID_PROG_DATA_MEM_CHK;
		static const long ID_PROG_CONFIG_MEM_CHK;
		static const long ID_USE_BULK_ERASE_CHK;
		static const long ID_DISCONNECT_AFTER_PROG_CHK;
		static const long ID_VERIFY_DIFF_VDDS_CHK;
		static const long ID_DONT_CARE_FOR_OSCCAL_CHK;
		static const long ID_DONT_CARE_FOR_BG_CALIB_CHK;
		static const long ID_CLR_BEFORE_LOAD_CHK;
		static const long ID_VDD_BEFORE_MCLR_CHK;
		static const long ID_STATICTEXT1;
		static const long ID_CHOICE1;
		static const long ID_CODE_MEM_COLOUR_BUTTON;
		static const long ID_DATA_MEM_COLOUR_BUTTON;
		static const long ID_STATICTEXT2;
		static const long ID_MPLAB_DEV_DIR_EDIT;
		static const long ID_MPLAB_DEV_DIR_BUTTON;
		static const long ID_CHECKBOX11;
		static const long ID_CHECKBOX12;
		static const long ID_START_TEST_BUTTON;
		static const long ID_TEST_CHOICE;
		//*)

	private:

		//(*Handlers(TOptionPanel)
		void onProgOptionChanged(wxCommandEvent& event);
		void onLanguageChoiceSelect(wxCommandEvent& event);
		void onCodeMemColourButtonClick(wxCommandEvent& event);
		void onDataMemColourButtonClick(wxCommandEvent& event);
		void onMPLabDevDirButtonClick(wxCommandEvent& event);
		void onDriverRadioSelect(wxCommandEvent& event);
		void onStartTestButtonClick(wxCommandEvent& event);
		void onMplabDevDirEditTextEnter(wxCommandEvent& event);
		//*)
        void onLooseFocus(wxFocusEvent& pEvent);

        bool InterfaceSpeedTest     (void);
        void updateMPLabDevDir      (const wxString &pDirPath);
        void updateFromMPLabDevText (void);

		DECLARE_EVENT_TABLE()
};

#endif
