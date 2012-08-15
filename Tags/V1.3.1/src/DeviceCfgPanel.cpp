/*-------------------------------------------------------------------------*/
/*  Filename: DeviceCfgPanel.cpp                                           */
/*                                                                         */
/*  Purpose:                                                               */
/*     Implementation of DeviceCfgPanel.h                                  */
/*                                                                         */
/*  Author:                                                                */
/*     Copyright 2009-2011 Philippe Chevrier pch @ laposte.net             */
/*     from software originally written by Wolfgang Buescher (DL4YHF)      */
/*                                                                         */
/*  License:                                                               */
/*     Ported Code is licensed under GPLV3 conditions with original code   */
/*     restriction :                                                       */
/*     Use of this sourcecode for commercial purposes strictly forbidden ! */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#include "DeviceCfgPanel.h"
#include "MainFrame.h"
#include "Appl.h"
#include "Config.h"

//(*InternalHeaders(TDeviceCfgPanel)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)
#include <wx/settings.h>
#include <wx/msgdlg.h>

#include <WinPicPr/PIC_PRG.h>
#include <WinPicPr/PIC_HEX.h>

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

    for (int i=0; i<PICDEV_MAX_CONFIG_BIT_INFOS; ++i)
        m_pConfigBitGridRowNumber_to_ConfigBitInfoPtr[i] = NULL;
}

TDeviceCfgPanel::~TDeviceCfgPanel()
{
	//(*Destroy(TDeviceCfgPanel)
	//*)
}


static bool adjustedBitMask (T_PicConfigBitInfo *pConfigBitInfo, uint32_t &pConfigWordIndex, uint32_t &pConfigMask, uint32_t *pConfigWordAddress = NULL)
{
    // Note: even if the config memory is buffered as 16-bit WORDs,
    //       ConfigWordIndex may be an ODD value ! In that case,
    //       The mask and the values must be converted to
    //       the HIGH BYTE (bits 15..8).
    bool     Result            = false;
    uint32_t ConfigWordAddress = pConfigBitInfo->dwAddress;
    pConfigMask                = pConfigBitInfo->dwBitmask;    // bitmask (usually only ONE bit set in this value)
    pConfigWordIndex           = ConfigWordAddress - PIC_DeviceInfo.lConfWordAdr;  // target offset to first config word

    if (PicBuf[PIC_BUF_CONFIG].dwAddressFactor > 1)
    {
        Result = ((pConfigWordIndex & 1) != 0);
        if (Result)
            pConfigMask <<= 8;

        pConfigWordIndex >>= 1;
        ConfigWordAddress &= ~1;
    }
    if (pConfigWordAddress != NULL)
        *pConfigWordAddress = ConfigWordAddress;
    return Result;
}


