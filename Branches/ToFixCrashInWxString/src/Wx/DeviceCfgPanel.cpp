#include "DeviceCfgPanel.h"
#include "MainFrame.h"
#include "Appl.h"

//(*InternalHeaders(TDeviceCfgPanel)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/settings.h>
#include <wx/msgdlg.h>

#include <WinPic/WinPicPr/Config.h>
#include <WinPic/WinPicPr/PIC_PRG.h>
#include <WinPic/WinPicPr/PIC_HEX.h>

enum
{
    configbitsCOL_NAME = -1,
    configbitsCOL_ADDR,
    configbitsCOL_MASK,
    configbitsCOL_VALUE,
};

//(*IdInit(TDeviceCfgPanel)
const long TDeviceCfgPanel::ID_STATICTEXT3 = wxNewId();
const long TDeviceCfgPanel::ID_PART_NAME_CHOICE = wxNewId();
const long TDeviceCfgPanel::ID_STATICTEXT4 = wxNewId();
const long TDeviceCfgPanel::ID_TEXTCTRL5 = wxNewId();
const long TDeviceCfgPanel::ID_STATICTEXT5 = wxNewId();
const long TDeviceCfgPanel::ID_STATICTEXT6 = wxNewId();
const long TDeviceCfgPanel::ID_TEXTCTRL6 = wxNewId();
const long TDeviceCfgPanel::ID_STATICTEXT7 = wxNewId();
const long TDeviceCfgPanel::ID_TEXTCTRL7 = wxNewId();
const long TDeviceCfgPanel::ID_STATICTEXT8 = wxNewId();
const long TDeviceCfgPanel::ID_TEXTCTRL8 = wxNewId();
const long TDeviceCfgPanel::ID_STATICTEXT9 = wxNewId();
const long TDeviceCfgPanel::ID_CHECKBOX13 = wxNewId();
const long TDeviceCfgPanel::ID_STATICTEXT21 = wxNewId();
const long TDeviceCfgPanel::ID_SAVED_BEFORE_ERASE_TEXT = wxNewId();
const long TDeviceCfgPanel::ID_DEV_CFG_GRID = wxNewId();
//*)

BEGIN_EVENT_TABLE(TDeviceCfgPanel,wxPanel)
	//(*EventTable(TDeviceCfgPanel)
	//*)
END_EVENT_TABLE()

