#include "InterfacePanel.h"
#include "MainFrame.h"
#include "Appl.h"
#include "Language.h"
#include <../resources/Resource.h>

//(*InternalHeaders(TInterfacePanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/settings.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>

#include <WinPicPr/Config.h>
#include <WinPicPr/PIC_HW.h>

//(*IdInit(TInterfacePanel)
const long TInterfacePanel::ID_BUTTON7 = wxNewId();
const long TInterfacePanel::ID_LPT_INTERF_HELP_BUTTON = wxNewId();
const long TInterfacePanel::ID_CHECKBOX15 = wxNewId();
const long TInterfacePanel::ID_CHECKBOX16 = wxNewId();
const long TInterfacePanel::ID_CHECKBOX17 = wxNewId();
const long TInterfacePanel::ID_CHECKBOX18 = wxNewId();
const long TInterfacePanel::ID_STATICTEXT15 = wxNewId();
const long TInterfacePanel::ID_STATICTEXT24 = wxNewId();
const long TInterfacePanel::ID_CHECKBOX19 = wxNewId();
const long TInterfacePanel::ID_CHECKBOX20 = wxNewId();
const long TInterfacePanel::ID_CHECKBOX21 = wxNewId();
const long TInterfacePanel::ID_CHECKBOX22 = wxNewId();
const long TInterfacePanel::ID_RADIOBOX3 = wxNewId();
const long TInterfacePanel::ID_BITMAPBUTTON1 = wxNewId();
const long TInterfacePanel::ID_STATICTEXT16 = wxNewId();
const long TInterfacePanel::ID_BITMAPBUTTON2 = wxNewId();
const long TInterfacePanel::ID_STATICTEXT17 = wxNewId();
const long TInterfacePanel::ID_STATICTEXT25 = wxNewId();
const long TInterfacePanel::ID_INTERFACE_TYPE_CHOICE = wxNewId();
const long TInterfacePanel::ID_STATICTEXT18 = wxNewId();
const long TInterfacePanel::ID_INTERFACE_PORT_CHOICE = wxNewId();
const long TInterfacePanel::ID_STATICTEXT19 = wxNewId();
const long TInterfacePanel::ID_IO_PORT_ADDRESS_EDIT = wxNewId();
const long TInterfacePanel::ID_STATICTEXT20 = wxNewId();
const long TInterfacePanel::ID_BUTTON10 = wxNewId();
const long TInterfacePanel::ID_TEXTCTRL10 = wxNewId();
const long TInterfacePanel::ID_INTERFACE_TYPE_TEXT = wxNewId();
const long TInterfacePanel::ID_STATICTEXT22 = wxNewId();
const long TInterfacePanel::ID_TEXTCTRL11 = wxNewId();
const long TInterfacePanel::ID_STATICTEXT23 = wxNewId();
const long TInterfacePanel::ID_TEXTCTRL12 = wxNewId();
const long TInterfacePanel::ID_CHECKBOX23 = wxNewId();
//*)

BEGIN_EVENT_TABLE(TInterfacePanel,wxPanel)
	//(*EventTable(TInterfacePanel)
	//*)
END_EVENT_TABLE()