//---------------------------------------------------------------------------
void TDeviceCfgPanel::UpdateDeviceConfigTab(bool fUpdateHexWord)
{
    wxChar str80[81];
    const wxChar *psz;
    bool fFoundDevName;

    ++(MainFrame::TheMainFrame->m_Updating);

    // Show the currently selected PIC type in the "Part"-combo list.
    // ex: Be sure that the item index matches the definition of
    //     PIC_DEV_TYPE_xxxx in PIC_PRG.h !!
    // Now all types in the list are filled during run-time from a table..
    fFoundDevName = aPartNameChoice->SetStringSelection(TSessionConfig::GetDeviceName());

    aHasFlashMemoryChk->SetValue(PIC_DeviceInfo.iCodeMemType==PIC_MT_FLASH) ;
    if (fFoundDevName && (TSessionConfig::GetDeviceName()[0]!=_T('u')) )
    {
        MainFrame::TheMainFrame->SetLabel(MainFrame::TheMainFrame->m_original_title
                                    + _T(" - ") + aPartNameChoice->GetStringSelection());
        aProgMemSizeText->SetEditable(false);
        aEepromMemSizeText->SetEditable(false);
        aHasFlashMemoryChk->Enable(false);    // we know if it's possible
    }
    else // unknown PIC device type.
    {
        // No Config word decoding, but "everything goes"...
        MainFrame::TheMainFrame->SetLabel(MainFrame::TheMainFrame->m_original_title
                                    + _(" - unknown PIC type !"));
        aProgMemSizeText->SetEditable(true);
        aEepromMemSizeText->SetEditable(true);
        aHasFlashMemoryChk->Enable(true);    // dunno, let the user decide
    }  // end if <unknown PIC device type

    if (fUpdateHexWord)
        updateCfgWords();

    // Fill the string grid ("table") with special configuration bits
    UpdateConfigBitGrid(/*Rebuild*/true);

    aProgMemSizeText  ->ChangeValue(wxString::Format(_T("%d"), PIC_DeviceInfo.lCodeMemSize));
    aEepromMemSizeText->ChangeValue(wxString::Format(_T("%d"), PIC_DeviceInfo.lDataEEPROMSizeInByte));


    // Some new PIC devices need to preserve Bandgap Calibration bits
    //      and oscillator calibration words. Show info about this:
    aSavedBeforeEraseText->SetLabel(_T(""));
    aSavedBeforeEraseText->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    wxString  SavedData;
    if ( PIC_DeviceInfo.wCfgmask_bandgap!=0 ) // bandgap calibration bits, 0x3000 for 12F675
    {
      if(PIC_lBandgapCalibrationBits>=0)
        {
            _stprintf(str80, _T("0x%06lX"), PIC_lBandgapCalibrationBits);
            psz = str80;
        }
        else
            psz = _("n/a");
        SavedData.Printf(_("bandgap_cal=%s"), psz);
    }
    if ( PIC_DeviceInfo.lAddressOscCal>0 ) // oscillator calibration word, at 0x03FF for 12F675
    {
        if (!SavedData.IsEmpty())
            SavedData += _T("   ");
        // (usually the very last word in the CODE memory)
        if (PIC_lOscillatorCalibrationWord>=0)
        {
            _stprintf(str80, _T("0x%06lX"), PIC_lOscillatorCalibrationWord);
            psz = str80;
            if ( (PIC_lOscillatorCalibrationWord & 0xFF00) != 0x3400/*RETLW*/ )
                aSavedBeforeEraseText->SetForegroundColour(*wxRED);

        }
        else
            psz = _("n/a");
        SavedData += wxString::Format(_("  oscillator_cal:%s"), psz);
    }
    aSavedBeforeEraseText->SetLabel(SavedData);

    if (MainFrame::TheMainFrame->m_Updating>0)
        --(MainFrame::TheMainFrame->m_Updating);
}
//---------------------------------------------------------------------------


uint32_t *TDeviceCfgPanel::getConfigWord (int pConfigWordIndex)
{
    return MainFrame::TheMainFrame->aConfigMemoryTab->GetConfigWord(pConfigWordIndex);
}

//---------------------------------------------------------------------------


void TDeviceCfgPanel::updateCfgWords (void)
{
    // only if the user is not editing the hex config word(s) at the moment...
    // Show the currently used config-word and some of its elements:
    aConfigWordHexEdit->ChangeValue( wxString::Format(_T("%04X"), *getConfigWord(0)) );

    // Since 2003-12, the PIC16F88 is supported, it was the first midrange-PIC
    //                with TWO configuration words !
    bool HasTwoConfigWord = (PIC_DeviceInfo.wCfgmask2_used != 0x0000);
    aConfigWordHexEdit2->Enable(HasTwoConfigWord);
    aConfigWordHexEdit2->ChangeValue((HasTwoConfigWord)
                                     ? wxString::Format(_T("%04X"), *getConfigWord(1) )
                                     : wxString(wxEmptyString) );
}