TDeviceCfgPanel::TDeviceCfgPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size, long style, const wxString& name)
{
	//(*Initialize(TDeviceCfgPanel)
	wxBoxSizer* BoxSizer10;
	wxBoxSizer* BoxSizer7;
	wxBoxSizer* BoxSizer8;
	wxStaticBoxSizer* StaticBoxSizer4;
	wxFlexGridSizer* FlexGridSizer3;
	wxFlexGridSizer* FlexGridSizer2;
	wxBoxSizer* BoxSizer11;
	wxBoxSizer* BoxSizer9;
	wxBoxSizer* BoxSizer25;

	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	BoxSizer7 = new wxBoxSizer(wxVERTICAL);
	BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer9 = new wxBoxSizer(wxVERTICAL);
	BoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Part"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	BoxSizer11->Add(StaticText3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	aPartNameChoice = new wxChoice(this, ID_PART_NAME_CHOICE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_PART_NAME_CHOICE"));
	BoxSizer11->Add(aPartNameChoice, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	BoxSizer9->Add(BoxSizer11, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
	FlexGridSizer2 = new wxFlexGridSizer(2, 3, 0, 0);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Config word #1:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer2->Add(StaticText4, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aConfigWordHexEdit = new wxTextCtrl(this, ID_TEXTCTRL5, _("3FFF"), wxDefaultPosition, wxSize(60,20), wxNO_BORDER, wxDefaultValidator, _T("ID_TEXTCTRL5"));
	wxFont aConfigWordHexEditFont(16,wxSWISS,wxFONTSTYLE_NORMAL,wxBOLD,false,_T("Courier New"),wxFONTENCODING_DEFAULT);
	aConfigWordHexEdit->SetFont(aConfigWordHexEditFont);
	FlexGridSizer2->Add(aConfigWordHexEdit, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("(hex)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer2->Add(StaticText5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Config word #2:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer2->Add(StaticText6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	aConfigWordHexEdit2 = new wxTextCtrl(this, ID_TEXTCTRL6, _("3FFF"), wxDefaultPosition, wxSize(60,20), wxNO_BORDER, wxDefaultValidator, _T("ID_TEXTCTRL6"));
	wxFont aConfigWordHexEdit2Font(16,wxSWISS,wxFONTSTYLE_NORMAL,wxBOLD,false,_T("Courier New"),wxFONTENCODING_DEFAULT);
	aConfigWordHexEdit2->SetFont(aConfigWordHexEdit2Font);
	FlexGridSizer2->Add(aConfigWordHexEdit2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("(hex)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	FlexGridSizer2->Add(StaticText7, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	BoxSizer9->Add(FlexGridSizer2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer8->Add(BoxSizer9, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 0);
	BoxSizer10 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer4 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Device Properties"));
	FlexGridSizer3 = new wxFlexGridSizer(2, 2, 0, 0);
	aProgMemSizeText = new wxTextCtrl(this, ID_TEXTCTRL7, _("00000"), wxDefaultPosition, wxSize(45,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL7"));
	aProgMemSizeText->SetMaxLength(5);
	FlexGridSizer3->Add(aProgMemSizeText, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText8 = new wxStaticText(this, ID_STATICTEXT8, _("word code memory"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	FlexGridSizer3->Add(StaticText8, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 1);
	aEepromMemSizeText = new wxTextCtrl(this, ID_TEXTCTRL8, _("00000"), wxDefaultPosition, wxSize(45,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL8"));
	aEepromMemSizeText->SetMaxLength(5);
	FlexGridSizer3->Add(aEepromMemSizeText, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticText9 = new wxStaticText(this, ID_STATICTEXT9, _("byte data EEPROM"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	FlexGridSizer3->Add(StaticText9, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	StaticBoxSizer4->Add(FlexGridSizer3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	aHasFlashMemoryChk = new wxCheckBox(this, ID_CHECKBOX13, _("FLASH"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX13"));
	aHasFlashMemoryChk->SetValue(false);
	StaticBoxSizer4->Add(aHasFlashMemoryChk, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 1);
	BoxSizer10->Add(StaticBoxSizer4, 1, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	BoxSizer8->Add(BoxSizer10, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer7->Add(BoxSizer8, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer25 = new wxBoxSizer(wxHORIZONTAL);
	aSavedBeforeEraseTitle = new wxStaticText(this, ID_STATICTEXT21, _("Saved before erase:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT21"));
	BoxSizer25->Add(aSavedBeforeEraseTitle, 0, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aSavedBeforeEraseText = new wxStaticText(this, ID_SAVED_BEFORE_ERASE_TEXT, _("-"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, _T("ID_SAVED_BEFORE_ERASE_TEXT"));
	BoxSizer25->Add(aSavedBeforeEraseText, 1, wxALL|wxALIGN_LEFT|wxALIGN_TOP, 1);
	BoxSizer7->Add(BoxSizer25, 0, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 1);
	aDevCfgGrid = new wxGrid(this, ID_DEV_CFG_GRID, wxDefaultPosition, wxDefaultSize, 0, _T("ID_DEV_CFG_GRID"));
	aDevCfgGrid->CreateGrid(1,3);
	aDevCfgGrid->SetMinSize(wxSize(0,0));
	BoxSizer7->Add(aDevCfgGrid, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(BoxSizer7);
	BoxSizer7->Fit(this);
	BoxSizer7->SetSizeHints(this);

	Connect(ID_PART_NAME_CHOICE,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&TDeviceCfgPanel::onPartNameChoiceSelect);
	Connect(ID_TEXTCTRL5,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&TDeviceCfgPanel::onConfigWordHexEditText);
	Connect(ID_TEXTCTRL6,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&TDeviceCfgPanel::onConfigWordHexEdit2Text);
	Connect(ID_TEXTCTRL7,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&TDeviceCfgPanel::onProgMemSizeTextText);
	Connect(ID_TEXTCTRL8,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&TDeviceCfgPanel::onEepromMemSizeTextText);
	Connect(ID_CHECKBOX13,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&TDeviceCfgPanel::onHasFlashMemoryChkClick);
	Connect(ID_DEV_CFG_GRID,wxEVT_GRID_CELL_CHANGE,(wxObjectEventFunction)&TDeviceCfgPanel::onDevCfgGridCellChange);
	//*)

    wxGridCellAttr *ReadOnlyAttr = new wxGridCellAttr;
    ReadOnlyAttr->SetReadOnly();
    aDevCfgGrid->SetColLabelValue(configbitsCOL_ADDR,  _("Addr"));
    aDevCfgGrid->SetColLabelValue(configbitsCOL_MASK,  _("Mask"));  // important debugging aid, also to add support for new devices
    aDevCfgGrid->SetColLabelValue(configbitsCOL_VALUE, _("Setting"));
    aDevCfgGrid->SetColSize(configbitsCOL_VALUE, 200);
    ReadOnlyAttr->IncRef();
    aDevCfgGrid->SetColAttr(configbitsCOL_ADDR, ReadOnlyAttr);
    ReadOnlyAttr->IncRef();
    aDevCfgGrid->SetColAttr(configbitsCOL_MASK, ReadOnlyAttr);
    aDevCfgGrid->EnableDragColMove(false);
    aDevCfgGrid->EnableDragRowSize(false);
    aDevCfgGrid->SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
    aDevCfgGrid->SetRowLabelSize(250);
    ReadOnlyAttr->DecRef();

    m_sz40DisplayedDeviceName[0] = 0;
    for (int i=0; i<2; ++i)
        m_displayed_config_word[i] = 0;
    for (int i=0; i<PICDEV_MAX_CONFIG_BIT_INFOS; ++i)
        m_pConfigBitGridRowNumber_to_ConfigBitInfoPtr[i] = NULL;
}

TDeviceCfgPanel::~TDeviceCfgPanel()
{
	//(*Destroy(TDeviceCfgPanel)
	//*)
}


//---------------------------------------------------------------------------
void TDeviceCfgPanel::UpdateDeviceConfigTab(bool fUpdateHexWord)
{
//  int i,n;
    wxChar str80[81];
    const wxChar *psz;
    bool fFoundDevName;
//  T_PicDeviceInfo MyDeviceInfo;
//  T_PicConfigBitInfo *pConfigBitInfo;
//  uint32_t dwConfigWord, dwConfigWordAddress, dwConfigMask;
//  wxString s;

    ++(MainFrame::TheMainFrame->m_Updating);
    m_displayed_config_word[0]  = PicBuf_GetConfigWord(0);
    m_displayed_config_word[1]  = PicBuf_GetConfigWord(1); // since 2003-12

    strncpy(m_sz40DisplayedDeviceName, PIC_DeviceInfo.sz40DeviceName, 40);

    // Show the currently selected PIC type in the "Part"-combo list.
    // ex: Be sure that the item index matches the definition of
    //     PIC_DEV_TYPE_xxxx in PIC_PRG.h !!
    // Now all types in the list are filled during run-time from a table..
    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("UpdateDeviceConfigTab: Listing devices..") );
//   fFoundDevName = false;
//   i=0;
    wxString DeviceName;
    fFoundDevName = aPartNameChoice->SetStringSelection(Iso8859_1_TChar(m_sz40DisplayedDeviceName, DeviceName));
//   while( i < aPartNameChoice->GetCount() )
//     {
//       if( >GetString(i) == DeviceName )
//        {
//          aPartNameChoice->SetSelection(i);
//          fFoundDevName = true;
//          break;
//        }
//       ++i;
//     }

    aHasFlashMemoryChk->SetValue(PIC_DeviceInfo.iCodeMemType==PIC_MT_FLASH) ;
    if (fFoundDevName && (m_sz40DisplayedDeviceName[0]!='u') )
    {
        MainFrame::TheMainFrame->SetLabel(MainFrame::TheMainFrame->m_original_title
                                    + _T(" - ") + aPartNameChoice->GetStringSelection());
        aProgMemSizeText->SetEditable(false);
//        aEepromMemSizeText->Color       = clBtnFace;
        aEepromMemSizeText->SetEditable(false);
//        Ed_DataEepromMemory->Color    = clBtnFace;
//        Ed_ConfigWordHex->Color       = clBtnFace;
        aHasFlashMemoryChk->Enable(false);    // we know if it's possible
    }
    else // unknown PIC device type.
    {
        // No Config word decoding, but "everything goes"...
        MainFrame::TheMainFrame->SetLabel(MainFrame::TheMainFrame->m_original_title
                                    + _(" - unknown PIC type !"));
        aProgMemSizeText->SetEditable(true);
//        Ed_ProgramMemory->Color       = clWindow;
        aEepromMemSizeText->SetEditable(true);
//        Ed_DataEepromMemory->Color    = clWindow;
//        Ed_ConfigWordHex->Color       = clWindow;
        aHasFlashMemoryChk->Enable(true);    // dunno, let the user decide
    }  // end if <unknown PIC device type

    if (fUpdateHexWord)
    {
        // only if the user is not editing the hex config word(s) at the moment...
        // Show the currently used config-word and some of its elements:
        aConfigWordHexEdit->ChangeValue(wxString::Format(_T("%04X"), m_displayed_config_word[0]));
//      _stprintf(str80, _T("%04X"), m_displayed_config_word[0]);
//      Ed_ConfigWordHex->Text = str80;

        // Since 2003-12, the PIC16F88 is supported, it was the first midrange-PIC
        //                with TWO configuration words !
        aConfigWordHexEdit2->Enable(PIC_DeviceInfo.wCfgmask2_used != 0x0000);
//      if( PIC_DeviceInfo.wCfgmask2_used == 0x0000)  // looks like there is no 2nd cfg word:
//         Ed_ConfigWord2->Color = clBtnShadow;
//      else // 2nd config word seems to exist:
//         Ed_ConfigWord2->Color = clWindow;
        aConfigWordHexEdit2->ChangeValue(wxString::Format(_T("%04X"), PicBuf_GetConfigWord(1)));
//      _stprintf(str80, _T("%04X"), PicBuf_GetConfigWord(1) );
//      Ed_ConfigWord2->Text = str80;
    }

    // Since 2005-03-11 : Fill the string grid ("table") with special configuration bits (or bit groups):
    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("UpdateDeviceConfigTab: Updating config BIT GRID..") );
    UpdateConfigBitGrid();
//   if( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
//       APPL_LogEvent( _("UpdateDeviceConfigTab: Updating config BIT COMBO..") );
//   UpdateConfigBitCombo();

    aProgMemSizeText  ->ChangeValue(wxString::Format(_T("%d"), PIC_DeviceInfo.lCodeMemSize));
    aEepromMemSizeText->ChangeValue(wxString::Format(_T("%d"), PIC_DeviceInfo.lDataEEPROMSizeInByte));


    // Some new PIC devices need to preserve Bandgap Calibration bits
    //      and oscillator calibration words. Show info about this:
    aSavedBeforeEraseText->SetLabel(_T(""));
    aSavedBeforeEraseText->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
//   aSavedBeforeEraseText->Color   = clBtnFace;
//    aSavedBeforeEraseTitle->Enable(false);
    wxString  SavedData;
    if ( PIC_DeviceInfo.wCfgmask_bandgap!=0 ) // bandgap calibration bits, 0x3000 for 12F675
    {
//        aSavedBeforeEraseTitle->Enable(true);
      if(PIC_lBandgapCalibrationBits>=0)
        {
//            _stprintf(str80, _("bandgap_cal=0x%06lX"), PIC_lBandgapCalibrationBits);
//          Lab_SavedBeforeErase->Caption = str80;
            _stprintf(str80, _T("0x%06lX"), PIC_lBandgapCalibrationBits);
            psz = str80;
        }
        else
            psz = _("n/a");
        SavedData.Printf(_("bandgap_cal=%s"), psz);
//            Lab_SavedBeforeErase->Caption = _("bandgap_cal:n/a");
    }
    if ( PIC_DeviceInfo.lAddressOscCal>0 ) // oscillator calibration word, at 0x03FF for 12F675
    {
        if (!SavedData.IsEmpty())
            SavedData += _T("   ");
        // (usually the very last word in the CODE memory)
//        aSavedBeforeEraseTitle->Enable(true);
        if (PIC_lOscillatorCalibrationWord>=0)
        {
            _stprintf(str80, _T("0x%06lX"), PIC_lOscillatorCalibrationWord);
            psz = str80;
//            _stprintf(str80,_("  oscillator_cal=0x%06lX"),(long)PIC_lOscillatorCalibrationWord);
//          Lab_SavedBeforeErase->Caption = Lab_SavedBeforeErase->Caption + wxString(str80);
            if ( (PIC_lOscillatorCalibrationWord & 0xFF00) != 0x3400/*RETLW*/ )
                aSavedBeforeEraseText->SetForegroundColour(*wxRED);
//              Lab_SavedBeforeErase->Color = clRed;

        }
        else
            psz = _("n/a");
        SavedData += wxString::Format(_("  oscillator_cal:%s"), psz);
    }
    aSavedBeforeEraseText->SetLabel(SavedData);

    if ( CommandOption.WinPic_iTestMode & WP_TEST_MODE_GUI_SPEED )
        APPL_LogEvent( _("UpdateDeviceConfigTab: DONE .") );

    if (MainFrame::TheMainFrame->m_Updating>0)
        --(MainFrame::TheMainFrame->m_Updating);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TDeviceCfgPanel::UpdateConfigBitGrid(void)
// Shows the state of  "all important config bits" in string grid .
// These bits were once part of a single "CONFIG WORD" (for PIC16Fxx),
// but these bits may now be spread all over the CONFIGURATION REGISTERS .
// Most items in this table are single-bit locations, while others
// may be 2,3 or more bits, like the oscillator selection .
// Since 2005-03, the WinPic GUI(!) contains no "special treatment"
//  for these bit groups, because all def's may be loaded from files .
//  See DEVICES.CPP,  PicDev_FillConfigBitInfoTable() for details .
{
    int i,n;
//  wxChar str80[84];
    wxString s, old_msg;  // VCL-stuff !
    T_PicConfigBitInfo *pConfigBitInfo;
    T_PicConfigBitSetting *pBitCombinations;
    uint32_t dwConfigWord, dwConfigWordAddress, dwConfigMask;

    ++(MainFrame::TheMainFrame->m_Updating);

    old_msg = MainFrame::TheMainFrame->aStatusBar->GetStatusText();
    MainFrame::TheMainFrame->aStatusBar->SetStatusText(_("Updating config-BIT-grid")) ;
    MainFrame::TheMainFrame->Update();


//   SG_ConfigBits->RowCount=PICDEV_MAX_CONFIG_BIT_INFOS;
//   SG_ConfigBits->Cells[configbitsCOL_NAME][0] = _("Configuration Bit Group (Name)");
//   SG_ConfigBits->Cells[configbitsCOL_ADDR][0] = _("Addr");
//   SG_ConfigBits->Cells[configbitsCOL_MASK][0] = _("Mask");  // important debugging aid, also to add support for new devices
//   SG_ConfigBits->Cells[configbitsCOL_VALUE][0]= _("Setting");
    aDevCfgGrid->BeginBatch();
    int RowCount = aDevCfgGrid->GetNumberRows();
    if (RowCount > 0)
        aDevCfgGrid->DeleteRows(0, RowCount);

//   for(i=0; i<SG_ConfigBits->ColCount; ++i)  // clear line 1, we MAY not be able to fill it
//    {
//        SG_ConfigBits->Cells[i][1] = _T("");
//    }

    wxArrayString ConfigChoice;
    pConfigBitInfo = PicDev_ConfigBitInfo;
    i=n=0;
    while ( pConfigBitInfo->szText && pConfigBitInfo->szText[0] ) // Fill the String Grid ("TStringGrid with Combos")..
    {
        dwConfigWordAddress = pConfigBitInfo->dwAddress; // target address (like 0x002007 for PIC16Fxxx)
        dwConfigMask =   pConfigBitInfo->dwBitmask;    // bitmask (usually only ONE bit set in this value)
        // Only if the bitmask for this "config bit group" is NON-ZERO, it may be listed in the string grid..
        if (  dwConfigMask != 0 )
        {
            aDevCfgGrid->AppendRows();
            aDevCfgGrid->SetRowLabelValue(n, pConfigBitInfo->szText);
//          SG_ConfigBits->Cells[configbitsCOL_NAME][n+1] = pConfigBitInfo->sz60Text;
            aDevCfgGrid->SetCellValue(n, configbitsCOL_ADDR, wxString::Format(_T("%06lX"), dwConfigWordAddress));
//        sprintf(str80, _T("%06lX"), dwConfigWordAddress);
//        SG_ConfigBits->Cells[configbitsCOL_ADDR][n+1] = str80;
            aDevCfgGrid->SetCellValue(n, configbitsCOL_MASK, wxString::Format(_T("%06lX"), dwConfigMask));
//        sprintf(str80, _T("%06lX"), dwConfigMask);
//        SG_ConfigBits->Cells[configbitsCOL_MASK][n+1] = str80;
            m_pConfigBitGridRowNumber_to_ConfigBitInfoPtr[n]= pConfigBitInfo;
//        if( dwConfigWordAddress == 0x300005 )
//            dwConfigWordAddress =  dwConfigWordAddress;
            if ( PicBuf_GetBufferWord(dwConfigWordAddress, &dwConfigWord) > 0 )
            {
                s = PicDev_ConfigBitValueToString( pConfigBitInfo, dwConfigWord );
            }
            else // couldn't read this word from the config memory buffer -> disable checkmark
            {
                s = _("-inaccessible-");
            }
            aDevCfgGrid->SetCellValue(n, configbitsCOL_VALUE, s);
            pBitCombinations = pConfigBitInfo->pBitCombinations;
//        pBitCombinations = pConfigBitInfo->pBitCombinations;
//        iComboItemIndex = -1; // "item index" for the new combo list still unknown
//        iComboItemCount = 0;  // no entry in the combo list (yet)
//        CB_ConfigBits->Clear();
            if ( pBitCombinations ) // only if there is a chained list of "bit combinations"...
            {
                while (pBitCombinations!=NULL && ConfigChoice.Count()<100 && pBitCombinations->szComboText[0]!=0 )
                {
                    ConfigChoice.Add(pBitCombinations->szComboText);
//            { CB_ConfigBits->Items->Add( pBitCombinations->sz60ComboText );
//              if( strncmp( str60, pBitCombinations->sz60ComboText, 60 ) == 0 )
//               { iComboItemIndex = iComboItemCount;  // select this item in the combo !
//               }
                    pBitCombinations = pBitCombinations->pNext;
//              ++iComboItemCount;
                }
//           CB_ConfigBits->ItemIndex = iComboItemIndex;
//           fShowCombo = true;
            } // end if( pBitCombinations )
//      }
//     else // there is no "pointer to info about this configuration bit" ->
//      {
//         // leave fShowCombo=false to turn the combo box "off" for this line !
//      }
            aDevCfgGrid->SetCellEditor(n, configbitsCOL_VALUE, new wxGridCellChoiceEditor(ConfigChoice));
            ConfigChoice.Empty();

            ++n;
        } // end if < exactly ONE bit set in the bitmask, or a bunch of CP bits >
        ++i;
        ++pConfigBitInfo;
        if (i>PICDEV_MAX_CONFIG_BIT_INFOS)
            break;  // emergency break, should never happen
    } // end while( pConfigBitInfo->sz60Text[0] )
//   if( n>=1 )
//      SG_ConfigBits->RowCount = n+1;   // remove unused lines (rows) at end of grid
//   else
//    {
//      SG_ConfigBits->RowCount = 2;  // not one (looks too ugly)
//    }

    MainFrame::TheMainFrame->aStatusBar->SetStatusText(old_msg);
    MainFrame::TheMainFrame->Update();

    aDevCfgGrid->EndBatch();
    --(MainFrame::TheMainFrame->m_Updating);

} // end UpdateConfigBitGrid()

//---------------------------------------------------------------------------
void TDeviceCfgPanel::ApplyConfigBitGrid(void)
// Parses the "values" in the string grid of "important config bits" .
// This is -more or less- inverse to UpdateConfigBitGrid(), see above.
{
    int iGridRow;
    T_PicConfigBitInfo *pConfigBitInfo;
    uint32_t dwConfigWord, dwConfigWordAddress, dwConfigMask, dwNewValue;
    wxChar str80[84];

    if ( APPL_i32CustomizeOptions & APPL_CUST_NO_CONFIG_EDITOR )
        return;

    int RowCount = aDevCfgGrid->GetNumberRows();
    for (iGridRow=0; iGridRow<RowCount && iGridRow<PICDEV_MAX_CONFIG_BIT_INFOS; ++iGridRow)
    {
        // Get a pointer to the device-specific info about this "Group of CONFIG-BITS":
        //  Note: Due to Microchip's strange way to do things, these bits may bot even be ADJACENT,
        //        but -for WinPic- all bits of one group must reside in ONE CONFIG WORD (a 16 bit).
        //        An example for a non-adjacent group of bits is the osc selection
        //        in the PIC16F628 with dwConfigMask=0x0013 .
        pConfigBitInfo = m_pConfigBitGridRowNumber_to_ConfigBitInfoPtr[iGridRow];
        if ( pConfigBitInfo ) // can only "handle" this grid line with this device-specific config-bit info...
        {
//        _tcsncpy(str80, SG_ConfigBits->Cells[configbitsCOL_VALUE][iGridRow].c_str(), 80 );
            _tcsncpy(str80, aDevCfgGrid->GetCellValue(iGridRow, configbitsCOL_VALUE).c_str(), 80 );
            dwConfigWordAddress = pConfigBitInfo->dwAddress;  // target address (example: 0x002007 for PIC16Fxx)
            dwConfigMask   = pConfigBitInfo->dwBitmask;    // bitmask (usually only ONE bit set in this value)
            // Try to convert the text from the COMBO BOX of this config bit group
            // into a numeric value, which can then be combined with the present content
            // of this configuration register. Use the "bitmask" to change only those bits
            // which belong to this bit group !
            if ( PicDev_ConfigBitStringToValue( pConfigBitInfo, str80, &dwNewValue ) )
            {
                // only if the string value could be "interpreted" :
                if ( PicBuf_GetBufferWord(dwConfigWordAddress, &dwConfigWord) > 0 )
                {
                    // Note: even if the config memory is buffered as 16-bit WORDs,
                    //       dwConfigWordAddress may be an ODD value ! In that case,
                    //       PicBuf_GetBufferWord() + PicBuf_SetBufferWord() only access
                    //       the HIGH BYTE (bits 15..8) of the 16-bit buffer entry .
                    dwConfigWord = (dwConfigWord & ~dwConfigMask) | ( dwNewValue & dwConfigMask);
                    PicBuf_SetBufferWord(dwConfigWordAddress, dwConfigWord);
                    MainFrame::TheMainFrame->m_update_id_and_config_display = true;  // must update "the other tab" too (later)
                } // end if < successfully READ this configuration word >
            }
        } // end if < state of checkmark CHECKED or UNCHECKED, but not GRAYED >
    } // for < all elements of the CONFIG BIT CHECK LIST >

    // Note: If the above edits had an effect on the 1st and 2nd "Config Word",
    //     the TIMER METHOD(!) will detect the change a bit later
    //     and update "everything which is needed" to make things a bit simpler.
    //     So no reason to call UpdateDeviceConfigTab() or UpdateConfigBitGrid() here !

} // end ApplyConfigBitGrid()


////---------------------------------------------------------------------------
//// Poor man's "TStringGrid with Combos" ... without any 3rd-party's controls :
//// Based on an idea from http://www.functionx.com/bcb/howto/cboxinstringgrid.htm
////  This is clumsy but it works: "a TComboBox plastered over a TStringGrid" !
////---------------------------------------------------------------------------
//
//void MainFrame::UpdateConfigBitCombo(void)
//{
//  int iGridRow = SG_ConfigBits->Row;
//  int iComboItemIndex, iComboItemCount;
//  wxChar str60[64];
//  T_PicConfigBitInfo *pConfigBitInfo;
//  T_PicConfigBitSetting *pBitCombinations;
//  bool fShowCombo = false;
//
//  ++m_Updating;
//
//  if( SG_ConfigBits->Col == configbitsCOL_VALUE )
//   {
//     // Fill the COMBO for this line with new data,
//     //  using the device-specific info table (in Devices.cpp) .
//     // Get a pointer to the device-specific info about this "CONFIG-BIT":
//     pConfigBitInfo = m_pConfigBitGridRowNumber_to_ConfigBitInfoPtr[iGridRow];
//     if( pConfigBitInfo )  // can only "handle" this grid line with this device-specific config-bit info...
//      { strncpy(str60, SG_ConfigBits->Cells[configbitsCOL_VALUE][iGridRow].c_str(), 60 ); // string to select in combo
//        str60[60] = '\0';  // important for string compare, if strings too long
//        pBitCombinations = pConfigBitInfo->pBitCombinations;
//        iComboItemIndex = -1; // "item index" for the new combo list still unknown
//        iComboItemCount = 0;  // no entry in the combo list (yet)
//        CB_ConfigBits->Clear();
//        if( pBitCombinations )  // only if there is a chained list of "bit combinations"...
//         {
//           while(pBitCombinations!=NULL && iComboItemCount<100 && pBitCombinations->sz60ComboText[0]!=0 )
//            { CB_ConfigBits->Items->Add( pBitCombinations->sz60ComboText );
//              if( strncmp( str60, pBitCombinations->sz60ComboText, 60 ) == 0 )
//               { iComboItemIndex = iComboItemCount;  // select this item in the combo !
//               }
//              pBitCombinations = pBitCombinations->pNext;
//              ++iComboItemCount;
//            }
//           CB_ConfigBits->ItemIndex = iComboItemIndex;
//           fShowCombo = true;
//         } // end if( pBitCombinations )
//      }
//     else // there is no "pointer to info about this configuration bit" ->
//      {
//         // leave fShowCombo=false to turn the combo box "off" for this line !
//      }
//   }  // end if( SG_ConfigBits->Col == configbitsCOL_VALUE )
//
//  if( fShowCombo )   // Ok to show the COMBO BOX for this line of the config-bit-grid ?
//   { // This "TStringGrid with COMBOS" is actually an ordinary standard String Grid control,
//     // with a TComboBox "plastered" over it. To let the combo look like a part of the grid,
//     // get the screen coordinates of a cell in the grid (CellRect)
//     // and place the TComboBox there .
//     TRect CellRect = SG_ConfigBits->CellRect( SG_ConfigBits->Col, SG_ConfigBits->Row);
// //  CB_ConfigBits->Top  = SG_ConfigBits->Top;   // why ?
// //  CB_ConfigBits->Left = SG_ConfigBits->Left;
// //  CB_ConfigBits->Top  = CB_ConfigBits->Top + CellRect.Top + SG_ConfigBits->GridLineWidth;
// //  CB_ConfigBits->Left = CB_ConfigBits->Left + CellRect.Left + SG_ConfigBits->GridLineWidth + 1;
//     CB_ConfigBits->Top  = SG_ConfigBits->Top  + CellRect.Top  + SG_ConfigBits->GridLineWidth + 1;
//     CB_ConfigBits->Left = SG_ConfigBits->Left + CellRect.Left + SG_ConfigBits->GridLineWidth + 1;
//     CB_ConfigBits->Height = (CellRect.Bottom - CellRect.Top) + 1;    // not accepted by ComboBox ?!
//     CB_ConfigBits->Width  = CellRect.Right - CellRect.Left;
//     CB_ConfigBits->Visible = True;
//   }
//  else // fShowCombo=false..
//   { CB_ConfigBits->Visible = False;
//   }
//
//  --m_Updating;
//
//} // end MainFrame::UpdateConfigBitCombo()

////---------------------------------------------------------------------------
//void MainFrame::SG_ConfigBitsClick(TObject *Sender)
//{ // Called when user selected a cell (row or column) in the "configuration bit grid" .
//  if( m_Updating )
//    return;
//  UpdateConfigBitCombo(); // adjust the TComboBox in this "Poor Man's TStringGrid with COMBOs" ;-)
//} // SG_ConfigBitsClick()
//---------------------------------------------------------------------------

//void MainFrame::SG_ConfigBitsTopLeftChanged(TObject *Sender)
//{ // > Use OnTopLeftChanged to perform special processing
//  // > when the non-fixed cells in the grid are scrolled.
//  // Here: Make sure the TComboBox is scrolled along with the grid cell:
//  if( m_Updating )
//    return;
//  UpdateConfigBitCombo();
//}
//---------------------------------------------------------------------------

////---------------------------------------------------------------------------
//void MainFrame::TS_DeviceConfigResize(TObject *Sender)
//{
// int iNewTableWidth = TS_DeviceConfig->Width - 20;
// int iWidth0;
//  // ... similar as above after RESIZING the tabsheet with the config bits:
//  if( m_Updating )
//    return;
//  if( iNewTableWidth>200 )
//   {
//    iWidth0 = (iNewTableWidth-90)/2;
//    if( iWidth0 > 250 )
//        iWidth0 = 250;
//    SG_ConfigBits->ColWidths[0] = iWidth0;
//    SG_ConfigBits->ColWidths[1] = 45;
//    SG_ConfigBits->ColWidths[2] = 45;
//    SG_ConfigBits->ColWidths[3] = iNewTableWidth-100-iWidth0;
//   }
//  UpdateConfigBitCombo();
//}
////---------------------------------------------------------------------------


/**static*/ void TDeviceCfgPanel::SetDevice(const char *pDeviceName)
{
    T_PicDeviceInfo MyDeviceInfo;

    wxColour Colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);
    if ( PicDev_GetDeviceInfoByName(pDeviceName, &MyDeviceInfo) < 0 )
        PIC_PRG_SetDeviceType( PIC_DEV_TYPE_UNKNOWN );
    else
    {
        if (!PIC_PRG_SetDeviceType(&MyDeviceInfo))
        {
            MainFrame::TheMainFrame->aNotebook->SetSelection(MainFrame::TS_Options);
            MainFrame::TheMainFrame->aOptionTab->aMPLabDevDirButton->SetFocus();
            Colour = *wxRED;
        }
    }
    if (Colour != MainFrame::TheMainFrame->aOptionTab->aMplabDirLabel->GetForegroundColour())
    {
        MainFrame::TheMainFrame->aOptionTab->aMplabDirLabel->SetForegroundColour(Colour);
        MainFrame::TheMainFrame->aOptionTab->aMplabDirLabel->Refresh();
    }
}


void TDeviceCfgPanel::onPartNameChoiceSelect(wxCommandEvent& event)
//void MainFrame::Combo_PartNameChange(TObject *Sender)
{
//    T_PicDeviceInfo MyDeviceInfo;

    if (MainFrame::TheMainFrame->m_Updating) return;
    wxCharBuffer DeviceName = aPartNameChoice->GetStringSelection().mb_str(wxConvISO8859_1);
    SetDevice(DeviceName);
//    if ( PicDev_GetDeviceInfoByName(DeviceName, &MyDeviceInfo ) >= 0 )
////  if( PicDev_GetDeviceInfoByName( Combo_PartName->Text.c_str(), &MyDeviceInfo ) >= 0 )
//        PIC_PRG_SetDeviceType( &MyDeviceInfo );
//    else PIC_PRG_SetDeviceType( PIC_DEV_TYPE_UNKNOWN );
    UpdateDeviceConfigTab( true/*fUpdateHexWord*/ );
    MainFrame::TheMainFrame->aOptionTab->UpdateOptionsDisplay();    // a lot may have changed, depending on chip type
    MainFrame::TheMainFrame->aConfigMemoryTab->UpdateIdAndConfMemDisplay(); // number of 'valid locations' may have changed
    MainFrame::TheMainFrame->aCodeMemTab->UpdateCodeMemDisplay();    // addresses and instruction width may have changed
    MainFrame::TheMainFrame->aDataMemTab->UpdateDataMemDisplay();    // address range for DATA(~EEPROM) memory may have changed

    // Now, ONLY HERE AFTER THE USER SELECTED A NEW DEVICE, check the 'Vdd/Vpp'
    // sequence. If it is defined in the device settings, and is incompatible
    // to the current state on the "Other Options" panel, show a warning .
    // Why not simply switch without asking ? Because for many devices,
    //   the proper Vdd/Vpp switching sequence is UNKNOWN or NOWHERE SPECIFIED !
    if ( ( (PIC_DeviceInfo.wVppVddSequence==PROGMODE_VPP_THEN_VDD/*0*/) && (Config.iNeedPowerBeforeRaisingMCLR) )
            ||( (PIC_DeviceInfo.wVppVddSequence==PROGMODE_VDD_THEN_VPP/*1*/) && (!Config.iNeedPowerBeforeRaisingMCLR) )
       )
    {
        // this message should occur after switching from 16F628 to F818 (for example)
        MainFrame::TheMainFrame->aNotebook->SetSelection(MainFrame::TS_Options);
//    PageControl1->ActivePage = TS_Options;
        MainFrame::TheMainFrame->aOptionTab->aVddBeforeMCLRChk->SetForegroundColour(*wxRED);
//    Chk_VddBeforeMCLR->Color = clRed;
        if (wxMessageBox(
                    _("Conflicting Vpp/Vdd switching sequence on the Options tab.\n Change the sequence to match the new device info ?"),
                    _("Vpp/Vdd switching sequence possibly wrong"),
                    wxICON_QUESTION | wxYES_NO | wxCANCEL ) == wxYES )
        {
            // user agrees to let the program select the 'proper'(??) setting...
            if (PIC_DeviceInfo.wVppVddSequence==PROGMODE_VDD_THEN_VPP)
                Config.iNeedPowerBeforeRaisingMCLR = 1;
            else
                Config.iNeedPowerBeforeRaisingMCLR = 0;
        }
        UpdateDeviceConfigTab( true/*fUpdateHexWord*/ );
        MainFrame::TheMainFrame->aOptionTab->UpdateOptionsDisplay();
        MainFrame::TheMainFrame->aOptionTab->aVddBeforeMCLRChk->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
//    Chk_VddBeforeMCLR->Color = clBtnFace;
        MainFrame::TheMainFrame->Update();
        wxMilliSleep(500);
        MainFrame::TheMainFrame->aNotebook->ChangeSelection(MainFrame::TS_DeviceConfig);
//    PageControl1->ActivePage = TS_DeviceConfig;
    } // end if < Vpp/Vdd switching sequence POSSIBLY wrong >
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TDeviceCfgPanel::onConfigWordHexEditText(wxCommandEvent& event)
//void ::Ed_ConfigWordHexChange(TObject *Sender)
{
// char str8[9];
// long l;

    if (MainFrame::TheMainFrame->m_Updating)
        return;
    if ( APPL_i32CustomizeOptions & APPL_CUST_NO_CONFIG_EDITOR )
        return;

    long ConfigWord;
    if (aConfigWordHexEdit->GetValue().ToLong(&ConfigWord,16))
        aConfigWordHexEdit->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    else
    {
        aConfigWordHexEdit->SetBackgroundColour(wxColour(0xFF,0x80,0x80));
        return;
    }

//  strncpy(str8,Ed_ConfigWordHex->Text.c_str(), 8);
//  l = HexStringToLongint(6, str8);  // usually the "config word" has 4 digits only
//  if(l<0) return; // someone entered an invalid hex config word .. ignore it !

    // Immediately 'decode' the new configuration word..
    ++(MainFrame::TheMainFrame->m_Updating);
    PicBuf_SetConfigWord( 0 , ConfigWord );
    UpdateDeviceConfigTab( false/*fUpdateHexWord*/ ); // don't disturb editing !
    if (MainFrame::TheMainFrame->m_Updating>0)
        --(MainFrame::TheMainFrame->m_Updating);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TDeviceCfgPanel::onConfigWordHexEdit2Text(wxCommandEvent& event)
//void ::Ed_ConfigWord2Change(TObject *Sender)
{
// char str8[9];
// long l;

    if (MainFrame::TheMainFrame->m_Updating) return;

    long ConfigWord;
    if (aConfigWordHexEdit2->GetValue().ToLong(&ConfigWord,16))
        aConfigWordHexEdit2->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    else
    {
        aConfigWordHexEdit2->SetBackgroundColour(wxColour(0xFF,0x80,0x80));
        return;
    }
//  strncpy(str8,Ed_ConfigWord2->Text.c_str(), 8);
//  l = HexStringToLongint(6, str8);
//  if(l<0) return; // someone entered an invalid 4-digit hex config word .. ignore

    // Immediately 'decode' the new configuration word..
    ++(MainFrame::TheMainFrame->m_Updating);
    PicBuf_SetConfigWord( 1/*!*/, ConfigWord ) ;
    UpdateDeviceConfigTab( false );
    if (MainFrame::TheMainFrame->m_Updating>0)
        --(MainFrame::TheMainFrame->m_Updating);

}
//---------------------------------------------------------------------------



void TDeviceCfgPanel::onProgMemSizeTextText(wxCommandEvent& event)
//void ::Ed_ProgramMemoryChange(TObject *Sender)
{
// bool fCorrected = false;
    if (MainFrame::TheMainFrame->m_Updating)
        return;

    aProgMemSizeText->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
//  Ed_ProgramMemory->Color = clWhite;
    if ( tolower(PIC_DeviceInfo.sz40DeviceName[0])=='u') // "unknown" ?
    {
        long Size;
        if (aProgMemSizeText->GetValue().ToLong(&Size)
                &&  (Size < PIC_BUF_CODE_SIZE)
                &&  (Size > 0))
        {
            Config.dwUnknownCodeMemorySize = Size;
//    Config.dwUnknownCodeMemorySize = StrToIntDef(Ed_ProgramMemory->Text,
//       Config.dwUnknownCodeMemorySize );
//    if(Config.dwUnknownCodeMemorySize > PIC_BUF_CODE_SIZE)
//     {
//       Config.dwUnknownCodeMemorySize = PIC_BUF_CODE_SIZE;
//       fCorrected = true;
//     }
//    if(Config.dwUnknownCodeMemorySize < 1)
//     {
//       Config.dwUnknownCodeMemorySize = 1;
//       fCorrected = true;
//     }
            PIC_DeviceInfo.lCodeMemSize = Config.dwUnknownCodeMemorySize;
        }
        else
            aProgMemSizeText->SetBackgroundColour(wxColour(0xFF,0x7F,0x7F));
//   }
//  if(fCorrected)
//   {
//     ++m_Updating;
//     Ed_ProgramMemory->Color = clRed;
//     Ed_ProgramMemory->Text  = IntToStr(Config.dwUnknownCodeMemorySize);
//     --m_Updating;
    }
}
//---------------------------------------------------------------------------



void TDeviceCfgPanel::onEepromMemSizeTextText(wxCommandEvent& event)
//void ::Ed_DataEepromMemoryChange(TObject *Sender)
{
    if (MainFrame::TheMainFrame->m_Updating)
        return;
    aEepromMemSizeText->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    if ( tolower(PIC_DeviceInfo.sz40DeviceName[0])=='u') // "unknown" ?
    {
        long Size;
        if (aEepromMemSizeText->GetValue().ToLong(&Size)
                && (Size >= 0))
            PIC_DeviceInfo.lDataEEPROMSizeInByte = Config.dwUnknownDataMemorySize = Size;
//    PIC_DeviceInfo.lDataEEPROMSizeInByte = Config.dwUnknownDataMemorySize =
//      StrToIntDef(Ed_DataEepromMemory->Text, Config.dwUnknownDataMemorySize);
        else
            aEepromMemSizeText->SetBackgroundColour(wxColour(0xFF,0x7F,0x7F));
    }
}
//---------------------------------------------------------------------------



void TDeviceCfgPanel::onHasFlashMemoryChkClick(wxCommandEvent& event)
//void ::Chk_HasFlashMemoryClick(TObject *Sender)
{
    if (MainFrame::TheMainFrame->m_Updating)
        return;
    if (aHasFlashMemoryChk->GetValue())
//  if(Chk_HasFlashMemory->Checked)
    {
        // which kind of FLASH memory, 12..14 bit per location, or more ?
        PIC_DeviceInfo.iCodeMemType=PIC_MT_FLASH;   // 12Fxxx, 16Fxxx -> 14 bit per word
        Config.iUnknownDevHasFlashMemory=1;
    }
    else
    {
        PIC_DeviceInfo.iCodeMemType=PIC_MT_EPROM;
        Config.iUnknownDevHasFlashMemory=0;
    }
}
//---------------------------------------------------------------------------



void TDeviceCfgPanel::onDevCfgGridCellChange(wxGridEvent& event)
//void ::CB_ConfigBitsChange(TObject *Sender)
{
    // Called when the user(?) selected an item from the "plastered" configuration bit COMBO.
    int   iGridRow = event.GetRow();
    uint32_t dwAddr;
//  const wxChar  *cp;
//  wxChar  str80[84];

    if ( MainFrame::TheMainFrame->m_Updating )
        return;   // it was not the user ("clicking"), but a programmed access to the COMBO

    ++(MainFrame::TheMainFrame->m_Updating);

//  // Here: Copy the "new" selected item as text into the string grid ..
//  SG_ConfigBits->Cells[SG_ConfigBits->Col][iGridRow] =
//        CB_ConfigBits->Items->Strings[CB_ConfigBits->ItemIndex];

    // .. and check the ADDRESS of this config bit, to mark this buffer entry as "used" :
//  _tcsncpy(str80, SG_ConfigBits->Cells[configbitsCOL_ADDR][iGridRow].c_str(), 80 );
//  cp = str80;
    wxString EnteredValue = aDevCfgGrid->GetCellValue(iGridRow, configbitsCOL_ADDR);
    if (EnteredValue.ToLong( (long*)&dwAddr, 16))
//  if( PicHex_GetHexValueFromSource( &cp, &dwAddr ) )  // parsed a valid address ?
    {
        PicBuf_SetMemoryFlags(dwAddr, PicBuf_GetMemoryFlags(dwAddr) | PIC_BUFFER_FLAG_USED );
    }

    // .. and, since the grid has been modified, it must be "applied"
    //  because other visible controls shall reflect the new setting too :
    ApplyConfigBitGrid();  // "apply" the values in the grid and write them to the config memory buffer
    // Note: If this has an effect on the 1st and 2nd "Config Word",
    //     the TIMER METHOD(!) will detect the change a bit later
    //     and update the display accordingly .

    --(MainFrame::TheMainFrame->m_Updating);

} // end CB_ConfigBitsChange()
//---------------------------------------------------------------------------


