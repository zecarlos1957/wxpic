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
#include <wx/filename.h>
#include <wx/dir.h>
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
const long TInterfacePanel::ID_ADDRESS_TEXT = wxNewId();
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

static struct
{
    int           InterfaceType;
    const wxChar *InterfaceName;
} InterfaceTab[PIC_INTF_TYPE_MAX] = {
    { PIC_INTF_TYPE_UNKNOWN,        _("(unknown)") },
    { PIC_INTF_TYPE_COM84,          _("COM84 programmer for serial port") },
    { PIC_INTF_TYPE_JDM2,           _("JDM (2) for serial port") },
    { PIC_INTF_TYPE_PIP84_V1,       _("PIP84 by SM6LKM, Data->PAPER (Pin12)") },
    { PIC_INTF_TYPE_PIP84_V2,       _("PIP84 by SM6LKM, Data->ACK (Pin10)")  },
    { PIC_INTF_TYPE_LKM_FLASHPR_V1, _("PIC Flash prog by SM6LKM (2002-10)") },
    { PIC_INTF_TYPE_TAIT_7406_4066, _("Tait, 7406(inverter)+4066(switch)") },
    { PIC_INTF_TYPE_TAIT_7407_4066, _("Tait, 7407(driver) + 4066(switch)") },
    { PIC_INTF_TYPE_TAIT_7406_PNP,  _("Tait, 7406(inverter)+PNP transistor") },
    { PIC_INTF_TYPE_TAIT_7407_PNP,  _("Tait, 7407(driver) + PNP transistor") },
    { PIC_INTF_TYPE_LPT_AN589,      _("Microchip AN589") },
    { PIC_INTF_TYPE_LPT_NOPPP,      _("NOPPP") },
    { PIC_INTF_TYPE_CUSTOM_LPT,     _("Custom, on LPT port, defined by FILE") },
    { PIC_INTF_TYPE_CUSTOM_COM,     _("Custom, on COM port, defined by FILE") },
};

static int InterfaceType2PortSelection [PIC_INTF_TYPE_MAX];

BEGIN_EVENT_TABLE(TInterfacePanel,wxPanel)
	//(*EventTable(TInterfacePanel)
	//*)
END_EVENT_TABLE()

TInterfacePanel::TInterfacePanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size, long style, const wxString& name)
: aPortType            (portUNDEFINED)
, aCurIoAddressUsage   (usageNONE)
, aCurIoAddress        (0)
, aAcceptAllIoAddresses(false)
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
	aAddressText = new wxStaticText(this, ID_ADDRESS_TEXT, _("Address:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_ADDRESS_TEXT"));
	aAddressText->Disable();
	BoxSizer20->Add(aAddressText, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
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
    aIoPortAddressEdit->Connect(wxID_ANY, wxEVT_SET_FOCUS,  (wxObjectEventFunction)&TInterfacePanel::onIoPortAddressGetFocus,  NULL, GetEventHandler());
    aIoPortAddressEdit->Connect(wxID_ANY, wxEVT_KILL_FOCUS, (wxObjectEventFunction)&TInterfacePanel::onIoPortAddressKillFocus, NULL, GetEventHandler());

    for (int i = 0; i < PIC_INTF_TYPE_MAX; ++i)
    {
        aInterfaceTypeChoice->Append(InterfaceTab[i].InterfaceName);
        InterfaceType2PortSelection[InterfaceTab[i].InterfaceType] = i;
    }
}

TInterfacePanel::~TInterfacePanel()
{
	//(*Destroy(TInterfacePanel)
	//*)
}


//---------------------------------------------------------------------------

static bool WinPic_fAcceptAllIoAddresses = false;

static bool IsWeirdLptPortAddr (int pPortAddress)
{
    return (pPortAddress != 0)
        && (pPortAddress != 0x0278)
        && (pPortAddress != 0x0378)
        && (pPortAddress != 0x02BC)
        && (pPortAddress != 0x03BC)
        && (pPortAddress != 0xB800)
        && (pPortAddress != 0xF8FC);
}