//---------------------------------------------------------------------------
void TDeviceCfgPanel::UpdateConfigBitGrid (bool pRebuild)
// Shows the state of  "all important config bits" in string grid .
// These bits were once part of a single "CONFIG WORD" (for PIC16Fxx),
// but these bits may now be spread all over the CONFIGURATION REGISTERS .
// Most items in this table are single-bit locations, while others
// may be 2,3 or more bits, like the oscillator selection .
// Since 2005-03, the WinPic GUI(!) contains no "special treatment"
//  for these bit groups, because all def's may be loaded from files .
//  See DEVICES.CPP,  PicDev_FillConfigBitInfoTable() for details .
{
    T_PicConfigBitInfo *ConfigBitInfo;

    ++(MainFrame::TheMainFrame->m_Updating);

    aDevCfgGrid->BeginBatch();
    int RowCount = aDevCfgGrid->GetNumberRows();
    if ((RowCount > 0) && pRebuild)
        aDevCfgGrid->DeleteRows(0, RowCount);

    wxArrayString ConfigChoice;
    ConfigBitInfo = PicDev_ConfigBitInfo;
    int CurRow = 0;
    while ( ConfigBitInfo->szText && ConfigBitInfo->szText[0] )
    {
        uint32_t ConfigWordAddress;// Address of the config word
        uint32_t ConfigWordIndex;  // target offset to first config word
        uint32_t ConfigMask;       // bitmask (usually only ONE bit set in this value)

        bool MustAdjust = adjustedBitMask(ConfigBitInfo, ConfigWordIndex, ConfigMask, &ConfigWordAddress);

        // Only if the bitmask for this "config bit group" is NON-ZERO, it may be listed in the string grid..
        if (ConfigMask != 0)
        {
            if (pRebuild)
            {
                aDevCfgGrid->AppendRows();
                aDevCfgGrid->SetRowLabelValue(CurRow, ConfigBitInfo->szText);
                aDevCfgGrid->SetCellValue(CurRow, configbitsCOL_ADDR, wxString::Format(_T("%06lX"), ConfigWordAddress));
                aDevCfgGrid->SetCellValue(CurRow, configbitsCOL_MASK, wxString::Format(_T("%06lX"), ConfigMask));
                m_pConfigBitGridRowNumber_to_ConfigBitInfoPtr[CurRow]= ConfigBitInfo;
            }

            //-- Update the cell value
            uint32_t ConfigWordValue = *getConfigWord(ConfigWordIndex);
            if (MustAdjust)
                //-- The bits are on the High Byte, move them to low byte
                ConfigWordValue   >>= 8;
            aDevCfgGrid->SetCellValue(CurRow, configbitsCOL_VALUE, PicDev_ConfigBitValueToString(ConfigBitInfo, ConfigWordValue) );

            if (pRebuild)
            {
                //-- Add the possible enumerated values for the cell editor
                T_PicConfigBitSetting *pBitCombinations = ConfigBitInfo->pBitCombinations;
                if ( pBitCombinations ) // only if there is a chained list of "bit combinations"...
                {
                    while ( (pBitCombinations != NULL) && (pBitCombinations->szComboText[0] != 0) )
                    {
                        ConfigChoice.Add(pBitCombinations->szComboText);
                        pBitCombinations = pBitCombinations->pNext;
                    }
                } // end if( pBitCombinations )
                aDevCfgGrid->SetCellEditor(CurRow, configbitsCOL_VALUE, new wxGridCellChoiceEditor(ConfigChoice));
                ConfigChoice.Empty();
            }

            ++CurRow;
        } // end if < exactly ONE bit set in the bitmask, or a bunch of CP bits >
        ++ConfigBitInfo;
    } // end while( ConfigBitInfo->szText && ConfigBitInfo->szText[0] )

    aDevCfgGrid->EndBatch();
    --(MainFrame::TheMainFrame->m_Updating);

} // end UpdateConfigBitGrid()


void TDeviceCfgPanel::UpdateCfgWordValue (void)
{
    updateCfgWords();
    UpdateConfigBitGrid(/*Rebuild*/false);
}