TInterfacePanel::TInterfacePanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size, long style, const wxString& name)
{
	//(*Initialize(TInterfacePanel)
	wxFlexGridSizer* FlexGridSizer4;
	wxBoxSizer* BoxSizer19;
	wxBoxSizer* BoxSizer15;
	wxBoxSizer* BoxSizer20;
	wxBoxSizer* BoxSizer21;
	wxBoxSizer* BoxSizer23;
	wxBoxSizer* BoxSizer16;
	wxBoxSizer* BoxSizer18;
	wxBoxSizer* BoxSizer14;
	wxStaticBoxSizer* StaticBoxSizer6;
	wxBoxSizer* BoxSizer17;
	wxBoxSizer* BoxSizer26;
	wxBoxSizer* BoxSizer22;
	wxStaticBoxSizer* StaticBoxSizer5;

	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	BoxSizer14 = new wxBoxSizer(wxVERTICAL);
	BoxSizer16 = new wxBoxSizer(wxVERTICAL);
	BoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
	aInitInterfButton = new wxButton(this, ID_BUTTON7, _("Initialize!"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON7"));
	BoxSizer15->Add(aInitInterfButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	aLptInterfHelpButton = new wxButton(this, ID_LPT_INTERF_HELP_BUTTON, _("Help on LPT interfaces"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_LPT_INTERF_HELP_BUTTON"));
	BoxSizer15->Add(aLptInterfHelpButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	BoxSizer16->Add(BoxSizer15, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	BoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer5 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Interface Test"));
	BoxSizer18 = new wxBoxSizer(wxVERTICAL);
	aTestVppChk = new wxCheckBox(this, ID_CHECKBOX15, _("Vpp (+13V)"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX15"));
	aTestVppChk->SetValue(false);
	BoxSizer18->Add(aTestVppChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aTestVddChk = new wxCheckBox(this, ID_CHECKBOX16, _("Vdd on/off"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX16"));
	aTestVddChk->SetValue(false);
	BoxSizer18->Add(aTestVddChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aTestClockChk = new wxCheckBox(this, ID_CHECKBOX17, _("Clock"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX17"));
	aTestClockChk->SetValue(false);
	BoxSizer18->Add(aTestClockChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aTestDataChk = new wxCheckBox(this, ID_CHECKBOX18, _("Data to PIC"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX18"));
	aTestDataChk->SetValue(false);
	BoxSizer18->Add(aTestDataChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	BoxSizer26 = new wxBoxSizer(wxHORIZONTAL);
	StaticText15 = new wxStaticText(this, ID_STATICTEXT15, _("Data In ="), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT15"));
	BoxSizer26->Add(StaticText15, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 3);
	aTestDataInText = new wxStaticText(this, ID_STATICTEXT24, _("---"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxALIGN_LEFT, _T("ID_STATICTEXT24"));
	BoxSizer26->Add(aTestDataInText, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	BoxSizer18->Add(BoxSizer26, 1, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 0);
	aClockEnableChk = new wxCheckBox(this, ID_CHECKBOX19, _("Clock Enable"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX19"));
	aClockEnableChk->SetValue(false);
	BoxSizer18->Add(aClockEnableChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aDataEnableChk = new wxCheckBox(this, ID_CHECKBOX20, _("Data Enable"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX20"));
	aDataEnableChk->SetValue(false);
	BoxSizer18->Add(aDataEnableChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aPullMclrToGndChk = new wxCheckBox(this, ID_CHECKBOX21, _("Pull MCLR low"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX21"));
	aPullMclrToGndChk->SetValue(false);
	BoxSizer18->Add(aPullMclrToGndChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aConnectICSPTargetChk = new wxCheckBox(this, ID_CHECKBOX22, _("Connect target"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX22"));
	aConnectICSPTargetChk->SetValue(false);
	BoxSizer18->Add(aConnectICSPTargetChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	StaticBoxSizer5->Add(BoxSizer18, 1, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
	BoxSizer19 = new wxBoxSizer(wxVERTICAL);
	wxString __wxRadioBoxChoices_1[3] =
	{
		_("low"),
		_("norm (5V)"),
		_("high")
	};
	aVddSelectRadio = new wxRadioBox(this, ID_RADIOBOX3, _("Vdd Select"), wxDefaultPosition, wxDefaultSize, 3, __wxRadioBoxChoices_1, 1, wxRA_HORIZONTAL, wxDefaultValidator, _T("ID_RADIOBOX3"));
	BoxSizer19->Add(aVddSelectRadio, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer4 = new wxFlexGridSizer(2, 2, 0, 0);
	aRedLedBitmap = new wxBitmapButton(this, ID_BITMAPBUTTON1, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
	aRedLedBitmap->SetBitmapLabel(*TResource::GetLedRedIcon());
	FlexGridSizer4->Add(aRedLedBitmap, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aRedLedText = new wxStaticText(this, ID_STATICTEXT16, _("Red Led"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT16"));
	FlexGridSizer4->Add(aRedLedText, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aGreenLedBitmap = new wxBitmapButton(this, ID_BITMAPBUTTON2, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
	aGreenLedBitmap->SetBitmapLabel(*TResource::GetLedGreenIcon());
	FlexGridSizer4->Add(aGreenLedBitmap, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aGreenLedText = new wxStaticText(this, ID_STATICTEXT17, _("Green Led"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT17"));
	FlexGridSizer4->Add(aGreenLedText, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	BoxSizer19->Add(FlexGridSizer4, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	aOkButtonText = new wxStaticText(this, ID_STATICTEXT25, _("---"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, _T("ID_STATICTEXT25"));
	BoxSizer19->Add(aOkButtonText, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	StaticBoxSizer5->Add(BoxSizer19, 1, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
	BoxSizer17->Add(StaticBoxSizer5, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	StaticBoxSizer6 = new wxStaticBoxSizer(wxVERTICAL, this, _("Interface Type"));
	aInterfaceTypeChoice = new wxChoice(this, ID_INTERFACE_TYPE_CHOICE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_INTERFACE_TYPE_CHOICE"));
	StaticBoxSizer6->Add(aInterfaceTypeChoice, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	BoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
	StaticText18 = new wxStaticText(this, ID_STATICTEXT18, _("on port"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT18"));
	BoxSizer20->Add(StaticText18, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aInterfacePortChoice = new wxChoice(this, ID_INTERFACE_PORT_CHOICE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_INTERFACE_PORT_CHOICE"));
	BoxSizer20->Add(aInterfacePortChoice, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	BoxSizer20->Add(5,-1,0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	StaticText19 = new wxStaticText(this, ID_STATICTEXT19, _("Address:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT19"));
	BoxSizer20->Add(StaticText19, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aIoPortAddressEdit = new wxTextCtrl(this, ID_IO_PORT_ADDRESS_EDIT, _("03F8"), wxDefaultPosition, wxSize(40,-1), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_IO_PORT_ADDRESS_EDIT"));
	aIoPortAddressEdit->Disable();
	BoxSizer20->Add(aIoPortAddressEdit, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer6->Add(BoxSizer20, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer21 = new wxBoxSizer(wxHORIZONTAL);
	StaticText20 = new wxStaticText(this, ID_STATICTEXT20, _("Custom interface definition file"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT20"));
	BoxSizer21->Add(StaticText20, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aCustomInterfSelectButton = new wxButton(this, ID_BUTTON10, _("Select"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON10"));
	BoxSizer21->Add(aCustomInterfSelectButton, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer6->Add(BoxSizer21, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	aCustomDefFileText = new wxTextCtrl(this, ID_TEXTCTRL10, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL10"));
	StaticBoxSizer6->Add(aCustomDefFileText, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	aInterfaceTypeText = new wxStaticText(this, ID_INTERFACE_TYPE_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSIMPLE_BORDER, _T("ID_INTERFACE_TYPE_TEXT"));
	StaticBoxSizer6->Add(aInterfaceTypeText, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	BoxSizer22 = new wxBoxSizer(wxHORIZONTAL);
	StaticText22 = new wxStaticText(this, ID_STATICTEXT22, _("Extra delay before reading data line [us]"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT22"));
	BoxSizer22->Add(StaticText22, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aExtraRdDelayEdit = new wxTextCtrl(this, ID_TEXTCTRL11, wxEmptyString, wxDefaultPosition, wxSize(40,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL11"));
	BoxSizer22->Add(aExtraRdDelayEdit, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer6->Add(BoxSizer22, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer23 = new wxBoxSizer(wxHORIZONTAL);
	StaticText23 = new wxStaticText(this, ID_STATICTEXT23, _("Extra lengthening for clock pulses [us]"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT23"));
	BoxSizer23->Add(StaticText23, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aExtraClkDelayEdit = new wxTextCtrl(this, ID_TEXTCTRL12, wxEmptyString, wxDefaultPosition, wxSize(40,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL12"));
	BoxSizer23->Add(aExtraClkDelayEdit, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer6->Add(BoxSizer23, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	aSlowInterfaceChk = new wxCheckBox(this, ID_CHECKBOX23, _("slow mode (to check for timing problems)"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX23"));
	aSlowInterfaceChk->SetValue(false);
	StaticBoxSizer6->Add(aSlowInterfaceChk, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	BoxSizer17->Add(StaticBoxSizer6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	BoxSizer16->Add(BoxSizer17, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
	BoxSizer14->Add(BoxSizer16, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
	SetSizer(BoxSizer14);
	BoxSizer14->Fit(this);
	BoxSizer14->SetSizeHints(this);

	Connect(ID_BUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TInterfacePanel::onInitInterfButtonClick);
	Connect(ID_LPT_INTERF_HELP_BUTTON,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TInterfacePanel::onLptInterfHelpButtonClick);
	Connect(ID_CHECKBOX15,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TInterfacePanel::onTestXXXChkClick);
	Connect(ID_CHECKBOX16,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TInterfacePanel::onTestVddChkClick);
	Connect(ID_CHECKBOX17,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TInterfacePanel::onTestXXXChkClick);
	Connect(ID_CHECKBOX18,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TInterfacePanel::onTestXXXChkClick);
	Connect(ID_CHECKBOX19,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TInterfacePanel::onClockEnableChkClick);
	Connect(ID_CHECKBOX20,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TInterfacePanel::onDataEnableChkClick);
	Connect(ID_CHECKBOX21,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TInterfacePanel::onPullMclrToGndChkClick);
	Connect(ID_CHECKBOX22,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TInterfacePanel::onConnectICSPTargetChkClick);
	Connect(ID_RADIOBOX3,wxEVT_COMMAND_RADIOBOX_SELECTED,(wxObjectEventFunction)&TInterfacePanel::onVddSelectRadioSelect);
	Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TInterfacePanel::onRedLedBitmapClick);
	Connect(ID_BITMAPBUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TInterfacePanel::onGreenLedBitmapClick);
	Connect(ID_INTERFACE_TYPE_CHOICE,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&TInterfacePanel::onInterfaceTypeChoiceSelect);
	Connect(ID_INTERFACE_PORT_CHOICE,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&TInterfacePanel::onInterfacePortChoiceSelect);
	Connect(ID_IO_PORT_ADDRESS_EDIT,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&TInterfacePanel::onIoPortAddressEditTextEnter);
	Connect(ID_BUTTON10,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TInterfacePanel::onCustomInterfSelectButtonClick);
	Connect(ID_TEXTCTRL11,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&TInterfacePanel::onSlowInterfaceChkClick);
	Connect(ID_TEXTCTRL12,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&TInterfacePanel::onSlowInterfaceChkClick);
	Connect(ID_CHECKBOX23,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TInterfacePanel::onSlowInterfaceChkClick);
	//*)
    aIoPortAddressEdit->Connect(wxID_ANY, wxEVT_SET_FOCUS, (wxObjectEventFunction)&TInterfacePanel::onIoPortAddressGetFocus, NULL, GetEventHandler());
    m_fUseSerialPort          = false;
    m_displayed_interface_type= -1;
}

TInterfacePanel::~TInterfacePanel()
{
	//(*Destroy(TInterfacePanel)
	//*)
}


//---------------------------------------------------------------------------
void TInterfacePanel::UpdateInterfaceType(int interface_type)
{
    int i;
    wxChar str80[81];
    ++(MainFrame::TheMainFrame->m_Updating);

    // Set the Items in the interface selection combo (TComboBox.Items = TStrings)
    // Note: This is only the bloody user interface !
    aInterfaceTypeChoice->Clear();
    aInterfaceTypeChoice->Append( _("(unknown)") );
    aIntfItemIndex2InterfaceType[0]=0;
    aInterfaceTypeChoice->Append( _("COM84 programmer for serial port") );
    aIntfItemIndex2InterfaceType[1]=PIC_INTF_TYPE_COM84;
    aInterfaceTypeChoice->Append( _("JDM (2) for serial port") );
    aIntfItemIndex2InterfaceType[2]=PIC_INTF_TYPE_JDM2;
    aInterfaceTypeChoice->Append( _("PIP84 by SM6LKM, Data->PAPER (Pin12)") );
    aIntfItemIndex2InterfaceType[3]= PIC_INTF_TYPE_PIP84_V1;
    aInterfaceTypeChoice->Append( _("PIP84 by SM6LKM, Data->ACK (Pin10)") );
    aIntfItemIndex2InterfaceType[4]= PIC_INTF_TYPE_PIP84_V2;
    aInterfaceTypeChoice->Append( _("PIC Flash prog by SM6LKM (2002-10)" ));
    aIntfItemIndex2InterfaceType[5]= PIC_INTF_TYPE_LKM_FLASHPR_V1;
    aInterfaceTypeChoice->Append( _("Tait, 7406(inverter)+4066(switch)" ));
    aIntfItemIndex2InterfaceType[6]= PIC_INTF_TYPE_TAIT_7406_4066;
    aInterfaceTypeChoice->Append( _("Tait, 7407(driver) + 4066(switch)") );
    aIntfItemIndex2InterfaceType[7]= PIC_INTF_TYPE_TAIT_7407_4066;
    aInterfaceTypeChoice->Append( _("Tait, 7406(inverter)+PNP transistor") );
    aIntfItemIndex2InterfaceType[8]= PIC_INTF_TYPE_TAIT_7406_PNP;
    aInterfaceTypeChoice->Append( _("Tait, 7407(driver) + PNP transistor") );
    aIntfItemIndex2InterfaceType[9]= PIC_INTF_TYPE_TAIT_7407_PNP;
    aInterfaceTypeChoice->Append( _("Microchip AN589") );
    aIntfItemIndex2InterfaceType[10]= PIC_INTF_TYPE_LPT_AN589;
    aInterfaceTypeChoice->Append( _("NOPPP") );
    aIntfItemIndex2InterfaceType[11]= PIC_INTF_TYPE_LPT_NOPPP;
    aInterfaceTypeChoice->Append( _("Custom, on LPT port, defined by FILE") );
    aIntfItemIndex2InterfaceType[12]= PIC_INTF_TYPE_CUSTOM_LPT;
    aInterfaceTypeChoice->Append( _("Custom, on COM port, defined by FILE") );
    aIntfItemIndex2InterfaceType[13]= PIC_INTF_TYPE_CUSTOM_COM;
//    aInterfaceTypeChoice->Append( _("Custom, on any port, from plugin-DLL") );
//    aIntfItemIndex2InterfaceType[13]= PIC_INTF_TYPE_PLUGIN_DLL;

    int Index;
    switch (interface_type) // a PIC_INTF_TYPE_xxx - constant
    {
    case PIC_INTF_TYPE_COM84:     // serial, clock=RTS, data=DTR+CTS, Vpp=TXD
        Index = 1;
        m_fUseSerialPort = true;
        break;
    case PIC_INTF_TYPE_JDM2:      // serial, clock=RTS, data=DTR+CTS, Vpp=TXD,
        // plus some precautions to charge / discharge two capacitors
        Index = 2;
        m_fUseSerialPort = true;
        break;
    case PIC_INTF_TYPE_PIP84_V1:         // parallel, used by SM6LKM, RB7->PAPER
        Index = 3;
        m_fUseSerialPort = false;
        break;
    case PIC_INTF_TYPE_PIP84_V2:         // parallel, used by SM6LKM, RB7->ACKN
        Index = 4;
        m_fUseSerialPort = false;
        break;
    case PIC_INTF_TYPE_LKM_FLASHPR_V1:
        Index = 5;
        m_fUseSerialPort = false;
        break;
    case PIC_INTF_TYPE_TAIT_7406_4066:   // parallel, by David Tait, 1st way
        Index = 6;
        m_fUseSerialPort = false;
        break;
    case PIC_INTF_TYPE_TAIT_7407_4066:   // parallel, by David Tait, 2nd way
        Index = 7;
        m_fUseSerialPort = false;
        break;
    case PIC_INTF_TYPE_TAIT_7406_PNP :   // parallel, by David Tait, 3rd way
        Index = 8;
        m_fUseSerialPort = false;
        break;
    case PIC_INTF_TYPE_TAIT_7407_PNP :   // parallel, by David Tait, 4th way
        Index = 9;
        m_fUseSerialPort = false;
        break;
    case PIC_INTF_TYPE_LPT_AN589:  // parallel, Microchip's Application Note "AN589"
        Index = 10;
        m_fUseSerialPort = false;
        break;
    case PIC_INTF_TYPE_LPT_NOPPP:  // parallel, NOPPP
        Index = 11;
        m_fUseSerialPort = false;
        break;
    case PIC_INTF_TYPE_CUSTOM_LPT:  // "custom" interface on LPT, I/O lines defined by file
        Index = 12;
        m_fUseSerialPort = false;
        break;
    case PIC_INTF_TYPE_CUSTOM_COM:  // "custom" interface on COM, I/O lines defined by file
        Index = 13;
        m_fUseSerialPort = true;
        break;
//    case PIC_INTF_TYPE_PLUGIN_DLL:  // "custom" interface on any port, using a plugin-DLL
//        Index = 13;
//        m_fUseSerialPort = false;
//        break;

    default:
        m_fUseSerialPort = false;
        Index = 0;
        aInterfaceTypeText->SetLabel(_("Illegal interface code number"));
        break;
    } // end switch(m_displayed_interface_type)
    aInterfaceTypeChoice->SetSelection(Index);

    aCustomDefFileText->SetValue(Config.sz255InterfaceSupportFile);
    if (  (interface_type==PIC_INTF_TYPE_CUSTOM_LPT)
            ||(interface_type==PIC_INTF_TYPE_CUSTOM_COM) )
    {
        aCustomDefFileText->Enable();
//     aCustomDefFileText->Color = clWindow;
    }
//    else if ( interface_type==PIC_INTF_TYPE_PLUGIN_DLL )
//    {
//        aCustomDefFileText->SetValue(Config.sz80InterfacePluginDLL);
//        aCustomDefFileText->Enable();
////     aCustomDefFileText->Color = clWindow;
//    }
    else
    {
        aCustomDefFileText->Enable(false);
//     aCustomDefFileText->Color = clBtnFace;
    }

//  Combo_InterfacePort->Color = clWindow;
//    MainFrame::TheMainFrame->aOptionTab->aDriverRadio->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
//  Rb_UseWinAPIOnly->Color = clBtnFace;
    if (m_fUseSerialPort)
    {
        aInterfacePortChoice->Enable();
        aInterfacePortChoice->Clear();
        aInterfacePortChoice->Append(_("(unknown)"));
        for (i=1;i<=16;++i)
        {
            aInterfacePortChoice->Append(wxString::Format(_T("COM%d"),i));
        }
        if (Config.iComPortNr>=0 && Config.iComPortNr<=16)
            aInterfacePortChoice->SetSelection(Config.iComPortNr);
        else
            aInterfacePortChoice->SetSelection(0);
        aIoPortAddressEdit->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
        if ( Config.iComIoAddress != 0 )
        {
            // use a "non-standard" I/O-address for the COM-port, entered manually:
            _stprintf(str80,_T("%04X"),(int)Config.iComIoAddress);
            aIoPortAddressEdit->Disable();
//        Ed_IoPortAddress->ReadOnly = false;
//        Ed_IoPortAddress->Color = clWindow;
        }
        else
        {
            // use a "standard" COM-port with the default I/O address :
            switch ( Config.iComPortNr )
            {
            case 1:
                _tcscpy(str80, _T("03F8"));
                break;
            case 2:
                _tcscpy(str80, _T("02F8"));
                break;
            case 3:
                _tcscpy(str80, _T("03E8"));
                break;
            case 4:
                _tcscpy(str80, _T("02E8"));
                break;
            default:
                _tcscpy(str80, _("-std-"));
                break;
            }
            aIoPortAddressEdit->Enable();
//        Ed_IoPortAddress->ReadOnly = true;
//        Ed_IoPortAddress->Color = clBtnFace;
        }
        aIoPortAddressEdit->SetValue(str80);
//    Ed_IoPortAddress->Text = str80;

//        if ( Config.iComPortNr>4 && (MainFrame::TheMainFrame->aOptionTab->aDriverRadio->GetSelection() != TOptionPanel::radioDriverUSE_WIN_API_ONLY) )
//        {
//            APPL_ShowMsg( APPL_CALLER_PIC_PRG, 0, _("COM ports above 4 can be accessed via windows API only (on Options tab) !") );
//            MainFrame::TheMainFrame->m_iMessagePanelUsage = MainFrame::MP_USAGE_ERROR;
//            aInterfacePortChoice->SetForegroundColour(*wxRED);
//            MainFrame::TheMainFrame->aOptionTab->aDriverRadio->SetForegroundColour(*wxRED);
//        }
    }
    else  // not serial but parallel port ?
//        if ( interface_type != PIC_INTF_TYPE_PLUGIN_DLL )
        {
            aInterfacePortChoice->Enable();
            aInterfacePortChoice->Clear();
            aInterfacePortChoice->Append(_("(unknown)") );
            aInterfacePortChoice->Append(_T("LPT1"));
            aInterfacePortChoice->Append(_T("LPT2"));
            aInterfacePortChoice->SetSelection(Config.iLptPortNr);

            if (Config.iLptIoAddress == 0)
                _tcscpy(str80, _("-std-"));
            else
                _stprintf(str80,_T("%04X"),Config.iLptIoAddress);
            aIoPortAddressEdit->SetValue(str80);
//    Ed_IoPortAddress->SetValue(str80);
            aIoPortAddressEdit->Enable();
//    Ed_IoPortAddress->Color = clWindow;

            // To drive an interface on the LPT port, WinPic must use
            // SMPORT or PORTTALK. So the option "use windows API only"
            // doesn't work here. Inform the user about this problem :
//            if (MainFrame::TheMainFrame->aOptionTab->aDriverRadio->GetSelection() == TOptionPanel::radioDriverUSE_WIN_API_ONLY)
//            {
//                APPL_ShowMsg( APPL_CALLER_PIC_PRG, 0, _( "LPT port interface requires SMPORT or PortTalk (on Options tab) !") );
//                MainFrame::TheMainFrame->m_iMessagePanelUsage = MainFrame::MP_USAGE_ERROR;
//                aInterfacePortChoice->SetForegroundColour(*wxRED);
////       Combo_InterfacePort->Color = clRed;
//            }
        } // end if < interface on LPT port >
//        else
//        {
//            // neither SERIAL nor PARALLEL port (but a hardware-access driver in a DLL)
//            aInterfacePortChoice->Clear();
//            aInterfacePortChoice->Disable();
////    Combo_InterfacePort->Items->Clear();
////    Combo_InterfacePort->Enabled = false;
//            aIoPortAddressEdit->Clear();
//            aIoPortAddressEdit->Disable();
////    Ed_IoPortAddress->Text = "";
////    Ed_IoPortAddress->ReadOnly = true;
////    Ed_IoPortAddress->Color = clBtnFace;
//        } // end else < neither serial nor parallel port >
    aExtraRdDelayEdit ->SetValue(wxString::Format(_T("%d"), Config.iExtraRdDelay_us ));
    aExtraClkDelayEdit->SetValue(wxString::Format(_T("%d"), Config.iExtraClkDelay_us));
//  Ed_ExtraRdDelay->Text = IntToStr(Config.iExtraRdDelay_us);
//  Ed_ExtraClkDelay->Text= IntToStr(Config.iExtraClkDelay_us);
    aSlowInterfaceChk->SetValue(Config.iSlowInterface);
//  Chk_SlowInterface->Checked = Config.iSlowInterface;


    UpdateInterfaceTestDisplay();

    if (MainFrame::TheMainFrame->m_Updating>0)
        --(MainFrame::TheMainFrame->m_Updating);

} // end UpdateInterfaceType()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TInterfacePanel::UpdateInterfaceTestDisplay(void)
{
    int i;
    ++(MainFrame::TheMainFrame->m_Updating);
    i = PicHw_IsVppOn();
    aTestVppChk->Enable      (i>=0);
    aTestVppChk->SetValue    (i==1);
    i = PicHw_IsVddOn();
    aTestVddChk->Enable      (i>=0);
    aTestVddChk->SetValue    (i==1);
    i = PicHw_IsClockHigh();
    aTestClockChk->Enable    (i>=0);
    aTestClockChk->SetValue  (i==1);
    i = PicHw_IsDataOutHigh();
    aTestDataChk->Enable     (i>=0);
    aTestDataChk->SetValue   (i==1);
    i = PicHw_IsClockEnabled();
    aClockEnableChk->Enable  (i>=0);
    aClockEnableChk->SetValue(i==1);
    i = PicHw_IsDataEnabled();
    aDataEnableChk->Enable   (i>=0);
    aDataEnableChk->SetValue (i==1);
    i = PicHw_IsMclrPulledToGnd();
    aPullMclrToGndChk->Enable  (i>=0);
    aPullMclrToGndChk->SetValue(i==1);
    i = PicHw_IsTargetConnected();
    aConnectICSPTargetChk->Enable  (i>=0);
    aConnectICSPTargetChk->SetValue(i==1);


//    if (PicHw_FuncPtr)
    if (PIC_HW_CanSelectVdd())
    {
        // Only if the currently used "hardware I/O functions" are valid,
        // (and only if the installed hardware is a "production grade" programmer):
        aVddSelectRadio->SetSelection(PicHw_iPresentVddSelection /*PHWInfo.iPresentVddSelection*/);
        aVddSelectRadio->Enable(PicHw_FuncPtr->SelectVddLow.pFunc[0] != NULL);
//     RB_VddLow->Enabled = (PicHw_FuncPtr->SelectVddLow.pFunc[0] != NULL);
//     RB_VddLow->Checked = (PHWInfo.iPresentVddSelection == 0);
//     RB_VddNorm->Enabled = (PicHw_FuncPtr->SelectVddNorm.pFunc[0] != NULL);
//     RB_VddNorm->Checked = (PHWInfo.iPresentVddSelection == 1);
//     RB_VddHigh->Enabled = (PicHw_FuncPtr->SelectVddHigh.pFunc[0] != NULL);
//     RB_VddHigh->Checked = (PHWInfo.iPresentVddSelection == 2);
    }
    else
    {
        aVddSelectRadio->SetSelection(1);
        aVddSelectRadio->Disable();
    }

    UpdateInterfaceInputSignalDisplay();  // "data" input, "OK"-button, and indicator LEDs

    if (MainFrame::TheMainFrame->m_Updating>0)
        --(MainFrame::TheMainFrame->m_Updating);
}
//---------------------------------------------------------------------------


void TInterfacePanel::UpdateInterfaceInputSignalDisplay(void)
{
    int i;
    static int LastValueDataIn = -2;
    ++(MainFrame::TheMainFrame->m_Updating);

    i = PIC_HW_GetDataBit();
    if (i != LastValueDataIn)
    {
        aTestDataInText->SetLabel((i>=0)?wxString::Format(_T("%d"),i):wxString(_("n/a")));
        LastValueDataIn = i;
    }
//  if(i>=0) aTestDataInText->Caption = IntToStr( i );
//      else aTestDataInText->Caption = _("n/a");

    static int LastValueRed = -2;
    i = PIC_HW_SetRedLed( -1/*don't change, just get state*/ );
    if (i != LastValueRed)
    {
        aRedLedBitmap->SetBitmapLabel(*((i>0) ? TResource::GetLedRedIcon() : TResource::GetLedOffIcon()));
        aRedLedText->Enable(i>=0);
        LastValueRed = i;
    }
//  if(i>=0)
//    {
//      aRedLEDText->Enabled = true;
//      Shp_RedLED->Brush->Color = (i==0)?clGray:clRed;
//    }
//   else
//    { Lab_RedLED->Enabled = false;
//      Shp_RedLED->Brush->Color = clLtGray;
//    }

    static int LastValueGreen = -2;
    i = PIC_HW_SetGreenLed( -1/*don't change, just get state*/ );
    if (i != LastValueGreen)
    {
        aGreenLedBitmap->SetBitmapLabel(*((i>0) ? TResource::GetLedGreenIcon() : TResource::GetLedOffIcon()));
        aGreenLedText->Enable(i>=0);
        LastValueGreen = i;
    }
//  if(i>=0)
//    { Lab_GreenLED->Enabled = true;
//      Shp_GreenLED->Brush->Color = (i==0)?clGray:clLime;
//    }
//   else
//    { Lab_GreenLED->Enabled = false;
//      Shp_GreenLED->Brush->Color = clLtGray;
//    }

    static int LastValueButton = -2;
    i = PicHw_GetOkButtonState();
    if (i != LastValueButton)
    {
        if (i>=0)
        {
            aOkButtonText->Enable(true);
            if (i==0) aOkButtonText->SetLabel(_("button up"));
            else  aOkButtonText->SetLabel(_("button DOWN"));
        }
        else
        {
            aOkButtonText->Enable(false);
            aOkButtonText->SetLabel(_("(no button)"));
        }
        LastValueButton = i;
    }

    if (MainFrame::TheMainFrame->m_Updating>0)
        --(MainFrame::TheMainFrame->m_Updating);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TInterfacePanel::SetInterfaceInternal(int iInterfaceType)
{
    aInterfaceTypeText->SetLabel((PIC_HW_SetInterfaceType(iInterfaceType))
                                 ? wxString(_("SetInterfaceType ok."))
                                 : _("Error in SetIntf() : ") + wxString(PicHw_sz255LastError));
//  if(PIC_HW_SetInterfaceType( iInterfaceType ) )
//         Txt_InterfaceType->Caption = TE( "SetInterfaceType ok." );
//    else Txt_InterfaceType->Caption = TE( "Error in SetIntf() : " )
//                     + wxString(PicHw_sz255LastError);
    Config.pic_interface_type = PIC_HW_interface.type; // after checking..
    UpdateInterfaceType(iInterfaceType);
    MainFrame::TheMainFrame->aOptionTab->UpdateOptionsDisplay();
} // end SetInterfaceInternal()



static bool WinPic_fAcceptAllIoAddresses = false;

//---------------------------------------------------------------------------
bool TInterfacePanel::UnlockEditFieldForIOPortAddress(void)
{
    // Inform the user about the potentially disastrous effects
    // of entering the wrong I/O-address in the "I/O-port address" field !
    if ((wxMessageBox(
                _( "Caution: Entering the wrong I/O-address here CAN KILL YOUR PC !\nDo you really want to continue ?" ) ,
                _( "WxPic WARNING" ),
                wxICON_EXCLAMATION | wxYES_NO | wxCANCEL ) != wxYES)
    || (wxMessageBox(
                _( "Ok, but don't say we didn't warn you !" ) ,
                _( "WxPic WARNING" ),
                wxICON_EXCLAMATION | wxOK | wxCANCEL ) != wxOK ))
    {
        Config.iComIoAddress = 0;    // disable this dangerous input field again
        WinPic_fAcceptAllIoAddresses = false;
        UpdateInterfaceType(m_displayed_interface_type);
        aInterfacePortChoice->SetFocus();
        return false;
    }
    WinPic_fAcceptAllIoAddresses = true;
    aIoPortAddressEdit->Enable();
//  Ed_IoPortAddress->ReadOnly = false;
//  Ed_IoPortAddress->Color = clWindow;
    if (aIoPortAddressEdit->GetValue() == _("-std-"))
        aIoPortAddressEdit->ChangeValue(wxEmptyString);
//  if( Ed_IoPortAddress->Text == "-std-" )
//   {  Ed_IoPortAddress->Text = "";
//   }
    return true;

} // end UnlockEditFieldForIOPortAddress()

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
bool TInterfacePanel::TestTheInterface(void)
{
    int n_errors = 0;
    bool fResult;

    PIC_HW_ConnectToTarget();  // here since 2005-03-29
    PIC_HW_SetDataEnable( true ); // rarely used, but may be necessary

    // A little test to find out if an interface MAY be present:
    if (!PIC_HW_SetVpp(false) )
    {
        MainFrame::AddTextToLog(_("Could not switch off Vpp !"));
        ++n_errors;
    }

    // Only for PICs where an extra Vdd switch exists, which does NOT
    // turn "Vpp" on....  turn Vdd on, otherwise 'read back' may not work,
    // for example in the PIP84 programmer:
    if ( PicHw_IsVddOn() >= 0) // '-1' if no extra Vdd switch exists...
        PIC_HW_SetVdd(true);

    UpdateInterfaceTestDisplay();

    if (!PIC_HW_SetClockAndData( false/*clock_high*/, false/*data_high*/ ) )
    {
        MainFrame::AddTextToLog( _("Could not switch Clock or Data !") );
        ++n_errors;
    }
    UpdateInterfaceTestDisplay();

    PIC_HW_Delay_us(500);
    if ( PIC_HW_GetDataBit() != 0 )
    {
        MainFrame::AddTextToLog(_("Failed read back of DATA line (LOW) !") );
        ++n_errors;
    }

    if (!PIC_HW_SetClockAndData( false/*clock_high*/, true/*data_high*/ ) )
    {
        MainFrame::AddTextToLog( _("Could not switch Clock or Data !") );
        ++n_errors;
    }

    UpdateInterfaceTestDisplay();

    PIC_HW_Delay_us(20000);  // 10uF-cap may be discharged !
    PIC_HW_Delay_us(20000);  // 10uF-cap may be discharged !
    if ( PIC_HW_GetDataBit() != 1 )
    {
        MainFrame::AddTextToLog( _("Failed read back of DATA line (HIGH) !") );
        ++n_errors;
    }


    PIC_HW_SetClockAndData( false/*clock_high*/, false/*data_high*/ );
    if ( n_errors == 0 )
    {
        APPL_ShowMsg(APPL_CALLER_MAIN,0, _("Initialising PIC-Programmer: Success.") );
        aInterfaceTypeText->SetLabel(_("Interface tested, OK."));
        fResult = true;
    }
    else
    {
        APPL_ShowMsg(APPL_CALLER_MAIN,0, _("WARNING: Could not initialize programmer !") );
        aInterfaceTypeText->SetLabel(_("Error testing the Interface") );
        fResult = false;
    }

    UpdateInterfaceTestDisplay();


    if (PIC_HW_interface.type==PIC_INTF_TYPE_JDM2)
    {
        // for the JDM programmer, things are very different.
        // Here, TXD(Vpp), DTR(DataOut), and RTS(Clock) must be set HIGH
        //       to turn the PIC's supply voltage off :
        PIC_HW_SetClockAndData( true, true );
        PIC_HW_SetVpp( true );   // Yes, indeed true to turn the PIC's supply voltage off !
    }
    else  // Not JDM... for all other interfaces : turn everything OFF by setting it LOW :
    {
        if ( PicHw_IsVddOn() >= 0) // '-1' if no extra Vdd switch exists...
            PIC_HW_SetVdd(false);
    }

    PIC_HW_SetDataEnable( false );
    PIC_HW_DisconnectFromTarget();  // here since 2005-03-29

    UpdateInterfaceTestDisplay();


    return fResult;
} // end TestTheInterface()



//---------------------------------------------------------------------------
void TInterfacePanel::onInitInterfButtonClick(wxCommandEvent& event)
//void ::Btn_InitInterfaceClick(TObject *Sender)
{
    int iInterfaceType;

    iInterfaceType = aInterfaceTypeChoice->GetSelection();
//  iInterfaceType = Combo_InterfaceType->ItemIndex;
    if (iInterfaceType<0) iInterfaceType=0;
    iInterfaceType = aIntfItemIndex2InterfaceType[iInterfaceType];

//    if ( iInterfaceType == PIC_INTF_TYPE_PLUGIN_DLL )
//        _tcsncpy( Config.sz80InterfacePluginDLL,   aCustomDefFileText->GetValue().c_str(), 80 );
//    else
        _tcsncpy( Config.sz255InterfaceSupportFile, aCustomDefFileText->GetValue().c_str(), 80 );
    PIC_HW_SetInterfaceType( iInterfaceType );
    UpdateInterfaceType(PIC_HW_interface.type);
    TestTheInterface();
    MainFrame::TheMainFrame->aOptionTab->UpdateOptionsDisplay();
}
//---------------------------------------------------------------------------



void TInterfacePanel::onLptInterfHelpButtonClick(wxCommandEvent& event)
//void ::Btn_HelpLptInterfaceClick(TObject *Sender)
{
    wxHelpControllerBase *Help = TLanguage::GetHelpController();
    if (Help != NULL)
        Help->DisplaySection(TResource::HELPID_LPT_INTERFACES);
//  Application->HelpContext(HELPID_LPT_INTERFACES);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void TInterfacePanel::onTestXXXChkClick(wxCommandEvent& event)
//void ::Chk_TestXXXClick(TObject *Sender)
{
    if (MainFrame::TheMainFrame->m_Updating>0)
        return;

    if (! PIC_HW_SetClockAndData(
                aTestClockChk->GetValue(), /* PIC - clock */
                aTestDataChk->GetValue())) /* PIC - data  */
    {
        MainFrame::AddTextToLog( _("PIC- Clock/Data failed") );
    }
    if (! PIC_HW_SetVpp(     aTestVppChk->GetValue() ))
    {
        MainFrame::AddTextToLog( _("PIC- Vpp failed") );
    }
}
//---------------------------------------------------------------------------



void TInterfacePanel::onTestVddChkClick(wxCommandEvent& event)
//void MainFrame::Chk_TestVddClick(TObject *Sender)
{
    if (MainFrame::TheMainFrame->m_Updating>0)
        return;

    if (! PIC_HW_SetVdd( aTestVddChk->GetValue() ) )
    {
        MainFrame::AddTextToLog( _("PIC- Vdd failed") );
    }
}
//---------------------------------------------------------------------------



void TInterfacePanel::onClockEnableChkClick(wxCommandEvent& event)
//void ::Chk_ClockEnableClick(TObject *Sender)
{
    if (MainFrame::TheMainFrame->m_Updating>0)
        return;

    if (! PIC_HW_SetClockEnable( aClockEnableChk->GetValue() ) )
    {
        MainFrame::AddTextToLog( _("PIC- ClockEnable failed") );
    }
}
//---------------------------------------------------------------------------



void TInterfacePanel::onDataEnableChkClick(wxCommandEvent& event)
//void ::Chk_DataEnableClick(TObject *Sender)
{
    if (MainFrame::TheMainFrame->m_Updating>0)
        return;

    if (! PIC_HW_SetDataEnable( aDataEnableChk->GetValue() ) )
    {
        MainFrame::AddTextToLog( _("PIC- DataEnable failed") );
    }
}
//---------------------------------------------------------------------------



void TInterfacePanel::onPullMclrToGndChkClick(wxCommandEvent& event)
//void ::Chk_PullMclrToGndClick(TObject *Sender)
{
    // since 2003-01-22 ...  for "AN589" where D4=HIGH pulls MCLR active LOW
    if (MainFrame::TheMainFrame->m_Updating>0)
        return;
    PIC_HW_PullMclrToGnd( aPullMclrToGndChk->GetValue() );
}
//---------------------------------------------------------------------------



void TInterfacePanel::onConnectICSPTargetChkClick(wxCommandEvent& event)
//void ::Chk_ConnectICSPTargetClick(TObject *Sender)
{
    if (MainFrame::TheMainFrame->m_Updating>0)
        return;
    if ( aConnectICSPTargetChk->GetValue() )
        PIC_HW_ConnectToTarget(); // since 2002-09-09 .
    else
        PIC_HW_DisconnectFromTarget(); // nothing "else" in this special case !

    UpdateInterfaceTestDisplay();  // since 2004-01-27, because VPP(!) may be affected
}



void TInterfacePanel::onVddSelectRadioSelect(wxCommandEvent& event)
//void ::RB_VddLowClick(TObject *Sender)
{
    if (MainFrame::TheMainFrame->m_Updating>0)
        return;
    Config.iIdleSupplyVoltage = aVddSelectRadio->GetSelection(); /* low supply voltage (typically 2.X volts) */
    if (! PIC_HW_SelectVdd( Config.iIdleSupplyVoltage ) )
        MainFrame::AddTextToLog( _("PIC- Select Vdd failed") );
}
//---------------------------------------------------------------------------



void TInterfacePanel::onRedLedBitmapClick(wxCommandEvent& event)
//void ::Shp_RedLEDMouseDown(TObject *Sender,
//      TMouseButton Button, TShiftState Shift, int X, int Y)
{
   PIC_HW_SetRedLed( 1-PIC_HW_SetRedLed(-1) );
}
//---------------------------------------------------------------------------


void TInterfacePanel::onGreenLedBitmapClick(wxCommandEvent& event)
//void ::Shp_GreenLEDMouseDown(TObject *Sender,
//      TMouseButton Button, TShiftState Shift, int X, int Y)
{
   PIC_HW_SetGreenLed( 1-PIC_HW_SetGreenLed(-1) );
}
//---------------------------------------------------------------------------



void TInterfacePanel::onInterfaceTypeChoiceSelect(wxCommandEvent& event)
//void ::Combo_InterfaceTypeChange(TObject *Sender)
{
    int iInterfaceType;

    if (MainFrame::TheMainFrame->m_Updating)
        return;
    if ( APPL_i32CustomizeOptions & APPL_CUST_NO_INTERFACE_SELECTION )
        return;

    iInterfaceType = aInterfaceTypeChoice->GetSelection();
//  iInterfaceType = Combo_InterfaceType->ItemIndex;
    if (iInterfaceType<0) iInterfaceType=0;
    iInterfaceType = aIntfItemIndex2InterfaceType[iInterfaceType];
    _tcsncpy(
//        (iInterfaceType == PIC_INTF_TYPE_PLUGIN_DLL) ? Config.sz80InterfacePluginDLL :
        Config.sz255InterfaceSupportFile, aCustomDefFileText->GetValue().c_str(), 256 );
//  if( iInterfaceType == PIC_INTF_TYPE_PLUGIN_DLL )
//       strncpy( Config.sz80InterfacePluginDLL,   Ed_CustomInterfaceDefFile->Text.c_str(), 80 );
//  else strncpy( Config.sz255InterfaceSupportFile, Ed_CustomInterfaceDefFile->Text.c_str(), 256 );
    SetInterfaceInternal( iInterfaceType );
    Config_changed |= APPL_CALLER_SAVE_CFG ;  // save on exit
}
//---------------------------------------------------------------------------



void TInterfacePanel::onInterfacePortChoiceSelect(wxCommandEvent& event)
//void ::Combo_InterfacePortChange(TObject *Sender)
{
// char str80[81];
    int iInterfaceType;

    if (MainFrame::TheMainFrame->m_Updating)
        return;
    if ( APPL_i32CustomizeOptions & APPL_CUST_NO_INTERFACE_SELECTION )
        return;

    iInterfaceType = aInterfaceTypeChoice->GetSelection();
//  iInterfaceType = Combo_InterfaceType->ItemIndex;
    if (iInterfaceType<0) iInterfaceType=0;
    iInterfaceType = aIntfItemIndex2InterfaceType[ iInterfaceType ];

    // ex: strncpy(Config.sz255InterfaceSupportFile, Ed_CustomInterfaceDefFile->Text.c_str(), 256);
    if ( m_fUseSerialPort )
    {
        if ( Config.iComPortNr != aInterfacePortChoice->GetSelection() )
//     if( Config.iComPortNr != Combo_InterfacePort->ItemIndex )
        {
            // forget "unusual" I/O-address when changing port number :
            Config.iComIoAddress = 0;
        }
        Config.iComPortNr = aInterfacePortChoice->GetSelection();
//     Config.iComPortNr = Combo_InterfacePort->ItemIndex;
        aIoPortAddressEdit->Clear();
        aIoPortAddressEdit->Disable();
//     Ed_IoPortAddress->Text = "";
//     Ed_IoPortAddress->ReadOnly = true;
    }
    else // not SERIAL but PARALLEL interface... please don't ask for USB ;-)
    {
        Config.iLptPortNr = aInterfacePortChoice->GetSelection();
//     Config.iLptPortNr = Combo_InterfacePort->ItemIndex;
    }
    SetInterfaceInternal( iInterfaceType );
    Config_changed |= APPL_CALLER_SAVE_CFG ;  // save changes on exit
}
//---------------------------------------------------------------------------


void TInterfacePanel::onIoPortAddressEditTextEnter(wxCommandEvent& event)
//void ::Ed_IoPortAddressChange(TObject *Sender)
{
// char str8[9];
    long PortAddr;

    if (MainFrame::TheMainFrame->m_Updating)
        return;


    if (!aIoPortAddressEdit->GetValue().ToLong(&PortAddr,16))
    {
        aExtraRdDelayEdit->SetBackgroundColour(wxColour(0xFF,0x80,0x80));
        return;
    }
    aExtraRdDelayEdit->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
//  strncpy(str8,Ed_IoPortAddress->Text.c_str(), 8);
//  l = HexStringToLongint(4, str8);  // PC's "I/O"-adresses have no more than 16 bit !
//  if(l<0) return; // someone entered an invalid hex word .. ignore

    if (m_fUseSerialPort)
    {
        if ( (PortAddr<PIC_HW_COM_ADDR_MIN) || (PortAddr>PIC_HW_COM_ADDR_MAX)
                || ( (PortAddr & 0x007) != 0) )  // lower 3 bits of address should be zero ( addr = n * 8 )
        {
            // VERY unusual address for a COM port.. paint it red
            aIoPortAddressEdit->SetForegroundColour(*wxRED);
            wxBell();
//       Ed_IoPortAddress->Color = clRed;
        }
        else
        {
            aIoPortAddressEdit->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
//       Ed_IoPortAddress->Color = clWindow;
            Config.iComIoAddress = PortAddr;
        }
    }
    else // not SERIAL ("COM") , but PARALLEL port ("LPT") :
    {
        if (PortAddr!=0 && PortAddr!=0x0278 && PortAddr!=0x0378 && PortAddr!= 0x02BC && PortAddr!=0x03BC && PortAddr!=0xB800)
        {
            // VERY unusual address for an LPT port..
            if ( WinPic_fAcceptAllIoAddresses )
            {
                // Accept the address "under protest"
                aIoPortAddressEdit->SetForegroundColour(wxColour(0xFF,0x7F,0x00)); // orange
//          Ed_IoPortAddress->Color = (TColor)0x00007FFF; // orange
                if ( PortAddr!=0 )
                {
                    Config.iLptIoAddress = PortAddr;
                }
            }
            else // do NOT accept the address (leave it unchanged)
            {
                aIoPortAddressEdit->SetForegroundColour(*wxRED);
                wxBell();
//            Ed_IoPortAddress->Color = clRed;
            }
        }
        else
        {
            aIoPortAddressEdit->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
//       Ed_IoPortAddress->Color = clWindow;
            Config.iLptIoAddress = PortAddr;
        }
    } // end else < COM or LPT ? >
}
//---------------------------------------------------------------------------


void TInterfacePanel::onCustomInterfSelectButtonClick(wxCommandEvent& event)
//void ::Btn_SelectCustomInterfaceClick(TObject *Sender)
{
    wxString sFname;
    int interface_type;

    if (MainFrame::TheMainFrame->m_Updating)
        return;
    if ( APPL_i32CustomizeOptions & APPL_CUST_NO_INTERFACE_SELECTION )
        return;


    ++(MainFrame::TheMainFrame->m_Updating);

    interface_type = aInterfaceTypeChoice->GetSelection();
//  interface_type = Combo_InterfaceType->ItemIndex;
    if (interface_type<0)
        interface_type=0;
    interface_type = aIntfItemIndex2InterfaceType[interface_type];


    // see help on TOpenDialog .
    MainFrame::TheMainFrame->aFileDialog->SetMessage(_( "Load custom interface definition file" ));
    MainFrame::TheMainFrame->aFileDialog->SetWindowStyle(wxFD_FILE_MUST_EXIST | wxFD_OPEN);
//  OpenDialog->Title = TE( "Load custom interface definition file (don't change dir)" );
    wxFileName Filename (wxStandardPaths::Get().GetExecutablePath());
//    if ( interface_type == PIC_INTF_TYPE_PLUGIN_DLL )
//    {
//        aFileDialog->SetWildcard(_("Plugin DLLs (*.dll)|*.dll"));
//        Filename.AppendDir(_T("interface_dlls"));
////       OpenDialog->DefaultExt = "dll";
////     OpenDialog->InitialDir = ExtractFilePath(Application->ExeName) + "interface_dlls";
////     OpenDialog->Filter = "Plugin DLLs (*.dll)|*.dll";
//    }
//    else
    {
        MainFrame::TheMainFrame->aFileDialog->SetWildcard(_("Xml files (*.xml)|*.xml"));
//        Filename.AppendDir(_T("interfaces"));
//       OpenDialog->DefaultExt = "ini";
//     OpenDialog->InitialDir = ExtractFilePath(Application->ExeName) + "interfaces";
//     OpenDialog->Filter = "INI files (*.ini)|*.ini";
    }
    MainFrame::TheMainFrame->aFileDialog->SetDirectory(Filename.GetFullPath());
    MainFrame::TheMainFrame->aFileDialog->SetFilename(aCustomDefFileText->GetValue());
    // Warning: TOpenDialog is buggy ! Sometimes, if there is something fishy with 'FileName',
    //          it simply won't open ?  Cured ONE of these situations as follows:
    // ex:  OpenDialog->FileName = OpenDialog->InitialDir + "\\" + Ed_CustomInterfaceDefFile->Text;
//  sFname = OpenDialog->InitialDir + "\\" + Ed_CustomInterfaceDefFile->Text;
//  if( FileExists(sFname) )
//    {
//        OpenDialog->FileName = sFname;  // rotten dialog ... didn't open when filename was empty !
//    }
//  else
//    {
//      if( interface_type == PIC_INTF_TYPE_PLUGIN_DLL )
//          OpenDialog->FileName = OpenDialog->InitialDir + "\\*.dll";
//      else
//          OpenDialog->FileName = OpenDialog->InitialDir + "\\*.ini";  // dig this, TOpenDialog ?!
//    }
//  OpenDialog->HelpContext = HELPID_CUSTOM_INTERFACES;
//  OpenDialog->Options.Clear();
//  OpenDialog->Options << ofFileMustExist << ofHideReadOnly << ofNoChangeDir << ofShowHelp;
    if (MainFrame::TheMainFrame->aFileDialog->ShowModal() == wxID_OK)
//  if (OpenDialog->Execute())
    {
        Filename.Assign(MainFrame::TheMainFrame->aFileDialog->GetPath());
//    Ed_CustomInterfaceDefFile->Text = ExtractFileName(OpenDialog->FileName);
        _tcsncpy(
//            (interface_type == PIC_INTF_TYPE_PLUGIN_DLL)?Config.sz80InterfacePluginDLL:
            Config.sz255InterfaceSupportFile, Filename.GetFullPath(), 256);
//    if( interface_type == PIC_INTF_TYPE_PLUGIN_DLL )
//         strncpy( Config.sz80InterfacePluginDLL,   Ed_CustomInterfaceDefFile->Text.c_str(), 80 );
//    else strncpy( Config.sz255InterfaceSupportFile, Ed_CustomInterfaceDefFile->Text.c_str(), 256 );
        SetInterfaceInternal(Config.pic_interface_type);
        Config_changed |= APPL_CALLER_SAVE_CFG ;  // save on exit
    }

    if ( MainFrame::TheMainFrame->m_Updating )
        --(MainFrame::TheMainFrame->m_Updating);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void TInterfacePanel::onSlowInterfaceChkClick(wxCommandEvent& event)
//void ::InterfaceTimingChanged(TObject *Sender)
{
    if (MainFrame::TheMainFrame->m_Updating)
        return;

    long ExtraDelay;
    if (aExtraRdDelayEdit->GetValue().ToLong(&ExtraDelay,10))
    {
        Config.iExtraRdDelay_us = std::max(0L, std::min(10000L, ExtraDelay));
        aExtraRdDelayEdit->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    }
    else
        aExtraRdDelayEdit->SetBackgroundColour(wxColour(0xFF,0x80,0x80));
//  Config.iExtraRdDelay_us = StrToIntDef(Ed_ExtraRdDelay->Text, Config.iExtraRdDelay_us);
//  if( Config.iExtraRdDelay_us<0 )     Config.iExtraRdDelay_us = 0;
//  if( Config.iExtraRdDelay_us>10000 ) Config.iExtraRdDelay_us = 10000;
    if (aExtraClkDelayEdit->GetValue().ToLong(&ExtraDelay,10))
    {
        Config.iExtraClkDelay_us = std::max(0L, std::min(10000L, ExtraDelay));
        aExtraClkDelayEdit->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    }
    else
        aExtraClkDelayEdit->SetBackgroundColour(wxColour(0xFF,0x80,0x80));
//  Config.iExtraClkDelay_us= StrToIntDef(Ed_ExtraClkDelay->Text,Config.iExtraClkDelay_us);
//  if( Config.iExtraClkDelay_us<0 )    Config.iExtraClkDelay_us = 0;
//  if( Config.iExtraClkDelay_us>10000) Config.iExtraClkDelay_us = 10000;
    Config.iSlowInterface  = aSlowInterfaceChk->GetValue();
//  Config.iSlowInterface = Chk_SlowInterface->Checked;
//    PHWInfo.iSlowInterface = Config.iSlowInterface;
}
//---------------------------------------------------------------------------



void TInterfacePanel::onIoPortAddressGetFocus(wxFocusEvent &pEvent)
//void ::Ed_IoPortAddressClick(TObject *Sender)
{
    if ( !MainFrame::TheMainFrame->m_Updating )
    {
        // When clicking into this field, the user must be informed
        // about the potentially disastrous effect
        // of entering the wrong I/O-address in this field !
        ++(MainFrame::TheMainFrame->m_Updating);
        if ( ! WinPic_fAcceptAllIoAddresses )
            UnlockEditFieldForIOPortAddress();
        --(MainFrame::TheMainFrame->m_Updating);
    }
}
//---------------------------------------------------------------------------