TInterfacePanel::EIoAddressUsage TInterfacePanel::setLptPortAddress (void)
{
    EIoAddressUsage Result;
    switch(Config.iLptPortNr)
    {
        case 1:
            Config.iLptIoAddress = 0x0378;
            Result = usageDISPLAY;
            break;
        case 2:
            Config.iLptIoAddress = 0x0278;
            Result = usageDISPLAY;
            break;
        default:
            if (Config.iLptIoAddress == 0)
                Config.iLptIoAddress = 0x0378;
            Result = usageINPUT;
            if (IsWeirdLptPortAddr(Config.iLptIoAddress))
            {
                aAcceptAllIoAddresses = true;
                Result = usageINPUT_WARN;
            }
    }
    return Result;
}


static uint32_t IsSerialInterface =
    (true  << PIC_INTF_TYPE_UNKNOWN) +
    (true  << PIC_INTF_TYPE_COM84) +
    (true  << PIC_INTF_TYPE_JDM2) +
    (false << PIC_INTF_TYPE_PIP84_V1) +
    (false << PIC_INTF_TYPE_PIP84_V2) +
    (false << PIC_INTF_TYPE_LKM_FLASHPR_V1) +
    (false << PIC_INTF_TYPE_TAIT_7406_4066) +
    (false << PIC_INTF_TYPE_TAIT_7407_4066) +
    (false << PIC_INTF_TYPE_TAIT_7406_PNP) +
    (false << PIC_INTF_TYPE_TAIT_7407_PNP) +
    (false << PIC_INTF_TYPE_LPT_AN589) +
    (false << PIC_INTF_TYPE_LPT_NOPPP) +
    (false << PIC_INTF_TYPE_CUSTOM_LPT) +
    (true  << PIC_INTF_TYPE_CUSTOM_COM);

//---------------------------------------------------------------------------
void TInterfacePanel::UpdateInterfaceType(void)
{
    ++(MainFrame::TheMainFrame->m_Updating);
    if ((Config.pic_interface_type >= PIC_INTF_TYPE_MAX)
    ||  (Config.pic_interface_type < 0))
    {
        Config.pic_interface_type = 0;
        ConfigChanged = true;
    }

    aInterfaceTypeText->SetLabel((PIC_HW_SetInterfaceType(Config.pic_interface_type))
                                 ? wxString(_("Interface Type ok."))
                                 : _("Interface error: ") + wxString(PicHw_sz255LastError));

    EInterfacePortType IsSerialPort = ((IsSerialInterface>>Config.pic_interface_type) & true) ? portSERIAL : portPARALLEL;
    if (IsSerialPort != aPortType)
    {
        aPortType = IsSerialPort;

        //-- Change of port type: rebuild the list
        //----------------------------------------
        if (aPortType == portSERIAL)
        {
            aInterfacePortChoice->Clear();
            wxString DeviceName;

#ifdef __WXMSW__
            for (int i=0;i<16;++i)
                aInterfacePortChoice->Append(wxString::Format(_T("COM%d"),i));
#else
            wxString device_path;
            wxDir temp_dir(wxT("/dev"));
            static const wxChar *SerialPortDevNames [] = { wxT("ttyS*"), wxT("ttyUSB*") };
            for (unsigned i = 0; i < sizeof(SerialPortDevNames)/sizeof(wxChar*); ++i)
            {
                if(temp_dir.GetFirst( &device_path, SerialPortDevNames[i] ) )
                    do{
                        aInterfacePortChoice->Append( wxT("/dev/") + device_path );
                    }while( temp_dir.GetNext( &device_path ) );
            }
            if (aInterfacePortChoice->IsEmpty())
                aInterfacePortChoice->Append( _("None") );
#endif

            if (!aInterfacePortChoice->SetStringSelection(Config.sz40ComPortName))
            {
                aInterfacePortChoice->SetSelection(0);
                copyPortSelectionToConfig();
            }
        }
        else  // not serial but parallel port ?
    //        if ( interface_type != PIC_INTF_TYPE_PLUGIN_DLL )
        {
            aInterfacePortChoice->Clear();
            aInterfacePortChoice->Append(_("(at address)") );
            aInterfacePortChoice->Append(_T("LPT1"));
            aInterfacePortChoice->Append(_T("LPT2"));
            aInterfacePortChoice->SetSelection(Config.iLptPortNr);
        } // end if < interface on LPT port >
//        else
//        {
//            // neither SERIAL nor PARALLEL port (but a hardware-access driver in a DLL)
//        } // end else < neither serial nor parallel port >
    }

    aInterfaceTypeChoice->SetSelection(InterfaceType2PortSelection[Config.pic_interface_type]);

    aCustomDefFileText->SetValue(Config.sz255InterfaceSupportFile);
    aCustomDefFileText->Enable((Config.pic_interface_type==PIC_INTF_TYPE_CUSTOM_LPT)
            ||(Config.pic_interface_type==PIC_INTF_TYPE_CUSTOM_COM));

    updateIoAddressDisplay((aPortType == portSERIAL) ? usageNONE : setLptPortAddress());

    aExtraRdDelayEdit ->SetValue(wxString::Format(_T("%d"), Config.iExtraRdDelay_us ));
    aExtraClkDelayEdit->SetValue(wxString::Format(_T("%d"), Config.iExtraClkDelay_us));
    aSlowInterfaceChk ->SetValue(Config.iSlowInterface);

    if (MainFrame::TheMainFrame->m_Updating>0)
        --(MainFrame::TheMainFrame->m_Updating);

} // end UpdateInterfaceType()
//---------------------------------------------------------------------------