//---------------------------------------------------------------------------
void TDeviceCfgPanel::ApplyConfigBitGrid(void)
// Parses the "values" in the string grid of "important config bits" .
// This is -more or less- inverse to UpdateConfigBitGrid(), see above.
{
    if ( APPL_i32CustomizeOptions & APPL_CUST_NO_CONFIG_EDITOR )
        return;

    int RowCount = aDevCfgGrid->GetNumberRows();
    for (int iGridRow=0; iGridRow<RowCount && iGridRow<PICDEV_MAX_CONFIG_BIT_INFOS; ++iGridRow)
    {
        // Get a pointer to the device-specific info about this "Group of CONFIG-BITS":
        //  Note: Due to Microchip's strange way to do things, these bits may bot even be ADJACENT,
        //        but -for WxPic- all bits of one group must reside in ONE CONFIG WORD (a 16 bit).
        //        An example for a non-adjacent group of bits is the osc selection
        //        in the PIC16F628 with dwConfigMask=0x0013 .
        T_PicConfigBitInfo *ConfigBitInfo = m_pConfigBitGridRowNumber_to_ConfigBitInfoPtr[iGridRow];
        if ( ConfigBitInfo ) // can only "handle" this grid line with this device-specific config-bit info...
        {
            // Try to convert the text from the COMBO BOX of this config bit group
            // into a numeric value, which can then be combined with the present content
            // of this configuration register. Use the "bitmask" to change only those bits
            // which belong to this bit group !
            uint32_t NewValue;
            if ( PicDev_ConfigBitStringToValue( ConfigBitInfo,
                                               aDevCfgGrid->GetCellValue(iGridRow, configbitsCOL_VALUE).c_str(),
                                               &NewValue ) )
            {
                // only if the string value could be "interpreted" :

                uint32_t ConfigWordIndex;  // target offset to first config word
                uint32_t ConfigMask;       // bitmask (usually only ONE bit set in this value)
                if (adjustedBitMask(ConfigBitInfo, ConfigWordIndex, ConfigMask))
                    //-- The bits are on the High Byte
                    NewValue   <<= 8;

                uint32_t &EditedCfgWord = *getConfigWord(ConfigWordIndex);
                EditedCfgWord = (EditedCfgWord & ~ConfigMask) | ( NewValue & ConfigMask);
                updateCfgWords();       //-- Update the Hexa Cfg Words in accordance with flag change
                MainFrame::TheMainFrame->m_update_id_and_config_display = true;  // must update "the other tab" too (later)
            }
        } // end if < state of checkmark CHECKED or UNCHECKED, but not GRAYED >
    } // for < all elements of the CONFIG BIT CHECK LIST >
} // end ApplyConfigBitGrid()


void TDeviceCfgPanel::editConfigWord(int pCfgWordIndex, wxTextCtrl* pEditCtrl)
{
    if (MainFrame::TheMainFrame->m_Updating)
        return;

    long ConfigWord;
    pEditCtrl->Refresh();
    if (pEditCtrl->GetValue().ToLong(&ConfigWord,16))
        pEditCtrl->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    else
    {
        pEditCtrl->SetBackgroundColour(wxColour(0xFF,0x80,0x80));
        return;
    }

    // Immediately 'decode' the new configuration word..
    ++(MainFrame::TheMainFrame->m_Updating);

    *getConfigWord(pCfgWordIndex) = ConfigWord;
    UpdateConfigBitGrid(/*Rebuild*/false);
    MainFrame::TheMainFrame->m_update_id_and_config_display = true;  // must update "the other tab" too (later)

    if (MainFrame::TheMainFrame->m_Updating>0)
        --(MainFrame::TheMainFrame->m_Updating);
}


