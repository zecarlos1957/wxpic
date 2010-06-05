#ifndef INTERFACEPANEL_H
#define INTERFACEPANEL_H

//(*Headers(TInterfacePanel)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/radiobox.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
//*)

class TInterfacePanel: public wxPanel
{
	public:

		TInterfacePanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size, long style, const wxString& name);
		virtual ~TInterfacePanel();

		//(*Declarations(TInterfacePanel)
		wxBitmapButton* aRedLedBitmap;
		wxStaticText* StaticText22;
		wxCheckBox* aSlowInterfaceChk;
		wxStaticText* StaticText20;
		wxChoice* aInterfacePortChoice;
		wxRadioBox* aVddSelectRadio;
		wxStaticText* aInterfaceTypeText;
		wxTextCtrl* aIoPortAddressEdit;
		wxButton* aLptInterfHelpButton;
		wxCheckBox* aTestVppChk;
		wxCheckBox* aConnectICSPTargetChk;
		wxStaticText* aGreenLedText;
		wxChoice* aInterfaceTypeChoice;
		wxStaticText* StaticText19;
		wxTextCtrl* aExtraClkDelayEdit;
		wxStaticText* StaticText18;
		wxCheckBox* aDataEnableChk;
		wxCheckBox* aClockEnableChk;
		wxStaticText* aRedLedText;
		wxStaticText* StaticText23;
		wxCheckBox* aTestVddChk;
		wxButton* aCustomInterfSelectButton;
		wxBitmapButton* aGreenLedBitmap;
		wxCheckBox* aTestClockChk;
		wxCheckBox* aTestDataChk;
		wxStaticText* aOkButtonText;
		wxStaticText* StaticText15;
		wxButton* aInitInterfButton;
		wxTextCtrl* aExtraRdDelayEdit;
		wxStaticText* aTestDataInText;
		wxCheckBox* aPullMclrToGndChk;
		wxTextCtrl* aCustomDefFileText;
		//*)

        int                 aIntfItemIndex2InterfaceType[100];
        int                 m_displayed_interface_type;
        bool                m_fUseSerialPort;

        void UpdateInterfaceType(int interface_type);
        void UpdateInterfaceTestDisplay(void);
        void UpdateInterfaceInputSignalDisplay(void);
        void SetInterfaceInternal(int iInterfaceType);
        bool UnlockEditFieldForIOPortAddress(void);
        bool TestTheInterface(void);

	protected:

		//(*Identifiers(TInterfacePanel)
		static const long ID_BUTTON7;
		static const long ID_LPT_INTERF_HELP_BUTTON;
		static const long ID_CHECKBOX15;
		static const long ID_CHECKBOX16;
		static const long ID_CHECKBOX17;
		static const long ID_CHECKBOX18;
		static const long ID_STATICTEXT15;
		static const long ID_STATICTEXT24;
		static const long ID_CHECKBOX19;
		static const long ID_CHECKBOX20;
		static const long ID_CHECKBOX21;
		static const long ID_CHECKBOX22;
		static const long ID_RADIOBOX3;
		static const long ID_BITMAPBUTTON1;
		static const long ID_STATICTEXT16;
		static const long ID_BITMAPBUTTON2;
		static const long ID_STATICTEXT17;
		static const long ID_STATICTEXT25;
		static const long ID_INTERFACE_TYPE_CHOICE;
		static const long ID_STATICTEXT18;
		static const long ID_INTERFACE_PORT_CHOICE;
		static const long ID_STATICTEXT19;
		static const long ID_IO_PORT_ADDRESS_EDIT;
		static const long ID_STATICTEXT20;
		static const long ID_BUTTON10;
		static const long ID_TEXTCTRL10;
		static const long ID_INTERFACE_TYPE_TEXT;
		static const long ID_STATICTEXT22;
		static const long ID_TEXTCTRL11;
		static const long ID_STATICTEXT23;
		static const long ID_TEXTCTRL12;
		static const long ID_CHECKBOX23;
		//*)

	private:

		//(*Handlers(TInterfacePanel)
		void onInitInterfButtonClick(wxCommandEvent& event);
		void onLptInterfHelpButtonClick(wxCommandEvent& event);
		void onTestXXXChkClick(wxCommandEvent& event);
		void onTestVddChkClick(wxCommandEvent& event);
		void onClockEnableChkClick(wxCommandEvent& event);
		void onDataEnableChkClick(wxCommandEvent& event);
		void onPullMclrToGndChkClick(wxCommandEvent& event);
		void onConnectICSPTargetChkClick(wxCommandEvent& event);
		void onVddSelectRadioSelect(wxCommandEvent& event);
		void onRedLedBitmapClick(wxCommandEvent& event);
		void onGreenLedBitmapClick(wxCommandEvent& event);
		void onInterfaceTypeChoiceSelect(wxCommandEvent& event);
		void onInterfacePortChoiceSelect(wxCommandEvent& event);
		void onCustomInterfSelectButtonClick(wxCommandEvent& event);
		void onIoPortAddressEditTextEnter(wxCommandEvent& event);
		void onSlowInterfaceChkClick(wxCommandEvent& event);
		//*)
        void onIoPortAddressGetFocus(wxFocusEvent &pEvent);

		DECLARE_EVENT_TABLE()
};

#endif