void TInterfacePanel::updateIoAddressDisplay(EIoAddressUsage pUsage)
{
    wxChar str80[81];
    bool Enable = (pUsage >= usageINPUT);
    if (Enable != (aCurIoAddressUsage >= usageINPUT))
    {
        aIoPortAddressEdit->Enable(Enable);
        aAddressText->Enable(Enable);
    }
    if ((pUsage != aCurIoAddressUsage)
    &&  ((pUsage > usageINPUT) || (aCurIoAddressUsage > usageINPUT)))
    {
        static const wxColour Orange(0xFF,0x7F,0x00);
        wxColour Colour;
        switch (pUsage)
        {
        case usageNONE:
        case usageDISPLAY:
        case usageINPUT:
            Colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);
            break;
        case usageINPUT_WARN:
            Colour = Orange;
            break;
        case usageINPUT_ERROR:
            Colour = *wxRED;
            wxBell();
            break;
        }
        aIoPortAddressEdit->SetForegroundColour(Colour);
        aIoPortAddressEdit->Refresh();
    }
    bool Empty = (pUsage == usageNONE);
    if (Empty)
    {
        if (aCurIoAddressUsage != usageNONE)
            aIoPortAddressEdit->SetValue(wxEmptyString);
        aCurIoAddress = 0;
    }
    else if (Config.iLptIoAddress != aCurIoAddress)
    {
        _stprintf(str80,_T("%04X"), Config.iLptIoAddress);
        aIoPortAddressEdit->SetValue(str80);
        aCurIoAddress = Config.iLptIoAddress;
    }
    aCurIoAddressUsage = pUsage;
}

//---------------------------------------------------------------------------
void TInterfacePanel::changeIoPortAddress (void)
{
    long PortAddr;

    if (MainFrame::TheMainFrame->m_Updating)
        return;

    EIoAddressUsage IoAddressUsage = usageINPUT_ERROR;

    if (!aIoPortAddressEdit->GetValue().ToLong(&PortAddr,16))
        IoAddressUsage = usageINPUT_ERROR;  // someone entered an invalid hex word .. ignore

    else if (aPortType == portPARALLEL)
    {   //-- PARALLEL port ("LPT")
        if (IsWeirdLptPortAddr(PortAddr))
        {
            // VERY unusual address for an LPT port..
            if ( WinPic_fAcceptAllIoAddresses )
            {
                // Accept the address "under protest"
                IoAddressUsage = usageINPUT_WARN;
                Config.iLptIoAddress = PortAddr;
                ConfigChanged = true ;  // save on exit
            }
            // else // do NOT accept the address (leave it unchanged)
        }
        else
        {
            IoAddressUsage = usageINPUT;
            Config.iLptIoAddress = PortAddr;
            ConfigChanged = true ;  // save on exit
        }
    }
    else // SERIAL ("COM")
        wxASSERT_MSG(false,_T("Editing of Port Address not allowed with serial port"));

    //-- Render the IO Address status
    updateIoAddressDisplay(IoAddressUsage);
}

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

    static int LastValueRed = -2;
    i = PIC_HW_SetRedLed( -1/*don't change, just get state*/ );
    if (i != LastValueRed)
    {
        aRedLedBitmap->SetBitmapLabel(*((i>0) ? TResource::GetLedRedIcon() : TResource::GetLedOffIcon()));
        aRedLedText->Enable(i>=0);
        LastValueRed = i;
    }

    static int LastValueGreen = -2;
    i = PIC_HW_SetGreenLed( -1/*don't change, just get state*/ );
    if (i != LastValueGreen)
    {
        aGreenLedBitmap->SetBitmapLabel(*((i>0) ? TResource::GetLedGreenIcon() : TResource::GetLedOffIcon()));
        aGreenLedText->Enable(i>=0);
        LastValueGreen = i;
    }

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
        WinPic_fAcceptAllIoAddresses = false;
        UpdateInterfaceType();
        aInterfacePortChoice->SetFocus();
        return false;
    }
    WinPic_fAcceptAllIoAddresses = true;
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
        APPL_ShowMsg( 0, _("Initialising PIC-Programmer: Success.") );
        aInterfaceTypeText->SetLabel(_("Interface tested, OK."));
        fResult = true;
    }
    else
    {
        APPL_ShowMsg( 0, _("WARNING: Could not initialize programmer !") );
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
    UpdateInterfaceType();
    TestTheInterface();
    MainFrame::TheMainFrame->aOptionTab->UpdateOptionsDisplay();
}
//---------------------------------------------------------------------------