/**static*/ bool TDeviceCfgPanel::UpdateDevice(void)
{
    T_PicDeviceInfo MyDeviceInfo;

#ifdef __WXMSW__
    wxString MplabDevDir = TSessionConfig::GetMplabDevDir();
    if (MplabDevDir.IsEmpty())
    {

        static struct TKeyDef
        {
            HKEY          Root;
            const wxChar *Path;
            const wxChar *Name;
        }
        KeyDefTab[] =
        {
            { HKEY_CURRENT_USER,  _T("Software\\Microchip\\MPLAB IDE\\ProcAbout\\MasterDB"), _T("ModulePath")  },
            { HKEY_LOCAL_MACHINE, _T("Software\\Microchip\\MPLAB IDE"),                      _T("DeviceDBDir") },
        };
        enum
        {
             KEY_COUNT = sizeof(KeyDefTab)/sizeof(struct TKeyDef),
        };
        for (int i = 0; (i < KEY_COUNT) && MplabDevDir.IsEmpty(); ++i)
        {
            HKEY  Key;
            DWORD Length = 256;
            bool  Result = false;

            if (RegOpenKeyEx(KeyDefTab[i].Root, KeyDefTab[i].Path, 0, KEY_READ, &Key) == ERROR_SUCCESS)
            {
                Result = (RegQueryValueEx (Key, KeyDefTab[i].Name, NULL, NULL, (LPBYTE)MplabDevDir.GetWriteBuf(Length), &Length) == ERROR_SUCCESS);
                MplabDevDir.UngetWriteBuf( (Result) ? Length : 0 );
            }
        }
        TSessionConfig::SetMplabDevDir(MplabDevDir);
    }
#endif

    wxCharBuffer DeviceName   = wxString(TSessionConfig::GetDeviceName()).mb_str(wxConvISO8859_1);
    bool         Result       = ( PicDev_GetDeviceInfoByName(DeviceName, &MyDeviceInfo) >= 0 );
    bool         MplabDirIsOK = true;

    if ( ! Result )
        PIC_PRG_SetDeviceType( PIC_DEV_TYPE_UNKNOWN );
    else
        MplabDirIsOK = PIC_PRG_SetDeviceType(&MyDeviceInfo);

    MainFrame::TheMainFrame->aOptionTab->UpdateMplabDevColour(MplabDirIsOK);

    static bool Initialized = false;
    if (!Initialized)
    {
        Initialized = true;
        PIC_HEX_ClearBuffers();
    }
    MainFrame::TheMainFrame->UpdateAllSheets();
    return Result;
}