void TInterfacePanel::onLptInterfHelpButtonClick(wxCommandEvent& event)
{
    wxHelpControllerBase *Help = TLanguage::GetHelpController();
    if (Help != NULL)
        Help->DisplaySection(TResource::HELPID_LPT_INTERFACES);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void TInterfacePanel::onTestXXXChkClick(wxCommandEvent& event)
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
{
    // since 2003-01-22 ...  for "AN589" where D4=HIGH pulls MCLR active LOW
    if (MainFrame::TheMainFrame->m_Updating>0)
        return;
    PIC_HW_PullMclrToGnd( aPullMclrToGndChk->GetValue() );
}
//---------------------------------------------------------------------------



void TInterfacePanel::onConnectICSPTargetChkClick(wxCommandEvent& event)
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
{
    if (MainFrame::TheMainFrame->m_Updating>0)
        return;
    Config.iIdleSupplyVoltage = aVddSelectRadio->GetSelection(); /* low supply voltage (typically 2.X volts) */
    if (! PIC_HW_SelectVdd( Config.iIdleSupplyVoltage ) )
        MainFrame::AddTextToLog( _("PIC- Select Vdd failed") );
    ConfigChanged = true ;  // save on exit
}
//---------------------------------------------------------------------------



void TInterfacePanel::onRedLedBitmapClick(wxCommandEvent& event)
{
   PIC_HW_SetRedLed( 1-PIC_HW_SetRedLed(-1) );
}
//---------------------------------------------------------------------------


void TInterfacePanel::onGreenLedBitmapClick(wxCommandEvent& event)
{
   PIC_HW_SetGreenLed( 1-PIC_HW_SetGreenLed(-1) );
}
//---------------------------------------------------------------------------



void TInterfacePanel::onInterfaceTypeChoiceSelect(wxCommandEvent& event)
{
    int iInterfaceType;

    if (MainFrame::TheMainFrame->m_Updating)
        return;
    if ( APPL_i32CustomizeOptions & APPL_CUST_NO_INTERFACE_SELECTION )
        return;

    iInterfaceType = aInterfaceTypeChoice->GetSelection();
    if (iInterfaceType<0) iInterfaceType=0;
    iInterfaceType = InterfaceTab[iInterfaceType].InterfaceType;

    _tcsncpy(
//        (iInterfaceType == PIC_INTF_TYPE_PLUGIN_DLL) ? Config.sz80InterfacePluginDLL :
        Config.sz255InterfaceSupportFile, aCustomDefFileText->GetValue().c_str(), 256 );
//  if( iInterfaceType == PIC_INTF_TYPE_PLUGIN_DLL )
//       strncpy( Config.sz80InterfacePluginDLL,   Ed_CustomInterfaceDefFile->Text.c_str(), 80 );
//  else strncpy( Config.sz255InterfaceSupportFile, Ed_CustomInterfaceDefFile->Text.c_str(), 256 );
    Config.pic_interface_type = iInterfaceType;
    UpdateInterfaceType();
//    MainFrame::TheMainFrame->aOptionTab->UpdateOptionsDisplay();
    ConfigChanged = true ;  // save on exit
}
//---------------------------------------------------------------------------



void TInterfacePanel::onInterfacePortChoiceSelect(wxCommandEvent& event)
{
    if (MainFrame::TheMainFrame->m_Updating)
        return;
    if ( APPL_i32CustomizeOptions & APPL_CUST_NO_INTERFACE_SELECTION )
        return;

    copyPortSelectionToConfig();
    UpdateInterfaceType();
}
//---------------------------------------------------------------------------


void TInterfacePanel::copyPortSelectionToConfig (void)
{
    if ( aPortType == portSERIAL )
    {
        wxString portname = aInterfacePortChoice->GetStringSelection();
        _tcsncpy(Config.sz40ComPortName, portname.c_str(), 40 );
        Config.sz40ComPortName[40]=_T('\0');
    }
    else // not SERIAL but PARALLEL interface... please don't ask for USB ;-)
    {
        Config.iLptPortNr = aInterfacePortChoice->GetSelection();
    }
    ConfigChanged = true ;  // save changes on exit
//---------------------------------------------------------------------------
}

void TInterfacePanel::onIoPortAddressEditTextEnter(wxCommandEvent& event)
{
    changeIoPortAddress();
}

void TInterfacePanel::onIoPortAddressKillFocus(wxFocusEvent &pEvent)
{
    changeIoPortAddress();
}

//---------------------------------------------------------------------------

void TInterfacePanel::onCustomInterfSelectButtonClick(wxCommandEvent& event)
{
    wxString sFname;

    if (MainFrame::TheMainFrame->m_Updating)
        return;
    if ( APPL_i32CustomizeOptions & APPL_CUST_NO_INTERFACE_SELECTION )
        return;


    ++(MainFrame::TheMainFrame->m_Updating);


    MainFrame::TheMainFrame->aFileDialog->SetMessage(_( "Load custom interface definition file" ));
    MainFrame::TheMainFrame->aFileDialog->SetWindowStyle(wxFD_FILE_MUST_EXIST | wxFD_OPEN);
    wxFileName Filename (wxStandardPaths::Get().GetDataDir(), wxEmptyString);
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
    }
    MainFrame::TheMainFrame->aFileDialog->SetDirectory(Filename.GetFullPath());
    MainFrame::TheMainFrame->aFileDialog->SetFilename(aCustomDefFileText->GetValue());
//      if( interface_type == PIC_INTF_TYPE_PLUGIN_DLL )
//          OpenDialog->FileName = OpenDialog->InitialDir + "\\*.dll";
//      else
//          OpenDialog->FileName = OpenDialog->InitialDir + "\\*.ini";  // dig this, TOpenDialog ?!
//    }
//  OpenDialog->HelpContext = HELPID_CUSTOM_INTERFACES;
//  OpenDialog->Options.Clear();
//  OpenDialog->Options << ofFileMustExist << ofHideReadOnly << ofNoChangeDir << ofShowHelp;
    if (MainFrame::TheMainFrame->aFileDialog->ShowModal() == wxID_OK)
    {
        Filename.Assign(MainFrame::TheMainFrame->aFileDialog->GetPath());
        _tcsncpy(
//            (interface_type == PIC_INTF_TYPE_PLUGIN_DLL)?Config.sz80InterfacePluginDLL:
            Config.sz255InterfaceSupportFile, Filename.GetFullPath(), 256);
//    if( interface_type == PIC_INTF_TYPE_PLUGIN_DLL )
//         strncpy( Config.sz80InterfacePluginDLL,   Ed_CustomInterfaceDefFile->Text.c_str(), 80 );
//    else strncpy( Config.sz255InterfaceSupportFile, Ed_CustomInterfaceDefFile->Text.c_str(), 256 );

        UpdateInterfaceType();
        ConfigChanged = true ;  // save on exit
    }

    if ( MainFrame::TheMainFrame->m_Updating )
        --(MainFrame::TheMainFrame->m_Updating);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void TInterfacePanel::onSlowInterfaceChkClick(wxCommandEvent& event)
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
    if (aExtraClkDelayEdit->GetValue().ToLong(&ExtraDelay,10))
    {
        Config.iExtraClkDelay_us = std::max(0L, std::min(10000L, ExtraDelay));
        aExtraClkDelayEdit->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    }
    else
        aExtraClkDelayEdit->SetBackgroundColour(wxColour(0xFF,0x80,0x80));
    Config.iSlowInterface  = aSlowInterfaceChk->GetValue();
    ConfigChanged = true ;  // save on exit
}
//---------------------------------------------------------------------------



void TInterfacePanel::onIoPortAddressGetFocus(wxFocusEvent &pEvent)
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