void TDeviceCfgPanel::onPartNameChoiceSelect(wxCommandEvent& event)
{
    if (MainFrame::TheMainFrame->m_Updating) return;
    TSessionConfig::SetDeviceName(aPartNameChoice->GetStringSelection());
    UpdateDevice();
    MainFrame::TheMainFrame->aConfigMemoryTab->LoadConfigBuffer();  // Number of 'valid locations' may have changed (Load Config Memory)
    MainFrame::TheMainFrame->aOptionTab->UpdateOptionsDisplay();    // a lot may have changed, depending on chip type
    MainFrame::TheMainFrame->aCodeMemTab->UpdateCodeMemDisplay();   // addresses and instruction width may have changed
    MainFrame::TheMainFrame->aDataMemTab->UpdateDataMemDisplay();   // address range for DATA(~EEPROM) memory may have changed

    // Now, ONLY HERE AFTER THE USER SELECTED A NEW DEVICE, check the 'Vdd/Vpp'
    // sequence. If it is defined in the device settings, and is incompatible
    // to the current state on the "Other Options" panel, show a warning .
    // Why not simply switch without asking ? Because for many devices,
    //   the proper Vdd/Vpp switching sequence is UNKNOWN or NOWHERE SPECIFIED !
    if ( ( (PIC_DeviceInfo.wVppVddSequence==PROGMODE_VPP_THEN_VDD/*0*/) && (TSessionConfig::GetNeedPowerBeforeMCLR()) )
            ||( (PIC_DeviceInfo.wVppVddSequence==PROGMODE_VDD_THEN_VPP/*1*/) && (!TSessionConfig::GetNeedPowerBeforeMCLR()) )
       )
    {
        // this message should occur after switching from 16F628 to F818 (for example)
        MainFrame::TheMainFrame->aNotebook->SetSelection(MainFrame::TS_Options);
        MainFrame::TheMainFrame->aOptionTab->aVddBeforeMCLRChk->SetForegroundColour(*wxRED);
        if (wxMessageBox(
                    _("Conflicting Vpp/Vdd switching sequence on the Options tab.\n Change the sequence to match the new device info ?"),
                    _("Vpp/Vdd switching sequence possibly wrong"),
                    wxICON_QUESTION | wxYES_NO | wxCANCEL ) == wxYES )
        {
            // user agrees to let the program select the 'proper'(??) setting...
            if (PIC_DeviceInfo.wVppVddSequence==PROGMODE_VDD_THEN_VPP)
                TSessionConfig::SetNeedPowerBeforeMCLR(1);
            else
                TSessionConfig::SetNeedPowerBeforeMCLR(0);
        }
        MainFrame::TheMainFrame->aOptionTab->UpdateOptionsDisplay();
        MainFrame::TheMainFrame->aOptionTab->aVddBeforeMCLRChk->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
        MainFrame::TheMainFrame->Update();
        wxMilliSleep(500);
        MainFrame::TheMainFrame->aNotebook->ChangeSelection(MainFrame::TS_DeviceConfig);
    } // end if < Vpp/Vdd switching sequence POSSIBLY wrong >
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TDeviceCfgPanel::onConfigWordHexEditText(wxCommandEvent& event)
{
    editConfigWord(0, aConfigWordHexEdit);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TDeviceCfgPanel::onConfigWordHexEdit2Text(wxCommandEvent& event)
{
    editConfigWord(1, aConfigWordHexEdit2);
}
//---------------------------------------------------------------------------



void TDeviceCfgPanel::onProgMemSizeTextText(wxCommandEvent& event)
{
    if (MainFrame::TheMainFrame->m_Updating)
        return;

    aProgMemSizeText->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    if ( tolower(PIC_DeviceInfo.sz40DeviceName[0])=='u') // "unknown" ?
    {
        long Size;
        if (aProgMemSizeText->GetValue().ToLong(&Size)
                &&  (Size < PIC_BUF_CODE_SIZE)
                &&  (Size > 0))
        {
            TSessionConfig::SetUnknownCodeMemorySize(Size);
            PIC_DeviceInfo.lCodeMemSize = TSessionConfig::GetUnknownCodeMemorySize();
        }
        else
            aProgMemSizeText->SetBackgroundColour(wxColour(0xFF,0x7F,0x7F));
    }
}
//---------------------------------------------------------------------------



void TDeviceCfgPanel::onEepromMemSizeTextText(wxCommandEvent& event)
{
    if (MainFrame::TheMainFrame->m_Updating)
        return;
    aEepromMemSizeText->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    if ( tolower(PIC_DeviceInfo.sz40DeviceName[0])=='u') // "unknown" ?
    {
        long Size;
        if (aEepromMemSizeText->GetValue().ToLong(&Size)
                && (Size >= 0))
        {
            TSessionConfig::SetUnknownDataMemorySize(Size);
            PIC_DeviceInfo.lDataEEPROMSizeInByte = Size;
        }
        else
            aEepromMemSizeText->SetBackgroundColour(wxColour(0xFF,0x7F,0x7F));
    }
}
//---------------------------------------------------------------------------



void TDeviceCfgPanel::onHasFlashMemoryChkClick(wxCommandEvent& event)
{
    if (MainFrame::TheMainFrame->m_Updating)
        return;
    if (aHasFlashMemoryChk->GetValue())
    {
        // which kind of FLASH memory, 12..14 bit per location, or more ?
        PIC_DeviceInfo.iCodeMemType=PIC_MT_FLASH;   // 12Fxxx, 16Fxxx -> 14 bit per word
        TSessionConfig::SetHasFlashMemory(1);
    }
    else
    {
        PIC_DeviceInfo.iCodeMemType=PIC_MT_EPROM;
        TSessionConfig::SetHasFlashMemory(0);
    }
}
//---------------------------------------------------------------------------



void TDeviceCfgPanel::onDevCfgGridCellChange(wxGridEvent& event)
{
    // Called when the user(?) selected an item from the "plastered" configuration bit COMBO.
    int   iGridRow = event.GetRow();
    uint32_t dwAddr;

    if ( MainFrame::TheMainFrame->m_Updating )
        return;   // it was not the user ("clicking"), but a programmed access to the COMBO

    ++(MainFrame::TheMainFrame->m_Updating);

	wxString tempstring= aDevCfgGrid->GetCellValue(iGridRow, configbitsCOL_ADDR);
    if (tempstring.ToLong( (long*)&dwAddr, 16))
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
}
//---------------------------------------------------------------------------


